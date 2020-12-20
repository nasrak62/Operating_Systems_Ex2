#include "ThreadsMainFunction.h"
#include "Prime.h"

void ReadLineFromFileFunction(PMYDATA ThreadpointerData, int StartingByte, int FinishByte, HANDLE HandleFile, DWORD* dwBytesRead, char** Line, int LineSize) {
	OVERLAPPED OverLappedRead = { 0 };
	OverLappedRead.Offset = StartingByte;
	DWORD WaitReadLock;
	WaitReadLock = read_lock(ThreadpointerData->FileLock);
	if (WaitReadLock == WAIT_TIMEOUT) {
		printf("error should terminate stuff now\n");
	}
	else {
		ReadFile(HandleFile, *Line, LineSize, dwBytesRead, &OverLappedRead);
		if (*dwBytesRead > 0 && *dwBytesRead <= LineSize)
		{
			Line[*dwBytesRead] = '\0'; // NULL character
		}
		read_release(ThreadpointerData->FileLock);
	}

}


int ProcessLineToInt(char* Line, int LineSize) {
	int i = 0;
	int ExtractNumber = 0;

	while (isdigit(Line[i])) {
		i++;
	}
	Line[i] = '\0';
	ExtractNumber = atoi(Line);
	return ExtractNumber;

}

char* ConvertArrayToString(int* ThePrimeNumbersArray, int NumberToPrime,  int StartingByte, int FinishByte) {
	int SizeOfArray;
	int SizeOfLine;
	if (NumberToPrime == ZERO_HAS_NO_PRIME || NumberToPrime == ONE_HAS_NO_PRIME) {
		SizeOfLine = (FinishByte - StartingByte) + sizeof("The prime factors of %d are: \r\n") + 1;
	}
	else {
		SizeOfArray = how_many_prime_factors(NumberToPrime);
		SizeOfLine = (FinishByte - StartingByte) + sizeof("The prime factors of %d are: \r\n") + SizeOfArray * sizeof(ThePrimeNumbersArray);
	}
	char* PrimeNumberWriteFormat= (char*)malloc(SizeOfLine * sizeof(char));
	char* TheNumbers = (char*)malloc(SizeOfLine * sizeof(char));
	if (NULL!= TheNumbers || NULL != PrimeNumberWriteFormat) {

		
		if (NumberToPrime == ZERO_HAS_NO_PRIME || NumberToPrime== ONE_HAS_NO_PRIME) {
			if (sprintf_s(PrimeNumberWriteFormat, SizeOfLine, "The prime factors of %d are: \r\n", NumberToPrime) == -1) {
				printf("sprintf_s failed\n");
			}
		}
		else {
			if (sprintf_s(PrimeNumberWriteFormat, SizeOfLine, "The prime factors of %d are: ", NumberToPrime) == -1) {
				printf("sprintf_s failed\n");
			}
			for (int i = 0; i < SizeOfArray; i++) {
				if (i < SizeOfArray - 1) {
					if (sprintf_s(TheNumbers, SizeOfLine, "%d, ", ThePrimeNumbersArray[i]) == -1) {
						printf("sprintf_s failed\n");
					}

				}
				else {
					if (sprintf_s(TheNumbers, SizeOfLine, "%d\r\n", ThePrimeNumbersArray[i]) == -1) {
						printf("sprintf_s failed\n");
					}
				}
				if (strcat_s(PrimeNumberWriteFormat, SizeOfLine, TheNumbers) != 0) {
					printf("strcat failed\n");
				}


			}
		}
		

	}
	
	return  PrimeNumberWriteFormat;
}

