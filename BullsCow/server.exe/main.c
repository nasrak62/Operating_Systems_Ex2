/* File Header: Comments:
* Authors- Saar Zaidfunden -205892334, Jonathan Johansson-308034107
* Project- IOS_EX4 -
Description- 
*/

#include "SocketFunctions.h"
int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Number of arguments is not 2!");
	}
	else {
		int PortNumber = atoi(argv[1]);
		WSADATA wsa_data;
		int  WSA_Result;
		SOCKET SocketForClient;
		// Initialize Winsock
		WSA_Result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
		if (WSA_Result != 0) {
			printf("WSAStartup failed: %d\n", WSA_Result);
			return 1;
		}
		else {
			CreateSocketBindItAndListen(PortNumber);
			
			
			// Deinitialize Winsock
			WSA_Result = WSACleanup();
			if (WSA_Result != 0) {
				printf("WSACleanup failed: %d\n", WSA_Result);
				return 1;
			}

		}




		/*int* BytesForEachLine = MapRowsToBytesInFile(MissionFilePath, NumberOfMissions);
		Queue* MissionsPriorityQueue = InitializeQueue(NumberOfMissions);
		FillMissionsPriorityQueue(NumberOfMissions, MissionsPriorityQueue, PriorityFilePath);


		HANDLE* p_thread_handles = (HANDLE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(HANDLE)));
		DWORD* p_thread_ids = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(DWORD)));
		PMYDATA ThreadpointerDataArray = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (NumberOfActiveThreadsForTheProgram * sizeof(MYDATA)));
		HANDLE MutexForAccessingQueue = CreateMutex(NULL, FALSE, NULL);
		HANDLE IsSomeBodyWritingMutex = CreateMutex(NULL, FALSE, NULL);

		HANDLE Semaphore = CreateSemaphore(NULL, 0, NumberOfActiveThreadsForTheProgram, NULL);
		HANDLE IsSomeBodyReadingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		HANDLE IsSomeBodyReadingSemaphore = CreateSemaphore(NULL, NumberOfActiveThreadsForTheProgram, NumberOfActiveThreadsForTheProgram, NULL);

		if (p_thread_handles == NULL || p_thread_ids == NULL
			|| MutexForAccessingQueue == NULL || Semaphore == NULL
			|| IsSomeBodyReadingSemaphore == NULL || IsSomeBodyWritingMutex == NULL
			|| IsSomeBodyReadingEvent == NULL || ThreadpointerDataArray == NULL) {
			printf("Malloc Or Mutex Failed\n");

		}
		else {
			for (int i = 0; i < NumberOfActiveThreadsForTheProgram; i++) {

				Lock* FileLock;
				FileLock = InitializeLock();
				FileLock->IsSomeBodyReadingSemaphore = IsSomeBodyReadingSemaphore;
				FileLock->IsSomeBodyWritingMutex = IsSomeBodyWritingMutex;
				FileLock->IsSomeBodyReadingEvent = IsSomeBodyReadingEvent;
				FileLock->NumberOfActiveThreadsForTheProgram = NumberOfActiveThreadsForTheProgram;

				ThreadpointerDataArray[i].ThreadNumber = i;
				ThreadpointerDataArray[i].MissionFilePath = MissionFilePath;
				ThreadpointerDataArray[i].FileLock = FileLock;
				ThreadpointerDataArray[i].MissionsPriorityQueue = MissionsPriorityQueue;
				ThreadpointerDataArray[i].MutexForAccessingQueue = MutexForAccessingQueue;
				ThreadpointerDataArray[i].Semaphore = Semaphore;
				ThreadpointerDataArray[i].BytesForEachLine = BytesForEachLine;
				ThreadpointerDataArray[i].NumberOfMissions = NumberOfMissions;
				ThreadpointerDataArray[i].MaxNumberOfBytes = BytesForEachLine[NumberOfMissions - 1];
				(p_thread_handles)[i] = CreateThread(NULL, 0, SplitTheWorkForEachThread, &ThreadpointerDataArray[i], 0, NULL);
			}
			printf("all threads were created\n");
			ReleaseSemaphore(Semaphore, NumberOfActiveThreadsForTheProgram, NULL);
			CheckThreadsStatus(ThreadpointerDataArray, &p_thread_handles, NumberOfActiveThreadsForTheProgram);
			CloseHandle(IsSomeBodyWritingMutex);
			CloseHandle(IsSomeBodyReadingSemaphore);
			HeapFree(GetProcessHeap(), NULL, ThreadpointerDataArray);
			HeapFree(GetProcessHeap(), NULL, p_thread_handles);
			HeapFree(GetProcessHeap(), NULL, p_thread_ids);

		}



		*/

	}
	



	return 0;
}