#ifndef QUEUE

#define QUEUE
#include <stdbool.h>

typedef struct
{
    int capacity;
    int size;
    int front;
    int rear;
    int* elements;
}Queue;

Queue* InitializeQueue(int maxElements);
void Pop(Queue* Q);
bool Empty(Queue* Q);
int Top(Queue* Q);
void Push(Queue* Q, int element);
void DestroyQueue(Queue* Q);
void FillMissionsPriorityQueue(int LinesNumber, Queue* Q, char* PriorityFilePath);

#endif