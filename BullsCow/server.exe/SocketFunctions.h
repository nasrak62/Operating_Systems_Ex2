#ifndef SOCKET_FUNCTIONS
#define SOCKET_FUNCTIONS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define LOCAL_HOST "127.0.0.1"
#define MAX_NUM_OF_ACTIVE_CONNECTIONS 3
#define NUMBER_TO_REUSE_ADDRESS 1
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define TIMEOUT_IN_MILLISECONDS   15000
#define MILLISECONDS_DEFENITION   1000
#define MAXIMUM_NAME_LENGHT 20

#include <stdbool.h>
#include <stdio.h> 
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

typedef struct MyData {
    int ThreadNumber;
    char* MissionFilePath;
    HANDLE MutexForAccessingQueue;
    HANDLE Semaphore;
    SOCKET ServerSocket;
    char ClientName[MAXIMUM_NAME_LENGHT];
    bool InUse;
} MYDATA, * PMYDATA;


#pragma comment(lib, "Ws2_32.lib")

void HandleCommuniction(int PortNumber);
void CheckThreadsStatus(PMYDATA THreadDataArguments, HANDLE** p_thread_handles, int NumberOfActiveThreadsForTheProgram);

#endif