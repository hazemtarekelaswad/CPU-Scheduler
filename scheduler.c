#include "headers.h"
#include "DSs/PriQueue.h"
#include "DSs/queue.h"
#include "DSs/list.h"

// Structs ====================================
struct AlgorithmMsg {
    long type;
    int chosenAlgo;
    int parameter;
    int numOfProcesses;
};

struct ProcessMsg {
    long type;
    struct Process process;
};

// Functions =====================================================================
void HPF(int numOfProcesses);
void SRTN(int numOfProcesses);
void RR(int numOfProcesses, int quantum);

void TEST(int numOfProcesses);

// Global variabes and macros =====================================================
int msgQueueID;
bool isReceived = false;
bool isRunning = false;
bool isAlarmed = false;

// SIGUSR1
void HPFreceiveProcessHandler(int signum) {
    isReceived = true;
}

// SIGCHLD
void HPFprocessTermHandler(int signum) {
    isRunning = false;
}

//SIGALRM
void RRalarmHandler(int signum) {
    isAlarmed = false;
}

int main(int argc, char * argv[])
{
    initClk();
    signal(SIGUSR1, HPFreceiveProcessHandler);
    signal(SIGCHLD, HPFprocessTermHandler);
    signal(SIGALRM, RRalarmHandler);

    // system("touch Keys/scheduler_gen_msgQ");
    int fileKey = 41;/*ftok("Keys/gen_scheduler_msgQ", 'A');*/

    msgQueueID = msgget(fileKey, 0666 | IPC_CREAT);
    if (msgQueueID == -1) {
        perror("ERROR occured during getting the msg queue in the scheduler\n");
        exit(-1);
    }

    // Receive the information of the chosen algorithm from the generator
    struct AlgorithmMsg algoMsg;

    int isReceived = msgrcv(msgQueueID, &algoMsg, sizeof(algoMsg) - sizeof(algoMsg.type), 0, !IPC_NOWAIT);
    if (isReceived == -1) {
        perror("ERROR occured during receiving the algorithm information from the generator\n");
        exit(-1);
    }
    printf("Chosen Algo: %d\n", algoMsg.chosenAlgo);    // FOR DEBUGGING
    
    // Msg queue for receiving the process Finish Time
    // system("touch Keys/scheduler_process_msgQ");
    // int fileKeyProcess = ftok("Keys/scheduler_process_msgQ", 'B');

    // msgQueueProcessID = msgget(fileKeyProcess, 0666 | IPC_CREAT);
    // if (msgQueueProcessID == -1) {
    //     perror("ERROR occured during getting the msg queue from the process to scheduler\n");
    //     exit(-1);
    // }

    switch(algoMsg.chosenAlgo) {
    case 1:
        // HPF
        HPF(algoMsg.numOfProcesses);    // Recent
        // TEST(algoMsg.numOfProcesses);
        break;
    case 2:
        // SRTN
        SRTN(algoMsg.numOfProcesses);
        break;
    case 3:
        // RR
        RR(algoMsg.numOfProcesses, algoMsg.parameter);
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
    
    struct Process* processToRun;
    struct ProcessMsg processMsg;
    
    // Construct a priority queue
    struct PriQueue* hpfQueue = pqConstruct();

    // struct TerminationMsg termMsg;
    while (1) {
           
        pause();
        printf("clk after pausing %d\n", getClk());
        if (isReceived) {

            int receivedStatus = msgrcv(msgQueueID, &processMsg, sizeof(processMsg.process), 0, !IPC_NOWAIT);
            pqEnqueue(hpfQueue, &processMsg.process, processMsg.process.priority);
            isReceived = false;
            printf("ARRIVED | CLK: %d \t ID: %d \t Arrival: %d\n", 
                getClk(), 
                processMsg.process.id,
                processMsg.process.arrivalTime
            );
        }
        
        if (isRunning) continue;
        
        if (numOfProcesses == 0) break;
        if (pqIsEmpty(hpfQueue)) continue;

        // ==== Run the process ====
        --numOfProcesses;

        processToRun = pqDequeue(hpfQueue);

        printf("STARTED | CLK: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 
                getClk(), 
                processToRun->id,
                processToRun->arrivalTime,
                processToRun->runningTime
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

    // Destruct the priority queue
    pqDestruct(hpfQueue);
}

void SRTN(int numOfProcesses) {

}

void RR(int numOfProcesses, int quantum) {
    
    struct Process* processToRun = NULL;
    struct ProcessMsg processMsg;

    int startTime;
    int generatedPid;
    
    // Construct a list
    struct Queue* RRQueue = qConstruct();

    while(1) {
        pause();
        printf("clk after pausing %d\n", getClk());

        if (isReceived) {
            int receivedStatus = msgrcv(msgQueueID, &processMsg, sizeof(processMsg.process), 0, !IPC_NOWAIT);
            qEnqueue(RRQueue, &processMsg.process);
            isReceived = false;

            printf("ARRIVED | CLK: %d \t ID: %d \t Arrival: %d\n", 
                getClk(), 
                processMsg.process.id,
                processMsg.process.arrivalTime
            );
        }

        if (processToRun) { 

            if(processToRun -> remainingTime > quantum)
            {
                if (!isAlarmed) {
                    alarm(quantum);
                    isAlarmed = true;
                }
                if (getClk() < startTime + quantum)
                    continue;
                
                processToRun -> remainingTime -= quantum;
                // processToRun -> pid = generatedPid;
                printf("ProcessToRun PID: %d\n", processToRun->pid);
                kill(processToRun->pid, SIGSTOP); // need to be modified

                printf("Stopped | CLK: %d \t ID: %d \t Stop time: %d\n", 
                    getClk(), 
                    processToRun -> id,
                    startTime + quantum
                );

                qEnqueue(RRQueue , processToRun);
            }

            else
            {
                if (getClk() < startTime + processToRun -> remainingTime)
                    continue;

            
                printf("Finished | CLK: %d \t ID: %d \t Finish time: %d\n", 
                    getClk(), 
                    processToRun -> id,
                    startTime + processToRun -> remainingTime
                );
                --numOfProcesses;
                if (numOfProcesses == 0) break;
                
            }

        }
        
        processToRun = qDequeue(RRQueue);

        // First time to run this process
        if (processToRun->remainingTime == processToRun->runningTime) {

            // Convert both the quantum and remaining time to strings
            char remainingTimeString[(int)1e5];
            sprintf(remainingTimeString, "%d", processToRun->remainingTime);

            char quantumString[(int)1e5];
            sprintf(quantumString, "%d", quantum);

            startTime = getClk();
            printf("CLOCK before forking: %d\n", startTime);
            // Fork the process (first time running)
            generatedPid = fork();
            if (generatedPid == -1) {
                printf("ERROR occured while forking the process with ID: %d\n", processToRun->id);
                exit(-1);
            }

            if (generatedPid == 0) {
                if (processToRun->remainingTime > quantum)
                    execl("process.out", "process.out", quantumString, NULL);
                
                else
                    execl("process.out", "process.out", remainingTimeString, NULL);
            }
            processToRun -> pid = generatedPid;
            printf("Forked Process PID: %d\n", processToRun->pid);
        }
            
        // Progress the Processes

        else
        {
            startTime = getClk();
            kill (processToRun -> pid, SIGCONT);

            printf("Continued | CLK: %d \t ID: %d\n", 
                getClk(), 
                processToRun -> id
            );
        }  
    }
    qDestruct(RRQueue);
}

void printPerfFile(const char* filePath, float utilization, float avgWTA, float avgWaiting, float stdWTA) {
    FILE* perfFile = fopen(filePath, "w");		//open the file
    if (perfFile == NULL) {                    //can't open the file
        printf("ERROR! Could not open file %s\n", filePath);
        return;  // ERROR occured
    }
    fprintf(perfFile, "CPU utilization = %2f%%\n", utilization);
    fprintf(perfFile, "Avg WTA = %2f\n", avgWTA);
    fprintf(perfFile, "Avg Waiting = %2f\n", avgWaiting);
    fprintf(perfFile, "Std WTA = %2f", stdWTA);

    fclose(perfFile);
}

// void printLogFile(  
//     const char* filePath, 
//     int clk, 
//     int id, 
//     enum Status status, 
//     int arrivalTime, 
//     int totalTime, 
//     int remainingTime, 
//     int waitingTime, 
//     int TA, 
//     int WTA
// ) {

//     FILE* logFile = fopen(filePath, "w");		//open the file
//     if (logFile == NULL) {                    //can't open the file
//         printf("ERROR! Could not open file %s\n", filePath);
//         return;  // ERROR occured
//     }

    
//     fprintf(logFile, "At time ")
//     fclose(logFile);

// }




void TEST(int numOfProcesses) {
    
    bool isInitial = true;      // Flag: to control if it is the first iteration in the loop or not
    bool isFinished = false;    // Flag: to check if is the last process or not
    struct Process* processToRun;
    int startTime;

    // Construct a priority queue
    struct PriQueue* hpfQueue = pqConstruct();

    // struct TerminationMsg termMsg;
    struct ProcessMsg processMsg;
    while (1) {

         // Receive process info from the generator once arrived
        int isReceived = msgrcv(msgQueueID, &processMsg, sizeof(processMsg) - sizeof(processMsg.type), 0, IPC_NOWAIT);

        if (isReceived != -1) {     // If received new process info, enqueue it
            // printf("ARRIVED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d\n", 
            //     getClk(),
            //     processMsg.process.id, 
            //     processMsg.process.arrivalTime, 
            //     processMsg.process.runningTime,
            //     processMsg.process.priority
            // );  // FOR DEBUGGING

            pqEnqueue(hpfQueue, &processMsg.process, processMsg.process.priority);
        }

        if (!isInitial && startTime + processToRun->runningTime > getClk()) {
            continue;
        }


            // Wait for the processToRun to be terminated with NO HANG
        if (!isInitial) {
            //* NOTE: The finish time returned from the process as an exit code
            int status;
            int child_pid = waitpid(processToRun->pid, &status, WNOHANG);
            // if (!WIFEXITED(status))
            //     perror("ERROR in process termination\n");
            // else {
                printf("CLK: %d     Process with PID: %d is terminated successfully with Finish Time: %d\n", getClk(), child_pid, WEXITSTATUS(status));
                processToRun->finishTime = WEXITSTATUS(status);
            // }
    
            // printf("FINISHED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d  Finish Time: %d\n", 
            //     getClk(),
            //     processToRun->id, 
            //     processToRun->arrivalTime, 
            //     processToRun->runningTime,
            //     processToRun->priority,
            //     processToRun->finishTime
            // );  // FOR DEBUGGING
    
        }


        // int ReceivedFinishTime = msgrcv(msgQueueProcessID, &termMsg, sizeof(termMsg) - sizeof(termMsg.type), 0, IPC_NOWAIT);
        // if (ReceivedFinishTime != -1) { // if received     
        //     processToRun->finishTime = termMsg.finishTime;

        // }


        if (isFinished)
            break;

        if (pqIsEmpty(hpfQueue))
            continue;

        processToRun = pqDequeue(hpfQueue);

        // printf("STARTED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d\n", 
        //     getClk(),
        //     processToRun->id, 
        //     processToRun->arrivalTime, 
        //     processToRun->runningTime,
        //     processToRun->priority
        // );  // FOR DEBUGGING


        --numOfProcesses;
        if (numOfProcesses == 0)
            isFinished = true;
        
        // Convert the remaining time to string and pass it as a process argument 
        char remainingTimeString[(int)1e5];
        sprintf(remainingTimeString, "%d", processToRun->remainingTime);
        
        // Fork the arrived process
        int generatedPid = fork();
        if (generatedPid == -1) {
            printf("ERROR occured while forking the process with ID: %d\n", processToRun->id);
            exit(-1);
        }

        if (generatedPid == 0) {
            execl("process.out", "process.out", remainingTimeString, NULL);
        }
        startTime = getClk();
        processToRun->pid = generatedPid;   
        
        isInitial = false;
    }

    // Destruct the priority queue
    pqDestruct(hpfQueue);
}
