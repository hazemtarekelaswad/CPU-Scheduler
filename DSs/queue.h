/*
    Notice: The front of the queue is the head of the linked list
    and the back of the queue is the tail of the linked list.

      front              back
       |                  |
    --------------------------
    |   |->|   |->|   |->|   |
    -------------------------
*/

#ifndef QUEUE
#define QUEUE

#include "../headers.h"
#include <stdio.h>
#include <stdlib.h>

typedef int value_type; // modifiable according to the value type you want

struct Node {
    value_type value;
    struct Node* next;
};

struct Queue {
    struct Node* back;
    struct Node* front;
};

struct Queue* queue_construct() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->back = queue->front = NULL;
    return queue;
}

bool queue_is_empty(struct Queue* queue) {
    return (queue->back == NULL && queue->front == NULL);
}

value_type queue_front(struct Queue* queue) {
    if (queue_is_empty(queue)) 
        return -1;   // Assuming all values are non-negatives
    return queue->front->value;
}

void queue_enqueue(struct Queue* queue, value_type value) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->value = value;
    new_node->next = NULL;

    if (queue_is_empty(queue))
        queue->front = queue->back = new_node;
    else {
        queue->back->next = new_node;
        queue->back = new_node;
    }
    new_node = NULL;
}

bool queue_dequeue(struct Queue* queue) {
    if (queue_is_empty(queue))
        return false;
    struct Node* temp = queue->front;
    queue->front = queue->front->next;
    free(temp);
    if (queue->front == NULL)
        queue->back = NULL;
    return true; 
    
}

void queue_destruct(struct Queue* queue) {
    while (!queue_is_empty(queue))
        queue_dequeue(queue);
    queue = NULL;
}

void queue_print(struct Queue* queue) {
    struct Node* trav = queue->front;
    while (trav) {
        printf("%d ", trav->value);
        trav = trav->next;
    }
}

#endif