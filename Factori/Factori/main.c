/* File Header: Comments:
* Authors- Saar Zaidfunden -205892334, Jonathan Johansson-308034107
* Project- IOS_EX3 -
Description- This main function 

*/
#include "ThreadsMainFunction.h"
int main(int argc,char* argv[]) {
	if (argc != 5) {
		printf("Number of arguments is not 5!");
	}
	else {
		char* MissionFilePath = argv[1];
		char* PriorityFilePath = argv[2];
		int NumberOfMissions = atoi(argv[3]);
		int NumberOfActiveThreadsForTheProgram = atoi(argv[4]);
		// initialize queue with PriorityFile
		/*
		Queue MissionsPriorityQueue=InitializeQueue();
		FillMissionsPriorityQueue(MissionsPriorityQueue,PriorityFilePath);
		
		
		
		*/

		//Create Threads to the thread function  and get queue and lock struct as parameters

		
		HANDLE *p_thread_handles = (HANDLE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(HANDLE)));
		DWORD *p_thread_ids = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(DWORD)));
		PMYDATA ThreadpointerDataArray = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(MYDATA)));
		HANDLE MutexForAccessingQueue = CreateMutex(NULL,FALSE,NULL);
		HANDLE IsSomeBodyWritingMutex = CreateMutex(NULL, FALSE, NULL);
		HANDLE IsSomeBodyReadingSemaphore = CreateSemaphore(NULL, 0, NumberOfActiveThreadsForTheProgram, NULL);
		HANDLE Semaphore = CreateSemaphore(NULL,0, NumberOfActiveThreadsForTheProgram,NULL);
		HANDLE IsSomeBodyReadingEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
		

		if (p_thread_handles == NULL || p_thread_ids == NULL
			|| MutexForAccessingQueue == NULL || Semaphore == NULL
			|| IsSomeBodyReadingSemaphore == NULL || IsSomeBodyWritingMutex == NULL
			|| IsSomeBodyReadingEvent == NULL) {
			printf("Malloc Or Mutex Failed\n");

		}
		else {
			for (int i = 0; i <  NumberOfActiveThreadsForTheProgram ; i++) {
				Lock* FileLock = InitializeLock();
				FileLock->IsSomeBodyReadingSemaphore = IsSomeBodyReadingSemaphore;
				FileLock->IsSomeBodyWritingMutex = IsSomeBodyWritingMutex;
				FileLock->IsSomeBodyReadingEvent = IsSomeBodyReadingEvent;
				FileLock->NumberOfActiveThreadsForTheProgram = NumberOfActiveThreadsForTheProgram;
				ThreadpointerDataArray[i].ThreadNumber = i;
				ThreadpointerDataArray[i].MissionFilePath = MissionFilePath;
				ThreadpointerDataArray[i].FileLock = FileLock;
				//ThreadpointerDataArray[i].MissionsPriorityQueue = MissionsPriorityQueue;
				ThreadpointerDataArray[i].MutexForAccessingQueue= MutexForAccessingQueue;
				ThreadpointerDataArray[i].Semaphore = Semaphore;
				//(p_thread_handles)[i] = CreateThread(NULL, 0, Threadfunc, &ThreadpointerDataArray[i], 0, NULL);
			}
			ReleaseSemaphore(Semaphore, NumberOfActiveThreadsForTheProgram, NULL);
			CheckThreadsStatus(ThreadpointerDataArray, &p_thread_handles, NumberOfActiveThreadsForTheProgram);
			HeapFree(GetProcessHeap(), NULL, ThreadpointerDataArray);
			HeapFree(GetProcessHeap(), NULL, p_thread_handles);
			HeapFree(GetProcessHeap(), NULL, p_thread_ids);

		}
		
		



	}




	return 0;
}