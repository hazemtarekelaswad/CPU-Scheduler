#include "headers.h"
#include "DSs/PriQueue.h"
#include "DSs/queue.h"
#include "DSs/list.h"
#include "Outputfile_functions.h"

// Structs ====================================

struct ProcessMsg {
    long type;
    struct Process process;
};

// Functions =====================================================================
void HPF(int numOfProcesses);
void SRTN(int numOfProcesses);
void RR(int numOfProcesses, int quantum);


// Global variabes and macros =====================================================
int msgQueueID;
bool isReceived = false;
bool isRunning = false;
bool isAlarmed = false;
bool isFinished = false;

// SIGUSR1
void receiveProcessHandler(int signum) {
    isReceived = true;
}

// SIGCHLD
void HPFprocessTermHandler(int signum) {
    isRunning = false;
}

// SIGALRM
void RRalarmHandler(int signum) {
    // isAlarmed = false;
    // printf("alarm handler at clk: %d\n", getClk());
}

// SIGCHLD
void SRTNprocessTermHandler(int signum) {
    isFinished = true;
}


int main(int argc, char * argv[])
{
    signal(SIGUSR1, receiveProcessHandler);
    initClk();
    
  
    // Receive the information of the chosen algorithm from the generator
    // struct AlgorithmMsg algoMsg;

   

    int fileKey = 41;/*ftok("Keys/gen_scheduler_msgQ", 'A');*/

    msgQueueID = msgget(fileKey, 0666 | IPC_CREAT);
    if (msgQueueID == -1) {
        perror("ERROR occured during getting the msg queue in the scheduler\n");
        exit(-1);
    }

    switch(atoi(argv[1])) {
    case 1:
        // HPF
        HPF(atoi(argv[3]));    // Recent
        // TEST(algoMsg.numOfProcesses);
        break;
    case 2:
        // SRTN
        SRTN(atoi(argv[3]));
        break;
    case 3:
        // RR
        RR(atoi(argv[3]), atoi(argv[2]));
        break;
    default:
        perror("ERROR! Invalid choice number\n");
        exit(-1);
    }


    
    // //upon termination release the clock resources
    destroyClk(false);
    exit(0);
}


