#include "Lock.h"
#include <assert.h>
Lock* InitializeLock() {
	Lock* FileLock=(Lock*)malloc(sizeof(Lock));
	FileLock->IAmReading= CreateMutex(NULL, FALSE, NULL);
	FileLock->IAmWriting = CreateMutex(NULL, FALSE, NULL);
	assert(FileLock->IAmReading==NULL);
	assert(FileLock->IAmWriting == NULL);
	return FileLock;
}


DWORD read_lock(Lock* FileLock) {
	DWORD wait_IsSomeBodyWritingMutex;
	DWORD wait_IAmReading;
	DWORD wait_IsSomeBodyReadingSemaphore;
	DWORD release_IsSomeBodyWritingMutex;
	
	wait_IsSomeBodyWritingMutex = WaitForSingleObject(FileLock->IsSomeBodyWritingMutex, TIMEOUT_IN_MILLISECONDS);
	wait_IsSomeBodyReadingSemaphore = WaitForSingleObject(FileLock->IsSomeBodyReadingSemaphore, TIMEOUT_IN_MILLISECONDS);
	release_IsSomeBodyWritingMutex = ReleaseMutex(FileLock->IsSomeBodyWritingMutex);
	assert(release_IsSomeBodyWritingMutex == FALSE);
	wait_IAmReading = WaitForSingleObject(FileLock->IAmReading, TIMEOUT_IN_MILLISECONDS);
	
	if (wait_IsSomeBodyWritingMutex == WAIT_OBJECT_0 && wait_IAmReading == WAIT_OBJECT_0 && wait_IsSomeBodyReadingSemaphore == WAIT_OBJECT_0) {
	
		return wait_IsSomeBodyWritingMutex;
	}
	return WAIT_TIMEOUT;
}

void read_release(Lock* FileLock) {
	DWORD release_IsSomeBodyReadingSemaphore;
	DWORD release_IAmReading;
	release_IsSomeBodyReadingSemaphore =ReleaseSemaphore(FileLock->IsSomeBodyReadingSemaphore, 1, NULL);
	assert(release_IsSomeBodyReadingSemaphore == FALSE);
	release_IAmReading = ReleaseMutex(FileLock->IAmReading);
	assert(release_IAmReading == FALSE);
}


void write_lock(Lock* FileLock) {
	DWORD wait_IsSomeBodyWritingMutex;
	DWORD wait_IAmReading;
	DWORD wait_IsSomeBodyReadingSemaphore;
	DWORD release_IsSomeBodyReadingSemaphore;

	wait_IsSomeBodyWritingMutex = WaitForSingleObject(FileLock->IsSomeBodyWritingMutex, TIMEOUT_IN_MILLISECONDS);
	for (int i = 0; i < FileLock->NumberOfActiveThreadsForTheProgram;i++) {
		wait_IsSomeBodyReadingSemaphore = WaitForSingleObject(FileLock->IsSomeBodyReadingSemaphore, TIMEOUT_IN_MILLISECONDS);
	}
	release_IsSomeBodyReadingSemaphore = ReleaseSemaphore(FileLock->IsSomeBodyReadingSemaphore, FileLock->NumberOfActiveThreadsForTheProgram, NULL);
	assert(release_IsSomeBodyReadingSemaphore == FALSE);
	wait_IsSomeBodyReadingSemaphore = WaitForSingleObject(FileLock->IsSomeBodyReadingSemaphore, TIMEOUT_IN_MILLISECONDS);
	wait_IAmReading = WaitForSingleObject(FileLock->IAmReading, TIMEOUT_IN_MILLISECONDS);
	if (wait_IsSomeBodyWritingMutex == WAIT_TIMEOUT || wait_IAmReading == WAIT_TIMEOUT || wait_IsSomeBodyReadingSemaphore == WAIT_TIMEOUT) {

		printf("time out\n");
	}
	
}

void write_release(Lock* FileLock) {
	DWORD release_IsSomeBodyReadingSemaphore;
	DWORD release_IAmReading;
	DWORD release_IsSomeBodyWritingMutex;
	release_IsSomeBodyReadingSemaphore = ReleaseSemaphore(FileLock->IsSomeBodyReadingSemaphore, 1, NULL);
	assert(release_IsSomeBodyReadingSemaphore == FALSE);
	release_IAmReading = ReleaseMutex(FileLock->IAmReading);
	assert(release_IAmReading == FALSE);
	release_IsSomeBodyWritingMutex = ReleaseMutex(FileLock->IsSomeBodyWritingMutex);
	assert(release_IsSomeBodyWritingMutex == FALSE);
}

void DestroyLock(Lock* FileLock) {
	free(FileLock->IAmReading);
	free(FileLock->IAmWriting);
	free(FileLock->IsSomeBodyWritingMutex);
	free(FileLock->IsSomeBodyReadingSemaphore);
	free(FileLock->NumberOfActiveThreadsForTheProgram);
	free(FileLock);
	FileLock = NULL;
}