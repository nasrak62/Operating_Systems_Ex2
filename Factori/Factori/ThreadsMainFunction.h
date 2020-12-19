#ifndef THREADS_MAIN_FUNCTION
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Lock.h"
#include "Queue.h"
#define THREADS_MAIN_FUNCTION
#define RANDOM_RANGE_START_FROM_ONE 1
#define ONE_HAS_NO_PRIME 1
#define ZERO_HAS_NO_PRIME 0
typedef struct MyData {
    int ThreadNumber;
    char* MissionFilePath;
    Lock* FileLock;
    Queue* MissionsPriorityQueue;
    HANDLE MutexForAccessingQueue;
    HANDLE Semaphore;
    int* BytesForEachLine;
    int NumberOfMissions;
    int MaxNumberOfBytes;
} MYDATA, * PMYDATA;

DWORD WINAPI SplitTheWorkForEachThread(LPVOID lpParam);
void CheckThreadsStatus(PMYDATA THreadDataArguments, HANDLE** p_thread_handles, int NumberOfActiveThreadsForTheProgram);
int* MapRowsToBytesInFile(char* InputFilePath, int NumberOfLinesInFile);

#endif

