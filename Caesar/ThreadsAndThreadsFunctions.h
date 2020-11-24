#ifndef THREADS_AND_THREADS_FUNCTIONS
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define THREADS_AND_THREADS_FUNCTIONS
#define TIMEOUT_IN_MILLISECONDS   15000
typedef struct MyData {
    int ThreadNumber;
    int StartingByte;
    int EndingByte;
    char* InputFilePath;
    char* OutputFilePath;
    FILE* InPutFile;
    FILE* OutPutFile;
    int Key;
    int FileLastLine;
    char* WhatActionShouldWeTake;
    HANDLE Mutex;
    HANDLE Semaphore;
} MYDATA, *PMYDATA;


static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
    LPDWORD p_thread_id, MYDATA DataArray);
char* Return_Output_Path_From_Input_Path(char*  InputFilePath, char* WhatActionShouldWeTake);
int* Split_The_File_For_Each_Thread_Return_Int_Array_With_Starting_And_Ending_Row_Indexs(char* InputFilePath, int NumberOfActiveThreadsForTheProgram);
void CheckThreadsStatus(PMYDATA THreadDataArguments, HANDLE** p_thread_handles, int NumberOfActiveThreadsForTheProgram);
DWORD WINAPI Decipher_Or_Encrypt(LPVOID lpParam);
#endif 

