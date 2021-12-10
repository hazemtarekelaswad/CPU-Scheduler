#include "headers.h"
#include "DSs/PriQueue.h"
#include "DSs/queue.h"

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

struct TerminationMsg {
    long type;
    int finishTime;
};

// Functions =====================================================================
void HPF(int numOfProcesses);
void SRTN(int numOfProcesses);
void RR(int numOfProcesses, int quantum);

// Global variabes and macros =====================================================
int msgQueueID;
int msgQueueProcessID;

int main(int argc, char * argv[])
{

    initClk();

    system("touch Keys/scheduler_gen_msgQ");
    int fileKey = ftok("Keys/gen_scheduler_msgQ", 'A');

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
    system("touch Keys/scheduler_process_msgQ");
    int fileKeyProcess = ftok("Keys/scheduler_process_msgQ", 'B');

    msgQueueProcessID = msgget(fileKeyProcess, 0666 | IPC_CREAT);
    if (msgQueueProcessID == -1) {
        perror("ERROR occured during getting the msg queue from the process to scheduler\n");
        exit(-1);
    }

    switch(algoMsg.chosenAlgo) {
    case 1:
        // HPF
        HPF(algoMsg.numOfProcesses);
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
    
    // destroyClk(true);
}

void HPF(int numOfProcesses) {
    
    bool isInitial = true;      // Flag: to control if it is the first iteration in the loop or not
    bool isFinished = false;    // Flag: to check if is the last process or not
    struct Process* processToRun;
    int startTime;

    // Construct a priority queue
    struct PriQueue* hpfQueue = pqConstruct();

    struct TerminationMsg termMsg;
    struct ProcessMsg processMsg;
    while (1) {

         // Receive process info from the generator once arrived
        int isReceived = msgrcv(msgQueueID, &processMsg, sizeof(processMsg) - sizeof(processMsg.type), 0, IPC_NOWAIT);
        // if (isReceived == -1) {
        //     perror("ERROR occured during receiving the process information from the generator\n");
        //     exit(-1);
        // }

        if (isReceived != -1) {     // If received new process info, enqueue it
            printf("ARRIVED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d\n", 
                getClk(),
                processMsg.process.id, 
                processMsg.process.arrivalTime, 
                processMsg.process.runningTime,
                processMsg.process.priority
            );  // FOR DEBUGGING

            pqEnqueue(hpfQueue, &processMsg.process, processMsg.process.priority);
        }


        if (!isInitial && startTime + processToRun->runningTime > getClk()) {
            continue;
        }

        int ReceivedFinishTime = msgrcv(msgQueueProcessID, &termMsg, sizeof(termMsg) - sizeof(termMsg.type), 0, IPC_NOWAIT);
        if (ReceivedFinishTime != -1) { // if received     
            processToRun->finishTime = termMsg.finishTime;

        }


        if (isFinished)
            break;

        if (pqIsEmpty(hpfQueue))
            continue;

        processToRun = pqDequeue(hpfQueue);

            printf("FINISHED | CLK: %d     ID: %d    Arriaval: %d    RunningTime: %d    Pri: %d     FinishTime: %d\n", 
                getClk(),
                processToRun->id, 
                processToRun->arrivalTime, 
                processToRun->runningTime,
                processToRun->priority,
                processToRun->finishTime
            );  // FOR DEBUGGING


        --numOfProcesses;
        if (numOfProcesses == 0)
            isFinished = true;
        
        // Convert the remaining time to string and pass it as a process argument 
        char remainingTimeString[(int)1e5];
        sprintf(remainingTimeString, "%d", processMsg.process.remainingTime);
        
        // Fork the arrived process
        int generatedPid = fork();
        if (generatedPid == -1) {
            printf("ERROR occured while forking the process with ID: %d\n", processMsg.process.id);
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

}
