#ifndef THREADS_MAIN_FUNCTION
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Lock.h"
#include "Queue.h"
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define THREADS_MAIN_FUNCTION
#define TIMEOUT_IN_MILLISECONDS   15000
#define LOCK
typedef struct MyData {
    int ThreadNumber;
    char* MissionFilePath;
    Lock FileLock;
    Queue MissionsPriorityQueue;
    HANDLE MutexForAccessingQueue;
    HANDLE Semaphore;
} MYDATA, * PMYDATA;


#endif

