#include "Lock.h"
#include <assert.h>
Lock* InitializeLock() {
	Lock* FileLock;
	FileLock= (Lock*)malloc(sizeof(Lock));
	FileLock->IAmReading= CreateMutex(NULL, FALSE, NULL);
	FileLock->IAmWriting = CreateMutex(NULL, FALSE, NULL);
	if (FileLock->IAmWriting == NULL || FileLock->IAmReading == NULL) {
		printf("Malloc Or Mutex Failed\n");
	}
	else {
		printf("Lock is ready\n");
	}
	return FileLock;
}


DWORD read_lock(Lock* FileLock) {
	DWORD wait_IsSomeBodyWritingMutex;
	DWORD wait_IAmReading;
	DWORD wait_IsSomeBodyReadingSemaphore;
	DWORD release_IsSomeBodyWritingMutex;
	printf("readlock\n");
	wait_IsSomeBodyWritingMutex = WaitForSingleObject(FileLock->IsSomeBodyWritingMutex, TIMEOUT_IN_MILLISECONDS);
	printf("after some one writing\n");
	wait_IsSomeBodyReadingSemaphore = WaitForSingleObject(FileLock->IsSomeBodyReadingSemaphore, TIMEOUT_IN_MILLISECONDS);
	printf("after reading semaphore\n");
	release_IsSomeBodyWritingMutex = ReleaseMutex(FileLock->IsSomeBodyWritingMutex);
	printf("after release semaphore\n");
	if(release_IsSomeBodyWritingMutex == FALSE) {
		printf("release_IsSomeBodyWritingMutex not released\n");
	}
	wait_IAmReading = WaitForSingleObject(FileLock->IAmReading, TIMEOUT_IN_MILLISECONDS);
	
	if (wait_IsSomeBodyWritingMutex == WAIT_OBJECT_0 && wait_IAmReading == WAIT_OBJECT_0 && wait_IsSomeBodyReadingSemaphore == WAIT_OBJECT_0) {
	
		return wait_IsSomeBodyWritingMutex;
	}
	return WAIT_TIMEOUT;
}

void read_release(Lock* FileLock) {
	printf("read_release\n");
	DWORD release_IsSomeBodyReadingSemaphore;
	DWORD release_IAmReading;
	release_IsSomeBodyReadingSemaphore =ReleaseSemaphore(FileLock->IsSomeBodyReadingSemaphore, 1, NULL);
	if (release_IsSomeBodyReadingSemaphore == FALSE) {
		printf("release_IsSomeBodyReadingSemaphore not released\n");
	}
	release_IAmReading = ReleaseMutex(FileLock->IAmReading);
	if (release_IAmReading == FALSE) {
		printf("release_IAmReading not released\n");
	}
}


void write_lock(Lock* FileLock) {
	printf("write_lock\n");
	DWORD wait_IsSomeBodyWritingMutex;
	DWORD wait_IAmReading;
	DWORD wait_IsSomeBodyReadingSemaphore;
	DWORD release_IsSomeBodyReadingSemaphore;

	wait_IsSomeBodyWritingMutex = WaitForSingleObject(FileLock->IsSomeBodyWritingMutex, TIMEOUT_IN_MILLISECONDS);
	for (int i = 0; i < FileLock->NumberOfActiveThreadsForTheProgram;i++) {
		wait_IsSomeBodyReadingSemaphore = WaitForSingleObject(FileLock->IsSomeBodyReadingSemaphore, TIMEOUT_IN_MILLISECONDS);
	}
	release_IsSomeBodyReadingSemaphore = ReleaseSemaphore(FileLock->IsSomeBodyReadingSemaphore, FileLock->NumberOfActiveThreadsForTheProgram, NULL);
	if (release_IsSomeBodyReadingSemaphore == FALSE) {
		printf("release_IsSomeBodyReadingSemaphore not released\n");
	}
	wait_IsSomeBodyReadingSemaphore = WaitForSingleObject(FileLock->IsSomeBodyReadingSemaphore, TIMEOUT_IN_MILLISECONDS);
	wait_IAmReading = WaitForSingleObject(FileLock->IAmReading, TIMEOUT_IN_MILLISECONDS);
	if (wait_IsSomeBodyWritingMutex == WAIT_TIMEOUT || wait_IAmReading == WAIT_TIMEOUT || wait_IsSomeBodyReadingSemaphore == WAIT_TIMEOUT) {

		printf("time out\n");
	}
	
}

void write_release(Lock* FileLock) {
	printf("write_release\n");
	DWORD release_IsSomeBodyReadingSemaphore;
	DWORD release_IAmReading;
	DWORD release_IsSomeBodyWritingMutex;
	release_IsSomeBodyReadingSemaphore = ReleaseSemaphore(FileLock->IsSomeBodyReadingSemaphore, 1, NULL);
	if (release_IsSomeBodyReadingSemaphore == FALSE) {
		printf("release_IsSomeBodyReadingSemaphore not released\n");
	}
	release_IAmReading = ReleaseMutex(FileLock->IAmReading);
	if (release_IAmReading == FALSE) {
		printf("release_IAmReading not released\n");
	}
	release_IsSomeBodyWritingMutex = ReleaseMutex(FileLock->IsSomeBodyWritingMutex);
	if (release_IsSomeBodyWritingMutex == FALSE) {
		printf("release_IsSomeBodyWritingMutex not released\n");
	}

}

void DestroyLock(Lock* FileLock) {
	CloseHandle(FileLock->IAmReading);
	CloseHandle(FileLock->IAmWriting);
	
	free(FileLock);
	FileLock = NULL;
}