#include "headers.h"
#include "DSs/PriQueue.h"
#include "DSs/Queue.h"

// Structs ====================================
struct AlgorithmMsg {
    long type;
    int chosenAlgo;
    int parameter;
};

struct ProcessMsg {
    long type;
    struct Process process;
};

// Functions =====================================================================
void HPF();
void SRTN();
void RR(int quantum);

// Global variabes and macros =====================================================
int msgQueueID;

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
    
    switch(algoMsg.chosenAlgo) {
    case 1:
        // HPF
        HPF();
        break;
    case 2:
        // SRTN
        SRTN();
        break;
    case 3:
        // RR
        RR(algoMsg.parameter);
        break;
    default:
        perror("ERROR! Invalid choice number\n");
        exit(-1);
    }


    
    // //TODO implement the scheduler :)
    // //upon termination release the clock resources
    
    // destroyClk(true);
}

void HPF() {
    
    bool isRunning = false;

    // Construct a priority queue
    struct PriQueue* hpfQueue = pqConstruct();

    struct ProcessMsg processMsg;
    while (1) {

         // Receive process info from the generator once arrived
        int isReceived = msgrcv(msgQueueID, &processMsg, sizeof(processMsg) - sizeof(processMsg.type), 0, !IPC_NOWAIT);
        if (isReceived == -1) {
            perror("ERROR occured during receiving the process information from the generator\n");
            exit(-1);
        }
        printf("ID: %d    Arriaval: %d    Pri: %d    RunningTime: %d\n", 
            processMsg.process.id, 
            processMsg.process.arrivalTime, 
            processMsg.process.priority,
            processMsg.process.runningTime
        );  // FOR DEBUGGING

        pqEnqueue(hpfQueue, &processMsg.process, processMsg.process.priority);

        
        struct Process* processToRun = pqFront(hpfQueue);
        
        // Fork the arrived process
        int generatedPid = fork();
        if (generatedPid == -1) {
            printf("ERROR occured while forking the process with ID: %d\n", processMsg.process.id);
            exit(-1);
        }

        int prevClk = getClk();

        if (generatedPid == 0) {
            // Convert the remaining time to string and pass it as a process argument 
            char remainingTimeString[(int)1e5];
            sprintf(remainingTimeString, "%d", processMsg.process.remainingTime);
            execl("process.out", "process.out", remainingTimeString, NULL);
        }
        processMsg.process.pid = generatedPid;   

    
            

        
        // Stop it
        kill(processMsg.process.pid, SIGTSTP);
        // Push it in the queue

        if (prevClk + processToRun->runningTime == getClk()) {
            pqDequeue(hpfQueue);
            processToRun = pqFront(hpfQueue);
            kill(processToRun->pid, SIGCONT);
        }
        

    }

    // Destruct the priority queue
    pqDestruct(hpfQueue);
}

void SRTN() {

}

void RR(int quantum) {

}
