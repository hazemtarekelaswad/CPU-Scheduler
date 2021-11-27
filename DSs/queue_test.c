#include <stdio.h>
#include "Queue.h"

int main () {
    struct Queue* queue = qConstruct();
    for (int i = 0; i < 23; ++i) {
        qEnqueue(queue, i);
        qPrint(queue);
        printf("   %d", qFront(queue));
        puts("");
    }
    qDequeue(queue);
    qPrint(queue);
    printf("   %d", qFront(queue));
    puts("");
    
    return 0;
}