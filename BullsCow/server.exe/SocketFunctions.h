#ifndef SOCKET_FUNCTIONS
#define SOCKET_FUNCTIONS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#define LOCAL_HOST "127.0.0.1"
#define NUM_OF_WORKER_THREADS 2
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")


#endif