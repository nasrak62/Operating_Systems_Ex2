#ifndef LOCK


#include <windows.h>
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define TIMEOUT_IN_MILLISECONDS   15000
#define MILLISECONDS_DEFENITION   1000
#define RANDOM_MAX_MILLISECONDS_DEFENITION   10
#define LOCK
typedef struct{
	HANDLE IsSomeBodyReadingSemaphore;
	HANDLE IsSomeBodyWritingMutex;
	HANDLE IAmReading;
	HANDLE IAmWriting;
	HANDLE IsSomeBodyReadingEvent;
	int NumberOfActiveThreadsForTheProgram;
}Lock;

Lock* InitializeLock();
DWORD read_lock(Lock* FileLock);
void read_release(Lock* FileLock);
void write_lock(Lock* FileLock);
void write_release(Lock* FileLock);
void DestroyLock(Lock* FileLock);

#endif