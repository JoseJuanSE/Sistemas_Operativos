/*************************************************
 * circular_dynamic_queue
 * @author: Carlos Huerta García
 * @description: Unsigned har queue implementation
 * ***********************************************/
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

typedef unsigned char QueueEntry;

typedef struct QueueElement{
    QueueEntry entry;
    struct QueueElement * next;
} QueueElement;

typedef struct Queue{
    QueueElement * front, * rear;
} Queue;

Queue * createQueue(){
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    if(queue == NULL)
        exit(1);
    queue->front = NULL;
    queue->rear = NULL;
    return queue;
}

bool queueIsEmpty(Queue * queue) {
    return !queue->front || !queue;
}

QueueElement * createQueueElement(QueueEntry item) {
    QueueElement* element = (QueueElement*)malloc(sizeof(QueueElement));
    if (element == NULL) 
        exit(1);
    element->entry = item;
    element->next = NULL;
    return element;
}

void enQueue(QueueEntry item, Queue * queue) {
    QueueElement* element = createQueueElement(item);
    if(queueIsEmpty(queue))
        queue->front = element;
    else
        queue->rear->next = element;
    queue->rear = element;
    queue->rear->next = queue->front;
}

QueueEntry front(Queue * queue) {
    if(queueIsEmpty(queue))
        return CHAR_MIN;
    return queue->front->entry;
}

QueueEntry rear(Queue * queue) {
    if(queueIsEmpty(queue))
        return CHAR_MIN;
    return queue->rear->entry;
}

QueueEntry deQueue(Queue * queue) {
    if(queueIsEmpty(queue))
        return CHAR_MIN;
    QueueEntry dequeued;
    if(queue->front == queue->rear){
        dequeued = queue->front->entry;
        free(queue->front);
        queue->front = NULL;
        queue->rear = NULL;
    } else {
        QueueElement* temp = queue->front;
        dequeued = temp->entry;
        queue->front = queue->front->next;
        queue->rear->next = queue->front;
        free(temp);
    }
    return dequeued;
}

void printQueue(Queue * queue) {
    if(queueIsEmpty(queue)){
        puts("[ ]");
        return;
    }
    QueueElement* element = queue->front;
    while (element->next != queue->front) {
        printf("-[%c]-", element->entry);
        element = element->next;
    }
    printf("-[%c]-", element->entry);
}

void clearQueue(Queue * queue) {
    if(!queueIsEmpty(queue)){
        QueueElement* element;
        while ((element = queue->front) != queue->front) {
            queue->front = queue->front->next;
            free(element);
        }
        queue->front = NULL;
        queue->rear = NULL;
    }
}
