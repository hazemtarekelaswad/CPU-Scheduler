#include "headers.h"
#include "DSs/PriQueue.h"
#include <time.h>

// Structs ====================================

struct ProcessMsg {
    long type;
    struct Process process;
};

// Functions =====================================================================
void clearResources(int signum);

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
    // char filePath[100];
    // printf("Enter the input file path: ");
    // scanf("%s", filePath); 
    // read an input file specified and put it in processes.
    struct Process* processes;  
    int numOfProcesses = readFile("testProcesses2.txt", &processes); // returns the number of processes created
    if (numOfProcesses == -1)
        exit(-1);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    puts("Choose one of the following scheduling algorithms: ");
    puts("1. Non-preemptive Highest Priority First (HPF)");
    puts("2. Shortest Remaining time Next (SRTN)");
    puts("3. Round Robin (RR)");


    int chosenAlgo;
    scanf("%d", &chosenAlgo);

    int parameter;

    switch(chosenAlgo) {
    case 1:
        // HPF
        parameter = -1; // No parameters
        break;
    case 2:
        // SRTN
        parameter = -1; // No parameters
        break;
    case 3: {

        // RR
        printf("Enter the RR Quantum: ");
        scanf("%d", &parameter);
        break;
    }
    default:
        perror("ERROR! Invalid choice number\n");
        exit(-1);
    }

    char chosenAlgoString[(int)1e5];
    sprintf(chosenAlgoString, "%d", chosenAlgo);
    char parameterString[(int)1e5];
    sprintf(parameterString, "%d", parameter);
    char countString[(int)1e5];
    sprintf(countString, "%d", numOfProcesses);

    // Initiate and create the scheduler and the clock processes.
    int schedulerPID;
    for (int i = 0; i < 2; ++i) {
        int pid = fork();
        if (pid == -1) {
            perror("ERROR occured while forking clk or scheduler");
            exit(-1);
        }
        if (i == 0 && pid == 0)
            execl("scheduler.out", "scheduler.out", chosenAlgoString, parameterString, countString, NULL);
        else if (i == 1 && pid == 0)
            execl("clk.out", "clk.out", NULL);
        if (i == 0 && pid != 0)
            schedulerPID = pid;
    }

    // If you are the (process_generator)
    initClk();
    // Create msg queue between process generator and the scheduler
    // system("touch Keys/gen_scheduler_msgQ");
    int fileKey = 41;/*ftok("Keys/gen_scheduler_msgQ", 'A');*/
    msgQueueID = msgget(fileKey, 0666 | IPC_CREAT);
    if (msgQueueID == -1) {
        perror("ERROR occured during creating the message queue\n");
        exit(-1);
    }

    // Send each process read at their arrival times
    struct ProcessMsg processMsg;
    processMsg.type = 1;     // any type

    int start_time = 0;
    int end_time = 0;
    for (int i = 0; i < numOfProcesses; ++i) {
        int delay_duration = end_time - start_time;
        if (i == 0)
            sleep(processes[i].arrivalTime - delay_duration);
        else
            sleep(processes[i].arrivalTime - processes[i - 1].arrivalTime - delay_duration);

        // printf("CLK: %d ID: %d\n", getClk(), processes[i].id);
        // printf("CLK: %d\n", getClk());     

        start_time = getClk();
        processes[i].remainingTime = processes[i].runningTime;
        processes[i].waitingTime = 0;
        processes[i].finishTime = 0;
        // processes[i].status = NOT_ARRIVED;           
        processMsg.process = processes[i];
        // kill(schedulerPID, SIGUSR1);
        int isSent = msgsnd(msgQueueID, &processMsg, sizeof(processMsg.process), !IPC_NOWAIT);
        if (isSent == -1) {
            perror("ERROR occured during sending the process information to the scheduler\n");
            exit(-1);
        }
        end_time = getClk();

    }

    // Wait for the scheduler and the clock to terminate
    //* NOTE: Scheduler is resposible for destroying the clock
    int status;
    int child_pid = wait(&status);
    if (!WIFEXITED(status))
        perror("ERROR in generator's children termination\n");
    else
        printf("Process with PID: %d is terminated successfully with exit code: %d\n", child_pid, WEXITSTATUS(status));
    
    // free(processes);

    destroyClk(true);
    return 0;
}

// ================================= Function Definitions ============================================
void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    msgctl(msgQueueID, IPC_RMID, NULL);
    exit(0);
    
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