void WriteLineToFileFunction(PMYDATA ThreadpointerData, HANDLE HandleFile, DWORD dwBytesRead, char* Line) {
	
	write_lock(ThreadpointerData->FileLock);
	if (HandleFile == 0)
	{
		printf("Error in handle\n");

	}
	SetFilePointer(HandleFile, NULL,NULL,FILE_END);
	if ( HandleFile == 0 )
	{
		CloseHandle(HandleFile);
		HANDLE HandleFile = CreateFile(ThreadpointerData->MissionFilePath, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		printf("Error in Reading Or Malloc\n");

	}
	DWORD dwBytesWritten;
	BOOL bfile=WriteFile(HandleFile, Line, strlen(Line), &dwBytesWritten,NULL );
	if (bfile==false) {
		if ( HandleFile == 0 )
		{
			printf("HandleFile == 0\n");

		}
		WriteFile(HandleFile, Line, strlen(Line), &dwBytesWritten, NULL);
	}
	write_release(ThreadpointerData->FileLock);

}



void WorkWithTheFile(PMYDATA ThreadpointerData, int StartingByte, int FinishByte) {

	DWORD WaitReadLock;
	HANDLE HandleFile = CreateFile(ThreadpointerData->MissionFilePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	char* Line = (char*)malloc((FinishByte - StartingByte) * sizeof(char));
	int LineSize = ((FinishByte - StartingByte) * sizeof(char));
	DWORD dwBytesRead;
	int NumberToPrime=0;
	int* ThePrimeNumbersArray;
	char* LineToWrite;
	if (HandleFile == INVALID_HANDLE_VALUE  || Line==NULL)
	{
		printf("Error in Reading Or Malloc\n");

	}
	else {

		ReadLineFromFileFunction(ThreadpointerData, StartingByte, FinishByte, HandleFile,
			&dwBytesRead, &Line, LineSize);
		if (HandleFile == 0)
		{
			CloseHandle(HandleFile);
			HandleFile = CreateFile(ThreadpointerData->MissionFilePath, GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		}
		NumberToPrime = ProcessLineToInt(Line, LineSize);
		ThePrimeNumbersArray = primeFactors(NumberToPrime);
		LineToWrite=ConvertArrayToString(ThePrimeNumbersArray, NumberToPrime, StartingByte, FinishByte);
		WriteLineToFileFunction(ThreadpointerData, HandleFile, dwBytesRead, LineToWrite);
		
		free(LineToWrite);
		free(Line);
		CloseHandle(HandleFile);
	}


	
		

}

DWORD WINAPI SplitTheWorkForEachThread(LPVOID lpParam)
{
	PMYDATA ThreadpointerData;
	ThreadpointerData = (PMYDATA)lpParam;
	DWORD wait_res_semaphore,wait_res_MutexForAccessingQueue;
	BOOL release_res_MutexForAccessingQueue;
	int StartingByte=0;
	int FinishByte=0;
	int WhatLineToRead;
	wait_res_semaphore = WaitForSingleObject(ThreadpointerData->Semaphore, TIMEOUT_IN_MILLISECONDS);

	
	while (!Empty(ThreadpointerData->MissionsPriorityQueue)) {
		wait_res_MutexForAccessingQueue = WaitForSingleObject(ThreadpointerData->MutexForAccessingQueue, TIMEOUT_IN_MILLISECONDS);
		if (Empty(ThreadpointerData->MissionsPriorityQueue)) {
			release_res_MutexForAccessingQueue = ReleaseMutex(ThreadpointerData->MutexForAccessingQueue);
			break;
		}
		else {
			StartingByte = Top(ThreadpointerData->MissionsPriorityQueue);
			Pop(ThreadpointerData->MissionsPriorityQueue);
			release_res_MutexForAccessingQueue = ReleaseMutex(ThreadpointerData->MutexForAccessingQueue);
	
			for (int i = 0; i < ThreadpointerData->NumberOfMissions; i++) {
				if (StartingByte==0) {
					FinishByte = (ThreadpointerData->BytesForEachLine)[0];
					break;
				}
				else if ((ThreadpointerData->BytesForEachLine)[i] == StartingByte) {
					FinishByte = (ThreadpointerData->BytesForEachLine)[i + 1];
					break;
				}
					
			}
			if (FinishByte != 0) {
				WorkWithTheFile(ThreadpointerData, StartingByte, FinishByte);
			}
			else {
				printf("FinishByte is 0\n");
			}
			
		}
		
		
	}
	
	


}

void CheckThreadsStatus(PMYDATA THreadDataArguments, HANDLE** p_thread_handles, int NumberOfActiveThreadsForTheProgram) {
	DWORD wait_code;
	BOOL ret_val;
	size_t i;


	wait_code = WaitForMultipleObjects(NumberOfActiveThreadsForTheProgram, (*p_thread_handles), TRUE, TIMEOUT_IN_MILLISECONDS);



	switch (wait_code)
	{
	case WAIT_OBJECT_0 + 0:
		for (i = 0; i < NumberOfActiveThreadsForTheProgram; i++)
		{
			DestroyLock(THreadDataArguments[i].FileLock);
			DestroyQueue(THreadDataArguments[i].MissionsPriorityQueue);
		}
		printf("Finished.\n");
		break;
	case WAIT_TIMEOUT:
		printf("Wait timed out.\n");
		break;

		// Return value is invalid.
	default:
		printf("Error when waiting");
		return ERROR_CODE;
	}

	// Terminate the other thread
	// Normally, we would avoid terminating a thread so brutally,
	// because it might be in the middle of an operation that should not
	// be interrupted (like writing a file).
	// There are gentler ways of terminating a thread.
	/*ret_val = TerminateThread((*p_thread_handles)[ThreadNumber], BRUTAL_TERMINATION_CODE);
	if (false == ret_val)
	{
		printf("Error when terminating\n");
		return ERROR_CODE;
	}*/

	// Close thread handles
	for (i = 0; i < NumberOfActiveThreadsForTheProgram; i++)
	{
		ret_val = CloseHandle((*p_thread_handles)[i]);
		if (false == ret_val)
		{
			printf("Error when closing\n");
			return ERROR_CODE;
		}
	}


}


int* MapRowsToBytesInFile(char* InputFilePath, int NumberOfLinesInFile) {
	FILE* InPutFile = NULL;
	errno_t InputFileOpeningError = NULL;
	int Temp = 0;
	int linesCounter = 0;
	int* LineEndingBytes = (int*)malloc(NumberOfLinesInFile * sizeof(int));
	InputFileOpeningError = (fopen_s(&InPutFile, InputFilePath, "r"));
	if (InputFileOpeningError == 0) {
		int indexCountr = 0;
		for (Temp = getc(InPutFile); Temp != EOF; Temp = getc(InPutFile)) {
			if (Temp == '\n') {
				LineEndingBytes[indexCountr] = ftell(InPutFile);
				indexCountr++;
			}
		}
		

		fclose(InPutFile);
	}
	else {
		printf("Couldn't Open The Files\n");
		int CurrentThreadNumber = 0;
		for (int i = 0; i < NumberOfLinesInFile; i++) {
			LineEndingBytes[i] = 0;
		}
		
	}

	return LineEndingBytes;
}