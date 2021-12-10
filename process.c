#include "headers.h"

/* Modify this file as needed*/
int remainingTime;
int runningTime;
int finishTime;
int msgQueueProcessID;

struct TerminationMsg {
    long type;
    int finishTime;
};

int main(int agrc, char * argv[])
{
    // printf("PROCESS CREATED with PID: %d\n", getpid());
    initClk();
    int startTime = getClk();

    runningTime = atoi(argv[1]);  // Initially, the remaining time = running time
    remainingTime = runningTime;
    // printf("Running Time: %d\n", remainingTime);
    while (remainingTime > 0)
    {
        remainingTime = runningTime - (getClk() - startTime);
        
        if (remainingTime == 0) {
            finishTime = getClk();
        }
    }
    // printf("CLK: %d     PROCESS FINISHED with Finish Time: %d\n", getClk(), finishTime);

    // struct TerminationMsg termMsg = { 5, finishTime };
    
    // system("touch Keys/process_scheduler_msgQ");
    // int fileKey = ftok("Keys/process_scheduler_msgQ", 'B');

    // msgQueueProcessID = msgget(fileKey, 0666 | IPC_CREAT);
    // if (msgQueueProcessID == -1) {
    //     perror("ERROR occured during creating the message queue in the process file\n");
    //     exit(-1);
    // }

    // // 2. Send the finish time to the scheduler
    // int isSent = msgsnd(msgQueueProcessID, &termMsg, sizeof(termMsg) - sizeof(termMsg.type), !IPC_NOWAIT);
    // if (isSent == -1) {
    //     perror("ERROR occured during sending the finish time to the scheduler\n");
    //     exit(-1);
    // }


    destroyClk(false);
    exit(finishTime);
}
