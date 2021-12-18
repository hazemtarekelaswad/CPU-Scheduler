#include "headers.h"
#include <time.h>


int main(int agrc, char * argv[])
{    
    while (clock() < atoi(argv[1]) * CLOCKS_PER_SEC);
    // kill(getppid(), SIGUSR2);
    return 0;
}
