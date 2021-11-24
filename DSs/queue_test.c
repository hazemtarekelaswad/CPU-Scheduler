#include <stdio.h>
#include "queue.h"

int main () {
    struct Queue* queue = queue_construct();
    for (int i = 0; i < 23; ++i) {
        queue_enqueue(queue, i);
        queue_print(queue);
        printf("   %d", queue_front(queue));
        puts("");
    }
    queue_dequeue(queue);
    queue_print(queue);
    printf("   %d", queue_front(queue));
    puts("");
    
    return 0;
}