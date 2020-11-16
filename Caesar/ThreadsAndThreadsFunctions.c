#include "ThreadsAndThreadsFunctions.h"
static HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, PMYDATA pDataArray)
{
	HANDLE thread_handle;

	if (NULL == p_start_routine)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(ERROR_CODE);
	}

	if (NULL == p_thread_id)
	{
		printf("Error when creating a thread");
		printf("Received null pointer");
		exit(ERROR_CODE);
	}

	thread_handle = CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		p_start_routine, /*  thread function */
		pDataArray,            /*  argument to thread function */
		0,               /*  use default creation flags */
		p_thread_id);    /*  returns the thread identifier */

	if (NULL == thread_handle)
	{
		printf("Couldn't create thread\n");
		exit(ERROR_CODE);
	}

	return thread_handle;
}

DWORD WINAPI ThreadTempFunction(LPVOID lpParam){
	printf("Thread says hi\n");
	Sleep(1000);
	printf("Thread says hi\n");
}


DWORD WINAPI decipher(LPVOID lpParam)
{
	PMYDATA ThreadpointerData;
	ThreadpointerData = (PMYDATA)lpParam;
	char line[100], ch;
	int i, j;
	
	for (j = 0; j < ThreadpointerData->FileLastLine + 1; j++)
	{
		fgets(line, 100, ThreadpointerData->InPutFile);
		if (j >= ThreadpointerData->StartingRow && j <= ThreadpointerData->EndingRow)
		{
			for (i = 0; line[i] != '\0'; i++) {
				ch = line[i];
				if (isdigit(ch) != 0) {
					ch = '0' + (ch - '0' - ThreadpointerData->Key) % 10;
					line[i] = ch;
				}
				else {

					if (ch >= 'a' && ch <= 'z') {
						ch = ch - ThreadpointerData->Key;

						if (ch < 'a') {
							ch = ch + 'z' - 'a' + 1;
						}

						line[i] = ch;
					}
					else if (ch >= 'A' && ch <= 'Z') {
						ch = ch - ThreadpointerData->Key;

						if (ch < 'A') {
							ch = ch + 'Z' - 'A' + 1;
						}

						line[i] = ch;
					}
				}
			}
			fprintf(ThreadpointerData->OutPutFile, "%s", line);
		}
	}
}


void Create_Thread_And_Job(MYDATA THreadDataArguments, HANDLE  *p_thread_handles, DWORD *p_thread_ids) {
	DWORD wait_code;
	BOOL ret_val;
	size_t i;
	int ThreadNumber = THreadDataArguments.ThreadNumber;
	//p_thread_handles[ThreadNumber] = CreateThreadSimple(ThreadTempFunction , &p_thread_ids[ThreadNumber], pDataArray);
	p_thread_handles[ThreadNumber] = CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		decipher, /*  thread function */
		(LPVOID) &THreadDataArguments,            /*  argument to thread function */
		0,               /*  use default creation flags */
		&p_thread_ids[ThreadNumber]);    /*  returns the thread identifier */




	// Wait for IO thread to receive exit command and terminate
	wait_code = WaitForSingleObject(p_thread_handles[ThreadNumber], INFINITE);
	if (WAIT_OBJECT_0 != wait_code)
	{
		printf("Error when waiting");
		return ERROR_CODE;
	}

	// Terminate the other thread
	// Normally, we would avoid terminating a thread so brutally,
	// because it might be in the middle of an operation that should not
	// be interrupted (like writing a file).
	// There are gentler ways of terminating a thread.
	ret_val = TerminateThread(p_thread_handles[ThreadNumber], BRUTAL_TERMINATION_CODE);
	if (false == ret_val)
	{
		printf("Error when terminating\n");
		return ERROR_CODE;
	}

	// Close thread handles
	/*for (i = 0; i < NUM_THREADS; i++)
	{
		ret_val = CloseHandle(p_thread_handles[0]);
		if (false == ret_val)
		{
			printf("Error when closing\n");
			return ERROR_CODE;
		}
	}
	*/

	ret_val = CloseHandle(p_thread_handles[ThreadNumber]);
	if (false == ret_val)
	{
		printf("Error when closing\n");
		return ERROR_CODE;
	}

}

