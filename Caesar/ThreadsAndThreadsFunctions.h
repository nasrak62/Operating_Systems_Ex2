#ifndef THREADS_AND_THREADS_FUNCTIONS
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define THREADS_AND_THREADS_FUNCTIONS
typedef struct MyData {
    int ThreadNumber;
    int StartingRow;
    int EndingRow;
    FILE* InPutFile;
    FILE* OutPutFile;
    int Key;
    int FileLastLine;
} MYDATA, *PMYDATA;


static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
    LPDWORD p_thread_id, MYDATA DataArray);
void Create_Thread_And_Job(MYDATA THreadDataArguments, HANDLE* p_thread_handles, DWORD* p_thread_ids);
char* Return_Output_Path_From_Input_Path(char*  InputFilePath, char* WhatActionShouldWeTake);
int* Split_The_File_For_Each_Thread_Return_Int_Array_With_Starting_And_Ending_Row_Indexs(char* InputFilePath, int NumberOfActiveThreadsForTheProgram);
#endif 

