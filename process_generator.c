#include "headers.h"
#include "DSs/PriQueue.h"

void clearResources(int);


struct AlgorithmMsg {
    long type;
    int chosenAlgo;
    int parameter;
};

struct ProcessMsg {
    long type;
    struct Process process;
};

// Returns: -1 if error occured, 
// Returns: the number of processes in the read file if no errors
int readFile(char* fileName, struct Process** processes) {

    FILE* inputFile = fopen(fileName, "r");		//open the file
    if (inputFile == NULL) {                    //can't open the file
        printf("ERROR! Could not open file %s\n", fileName);
        return -1;  // ERROR occured
    }

	int num;
	int rows = 0, cols = 0;

    // Calculate the number of processes
	fseek(inputFile, 30, SEEK_SET);
    while (fscanf(inputFile, "%d", &num) != EOF) {
        cols = (cols + 1) % 4;
        if (cols == 0) ++rows; 
    }
    int numOfProcesses = rows;

    *processes = (struct Process*)malloc(sizeof(struct Process) * numOfProcesses);
    
    // Fill the array of processes with the read data from the input file 
    rows = 0, cols = 0;
	fseek(inputFile, 30, SEEK_SET);

    while (fscanf(inputFile, "%d", &num) != EOF) {
        switch (cols) {
        case 0:
            (*processes)[rows].id = num;
            break;
        case 1:
            (*processes)[rows].arrivalTime = num;
            break;
        case 2:
            (*processes)[rows].priority = num;
            break;
        case 3:
            (*processes)[rows].runningTime = num;
            break;
        }
        cols = (cols + 1) % 4;
        if (cols == 0) ++rows; 
    }

    fclose(inputFile);
    return numOfProcesses;
}
int msgQueueID;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // 1. Read the input files.
    if (argc != 2) {
        perror("ERROR! Not enough arguments\n");
        exit(-1);
    }

    // @husseinAhmed10: reads an input file specified as cmd argument and put it in processes.
    
    struct Process* processes;  //! array of processes, DON'T forget to DELETE it later
    int numOfProcesses = readFile(argv[1], &processes); // returns the number of processes created


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
    
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources

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

    for (int i = 0; i < numOfProcesses; ++i) {
        while (getClk() < processes[i].arrivalTime);

        processMsg.process = processes[i];
        int isSent = msgsnd(msgQueueID, &processMsg, sizeof(processMsg) - sizeof(processMsg.type), !IPC_NOWAIT);
        if (isSent == -1) {
            perror("ERROR occured during sending the process information to the scheduler\n");
            exit(-1);
        }
    }

    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    msgctl(msgQueueID, IPC_RMID, NULL);
    exit(0);
    
}
