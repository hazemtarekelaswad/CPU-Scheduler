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

typedef int valueType; // modifiable according to the value type you want

struct Node {
    valueType value;
    struct Node* next;
};

struct Queue {
    struct Node* back;
    struct Node* front;
};

struct Queue* qConstruct() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->back = queue->front = NULL;
    return queue;
}

bool qIsEmpty(struct Queue* queue) {
    return (queue->back == NULL && queue->front == NULL);
}

valueType qFront(struct Queue* queue) {
    if (qIsEmpty(queue)) 
        return -1;   // Assuming all values are non-negatives
    return queue->front->value;
}

void qEnqueue(struct Queue* queue, valueType value) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->value = value;
    newNode->next = NULL;

    if (qIsEmpty(queue))
        queue->front = queue->back = newNode;
    else {
        queue->back->next = newNode;
        queue->back = newNode;
    }
    newNode = NULL;
}

bool qDequeue(struct Queue* queue) {
    if (qIsEmpty(queue))
        return false;
    struct Node* temp = queue->front;
    queue->front = queue->front->next;
    free(temp);
    if (queue->front == NULL)
        queue->back = NULL;
    return true; 
    
}

void qDestruct(struct Queue* queue) {
    while (!qIsEmpty(queue))
        qDequeue(queue);
    queue = NULL;
}

void qPrint(struct Queue* queue) {
    struct Node* trav = queue->front;
    while (trav) {
        printf("%d ", trav->value);
        trav = trav->next;
    }
}

#endif