void HPF(int numOfProcesses) {
    // ========= for the O/P File ================
    int countProcesses = numOfProcesses;
    FILE* logFile = fopen("scheduler.log", "w");		
    if (logFile == NULL) {                    
        printf("ERROR! Could not open file");
        return;  // ERROR occured
    }
    int sumOfRunning = 0;
    float* WTA_array = (float*)malloc(sizeof(float) * numOfProcesses);
    float* Waiting_array = (float*)malloc(sizeof(float) * numOfProcesses);
    int index_waiting = 0;
    int index_WTA = 0;
    int finishClk;
    // ============================================

    // signal(SIGCHLD, HPFprocessTermHandler);
    struct Process* processToRun;
    struct ProcessMsg processMsg;
    
    // Construct a priority queue
    struct PriQueue* hpfQueue = pqConstruct();

    // struct TerminationMsg termMsg;
    while (1) {
        
        do {
            int receivedStatus = msgrcv(msgQueueID, &processMsg, sizeof(processMsg.process), 0, IPC_NOWAIT);
            if (receivedStatus == -1) break;
            // printf("ARRIVED | CLK: %d \t ID: %d \t Arrival: %d\n", 
            //     getClk(), 
            //     processMsg.process.id,
            //     processMsg.process.arrivalTime
            // );
            sumOfRunning += processMsg.process.runningTime;
            struct Process* heapProcess = (struct Process*)malloc(sizeof(struct Process));
            *heapProcess = processMsg.process;
            pqEnqueue(hpfQueue, heapProcess, processMsg.process.priority);
        } while(1);

        while (!pqIsEmpty(hpfQueue)) {
            int startClk = getClk();
            processToRun = pqDequeue(hpfQueue);
            
            processToRun->waitingTime = startClk - processToRun->arrivalTime;

            //! start
            OutFile_Starting(
                logFile, 
                1, 
                processToRun->id, 
                processToRun->arrivalTime,
                processToRun->runningTime,
                processToRun->remainingTime,
                processToRun->waitingTime,  // 0 for HPF
                startClk
            );

            // printf("STARTED | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 
            //     getClk(), 
            //     processToRun->id,
            //     processToRun->arrivalTime,
            //     processToRun->runningTime
            // );


            char remainingTimeString[(int)1e5];
            sprintf(remainingTimeString, "%d", processToRun->runningTime);
            
            // Fork the arrived process
            int generatedPid = fork();
            if (generatedPid == -1) {
                printf("ERROR occured while forking the process with ID: %d\n", processToRun->id);
                exit(-1);
            }

            else if (generatedPid == 0) {
                execl("process.out", "process.out", remainingTimeString, NULL);
            }
           
            int status;
            wait(&status);
            finishClk = getClk();   // for output file
            if (!WIFEXITED(status))
                perror("ERROR exiting the processes\n");
            
            //! finish
            OutFile_Starting(
                logFile, 
                4, 
                processToRun->id, 
                processToRun->arrivalTime,
                processToRun->runningTime,
                0,
                processToRun->waitingTime,  
                finishClk
            );
            WTA_array[index_WTA++] = 1.0 * (finishClk - processToRun->arrivalTime) / processToRun->runningTime;
            Waiting_array[index_waiting++] = processToRun->waitingTime;
            // printf("Finished | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 
            //     getClk(), 
            //     processToRun->id,
            //     processToRun->arrivalTime,
            //     processToRun->runningTime
            // );
            --numOfProcesses;

        }
     
        if (numOfProcesses == 0) break;
        
    }
    // ============ For O/P File ===========================
    float sumOfWTA = 0;
    float sumOfWaiting = 0;
    for (int i = 0; i < countProcesses; ++i) {
        sumOfWTA += WTA_array[i];
        sumOfWaiting += Waiting_array[i];
    }

    float accum = 0;
    float avgWTA = sumOfWTA / countProcesses;
    float avgWaiting = sumOfWaiting / countProcesses;
    for (int i = 0; i < countProcesses; ++i)
        accum += (WTA_array[i] - avgWTA) * (WTA_array[i] - avgWTA);

    printPerfFile(
        "scheduler.perf", 
        1.0 * sumOfRunning / finishClk, 
        avgWTA, 
        avgWaiting, 
        sqrt((double)accum / countProcesses)
    );
    free(WTA_array);
    fclose(logFile);
    // ====================================================
    pqDestruct(hpfQueue);

}



  void SRTN(int numOfProcesses) {
    signal(SIGUSR2, SRTNprocessTermHandler);
    struct Process* processToRun = NULL; //deeh el7atrun
    struct PriQueue* srtnQueue = pqConstruct(); // Construct a priority queue
    struct Process* temp = NULL; //bn3ml 3laiha compare
    int startTime=0;
    bool isInitialSrtn=true;
    struct ProcessMsg processMsg;//lma ywsl 7aga mn pgenerator ywsloo process by7otha feeh processmsg
    while (1) {
        pause();

        if(isFinished)
        {
             printf("FINISHED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d\n",
             getClk() - startTime,
             processToRun->id,
             processToRun->arrivalTime,
             processToRun->runningTime,
             processToRun->priority
            );  // FOR DEBUGGING
            pqDequeue(srtnQueue);
            --numOfProcesses;
            if(numOfProcesses ==0)
            break;
            processToRun = pqFront(srtnQueue);
            kill(processToRun->pid,SIGCONT);
            isFinished = false;
        }

        printf("clk after pausing %d\n", getClk());
        if (isReceived) {

            int receivedStatus = msgrcv(msgQueueID, &processMsg, sizeof(processMsg.process), 0, !IPC_NOWAIT);
            // pqEnqueue(srtnQueue, &processMsg.process, processMsg.process.runningTime);
            struct Process* heapProcess = (struct Process*)malloc(sizeof(struct Process));
            *heapProcess = processMsg.process;
            pqEnqueue(srtnQueue, heapProcess, processMsg.process.remainingTime);
            //startTime=getClk();

            isReceived = false;
            printf("ARRIVED | CLK: %d \t ID: %d \t Arrival: %d\n",
                getClk(),
                processMsg.process.id,
                processMsg.process.arrivalTime
            );
        }
        if (isInitialSrtn==true)//awl mra bs l2n feeh el lfat elqadma 7atb2a btrun asln
        processToRun = pqFront(srtnQueue);
        //kill(processToRun->pid,SIGCONT);

         printf("STARTED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d         %d\n",
             getClk(),
             processToRun->id,
             processToRun->arrivalTime,
             processToRun->runningTime,
             processToRun->priority,
             processToRun->remainingTime
        );  // FOR DEBUGGING
        // Convert the remaining time to string and pass it as a process argument
        char remainingTimeString[(int)1e5];
        sprintf(remainingTimeString, "%d", processMsg.process.remainingTime);
        // Fork the arrived process
        int generatedPid = fork();
        if (generatedPid == -1) {
            printf("ERROR occured while forking the process with ID: %d\n", processToRun->id);
            exit(-1);
        }

         else if (generatedPid == 0) {
            execl("process.out", "process.out", remainingTimeString, NULL);//fork process w t43'lha
        }
        else
        {
        if (isInitialSrtn==true) // awl mra mfee4 3,airha yb2a runha
        {
            isInitialSrtn=false;
            int estimatedrt = processToRun->remainingTime - (getClk() - processToRun->arrivalTime ) ;
            processToRun->remainingTime = estimatedrt;
            startTime=getClk();
            continue;
         // kda 5lst awl lfa
        }

        temp->pid = generatedPid;
        kill(generatedPid,SIGSTOP);//processs y5leeh elchild y2of

        if (startTime==0)
        {
            int estimatedrt = processToRun->remainingTime - (getClk() - processToRun->arrivalTime ) ;
            processToRun->remainingTime = estimatedrt;
        }
        else
        {
            int estimatedrt = processToRun->remainingTime - (getClk() - startTime);
            processToRun->remainingTime = estimatedrt;
        }

           printf("STARTED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d         %d\n",
             getClk(),
             processToRun->id,
             processToRun->arrivalTime,
             processToRun->runningTime,
             processToRun->priority,
             processToRun->remainingTime
        );


        if(temp->remainingTime < processToRun->remainingTime )
        {
            kill(processToRun->pid,SIGSTOP);
            processToRun=temp;
            kill(processToRun->pid,SIGCONT);
            startTime=getClk();

        }
        else
            startTime=getClk();
        }
    }

    //enque tb2a feeh elready state while deque tb2a finished
    // elm7tot 3laiha elfront hia elrunning
    // Destruct the priority queue
    pqDestruct(srtnQueue);

}


