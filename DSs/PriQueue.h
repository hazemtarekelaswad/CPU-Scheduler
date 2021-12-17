/*
    This priority queue assumes that the lower the priority value, 
    the highest priority it is. the lower values are at the front of
    queue.

    Notice: The front of the queue is the head of the linked list
    and the back of the queue is the tail of the linked list.

      front              back
       |                  |
    --------------------------
    |   |->|   |->|   |->|   |
    -------------------------
*/

#ifndef PRI_QUEUE
#define PRI_QUEUE

#include "../headers.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Process* pqValueType; // modifiable according to the value type you want

struct PriNode {
    int priority;
    pqValueType value;
    struct PriNode* next;
};

struct PriQueue {
    struct PriNode* back;
    struct PriNode* front;
};

struct PriQueue* pqConstruct() {
    struct PriQueue* priQueue = (struct PriQueue*)malloc(sizeof(struct PriQueue));
    priQueue->back = priQueue->front = NULL;
    return priQueue;
}

bool pqIsEmpty(struct PriQueue* priQueue) {
    return (priQueue->back == NULL && priQueue->front == NULL);
}

pqValueType pqFront(struct PriQueue* priQueue) {
    if (pqIsEmpty(priQueue)) 
        return NULL;   // Assuming all values are non-negatives
    return priQueue->front->value;
}

void pqEnqueue(struct PriQueue* priQueue, pqValueType value, int priority) {
    struct PriNode* newNode = (struct PriNode*)malloc(sizeof(struct PriNode));
    newNode->value = value;
    newNode->priority = priority;
    newNode->next = NULL;

    if (pqIsEmpty(priQueue)) {
        priQueue->front = priQueue->back = newNode;
        return;
    }

    struct PriNode* trav = priQueue->front;
    struct PriNode* prevTrav = NULL;

    while (trav && newNode->priority >= trav->priority) {
        prevTrav = trav;
        trav = trav->next;
    }
    newNode->next = trav;
    if (prevTrav)   // if the newNode will be placed inside the queue or at the back
        prevTrav->next = newNode;
    else            // if the newNode will be placed at the front
        priQueue->front = newNode;

    if (!trav)      // if the newNode will be placed at the back 
        priQueue->back = newNode;

    newNode = NULL;
}

pqValueType pqDequeue(struct PriQueue* priQueue) {
    if (pqIsEmpty(priQueue))
        return NULL;
    struct PriNode* temp = priQueue->front;
    priQueue->front = priQueue->front->next;
    pqValueType value = temp->value; 
    free(temp);
    if (priQueue->front == NULL)
        priQueue->back = NULL;
    return value; 
}

void pqDestruct(struct PriQueue* priQueue) {
    while (!pqIsEmpty(priQueue)) {
        pqValueType dequeued = pqDequeue(priQueue);
        free(dequeued);
    }
    priQueue = NULL;
}

// void pqPrint(struct PriQueue* priQueue) {
//     struct PriNode* trav = priQueue->front;
//     while (trav) {
//         printf("%d ", trav->value);
//         trav = trav->next;
//     }
// }

#endif