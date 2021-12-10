/*
    Notice: The front of the list is the head of the linked list
    and the back of the list is the tail of the linked list.

      front              back
       |                  |
    --------------------------
    |   |->|   |->|   |->|   |
    -------------------------
*/

#ifndef LIST
#define LIST

#include "../headers.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct Process* listValueType; // modifiable according to the value type you want

struct Node {
    listValueType value;
    struct Node* next;
};

struct List {
    struct Node* back;
    struct Node* front;
    struct Node* trav;
};

struct List* listConstruct() {
    struct List* list = (struct List*)malloc(sizeof(struct List));
    list->back = list->front = list->trav = NULL;
    return list;
}

bool listIsEmpty(struct List* list) {
    return (list->back == NULL && list->front == NULL);
}

listValueType listFront(struct List* list) {
    if (listIsEmpty(list)) 
        return NULL;   // Assuming all values are non-negatives
    return list->front->value;
}

void listPushBack(struct List* list, listValueType value) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->value = value;
    newNode->next = NULL;

    if (listIsEmpty(list))
        list->front = list->back = list->trav = newNode;
    else {
        list->back->next = newNode;
        list->back = newNode;
    }
    newNode = NULL;
}

listValueType listPopFront(struct List* list) {
    if (listIsEmpty(list))
        return NULL;
    struct Node* temp = list->front;
    if (list->trav == list->front)
        list->trav = list->trav->next;
    list->front = list->front->next;
    listValueType value = temp->value;
    free(temp);
    if (list->front == NULL)
        list->back = NULL;
    return value; 
    
}

listValueType listTravValue(struct List* list) {
    if (listIsEmpty(list) || list->trav == NULL) return NULL;
    return list->trav->value;
}

// Circular Advance
listValueType listAdvanceTrav(struct List* list) {
    if (listIsEmpty(list) || list->trav == NULL) return NULL;
    if (list->trav->next == NULL)
        list->trav = list->front;
    else
        list->trav = list->trav->next;
    return list->trav->value;
} 

// Delete the node that the "trav" points to and return its value
listValueType listDeleteTrav(struct List* list) {
    if (listIsEmpty(list) || list->trav == NULL) return NULL;
    if (list->trav == list->front) return listPopFront(list);

    struct Node* prev = list->front;
    while (prev->next != list->trav)
        prev = prev->next;
    prev->next = list->trav->next;
    listValueType value = list->trav->value;
    free(list->trav);

    list->trav = prev; //! trav now points to the previous of the delted node (CAN BE CHANGED)
    listAdvanceTrav(list); //! Now itspoints to the next to the deleted node
    if (list->front->next == NULL)
        list->back = list->front;

    return value;
}

void listDestruct(struct List* list) {
    while (!listIsEmpty(list))
        listPopFront(list);
    list = NULL;
}

// void qPrint(struct List* list) {
//     struct Node* trav = list->front;
//     while (trav) {
//         printf("%d ", trav->value);
//         trav = trav->next;
//     }
// }

#endif