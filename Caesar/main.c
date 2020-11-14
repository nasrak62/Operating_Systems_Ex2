#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[]) {
	
	if (argc < 5) {
		printf("Not enough arguments were specified");
	}
	else {
		//initializing the necessary variables
		char* InputFilePath = argv[1];
		int Key=atoi(argv[2]);
		int NumberOfActiveThreadsForTheProgram= atoi(argv[3]);
		char* WhatActionShouldWeTake = argv[4];
		Get_Output_Path();


	}





	return(0);
}