char* Return_Output_Path_From_Input_Path(char* InputFilePath, char* WhatActionShouldWeTake) {
	char* CopyOfInputFilePath = (char*)malloc((strlen(InputFilePath) + 1) * sizeof(char));
	char* OutputPath = NULL;
	char* EndOfPath = (char*)malloc((sizeof("BadArgumentOutput.txt")) * sizeof(char));
	int CopyError;
	int SizeOfInputfile=strlen(InputFilePath) + 1;
	int SizeOfOutputfile;
	CopyError = strcpy_s(CopyOfInputFilePath, strlen(InputFilePath) + 1, InputFilePath);
	if (EndOfPath == NULL || CopyError != 0) {
		printf("Failed To Enter\n");
	}
	else {

		if (strcmp(WhatActionShouldWeTake, "-e") == 0) {
			OutputPath = (char*)malloc((SizeOfInputfile + strlen("encrypted.txt") + 1) * sizeof(char));
			CopyError=strcpy_s(EndOfPath,strlen(EndOfPath)+1,"encrypted.txt");
			SizeOfOutputfile= ((SizeOfInputfile + strlen("encrypted.txt")+1) * sizeof(char));
		}
		else if (strcmp(WhatActionShouldWeTake, "-d") == 0) {
			OutputPath = (char*)malloc((SizeOfInputfile + sizeof("decrypted.txt")) * sizeof(char));
			CopyError = strcpy_s(EndOfPath, strlen(EndOfPath) + 1, "decrypted.txt");
			SizeOfOutputfile = ((SizeOfInputfile + sizeof("decrypted.txt")) * sizeof(char));
		}
		else {
			printf("Wrong action argument\n");
			OutputPath = (char*)malloc((sizeof(InputFilePath) + sizeof("BadArgumentOutput.txt")) * sizeof(char));
			CopyError = strcpy_s(EndOfPath, sizeof("BadArgumentOutput.txt"), "BadArgumentOutput.txt");
			SizeOfOutputfile = ((SizeOfInputfile + sizeof("BadArgumentOutput.txt")) * sizeof(char));
		}
		
		if (OutputPath == NULL) {
			printf("Malloc Failed\n");
		}
		else if (CopyError!=0) {
			printf("Copy Failed\n");
		}
		else {

			char* nexttoken = NULL;
			char* token = strtok_s(CopyOfInputFilePath, "\\", &nexttoken);
			
			if (*nexttoken == NULL ) {

				return(EndOfPath);
			}
			else {
				strcpy_s(OutputPath, SizeOfOutputfile, token);
				strcat_s(OutputPath, SizeOfOutputfile, "\\");
				token = strtok_s(NULL, "\\", &nexttoken);
				while ( *nexttoken != NULL && token != NULL) {
					strcat_s(OutputPath, SizeOfOutputfile, token);
					strcat_s(OutputPath, SizeOfOutputfile, "\\");
					token = strtok_s(NULL, "\\", &nexttoken);
				}
				strcat_s(OutputPath, SizeOfOutputfile, EndOfPath);
			}
			
		
			return OutputPath;
		}

		


	}
	
	
}

int* Split_The_File_For_Each_Thread_Return_Int_Array_With_Starting_And_Ending_Row_Indexs(char* InputFilePath, int NumberOfActiveThreadsForTheProgram) {
	FILE* InPutFile = NULL;
	errno_t InputFileOpeningError = NULL;
	int Temp = 0;
	int linesCounter = 0;
	int StartingLine = 0;
	int FinishLine = 0;
	int CurrentThreadNumber = 0;
	InputFileOpeningError = (fopen_s(&InPutFile, InputFilePath, "r"));
	if (InputFileOpeningError == 0) {
		printf("Input File Opened\n");
		while (Temp !=EOF)
		{
			Temp = fgetc(InPutFile);
			if (Temp == '\n')
			{
				linesCounter++;
			}
		}
		int linesForEachThread = linesCounter / NumberOfActiveThreadsForTheProgram;
		FinishLine = linesForEachThread;
		int* ArrayWithLineStartingAndEndingIndexs = (int*)malloc(NumberOfActiveThreadsForTheProgram * 2 * sizeof(int));
		for (int i = 0; i < NumberOfActiveThreadsForTheProgram; i++) {
			ArrayWithLineStartingAndEndingIndexs[CurrentThreadNumber]=StartingLine;
			ArrayWithLineStartingAndEndingIndexs[CurrentThreadNumber +1] = FinishLine;
			CurrentThreadNumber+=2;
			StartingLine = FinishLine+1;
			FinishLine = min(FinishLine + 1+ linesForEachThread, linesCounter);
		}

		fclose(InPutFile);
		return ArrayWithLineStartingAndEndingIndexs;
	}
	else {
		printf("Couldn't Open The Files\n");
		
	}




}