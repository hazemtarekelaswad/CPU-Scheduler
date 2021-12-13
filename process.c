#include "headers.h"
#include <time.h>


int main(int agrc, char * argv[])
{
    initClk();

    clock_t runningTime = (clock_t)atoi(argv[1]);  
    clock_t remainingTime = runningTime;    // Initially, the remaining time = running time
    
    clock_t startTime = clock();
    while (remainingTime > 0)
    {
        remainingTime = runningTime * CLOCKS_PER_SEC - (clock() - startTime);
    }
    clock_t finishTime = clock();
    int simFinishTime = getClk();
    
    // printf("Duration: %ld\n", (finishTime - startTime) / CLOCKS_PER_SEC);
    // printf("FinishTime: %ld\n", finishTime / CLOCKS_PER_SEC);
    // printf("FinishTime: %d\n", simFinishTime);
    destroyClk(false);
    exit(0);
    // exit (finishTime / CLOCKS_PER_SEC);
    // return 0;
}
