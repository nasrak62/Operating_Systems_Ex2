#ifndef THREADS_MAIN_FUNCTION
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Lock.h"
#include "Queue.h"
#define THREADS_MAIN_FUNCTION
typedef struct MyData {
    int ThreadNumber;
    char* MissionFilePath;
    Lock* FileLock;
    Queue MissionsPriorityQueue;
    HANDLE MutexForAccessingQueue;
    HANDLE Semaphore;
} MYDATA, * PMYDATA;


#endif

