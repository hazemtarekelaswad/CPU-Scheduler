#include <stdio.h>
#include "PriQueue.h"


int main () {
    struct PriQueue* priQueue = pqConstruct();
    
    pqEnqueue(priQueue, 236, 5);
    pqPrint(priQueue), puts("");
    pqEnqueue(priQueue, 36, 6);
    pqPrint(priQueue), puts("");
    pqEnqueue(priQueue, 597, 2);
    pqPrint(priQueue), puts("");
    pqEnqueue(priQueue, 659, 3);
    pqPrint(priQueue), puts("");
    pqEnqueue(priQueue, 789, 8);
    pqPrint(priQueue), puts("");
    pqEnqueue(priQueue, 91, 6);
    pqPrint(priQueue), puts("");

    printf("%d\n", pqDequeue(priQueue));
    pqPrint(priQueue), puts("");

    printf("%d\n", pqDequeue(priQueue));
    pqPrint(priQueue), puts("");

    printf("%d\n", pqDequeue(priQueue));
    pqPrint(priQueue), puts("");
    
    pqDestruct(priQueue);
    return 0;
}