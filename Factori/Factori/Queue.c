#include <stdio.h> 
#include <stdlib.h>
#include "Queue.h"

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

int max_line_size(char* PriorityFilePath)
{
    FILE* TasksPriorities = NULL;
    errno_t input_error;
    input_error = fopen_s(&TasksPriorities, PriorityFilePath, "r");
    char ch;
    int max = 0, i = 0;
    do
        if ((ch = fgetc(TasksPriorities)) != EOF && ch != '\n') i++;
        else {
            if (i > max) max = i;
            i = 0;
        }
    while (ch != EOF);
    fclose(TasksPriorities);
    return max + 4; //for \n and \r
}
void FillMissionsPriorityQueue(int LinesNumber, Queue* Q, char* PriorityFilePath)
{
    int i;
    int max_size = max_line_size(PriorityFilePath);
    FILE* TasksPriorities = NULL;
    errno_t input_error;
    input_error = fopen_s(&TasksPriorities, PriorityFilePath, "r");
    if (input_error == 0)
    {
        char* priority = (char*)malloc((max_size * sizeof(char)));
        if (NULL != priority)
        {
            for (i = 1; i < LinesNumber + 1; i++)
            {
                fgets(priority, max_size, TasksPriorities);
                Push(Q, atoi(priority));
            }
        }
        else
            printf("Memory Allocation failure!");
    }
    else
    {
        printf("Couldn't open TasksPriorities.txt");
    }
    fclose(TasksPriorities);
}