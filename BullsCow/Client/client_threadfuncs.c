#include "deff_main.h"
#include "client_main.h"

/*Description: this c file contains functions of creating and closing threads*/

//description –closing (and terminating if closing wasn't successful) of all three threads 
//parameters – inputs : array of thread handles' pointers
//returns: 0 if closing handle succeeded, 1 else

BOOL close_handels_func(HANDLE* p_thread_handles) {
	int i = 0;
	BOOL ret_val_close;

	for (i = 0; i < 2; i++)
	{
		if (p_thread_handles[i] != NULL) { // if the handel was opend - close it
			ret_val_close = CloseHandle(p_thread_handles[i]);
			if (0 == ret_val_close) // if couldnt close, Terminate Thread
			{
				printf("Error when closing handel,Terminating bruthaly \n");
				return 1;
			}
		}
	}
	return 0;
}


//CreateThreadSimple:
//description – this function creates new thread using CreateThread() known function by implenting all variable 
//inputs - pointer, path and arguments of a thread
//returns: return createthread function

HANDLE CreateThreadSimple(LPTHREAD_START_ROUTINE p_start_routine,
	LPDWORD p_thread_id, char* thread_path)
{
	if ((p_start_routine == NULL) || (p_thread_id == NULL) || (thread_path == NULL)) {
		printf("NULL pointers to threads parameters, couldn't creat thread\n");
		return NULL;
	}

	return CreateThread(
		NULL,            /*  default security attributes */
		0,               /*  use default stack size */
		p_start_routine, /*  thread function */
		thread_path,            /*  argument to thread function */
		0,               /*  use default creation flags */
		p_thread_id);    /*  returns the thread identifier */
}

