#include "headers.h"

struct AlgorithmMsg {
    long type;
    int chosenAlgo;
    int parameter;
};

struct ProcessMsg {
    long type;
    struct Process process;
};

int msgQueueID;

int main(int argc, char * argv[])
{

    system("touch Keys/scheduler_gen_msgQ");
    int fileKey = ftok("Keys/gen_scheduler_msgQ", 'A');

    msgQueueID = msgget(fileKey, 0666 | IPC_CREAT);
    if (msgQueueID == -1) {
        perror("ERROR occured during getting the msg queue in the scheduler\n");
        exit(-1);
    }

    struct AlgorithmMsg algoMsg;

    int isReceived = msgrcv(msgQueueID, &algoMsg, sizeof(algoMsg) - sizeof(algoMsg.type), 0, !IPC_NOWAIT);
    if (isReceived == -1) {
        perror("ERROR occured during receiving the algorithm information from the generator\n");
        exit(-1);
    }
    printf("Chosen Algo: %d\n", algoMsg.chosenAlgo);

    struct ProcessMsg processMsg;
    while (1) {
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
        );
    }


    // initClk();
    
    // //TODO implement the scheduler :)
    // //upon termination release the clock resources
    
    // destroyClk(true);
}
