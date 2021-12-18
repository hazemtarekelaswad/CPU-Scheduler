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

void TEST(int numOfProcesses);
void TESTSRTN(int numOfProcesses);


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

    
    // printf("Chosen Algo: %d\n", algoMsg.chosenAlgo);    // FOR DEBUGGING
    
    // Msg queue for receiving the process Finish Time
    // system("touch Keys/scheduler_process_msgQ");
    // int fileKeyProcess = ftok("Keys/scheduler_process_msgQ", 'B');

    // msgQueueProcessID = msgget(fileKeyProcess, 0666 | IPC_CREAT);
    // if (msgQueueProcessID == -1) {
    //     perror("ERROR occured during getting the msg queue from the process to scheduler\n");
    //     exit(-1);
    // }

      // system("touch Keys/scheduler_gen_msgQ");
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

void HPF3(int numOfProcesses) {
    signal(SIGCHLD, HPFprocessTermHandler);

    struct Process* processToRun;

    struct ProcessMsg processMsg;

    

    // Construct a priority queue

    struct PriQueue* hpfQueue = pqConstruct();



    // struct TerminationMsg termMsg;

    int receivedStatus;

    while (1) {

        

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



            struct Process* heapProcess = (struct Process*)malloc(sizeof(struct Process));

            *heapProcess = processMsg.process;

            pqEnqueue(hpfQueue, heapProcess, processMsg.process.priority);

        }



        /*

        if (isRunning) continue;

        

        if (numOfProcesses == 0) break;

        if (pqIsEmpty(hpfQueue)) continue;



        */



        // ==== Run the process ====

        // --numOfProcesses;



        while (pqFront(hpfQueue) != NULL)

        {

        processToRun = pqDequeue(hpfQueue);



        printf("STARTED | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                getClk(), 

                processToRun->id,

                processToRun->arrivalTime,

                processToRun->runningTime

            );



        char remainingTimeString[(int)1e5];

        sprintf(remainingTimeString, "%d", processToRun->remainingTime);

        

        // Fork the arrived process

        int generatedPid = fork();

        if (generatedPid == -1) {

            printf("ERROR occured while forking the process with ID: %d\n", processToRun->id);

            exit(-1);

        }



        else if (generatedPid == 0) {

            execl("process.out", "process.out", remainingTimeString, NULL);

        }



        int s; 

        generatedPid = wait(&s);



        //if (WIFEXITED(s))



        printf("Finished | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                getClk(), 

                processToRun->id,

                processToRun->arrivalTime,

                processToRun->runningTime

            );

        numOfProcesses--;



        }



        if (numOfProcesses == 0)

            break;

        //isRunning = true;    

        // Convert the remaining time to string and pass it as a process argument 

    

    }



    // Destruct the priority queue

    pqDestruct(hpfQueue);

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
    float* WTA_array = (float*)malloc(sizeof(float));
    int index = 0;
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

            processToRun = pqDequeue(hpfQueue);
            OutFile_Starting(
                logFile, 
                1, 
                processToRun->id, 
                processToRun->arrivalTime,
                processToRun->runningTime,
                processToRun->remainingTime,
                0,  // 0 for HPF
                getClk()
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
            
            OutFile_Starting(
                logFile, 
                4, 
                processToRun->id, 
                processToRun->arrivalTime,
                processToRun->runningTime,
                0,
                0,  // 0 for HPF
                finishClk
            );
            WTA_array[index++] = 1.0 * (finishClk - processToRun->arrivalTime) / processToRun->runningTime;
            
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
    for (int i = 0; i < countProcesses; ++i)
        sumOfWTA += WTA_array[i];

    float accum = 0;
    float avgWTA = sumOfWTA / countProcesses;
    for (int i = 0; i < countProcesses; ++i)
        accum += (WTA_array[i] - avgWTA) * (WTA_array[i] - avgWTA);

    printPerfFile(
        "scheduler.perf", 
        1.0 * sumOfRunning / finishClk, 
        avgWTA, 
        0, 
        sqrt((double)accum / countProcesses)
    );
    fclose(logFile);
    // ====================================================
    free(WTA_array);
    pqDestruct(hpfQueue);

}

