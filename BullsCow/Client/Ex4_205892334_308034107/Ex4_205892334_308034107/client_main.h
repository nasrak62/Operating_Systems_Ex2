#pragma once

#include "deff_main.h"

#define MAX_PARAM 4 
#define MAX_IP 20
#define MAX_NAME 20
#define MAX_PORT 5
#define MAX_PATHS 256 
#define SERVER_ADDRESS_STR "127.0.0.1"
#define MAX_THREADS 16

#define ERROR_CODE ((int)(-1))
#define BRUTAL_TERMINATION_CODE 0x55 
// create process ret vals: 
#define CREATING_PROC_SUCCESS 0
#define ERROR_CREATING_PROC 1
#define ERROR_WAIT_PROC 2
#define ERROR_GET_EXIT_CODE 3
#define ERROR_EXIT_CODE_1 4
#define ERROR_CLOSING_HANDEL 5

SOCKET client_socket;

typedef enum { //error definition
	ISP_SUCCESS,
	ISP_FILE_OPEN_FAILED,
	ISP_FILE_SEEK_FAILED,
	ISP_FILE_READING_FAILED,
	ISP_FTELL_FAILED,
	ISP_MUTEX_OPEN_FAILED,
	ISP_MUTEX_CREATE_FAILED,
	ISP_MUTEX_WAIT_FAILED,
	ISP_MUTEX_ABANDONED,
	ISP_MUTEX_RELEASE_FAILED,
	ISP_ILLEGAL_LETTER_WAS_READ,
	ISP_SEMAPHORE_CREATE_FAILED,
	ISP_SEMAPHORE_OPEN_FAILED,
	ISP_SEMAPHORE_WAIT_FAILED,
	ISP_SEMAPHORE_ABANDONED,
	ISP_SEMAPHORE_RELEASE_FAILED
} ErrorCode_t;

//structs

typedef struct
{

	int to_send;

} sendThreadInput;

typedef struct
{
	char* serv_ip;
	char* serv_port;
	char* username;
	int error;
} MessThreadInput;

typedef struct
{
	char* type;
	char* parameters[4];
	int parameter_num;
} MessSplit;

typedef struct {
	long tv_sec;
	long tv_usec;
} timeval;

////function declartions:

///main.c
int closing_socket(SOCKET sock);
int Connection_Loss(char* serv_ip, int serv_port, char* username, SOCKET clientSocket);
int check_params(int argc, char* argv[]);
char* main_menu_show(char* username);
void free_thread_mem(MessSplit* mess, char* send);
static DWORD SendRecvThread(LPVOID lpParam);
char* handle_recieved_message_client(MessSplit* rcvdMess, char* serv_ip, char* serv_port, char* username, sendThreadInput* send_mess_struct, SOCKET client_socket);
char* send_message_client(char* type, char* player_move, char* username);
///clientsendrecievefuncs.c
char* chop(char* string);
MessSplit* create_mess_struct();
char* create_message_to_send(char* type, char** parameters, int num);
int param_type(char* Type);
int MessageRecv(char* message, MessSplit* MessageStruct);

