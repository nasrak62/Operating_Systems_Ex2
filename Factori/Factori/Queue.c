#include <stdio.h> 
#include <stdlib.h>
#include <stdbool.h>

typedef struct Queue
{
    int capacity;
    int size;
    int front;
    int rear;
    int* elements;
}Queue;
Queue* InitializeQueue(int maxElements)
{
    Queue* Q;
    Q = (Queue*)malloc(sizeof(Queue));
    Q->elements = (int*)malloc(sizeof(int) * maxElements);
    Q->size = 0;
    Q->capacity = maxElements;
    Q->front = 0;
    Q->rear = -1;
    return Q;
}
void Pop(Queue* Q)
{
    if (Q->size == 0)
    {
        printf("Queue is Empty\n");
        return;
    }
    else
    {
        Q->size--;
        Q->front++;
        if (Q->front == Q->capacity)
        {
            Q->front = 0;
        }
    }
    return;
}
bool Empty(Queue* Q)
{
    if (Q->size == 0)
    {
        return true;
    }
    return false;
}
int Top(Queue* Q)
{
    if (Q->size == 0)
    {
        printf("Queue is Empty\n");
        exit(0);
    }
    return Q->elements[Q->front];
}
void Push(Queue* Q, int element)
{
    if (Q->size == Q->capacity)
    {
        printf("Queue is Full\n");
    }
    else
    {
        Q->size++;
        Q->rear = Q->rear + 1;
        if (Q->rear == Q->capacity)
        {
            Q->rear = 0;
        }
        Q->elements[Q->rear] = element;
    }
    return;
}
void DestroyQueue(Queue* Q)
{
    if (Q->size == 0)
    {
        printf("Queue is already Empty\n");
        return;
    }
    while (1) 
    {
        Pop(Q);
        if (Q->size == 0)
            break;
    }
    Q = NULL;
}