void HPF2(int numOfProcesses) {
    int countProcesses = numOfProcesses;
    FILE* logFile = fopen("scheduler.log", "w");		//open the file
    if (logFile == NULL) {                    //can't open the file
        printf("ERROR! Could not open file");
        return;  // ERROR occured
    }
    bool isFirst = true;
    int sumOfRunning = 0;
    float* WTA_array = (float*)malloc(sizeof(float));
    int index = 0;

    signal(SIGCHLD, HPFprocessTermHandler);
    struct Process* processToRun;
    struct ProcessMsg processMsg;
    
    // Construct a priority queue
    struct PriQueue* hpfQueue = pqConstruct();

    // struct TerminationMsg termMsg;
    while (1) {
           
        pause();
        if (isReceived) {

            int receivedStatus = msgrcv(msgQueueID, &processMsg, sizeof(processMsg.process), 0, !IPC_NOWAIT);
            sumOfRunning += processMsg.process.runningTime;
           
            struct Process* heapProcess = (struct Process*)malloc(sizeof(struct Process));
            *heapProcess = processMsg.process;
            pqEnqueue(hpfQueue, heapProcess, processMsg.process.priority);

            isReceived = false;
            printf("ARRIVED | CLK: %d \t ID: %d \t Arrival: %d\n", 
                getClk(), 
                processMsg.process.id,
                processMsg.process.arrivalTime
            );
        }
        
        if (isRunning) continue;
        if (!isFirst) {
            OutFile_Starting(
                logFile, 
                4, 
                processToRun->id, 
                processToRun->arrivalTime,
                processToRun->runningTime,
                0,
                0,  // 0 for HPF
                getClk()
            );
            WTA_array[index++] = 1.0 * (getClk() - processToRun->arrivalTime) / processToRun->runningTime;
        }
        isFirst = false;
    
        if (numOfProcesses == 0) break;
        if (pqIsEmpty(hpfQueue)) continue;

        // ==== Run the process ====
        --numOfProcesses;

        processToRun = pqDequeue(hpfQueue);
        OutFile_Starting(
                logFile, 
                1, 
                processToRun->id, 
                processToRun->arrivalTime,
                processToRun->runningTime,
                processToRun->remainingTime,
                0,  // 0 for HPF
                getClk()
        );
       

        isRunning = true;    
        // Convert the remaining time to string and pass it as a process argument 
        char remainingTimeString[(int)1e5];
        sprintf(remainingTimeString, "%d", processToRun->remainingTime);
        
        // Fork the arrived process
        int generatedPid = fork();
        if (generatedPid == -1) {
            printf("ERROR occured while forking the process with ID: %d\n", processToRun->id);
            exit(-1);
        }

        else if (generatedPid == 0) {
            execl("process.out", "process.out", remainingTimeString, NULL);
        }
        
    }
    int finishClk = getClk();

    float sumOfWTA = 0;
    for (int i = 0; i < countProcesses; ++i)
        sumOfWTA += WTA_array[i];

    float accum = 0;
    float avgWTA = sumOfWTA / countProcesses;
    for (int i = 0; i < countProcesses; ++i)
        accum += (WTA_array[i] - avgWTA) * (WTA_array[i] - avgWTA);

    printPerfFile(
        "scheduler.perf", 
        1.0 * sumOfRunning / finishClk, 
        avgWTA, 
        0, 
        ((double)accum / countProcesses)
    );
    // Destruct the priority queue
    pqDestruct(hpfQueue);
    fclose(logFile);
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
            // if(processToRun->remainingTime<=0)
            // {
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
            // }
            // else
                // isFinished = false;
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

    //enque tb2a feeh elready state while deque tb2a finished
    // elm7tot 3laiha elfront hia elrunning
    // Destruct the priority queue
    pqDestruct(srtnQueue);

}

