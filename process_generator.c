#include "headers.h"
#include "DSs/PriQueue.h"

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
void clearResources(int signum);
void sigalrmHandler(int signum);

// Returns: -1 if error occured, 
// Returns: the number of processes in the read file if no errors
int readFile(char* fileName, struct Process** processes);

// Global variabes and macros =====================================================
int msgQueueID;

// ================================================================================
int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);

    // 1. Read the input files.
    char filePath[100];
    printf("Enter the input file path: ");
    scanf("%s", filePath); 
    // read an input file specified and put it in processes.
    struct Process* processes;  //! array of processes, DON'T forget to DELETE it later
    int numOfProcesses = readFile("testProcesses.txt", &processes); // returns the number of processes created
    if (numOfProcesses == -1)
        exit(-1);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    puts("Choose one of the following scheduling algorithms: ");
    puts("1. Non-preemptive Highest Priority First (HPF)");
    puts("2. Shortest Remaining time Next (SRTN)");
    puts("3. Round Robin (RR)");


    int chosenAlgo;
    scanf("%d", &chosenAlgo);

    struct AlgorithmMsg algoMsg;
    algoMsg.type = 1;   // any type
    algoMsg.chosenAlgo = chosenAlgo;
    algoMsg.numOfProcesses = numOfProcesses;

    switch(chosenAlgo) {
    case 1:
        // HPF
        algoMsg.parameter = -1; // No parameters
        break;
    case 2:
        // SRTN
        algoMsg.parameter = -1; // No parameters
        break;
    case 3:
        // RR
        algoMsg.parameter = 5; // There exist a quantum
        break;
    default:
        perror("ERROR! Invalid choice number\n");
        exit(-1);
    }

    // 3. Initiate and create the scheduler and clock processes.

    for (int i = 0; i < 2; ++i) {
        int pid = fork();
        if (pid == -1) {
            perror("ERROR occured while forking clk or scheduler");
            exit(-1);
        }
        if (i == 0 && pid == 0)
            execl("clk.out", "clk.out", NULL);
        else if (i == 1 && pid == 0)
            execl("scheduler.out", "scheduler.out", NULL);
    }

    // If you are the (process_generator)
    initClk();

    // 1. Create msg queue
    system("touch Keys/gen_scheduler_msgQ");
    int fileKey = ftok("Keys/gen_scheduler_msgQ", 'A');

    msgQueueID = msgget(fileKey, 0666 | IPC_CREAT);
    if (msgQueueID == -1) {
        perror("ERROR occured during creating the message queue\n");
        exit(-1);
    }

    // 2. Send the algo choise and parameters to the scheduler
    int isSent = msgsnd(msgQueueID, &algoMsg, sizeof(algoMsg) - sizeof(algoMsg.type), !IPC_NOWAIT);
    if (isSent == -1) {
        perror("ERROR occured during sending the algorithm information to the scheduler\n");
        exit(-1);
    }

    // 3. Send each process read at their arrival times
    struct ProcessMsg processMsg;
    processMsg.type = 1;     // any type

    printf("Number of processes: %d\n", numOfProcesses);

    signal(SIGALRM, sigalrmHandler);
    for (int i = 0; i < numOfProcesses; ++i) {

        
        // alarm(processes[i].arrivalTime - (i != 0) * processes[i - 1].arrivalTime);
        // raise(S);

        // processMsg.process = processes[i];
        // int isSent = msgsnd(msgQueueID, &processMsg, sizeof(processMsg) - sizeof(processMsg.type), !IPC_NOWAIT);
        // if (isSent == -1) {
        //     perror("ERROR occured during sending the process information to the scheduler\n");
        //     exit(-1);
        // }

        while (getClk() < processes[i].arrivalTime);
            // printf("%d\n", getClk());  // Wait till the arrival time of any process is raised

        processes[i].remainingTime = processes[i].runningTime;
        processes[i].waitingTime = 0;
        processes[i].finishTime = 0;
        processes[i].status = NOT_ARRIVED;
        processMsg.process = processes[i];
        int isSent = msgsnd(msgQueueID, &processMsg, sizeof(processMsg) - sizeof(processMsg.type), !IPC_NOWAIT);
        if (isSent == -1) {
            perror("ERROR occured during sending the process information to the scheduler\n");
            exit(-1);
        }
    }

    free(processes);

    destroyClk(true);
}

// ================================= Function Definitions ============================================
void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    msgctl(msgQueueID, IPC_RMID, NULL);
    exit(0);
    
}

void sigalrmHandler(int signum) {
    raise(SIGCONT);
}

int readFile(char* fileName, struct Process** processes) {

    FILE* inputFile = fopen(fileName, "r");		//open the file
    if (inputFile == NULL) {                    //can't open the file
        printf("ERROR! Could not open file %s\n", fileName);
        return -1;  // ERROR occured
    }
	int num;
	int rows = 0, cols = 0;

    // Calculate the number of processes
	fseek(inputFile, 29, SEEK_SET);
    while (fscanf(inputFile, "%d", &num) != EOF) {
        cols = (cols + 1) % 4;
        if (cols == 0) ++rows; 
    }
    int numOfProcesses = rows;

    *processes = (struct Process*)malloc(sizeof(struct Process) * numOfProcesses);
    
    // Fill the array of processes with the read data from the input file 
    rows = 0, cols = 0;
	fseek(inputFile, 29, SEEK_SET);

    while (fscanf(inputFile, "%d", &num) != EOF) {
        switch (cols) {
        case 0:
            (*processes)[rows].id = num;
            break;
        case 1:
            (*processes)[rows].arrivalTime = num;
            break;
        case 2:
            (*processes)[rows].runningTime = num;
            break;
        case 3:
            (*processes)[rows].priority = num;
            break;
        }
        cols = (cols + 1) % 4;
        if (cols == 0) ++rows; 
    }

    fclose(inputFile);
    
    // for (int i = 0; i < numOfProcesses; ++i)
    //     printf("%d %d %d %d\n", 
    //         (*processes)[i].id, 
    //         (*processes)[i].arrivalTime, 
    //         (*processes)[i].priority, 
    //         (*processes)[i].runningTime
    //     );

    return numOfProcesses;
}
