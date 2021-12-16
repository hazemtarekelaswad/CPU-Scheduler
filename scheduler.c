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

// SIGUSR1
void HPFreceiveProcessHandler(int signum) {
    isReceived = true;
}

// SIGCHLD
void HPFprocessTermHandler(int signum) {
    isRunning = false;
}

int main(int argc, char * argv[])
{

    signal(SIGUSR1, HPFreceiveProcessHandler);
    signal(SIGCHLD, HPFprocessTermHandler);
    initClk();

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
        // HPF(algoMsg.numOfProcesses);
        TEST(algoMsg.numOfProcesses);
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
    puts("TErm scheuler\n");
    exit(0);
}



void TEST(int numOfProcesses) {
    // struct ProcessMsg processMsg;
    // while (1) {
    //     pause();
    //     if (isReceived) {

    //         int receivedStatus = msgrcv(msgQueueID, &processMsg, sizeof(processMsg) - sizeof(processMsg.type), 0, !IPC_NOWAIT);
    //         printf("ARRIVED | CLK: %d \t ID: %d \t Arrival: %d\n", 
    //             getClk(), 
    //             processMsg.process.id,
    //             processMsg.process.arrivalTime
    //         );
    //     }
        
    // }
    
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
        //   printf("FINISHED | CLK: %d \t PID: %d \t ID: %d \t Arrival: %d \t Running: %d\n", 
        //         getClk(), 
        //         processToRun->pid,
        //         processToRun->id,
        //         processToRun->arrivalTime,
        //         processToRun->runningTime
        //     );
        
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
void HPF(int numOfProcesses) {
    
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

void SRTN(int numOfProcesses) {

}

void RR(int numOfProcesses, int quantum) {
    
    struct Process* processToRun;
    struct ProcessMsg processMsg;
    
    // Construct a list
    struct List* RRlist = listConstruct();

    while (1) {
        pause();
        printf("clk after pausing %d\n", getClk());

        if (isReceived) {
            int receivedStatus = msgrcv(msgQueueID, &processMsg, sizeof(processMsg.process), 0, !IPC_NOWAIT);
            listPushBack(RRlist, &processMsg.process);
            isReceived = false;

            printf("ARRIVED | CLK: %d \t ID: %d \t Arrival: %d\n", 
                getClk(), 
                processMsg.process.id,
                processMsg.process.arrivalTime
            );
        }
        
        processToRun = listTravValue(RRlist);
        listAdvanceTrav(RRlist);

        // First time to run this process
        if (processToRun->remainingTime == processToRun->runningTime) {

            // Convert both the quantum and remaining time to strings
            char remainingTimeString[(int)1e5];
            sprintf(remainingTimeString, "%d", processToRun->remainingTime);

            char quantumString[(int)1e5];
            sprintf(quantumString, "%d", quantum);

            // Fork the process (first time running)
            int generatedPid = fork();
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
        }

        // if (getClk() == 
            

        
    }
}
