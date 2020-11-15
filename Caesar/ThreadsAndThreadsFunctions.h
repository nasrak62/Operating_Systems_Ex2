#ifndef THREADS_AND_THREADS_FUNCTIONS
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id);
#define THREADS_AND_THREADS_FUNCTIONS
void Create_Thread_And_Job(const int ThreadNumber, const int StartingRow, const int EndingRow, FILE* InPutFile, FILE* OutPutFile, HANDLE* p_thread_handles, DWORD* p_thread_ids);
char* Return_Output_Path_From_Input_Path(char*  InputFilePath, char* WhatActionShouldWeTake);
int* Split_The_File_For_Each_Thread_Return_Int_Array_With_Starting_And_Ending_Row_Indexs(char* InputFilePath, int NumberOfActiveThreadsForTheProgram);
#endif 

