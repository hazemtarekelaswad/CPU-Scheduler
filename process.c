#include "../headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    int start = getClk();

    remainingtime = atoi(argv[1]);  // Initially the remaining time = running time
    while (remainingtime > 0)
    {
        remainingtime -= getClk() - start; 
    }
    
    

    destroyClk(false);
    return 0;
}
