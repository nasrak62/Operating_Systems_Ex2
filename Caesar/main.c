/* File Header: Comments:
* Authors- Saar Zaidfunden -205892334, Jonathan Johansson-308034107
* Project- IOS_EX2 - Encryption and decryption using threads
Description- This main function gathers all the auxiliary functions that were created
in the others source files, and use them for reading an input text 

*/
#include "ThreadsAndThreadsFunctions.h"
#include <stdlib.h>
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
		//HANDLE p_thread_handles =(HANDLE) malloc(NumberOfActiveThreadsForTheProgram* sizeof(HANDLE));
		//DWORD p_thread_ids = (DWORD)malloc(NumberOfActiveThreadsForTheProgram * sizeof(DWORD));
		HANDLE p_thread_handles = (HANDLE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(HANDLE)));
		DWORD p_thread_ids = (DWORD)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(DWORD)));
		PMYDATA pDataArray = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(PMYDATA)));



		//Create_Thread_And_Job(0, 0, 0, InPutFile, OutPutFile, &p_thread_handles, &p_thread_ids);
		int* ArrayRowIndexs = Split_The_File_For_Each_Thread_Return_Int_Array_With_Starting_And_Ending_Row_Indexs(InputFilePath, NumberOfActiveThreadsForTheProgram);
		
		
		if (p_thread_handles == NULL || p_thread_ids == NULL) {
			printf("Malloc Failed\n");

		}
		else {
			InputFileOpeningError = (fopen_s(&InPutFile, InputFilePath, "r"));
			OutputFileOpeningError = (fopen_s(&OutPutFile, Return_Output_Path_From_Input_Path(InputFilePath,WhatActionShouldWeTake), "w"));
			if (OutputFileOpeningError == 0 && InputFileOpeningError == 0) {
				printf("Input and output File Opened\n");
				int ThreadCurrentNumber = 0;
				for (int i = 0; i < 2*NumberOfActiveThreadsForTheProgram; i+=2) {
					Create_Thread_And_Job(ThreadCurrentNumber, ArrayRowIndexs[i], ArrayRowIndexs[i+1], InPutFile, OutPutFile, &p_thread_handles, &p_thread_ids);
					ThreadCurrentNumber++;
				}


				fclose(OutPutFile);
				fclose(InPutFile);
			}
			else {
				printf("Couldn't Open The Files\n");
			}
		}
		

	}





	return(0);
}