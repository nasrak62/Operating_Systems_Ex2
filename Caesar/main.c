/* File Header: Comments:
* Authors- Saar Zaidfunden -205892334, Jonathan Johansson-308034107
* Project- IOS_EX2 - Encryption and decryption using threads
Description- This main function gathers all the auxiliary functions that were created
in the others source files, and use them for reading an input text, decyper or encrypt as to the user input, 
and printing to output file useing threads for all of the operations.

*/
#include "ThreadsAndThreadsFunctions.h"
int main(int argc, char* argv[]) {
	
	if (argc != 5) {
		printf("Number of arguments is not 5!");
	}
	else {
		//initializing the necessary variables
		char* InputFilePath = argv[1];
		int Key=atoi(argv[2]);
		int NumberOfActiveThreadsForTheProgram= atoi(argv[3]);
		char* WhatActionShouldWeTake = argv[4];
		FILE* OutPutFile = NULL;
		FILE* InPutFile = NULL;
		errno_t InputFileOpeningError = NULL;
		errno_t OutputFileOpeningError = NULL;
		HANDLE *p_thread_handles = (HANDLE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(HANDLE)));
		DWORD *p_thread_ids = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(DWORD)));
		PMYDATA ThreadpointerDataArray = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(MYDATA)));
		char* OutputFilePath = Return_Output_Path_From_Input_Path(InputFilePath, WhatActionShouldWeTake);
		HANDLE Mutex = CreateMutex(NULL,FALSE,NULL);
		HANDLE Semaphore = CreateSemaphore(NULL,0, NumberOfActiveThreadsForTheProgram,NULL);

		
		
		int* ArrayRowIndexs = Split_The_File_For_Each_Thread_Return_Int_Array_With_Starting_And_Ending_Row_Indexs(InputFilePath, NumberOfActiveThreadsForTheProgram);
		if (p_thread_handles == NULL || p_thread_ids == NULL 
			|| Mutex == NULL || Semaphore == NULL) {
			printf("Malloc Or Mutex Failed\n");

		}
		else {

			int ThreadCurrentNumber = 0;
			for (int i = 0; i < 2 * NumberOfActiveThreadsForTheProgram - 1; i += 2) {
				ThreadpointerDataArray[ThreadCurrentNumber].ThreadNumber = ThreadCurrentNumber;
				ThreadpointerDataArray[ThreadCurrentNumber].StartingByte = ArrayRowIndexs[i];
				ThreadpointerDataArray[ThreadCurrentNumber].EndingByte = ArrayRowIndexs[i + 1];
				ThreadpointerDataArray[ThreadCurrentNumber].InputFilePath = InputFilePath;
				ThreadpointerDataArray[ThreadCurrentNumber].OutputFilePath = OutputFilePath;
				ThreadpointerDataArray[ThreadCurrentNumber].Key = Key;
				ThreadpointerDataArray[ThreadCurrentNumber].FileLastLine = ArrayRowIndexs[2 * NumberOfActiveThreadsForTheProgram - 1];
				ThreadpointerDataArray[ThreadCurrentNumber].WhatActionShouldWeTake = WhatActionShouldWeTake;
				ThreadpointerDataArray[ThreadCurrentNumber].Mutex= Mutex;
				ThreadpointerDataArray[ThreadCurrentNumber].Semaphore = Semaphore;
				(p_thread_handles)[ThreadCurrentNumber] = CreateThread(NULL, 0, Decipher_Or_Encrypt, &ThreadpointerDataArray[ThreadCurrentNumber], 0, NULL);
				ThreadCurrentNumber++;
			}
			ReleaseSemaphore(Semaphore, NumberOfActiveThreadsForTheProgram, NULL);
			CheckThreadsStatus(ThreadpointerDataArray, &p_thread_handles, NumberOfActiveThreadsForTheProgram);
			free(ArrayRowIndexs);
			//free(OutputFilePath);
			HeapFree(GetProcessHeap(), NULL, ThreadpointerDataArray);
			HeapFree(GetProcessHeap(), NULL, p_thread_handles);
			HeapFree(GetProcessHeap(), NULL, p_thread_ids);
			
		}
		

	}





	return(0);
}