#ifndef PRI_QUEUE
#define PRI_QUEUE

#include "../headers.h"
#include <stdio.h>
#include <stdlib.h>

typedef int valueType; // modifiable according to the value type you want

struct PriNode {
    int priority;
    valueType value;
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

valueType pqFront(struct PriQueue* priQueue) {
    if (pqIsEmpty(priQueue)) 
        return -1;   // Assuming all values are non-negatives
    return priQueue->front->value;
}

void pqEnqueue(struct PriQueue* priQueue, valueType value, int priority) {
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

bool pqDequeue(struct PriQueue* priQueue) {
    if (pqIsEmpty(priQueue))
        return false;
    struct PriNode* temp = priQueue->front;
    priQueue->front = priQueue->front->next;
    free(temp);
    if (priQueue->front == NULL)
        priQueue->back = NULL;
    return true; 
}

void pqDestruct(struct PriQueue* priQueue) {
    while (!pqIsEmpty(priQueue))
        pqDequeue(priQueue);
    priQueue = NULL;
}

void pqPrint(struct PriQueue* priQueue) {
    struct PriNode* trav = priQueue->front;
    while (trav) {
        printf("%d ", trav->value);
        trav = trav->next;
    }
}

#endif