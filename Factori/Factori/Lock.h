#ifndef LOCK


#include <windows.h>

#define LOCK
typedef struct{
	HANDLE IsSomeBodyReadingSemaphore;
	HANDLE IsSomeBodyWritingMutex;
	HANDLE IAmReading;
	HANDLE IAmWriting;
}Lock;

Lock* initializeLock();

#endif