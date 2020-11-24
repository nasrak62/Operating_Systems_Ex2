#include "ThreadsAndThreadsFunctions.h"
BOOL FileExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void  Decipher(PMYDATA ThreadpointerData, HANDLE HandleFileForReading,HANDLE HandleFileForWriting) {
	char* line = (char*)malloc((ThreadpointerData->EndingByte - ThreadpointerData->StartingByte) * sizeof(char));
	int LineSize = ((ThreadpointerData->EndingByte - ThreadpointerData->StartingByte) * sizeof(char));
	int i, j;
	char  ch;
	OVERLAPPED oRead = { 0 };
	oRead.Offset = ThreadpointerData->StartingByte;
	DWORD dwBytesRead, dwBytesWritten, dwPos;

	ReadFile(HandleFileForReading, line, LineSize, &dwBytesRead, &oRead);
	if (dwBytesRead > 0 && dwBytesRead <= LineSize)
	{
		line[dwBytesRead] = '\0'; // NULL character
	}

	for (i = 0; i < dwBytesRead; i++) {
		if (line[i] == '\0' || i > LineSize) {
			break;
		}
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
	WriteFile(HandleFileForWriting, line, dwBytesRead, &dwBytesWritten, &oRead);
	//free(line);
}

void  Encrypt(PMYDATA ThreadpointerData, HANDLE HandleFileForReading, HANDLE HandleFileForWriting) {
	char* line = (char*)malloc((ThreadpointerData->EndingByte - ThreadpointerData->StartingByte) * sizeof(char));
	int LineSize = ((ThreadpointerData->EndingByte - ThreadpointerData->StartingByte) * sizeof(char));
	int i, j;
	char  ch;
	OVERLAPPED oRead = { 0 };
	oRead.Offset = ThreadpointerData->StartingByte;
	DWORD dwBytesRead, dwBytesWritten, dwPos;

	ReadFile(HandleFileForReading, line, LineSize, &dwBytesRead, &oRead);
	if (dwBytesRead > 0 && dwBytesRead <= LineSize)
	{
		line[dwBytesRead] = '\0'; // NULL character
	}

	for (i = 0; i < dwBytesRead; i++) {
		if (line[i] == '\0' || i > LineSize) {
			break;
		}
		ch = line[i];
		if (isdigit(ch) != 0) {
			ch = '0' + (ch - '0' + ThreadpointerData->Key) % 10;
			line[i] = ch;
		}
		else {
			if (ch >= 'a' && ch <= 'z') {
				ch = ch + ThreadpointerData->Key;
				if (ch > 'z') {
					ch = ch - 'z' + 'a' - 1;
				}

				line[i] = ch;
			}
			else if (ch >= 'A' && ch <= 'Z') {
				ch = ch + ThreadpointerData->Key;

				if (ch > 'Z') {
					ch = ch - 'Z' + 'A' - 1;
				}
				line[i] = ch;
			}
		}

	}
	WriteFile(HandleFileForWriting, line, dwBytesRead, &dwBytesWritten, &oRead);
	//free(line);
}

DWORD WINAPI Decipher_Or_Encrypt(LPVOID lpParam)
{
	PMYDATA ThreadpointerData;
	ThreadpointerData = (PMYDATA)lpParam;
	if (ThreadpointerData->EndingByte>0) {
		DWORD wait_res_mutex;
		DWORD wait_res_semaphore;
		BOOL release_res;
		wait_res_semaphore = WaitForSingleObject(ThreadpointerData->Semaphore, TIMEOUT_IN_MILLISECONDS);
		wait_res_mutex = WaitForSingleObject(ThreadpointerData->Mutex, TIMEOUT_IN_MILLISECONDS);
		if (wait_res_mutex == WAIT_OBJECT_0 && wait_res_semaphore == WAIT_OBJECT_0) {

			HANDLE HandleFileForReading = CreateFile(ThreadpointerData->InputFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			HANDLE HandleFileForWriting;

			if (FileExists(ThreadpointerData->OutputFilePath))
			{
				HandleFileForWriting = CreateFile(ThreadpointerData->OutputFilePath, GENERIC_WRITE,
					FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			else {
				HandleFileForWriting = CreateFile(ThreadpointerData->OutputFilePath, GENERIC_WRITE,
					FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
			}
			if (HandleFileForReading == INVALID_HANDLE_VALUE)
			{
				printf("Error Reading\n");

			}
			if (HandleFileForWriting == INVALID_HANDLE_VALUE)
			{
				printf("Error Writing\n");

			}
			release_res = ReleaseMutex(ThreadpointerData->Mutex);
			if (release_res == FALSE) {
				printf("Cant Relese Mutex\n");
			}
			if (HandleFileForReading != INVALID_HANDLE_VALUE && HandleFileForWriting != INVALID_HANDLE_VALUE) {
				if (strcmp(ThreadpointerData->WhatActionShouldWeTake, "-d") == 0) {
					Decipher(ThreadpointerData, HandleFileForReading, HandleFileForWriting);
				}
				else if (strcmp(ThreadpointerData->WhatActionShouldWeTake, "-e") == 0) {
					Encrypt(ThreadpointerData, HandleFileForReading, HandleFileForWriting);
				}
			}

			CloseHandle(HandleFileForReading);
			CloseHandle(HandleFileForWriting);


		}
	}
	
}
void CheckThreadsStatus(PMYDATA THreadDataArguments,HANDLE** p_thread_handles, int NumberOfActiveThreadsForTheProgram) {
	DWORD wait_code;
	BOOL ret_val;
	size_t i;
	
	
	wait_code = WaitForMultipleObjects(NumberOfActiveThreadsForTheProgram,(*p_thread_handles),TRUE, TIMEOUT_IN_MILLISECONDS);



	switch (wait_code)
	{
	case WAIT_OBJECT_0 + 0:
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
				if (FileExists(EndOfPath))
				{
					DeleteFileA(EndOfPath);
				}
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
			//free(EndOfPath);
			if (FileExists(OutputPath))
			{
				DeleteFileA(OutputPath);
			}
			return OutputPath;
		}

		


	}
	
	
}
int* SplitTheWorkUniformlyAndSetLeftOverThreadWithZeroLines(int** ArrayWithLineStartingAndEndingIndexs, int NumberOfActiveThreadsForTheProgram,  int* LineEndingBytes, int linesCounter) {
	int CurrentThreadNumber = 0;
	int StartingLine = 0;
	int FinishLine = 0;
	for (int i = 0; i < NumberOfActiveThreadsForTheProgram; i++) {
		if (i< linesCounter) {
			FinishLine = LineEndingBytes[i];
			(*ArrayWithLineStartingAndEndingIndexs)[CurrentThreadNumber] = StartingLine;
			(*ArrayWithLineStartingAndEndingIndexs)[CurrentThreadNumber + 1] = FinishLine;
			StartingLine = LineEndingBytes[i];
		}
		else {
			(*ArrayWithLineStartingAndEndingIndexs)[CurrentThreadNumber] = 0;
			(*ArrayWithLineStartingAndEndingIndexs)[CurrentThreadNumber + 1] = 0;
		}
		
		CurrentThreadNumber += 2;	
	}
}

int* SplitTheWorkEvenlyForAllThreads(int** ArrayWithLineStartingAndEndingIndexs, int NumberOfActiveThreadsForTheProgram, int* LineEndingBytes, int linesCounter) {
	int CurrentThreadNumber = 0;
	int StartingLine = 0;
	int FinishLine = 0;
	int LinesForEachThread = linesCounter / NumberOfActiveThreadsForTheProgram;
	int FinishLineIndex= LinesForEachThread - 1;
	FinishLine = LineEndingBytes[FinishLineIndex];

	for (int i = 0; i < NumberOfActiveThreadsForTheProgram; i++) {
		(*ArrayWithLineStartingAndEndingIndexs)[CurrentThreadNumber] = StartingLine;
		(*ArrayWithLineStartingAndEndingIndexs)[CurrentThreadNumber + 1] = FinishLine;
		CurrentThreadNumber += 2;
		StartingLine = FinishLine;
		FinishLineIndex= min(FinishLineIndex + LinesForEachThread, linesCounter-1);
		FinishLine = LineEndingBytes[FinishLineIndex];

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
	int TotalBytesInFile = 0;
	int* ArrayWithLineStartingAndEndingIndexs = (int*)malloc(NumberOfActiveThreadsForTheProgram * 2 * sizeof(int));
	InputFileOpeningError = (fopen_s(&InPutFile, InputFilePath, "r"));
	if (InputFileOpeningError == 0) {
		printf("Input File Opened\n");
		for (Temp = getc(InPutFile); Temp != EOF; Temp = getc(InPutFile)) {
			if (Temp == '\n') {
				linesCounter++;
			}		
		}
		linesCounter++;
		int TotalBytesInFile = ftell(InPutFile);
		if (fseek(InPutFile, 0L, SEEK_SET) != 0) {
			printf("File Error While Rewinding\n");
		}
		int* LineEndingBytes = (int*)malloc(linesCounter * sizeof(int));
		int indexCountr = 0;
		for (Temp = getc(InPutFile); Temp != EOF; Temp = getc(InPutFile)) {
			if (Temp == '\n' ) {
				LineEndingBytes[indexCountr]= ftell(InPutFile);
				indexCountr++;
			}
		}
		LineEndingBytes[indexCountr] = ftell(InPutFile);
		indexCountr++;
		
		if (NumberOfActiveThreadsForTheProgram>= linesCounter) {
			SplitTheWorkUniformlyAndSetLeftOverThreadWithZeroLines(&ArrayWithLineStartingAndEndingIndexs, NumberOfActiveThreadsForTheProgram,  LineEndingBytes, linesCounter);
		}
		else{
			SplitTheWorkEvenlyForAllThreads(&ArrayWithLineStartingAndEndingIndexs, NumberOfActiveThreadsForTheProgram,  LineEndingBytes, linesCounter);
		}
		

		fclose(InPutFile);
		return ArrayWithLineStartingAndEndingIndexs;
	}
	else {
		printf("Couldn't Open The Files\n");
		int CurrentThreadNumber = 0;
		for (int i = 0; i < NumberOfActiveThreadsForTheProgram; i++) {
			ArrayWithLineStartingAndEndingIndexs[CurrentThreadNumber] = 0;
			ArrayWithLineStartingAndEndingIndexs[CurrentThreadNumber + 1] = 0;
			CurrentThreadNumber += 2;
		}
	}




}