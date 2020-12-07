#ifndef LOCK


#include <windows.h>
#define BRUTAL_TERMINATION_CODE 0x55
#define ERROR_CODE ((int)(-1))
#define TIMEOUT_IN_MILLISECONDS   15000
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

#endif