void RR(int numOfProcesses, int quantum) {

     // ========= for the O/P File ================
    int countProcesses = numOfProcesses;
    FILE* logFile = fopen("scheduler.log", "w");		
    if (logFile == NULL) {                    
        printf("ERROR! Could not open file");
        return;  // ERROR occured
    }
    int sumOfRunning = 0;
    float* WTA_array = (float*)malloc(sizeof(float) * numOfProcesses);
    float* Waiting_array = (float*)malloc(sizeof(float) * numOfProcesses);
    int index_wta = 0;
    int index_waiting = 0;
    int finishClk;
    // ============================================

    signal(SIGALRM, RRalarmHandler);
    struct Process* processToRun = NULL;
    struct ProcessMsg processMsg;
    
    int startTime;
    int generatedPid;
    int receivedStatus;

    // Construct a queue

    struct Queue* RRQueue = qConstruct();
    struct Queue* STOPPED_RR = qConstruct();

    while(1) {
        receivedStatus = 1;
        while (receivedStatus != -1)

        {

            receivedStatus= msgrcv(msgQueueID, &processMsg, sizeof(processMsg.process), 0, IPC_NOWAIT);

            

            if (receivedStatus == -1)

                break;

            

            printf("ARRIVED | CLK: %d \t ID: %d \t Arrival: %d\n", 

                getClk(), 

                processMsg.process.id,

                processMsg.process.arrivalTime

            );

            sumOfRunning += processMsg.process.runningTime;

            struct Process* heapProcess = (struct Process*)malloc(sizeof(struct Process));

            *heapProcess = processMsg.process;

            qEnqueue(RRQueue, heapProcess);

       }



        if (qFront(RRQueue) != NULL)

        {
            processToRun = qDequeue(RRQueue);

            /*

            if first time to run 

                -if (remainig time < quantum) run remaining time 



                wait till finishing

                print finish

                numprocesses --



                -else run quantum 

                while (!(getclk() > start + quantum)) ---> SIGSTOP ---> Enque ---> print Stopped

            else

                 --if (remainig time < quantum) SIGCONT remaining time 



                    wait till finishing

                    print finish

                    numprocess -- 



                -else SIGCONT

                while (!(getclk() > start + quantum)) ---> SIGSTOP ---> Enque ---> print Stopped

            

            */



            //if (processToRun -> status == NOTFORKED)



            // Convert both the quantum and remaining time to strings

                char remainingTimeString[(int)1e5];

                sprintf(remainingTimeString, "%d", processToRun->remainingTime);



                char quantumString[(int)1e5];

                sprintf(quantumString, "%d", quantum);



            // printf("CLOCK before forking: %d\n", startTime);

            // Fork the process (first time running)

               // if (processToRun -> status == NOTFORKED)

                    int arrClk = getClk();
                    //! started
                    processToRun->waitingTime = arrClk - processToRun->arrivalTime;
                    printf("Started | CLK: %d \t ID: %d\n", 
                        arrClk, 
                        processToRun -> id
                    );

                    OutFile_Starting(
                        logFile,
                        1,
                        processToRun->id,
                        processToRun->arrivalTime,
                        processToRun->runningTime,
                        processToRun->remainingTime,
                        processToRun->waitingTime,
                        arrClk
                    );

                    generatedPid = fork();

                    if (generatedPid == -1) {
                        printf("ERROR occured while forking the process with ID: %d\n", processToRun->id);
                        exit(-1);
                    }

                    if (generatedPid == 0) {

                        if (processToRun->remainingTime > quantum)

                        {
                            execl("process.out", "process.out", quantumString, NULL);
                        }

                        else
                        {
                            execl("process.out", "process.out", remainingTimeString, NULL);
                        }

                    }



                    else

                    {

                        processToRun -> pid = generatedPid;



                        if (processToRun->remainingTime > quantum)

                        {

    

                            //startTime = getClk();

                            //while (!(getClk() > startTime + quantum));

                            

                            //alarm(quantum);

                            //pause();



                            sleep(quantum);

                            int stoppedClk = getClk();

                            processToRun->remainingTime -= quantum;


                            // ! stopped
                            processToRun->waitingTime = stoppedClk - (processToRun->runningTime - processToRun->remainingTime) - processToRun->arrivalTime;
                            
                            printf("Stopped after start | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                                getClk(), 
                                processToRun->id,
                                processToRun->arrivalTime,
                                processToRun->runningTime
                            );

                            OutFile_Starting(
                                logFile,
                                2,
                                processToRun->id,
                                processToRun->arrivalTime,
                                processToRun->runningTime,
                                processToRun->remainingTime,
                                processToRun->waitingTime,
                                stoppedClk
                            );


                            kill (processToRun->pid,SIGSTOP);



                             //processToRun -> status = STOPPED;



                            qEnqueue (STOPPED_RR, processToRun);

                        

                        }

                

                        else

                        {

                            //int s; 

                            //processToRun->pid = wait(&s);



                            sleep(processToRun -> remainingTime);
                            finishClk = getClk();


                            //alarm (processToRun -> remainingTime);

                            //pause ();


                            //! finished
                            processToRun->remainingTime = 0;

                            processToRun->waitingTime = finishClk - (processToRun->runningTime - processToRun->remainingTime) - processToRun->arrivalTime;
                            WTA_array[index_wta++] = 1.0 * (finishClk - processToRun->arrivalTime) / processToRun->runningTime;
                            Waiting_array[index_waiting++] = processToRun->waitingTime;
                            
                            printf("Finished | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                                getClk(), 

                                processToRun->id,

                                processToRun->arrivalTime,

                                processToRun->runningTime

                                );

                                OutFile_Starting(
                                    logFile,
                                    4,
                                    processToRun->id,
                                    processToRun->arrivalTime,
                                    processToRun->runningTime,
                                    0,
                                    processToRun->waitingTime,
                                    finishClk
                                );

                            numOfProcesses--;

                        

                        }

                    }

                    //processToRun -> status = STARTED;

            }   

        else

            {

                if (qFront(STOPPED_RR) != NULL)

                    {

                        processToRun = qDequeue(STOPPED_RR);

                        if (processToRun->remainingTime > quantum)

                        {

                            //! continue
                            printf("Continued | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                            getClk(), 

                            processToRun->id,

                            processToRun->arrivalTime,

                            processToRun->runningTime

                            );

                            processToRun->waitingTime = getClk() - (processToRun->runningTime - processToRun->remainingTime) - processToRun->arrivalTime;
                            
                            OutFile_Starting(
                                logFile,
                                3,
                                processToRun->id,
                                processToRun->arrivalTime,
                                processToRun->runningTime,
                                processToRun->remainingTime,
                                processToRun->waitingTime,
                                getClk()
                            );

                            kill (processToRun->pid, SIGCONT);



                            //startTime = getClk();

                            

                            //while (!(getClk() - startTime >= quantum));



                            sleep(quantum);
                            int stoppedClk = getClk();


                            //alarm(quantum);

                            //pause();

                       

                            processToRun->remainingTime -= quantum;


                            //! stopped
                            processToRun->waitingTime = stoppedClk - (processToRun->runningTime - processToRun->remainingTime) - processToRun->arrivalTime;
                            kill (processToRun->pid,SIGSTOP);



                            //processToRun -> status = STOPPED;



                            printf("Stopped after resume| CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                            getClk(), 

                            processToRun->id,

                            processToRun->arrivalTime,

                            processToRun->runningTime

                            );

                             OutFile_Starting(
                                    logFile,
                                    2,
                                    processToRun->id,
                                    processToRun->arrivalTime,
                                    processToRun->runningTime,
                                    processToRun->remainingTime,
                                    processToRun->waitingTime,
                                    stoppedClk
                                );

                            qEnqueue (STOPPED_RR,processToRun);

                       }

                       

                

                        else

                        {

                        
                            //! continue
                            int contClk = getClk();
                            processToRun->waitingTime = contClk - (processToRun->runningTime - processToRun->remainingTime) - processToRun->arrivalTime;

                            printf("Continued | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                            getClk(), 

                            processToRun->id,

                            processToRun->arrivalTime,

                            processToRun->runningTime

                            );

                             OutFile_Starting(
                                logFile,
                                3,
                                processToRun->id,
                                processToRun->arrivalTime,
                                processToRun->runningTime,
                                processToRun->remainingTime,
                                processToRun->waitingTime,
                                contClk
                            );


                            kill (processToRun->pid, SIGCONT);



                            /* int s; 

                            generatedPid = wait(&s);



                            WIFEXITED(s);

                            */

                            sleep (processToRun -> remainingTime);

                            finishClk = getClk();

                            //alarm (processToRun -> remainingTime);

                            //pause();


                            //! finished
                            processToRun->waitingTime = finishClk - (processToRun->runningTime - processToRun->remainingTime) - processToRun->arrivalTime;
                            WTA_array[index_wta++] = 1.0 * (finishClk - processToRun->arrivalTime) / processToRun->runningTime;
                            Waiting_array[index_waiting++] = processToRun->waitingTime;
                            

                            printf("Finished after CONT | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                            getClk(), 

                            processToRun->id,

                            processToRun->arrivalTime,

                            processToRun->runningTime

                            );
                             OutFile_Starting(
                                logFile,
                                4,
                                processToRun->id,
                                processToRun->arrivalTime,
                                processToRun->runningTime,
                                0,
                                processToRun->waitingTime,
                                finishClk
                            );


                            numOfProcesses--;    

                        }

                    }

            }

            if (numOfProcesses == 0)
                break;

        }


    // ============ For O/P File ===========================
    float sumOfWTA = 0;
    float sumOfWaiting = 0;
    for (int i = 0; i < countProcesses; ++i) {
        sumOfWTA += WTA_array[i];
        sumOfWaiting += Waiting_array[i];
    }

    float accum = 0;
    float avgWTA = sumOfWTA / countProcesses;
    float avgWaiting = sumOfWaiting / countProcesses;

    for (int i = 0; i < countProcesses; ++i)
        accum += (WTA_array[i] - avgWTA) * (WTA_array[i] - avgWTA);

    printPerfFile(
        "scheduler.perf", 
        1.0 * sumOfRunning / finishClk, 
        avgWTA, 
        avgWaiting, 
        sqrt((double)accum / countProcesses)
    );
    free(WTA_array);
    free(Waiting_array);
    
    fclose(logFile);
    // ====================================================

     qDestruct(RRQueue);

     qDestruct(STOPPED_RR);

}