void TESTSRTN(int numOfProcesses) {
    signal(SIGCHLD, SRTNprocessTermHandler);
    struct Process* processToRun = NULL;
    //bool isInitial = true;      // Flag: to control if it is the first iteration in the loop or not
    //isFinishedsr = false;    // Flag: to check if is the last process or not
    //struct Process* processToRun;//ay process t3mlhA deque hia elt3tbr hia elprocesstorun
    struct PriQueue* srtnQueue = pqConstruct();
    struct Process* temp;
    int startTime=0;

    // Construct a priority queue

    //struct TerminationMsg termMsg;
    struct ProcessMsg processMsg;//lma ywsl 7aga mn pgenerator ywsloo process by7otha feeh processmsg
    while (1) {
        pause();
        printf("clk after pausing %d\n", getClk());
        if (isReceived) {

            int receivedStatus = msgrcv(msgQueueID, &processMsg, sizeof(processMsg.process), 0, !IPC_NOWAIT);
            // pqEnqueue(srtnQueue, &processMsg.process, processMsg.process.runningTime);
            struct Process* heapProcess = (struct Process*)malloc(sizeof(struct Process));
            *heapProcess = processMsg.process;
            pqEnqueue(srtnQueue, heapProcess, processMsg.process.remainingTime);

            isReceived = false;
            printf("ARRIVED | CLK: %d \t ID: %d \t Arrival: %d\n", 
                getClk(), 
                processMsg.process.id,
                processMsg.process.arrivalTime
            );
        }
        
        processToRun = pqFront(srtnQueue);
        //kill(processToRun->pid,SIGCONT);
        
         printf("STARTED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d\n",
             getClk(),
             processToRun->id,
             processToRun->arrivalTime,
             processToRun->runningTime,
             processToRun->priority
        );  // FOR DEBUGGING


     //   if (numOfProcesses == 0)
            //isFinished = true;

        // if (processToRun->remainingTime == processToRun->runningTime) {

            // Convert the remaining time to string and pass it as a process argument
            char remainingTimeString[(int)1e5];
            sprintf(remainingTimeString, "%d", processMsg.process.remainingTime);

            // Fork the arrived process
            int generatedPid = fork();

            if (generatedPid != 0) {
                //pqEnqueue(srtnQueue, &processMsg.process, processMsg.process.remainingTime);
                processMsg.process.pid = generatedPid;
                pqEnqueue(srtnQueue, &processMsg.process, processMsg.process.remainingTime);
                //processToRun = pqDequeue(srtnQueue);
            }
            if (generatedPid == 0) {
                //3mlt process gdeeda e3mlha enque
                //processMsg.process.pid = generatedPid
                //pqEnqueue(srtnQueue, &newProcess.process, newProcess.process.runningTime);
                //processToRun = pqDequeue(srtnQueue);
                execl("process.out", "process.out", remainingTimeString, NULL);//fork process w t43'lha
            }
            kill(generatedPid,SIGSTOP);//processs y5leeh elchild y2of
        // }
         temp = pqFront(srtnQueue);


        if (startTime==0)
        {
            int estimatedrt = processToRun->remainingTime - (getClk() - processToRun->arrivalTime ) ;//
            processToRun->remainingTime = estimatedrt;
        }
        else
        {
            int estimatedrt = processToRun->remainingTime - (getClk() - startTime) ;//
            processToRun->remainingTime = estimatedrt;
        }


        if(temp->remainingTime < processToRun->remainingTime )
        {
            kill(processToRun->pid,SIGSTOP);
            pqEnqueue(srtnQueue, processToRun, processToRun->remainingTime);
            processToRun=temp;
            kill(processToRun->pid,SIGCONT);

            startTime=getClk();

        }

       // isInitial = false;
        //--numOfProcesses;
        //if (numOfProcesses == 0)
        //isFinishedsr = true;
        if(isFinished)
        {
            printf("FINISHED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d\n",
             getClk(),
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
            
    }

    //enque tb2a feeh elready state while deque tb2a runnuing
    // Destruct the priority queue
    pqDestruct(srtnQueue);
}

void RR(int numOfProcesses, int quantum) {



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

                

                    printf("Started | CLK: %d \t ID: %d\n", 

                    getClk(), 

                    processToRun -> id

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



                            processToRun->remainingTime -= quantum;



                            printf("Stopped after start | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                                getClk(), 

                                processToRun->id,

                                processToRun->arrivalTime,

                                processToRun->runningTime

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



                            //alarm (processToRun -> remainingTime);

                            //pause ();



                            printf("Finished | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                                getClk(), 

                                processToRun->id,

                                processToRun->arrivalTime,

                                processToRun->runningTime

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

                            printf("Continued | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                            getClk(), 

                            processToRun->id,

                            processToRun->arrivalTime,

                            processToRun->runningTime

                            );



                            kill (processToRun->pid, SIGCONT);



                            //startTime = getClk();

                            

                            //while (!(getClk() - startTime >= quantum));



                            sleep(quantum);



                            //alarm(quantum);

                            //pause();

                       

                            processToRun->remainingTime -= quantum;



                            kill (processToRun->pid,SIGSTOP);



                            //processToRun -> status = STOPPED;



                            printf("Stopped after resume| CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                            getClk(), 

                            processToRun->id,

                            processToRun->arrivalTime,

                            processToRun->runningTime

                            );



                            qEnqueue (STOPPED_RR,processToRun);

                       }

                       

                

                        else

                        {

                        

                            printf("Continued | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                            getClk(), 

                            processToRun->id,

                            processToRun->arrivalTime,

                            processToRun->runningTime

                            );



                            kill (processToRun->pid, SIGCONT);



                            /* int s; 

                            generatedPid = wait(&s);



                            WIFEXITED(s);

                            */

                            sleep (processToRun -> remainingTime);



                            //alarm (processToRun -> remainingTime);

                            //pause();



                            printf("Finished after CONT | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 

                            getClk(), 

                            processToRun->id,

                            processToRun->arrivalTime,

                            processToRun->runningTime

                            );



                            numOfProcesses--;    

                        }

                    }

            }

            if (numOfProcesses == 0)

                break;

        }



     qDestruct(RRQueue);

     qDestruct(STOPPED_RR);

}





// void TEST(int numOfProcesses) {
    
//     bool isInitial = true;      // Flag: to control if it is the first iteration in the loop or not
//     bool isFinished = false;    // Flag: to check if is the last process or not
//     struct Process* processToRun;
//     int startTime;

//     // Construct a priority queue
//     struct PriQueue* hpfQueue = pqConstruct();

//     // struct TerminationMsg termMsg;
//     struct ProcessMsg processMsg;
//     while (1) {

//          // Receive process info from the generator once arrived
//         int isReceived = msgrcv(msgQueueID, &processMsg, sizeof(processMsg) - sizeof(processMsg.type), 0, IPC_NOWAIT);

//         if (isReceived != -1) {     // If received new process info, enqueue it
//             // printf("ARRIVED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d\n", 
//             //     getClk(),
//             //     processMsg.process.id, 
//             //     processMsg.process.arrivalTime, 
//             //     processMsg.process.runningTime,
//             //     processMsg.process.priority
//             // );  // FOR DEBUGGING

//             pqEnqueue(hpfQueue, &processMsg.process, processMsg.process.priority);
//         }

//         if (!isInitial && startTime + processToRun->runningTime > getClk()) {
//             continue;
//         }


//             // Wait for the processToRun to be terminated with NO HANG
//         if (!isInitial) {
//             //* NOTE: The finishClk time returned from the process as an exit code
//             int status;
//             int child_pid = waitpid(processToRun->pid, &status, WNOHANG);
//             // if (!WIFEXITED(status))
//             //     perror("ERROR in process termination\n");
//             // else {
//                 printf("CLK: %d     Process with PID: %d is terminated successfully with Finish Time: %d\n", getClk(), child_pid, WEXITSTATUS(status));
//                 processToRun->finishTime = WEXITSTATUS(status);
//             // }
    
//             // printf("FINISHED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d  Finish Time: %d\n", 
//             //     getClk(),
//             //     processToRun->id, 
//             //     processToRun->arrivalTime, 
//             //     processToRun->runningTime,
//             //     processToRun->priority,
//             //     processToRun->finishTime
//             // );  // FOR DEBUGGING
    
//         }


//         // int ReceivedFinishTime = msgrcv(msgQueueProcessID, &termMsg, sizeof(termMsg) - sizeof(termMsg.type), 0, IPC_NOWAIT);
//         // if (ReceivedFinishTime != -1) { // if received     
//         //     processToRun->finishTime = termMsg.finishTime;

//         // }


//         if (isFinished)
//             break;

//         if (pqIsEmpty(hpfQueue))
//             continue;

//         processToRun = pqDequeue(hpfQueue);

//         // printf("STARTED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d\n", 
//         //     getClk(),
//         //     processToRun->id, 
//         //     processToRun->arrivalTime, 
//         //     processToRun->runningTime,
//         //     processToRun->priority
//         // );  // FOR DEBUGGING


//         --numOfProcesses;
//         if (numOfProcesses == 0)
//             isFinished = true;
        
//         // Convert the remaining time to string and pass it as a process argument 
//         char remainingTimeString[(int)1e5];
//         sprintf(remainingTimeString, "%d", processToRun->remainingTime);
        
//         // Fork the arrived process
//         int generatedPid = fork();
//         if (generatedPid == -1) {
//             printf("ERROR occured while forking the process with ID: %d\n", processToRun->id);
//             exit(-1);
//         }

//         if (generatedPid == 0) {
//             execl("process.out", "process.out", remainingTimeString, NULL);
//         }
//         startTime = getClk();
//         processToRun->pid = generatedPid;   
        
//         isInitial = false;
//     }

//     // Destruct the priority queue
//     pqDestruct(hpfQueue);
// }
