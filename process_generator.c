#include "headers.h"

void clearResources(int);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    if (argc != 2) {
        perror("ERROR! Not enough arguments\n");
        exit(-1);
    }

    // @husseinAhmed10: reads an input file specified as cmd argument and put it in processes.
    inputFile(arg[1]);  


    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    puts("Choose one of the following scheduling algorithms: ");
    pus("1. Non-preemptive Highest Priority First (HPF)");
    pus("2. Shortest Remaining time Next (SRTN)");
    pus("3. Round Robin (RR)");

    int chosenAlgo;
    scanf("Enter the algorithm's number: %d", &chosenAlgo);
    switch(chosenAlgo) {
    case 1:
        // Do something  HPF
        break;
    case 2:
        // Do something  SRTN
        break;
    case 3:
        // Do something  RR
        break;
    default:
        perror("ERROR! Invalid number\n");
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

    // If you are the parent (process_generator)
    initClk();
    while (1) {
        int x = getClk();
        printf("current time is %d\n", x);
    }

}
    


    
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources

    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
