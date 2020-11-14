/* File Header: Comments:
* Authors- Saar Zaidfunden -205892334, Jonathan Johansson-308034107
* Project- IOS_EX2 - Encryption and decryption using threads
Description- This main function gathers all the auxiliary functions that were created
in the others source files, and use them for reading an input text 

*/
#include <stdio.h>
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

		InputFileOpeningError = (fopen_s(&InPutFile, InputFilePath, "r"));
		OutputFileOpeningError = (fopen_s(&OutPutFile, Return_Output_Path_From_Input_Path(InputFilePath), "w"));
		if (OutputFileOpeningError == 0 && InputFileOpeningError == 0) {
			printf("Input and output File Opened\n");
			int *ArrayRowIndexs=Split_The_File_For_Each_Thread_Return_Int_Array_With_Starting_And_Ending_Row_Indexs(InputFileOpeningError, NumberOfActiveThreadsForTheProgram);
			for (int i = 0; i < NumberOfActiveThreadsForTheProgram; i++) {
				Create_Thread_And_Job(i,ArrayRowIndexs[i], ArrayRowIndexs[i], InputFileOpeningError, OutputFileOpeningError);

			}


			fclose(OutPutFile);
			fclose(InPutFile);
		}
		else {
			printf("Couldn't Open The Files\n");
		}

	}





	return(0);
}