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

typedef struct Process* qValueType; // modifiable according to the value type you want

struct qNode {
    qValueType value;
    struct qNode* next;
};

struct Queue {
    struct qNode* back;
    struct qNode* front;
};

struct Queue* qConstruct() {
    struct Queue* queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->back = queue->front = NULL;
    return queue;
}

bool qIsEmpty(struct Queue* queue) {
    return (queue->back == NULL && queue->front == NULL);
}

qValueType qFront(struct Queue* queue) {
    if (qIsEmpty(queue)) 
        return NULL;   // Assuming all values are non-negatives
    return queue->front->value;
}

void qEnqueue(struct Queue* queue, qValueType value) {
    struct qNode* newNode = (struct qNode*)malloc(sizeof(struct qNode));
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

qValueType qDequeue(struct Queue* queue) {
    if (qIsEmpty(queue))
        return NULL;
    struct qNode* temp = queue->front;
    queue->front = queue->front->next;
    qValueType value = temp->value;
    free(temp);
    if (queue->front == NULL)
        queue->back = NULL;
    return value; 
    
}

void qDestruct(struct Queue* queue) {
    while (!qIsEmpty(queue)) {
        qValueType dequeued = qDequeue(queue);
        free(dequeued);
    }

    queue = NULL;
}

// void qPrint(struct Queue* queue) {
//     struct qNode* trav = queue->front;
//     while (trav) {
//         printf("%d ", trav->value);
//         trav = trav->next;
//     }
// }

#endif