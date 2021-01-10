#include "deff_main.h"
#include "SocketSendRecvTools.h"
#include "client_main.h"
#include <time.h>
#include <winsock2.h>
#include <sys/types.h>

//#define WAIT 10000 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

struct timeval* timeout;
int timeout_flag = 0;
int reconnect = 0;
int timeout_occured = 0;
int exit_flag = 0;
SOCKET client_socket = INVALID_SOCKET;

MessSplit* mess_struct = NULL;
char* setup_choice()
{

	char user_input_string[20] = { NULL };
	int valid_input = 0, retval = 0;
	char* message_to_send = { NULL };
	while (valid_input == 0)
	{
		printf("Choose your own 4 digits sequence (without repeated digit!):\n");
		scanf("%s", user_input_string);
		int num = 0;
		num = atoi(user_input_string);
		int k, temp, frequency[9], flag = 0, i;
		for (i = 0; i < 10; i++)
		{
			frequency[i] = 0;
		}
		while (num > 0)
		{
			k = num % 10;
			frequency[k]++;
			num /= 10;
		}
		for (i = 0; i < 10; i++)
		{
			if (frequency[i] > 1)
			{
				flag = 1;
			}
		}
		if ((flag == 0) && len((user_input_string == 4) && isdigit(user_input_string) == 1))
		{
			valid_input = 1;
		}
		else
			printf("Invalid input.\nPlease type again your input and make sure it is valid!\n");
		if (valid_input == 1)
			message_to_send = send_message_client("SETUP", user_input_string, NULL);
	}
	return message_to_send;
}
/* move_player_choice
description – handling move of player
inputs - none
returns: string to send*/
char* move_player_choice() {

	char user_input_string[20] = { NULL };
	int valid_input = 0, retval = 0;
	char* message_to_send = { NULL };
	while (valid_input == 0)
	{
		printf("Choose a sequence of 4 digits sequence to guess:\n");
		scanf("%s", user_input_string);
		if (len((user_input_string == 4) && isdigit(user_input_string) == 1))
		{
			valid_input = 1;
		}
		else
			printf("Invalid input.\nPlease type again your input and make sure it is valid!\n");

		if (valid_input == 1)
			message_to_send = send_message_client("PLAYER_MOVE", user_input_string, NULL);
	}
	return message_to_send;
}


/* handle_recieved_message_client
description – handling incoming messages from the server
inputs - client socket
returns: 0 if all is correct, error  1 - otherwise*/
char* handle_recieved_message_client(MessSplit* rcvdMess, char* serv_ip, char* serv_port, char* username, sendThreadInput* send_mess_struct, SOCKET client_socket) //NEED TO CHANGE A LITTLE BIT
{
	char* user_input_char = NULL;
	char user_input_string[20] = { NULL };
	char* message_to_send = { NULL };
	int user_int = 0;
	int valid_input = 0, retval = 0;
	user_input_char = (char*)malloc(50 * sizeof(char));
	if (user_input_char == NULL)
	{
		printf("Malloc error");
		strcpy(message_to_send, "malloc");
	}
	if (strcmp(rcvdMess->type, "SERVER_MAIN_MENU") == 0)
	{
		message_to_send = main_menu_show(username);
	}
	else if (strcmp(rcvdMess->type, "SERVER_APPROVED") == 0)
	{
		printf("The server approved your access to the game.\n");
	}
	else if (strcmp(rcvdMess->type, "SERVER_DENIED") == 0)
	{
		closesocket(client_socket); // Close socket
		message_to_send = NULL;
		while (valid_input == 0)
		{
			printf("Server on %s:%s denied the connection request.\n", serv_ip, serv_port);
			printf("Choose what to do next:\n");
			printf("1. Try to reconnect\n");
			printf("2. Exit\n");
			scanf("%s", &user_int);
			switch (user_int)
			{
			case '1':
				printf("You chose to try connect again\n");
				valid_input = 1;
				exit_flag = 1;
				reconnect = 1;
				break;
			case '2':
				printf("Exiting...\n");
				valid_input = 1;
				exit_flag = 1;
				reconnect = 0;
				break;
			default:
				printf("Invalid input.\nPlease type again your input and make sure it is valid!\n");
			}
		}
	}
	else if (strcmp(rcvdMess->type, "SERVER_INVITE") == 0)
	{
		printf("Game is on!\n");
	}
	else if (strcmp(rcvdMess->type, "SERVER_SETUP_REQUEST") == 0)
	{
		message_to_send = setup_choice();
	}
	else if (strcmp(rcvdMess->type, "SERVER_PLAYER_MOVE_REQUEST") == 0)
	{
		message_to_send = move_player_choice();
	}
	else if (strcmp(rcvdMess->type, "SERVER_GAME_RESULTS") == 0)
	{
		printf("You played: %s\n", rcvdMess->parameters[2]);
		printf("%s played: %s\n", rcvdMess->parameters[0], rcvdMess->parameters[1]); //server plays - serve is in par[1]
		if (strcmp(rcvdMess->parameters[3], "") != 0)
			printf("%s won!\n", rcvdMess->parameters[3]);
	}
	else if (strcmp(rcvdMess->type, "SERVER_GAME_OVER_MENU") == 0)
	{
		while (valid_input == 0)
		{
			printf("Choose what to do next:\n1. Play again\n2. Return to the main menu\n");
			scanf("%s", user_input_char);
			switch (atoi(user_input_char))
			{
			case 1:
				message_to_send = send_message_client("REPLAY", NULL, NULL);
				valid_input = 1;
				break;
			case 2:
				message_to_send = send_message_client("MAIN_MENU", NULL, NULL);
				valid_input = 1;
				break;
			default:
				printf("Invalid input.\nPlease type again your input and make sure it is valid!\n");
			}
		}
	}
	else if (strcmp(rcvdMess->type, "SERVER_OPPONENT_QUIT") == 0)
	{
		printf("%s has left the game!\n", rcvdMess->parameters[0]);
	}
	else if (strcmp(rcvdMess->type, "SERVER_NO_OPPONENT") == 0)
	{
		printf("Currently there're no availeable players for a game!\n");
	}
	else
	{
		printf("Invalid message type!\nMessage is not sent based on the known protocol");
		printf("Exiting...\n");
		strcpy(message_to_send, "malloc"); //this enables us to exit the program
	}
	free(user_input_char);
	return message_to_send;
}

/*  closing_socket(SOCKET sock):
description – enables closing socket and WSA enviornment when needed
inputs - client socket
returns: 0 if all is correct, error  1 - otherwise*/
int closing_socket(SOCKET sock)
{
	int cleanup_res = -1;
	closesocket(client_socket);
	cleanup_res = WSACleanup();
	if (cleanup_res != 0)
	{
		printf("WSACleanup failed: %d\n", cleanup_res);
		return 1; //error
	}
	return cleanup_res;
}
/* Connection Loss: - will be implement when Timeout or another reason.
description – when connection is lost - offer the user to reconnect or exit entirly.
inputs - server ip and port, client socket
returns: 0 if all is correct, error  1 - otherwise*/
int Connection_Loss(char* serv_ip, int serv_port, char* username, SOCKET clientSocket)
{
	int user_input = 0;
	int valid_input = 0;
	int SendRes = -1;
	char* SendStr = NULL;
	SendStr = send_message_client("DISCONNECT", NULL, NULL);
	SendRes = SendString(SendStr, client_socket);
	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(client_socket);
		return 1;
	}
	Sleep(100);
	closesocket(client_socket);
	while (valid_input == 0)
	{
		printf("Connection to server on %s:%d has been lost\n", serv_ip, serv_port);
		printf("Choose what to do next:\n");
		printf("1. Try to reconnect\n");
		printf("2. Exit\n");
		scanf("%s", &user_input);
		switch (user_input)
		{
		case '1':
			printf("You chose to try connect again\n");
			valid_input = 1;
			exit_flag = 1;
			reconnect = 1;
			break;
		case '2':
			printf("Exiting...\n");
			valid_input = 1;
			exit_flag = 1;
			reconnect = 0;
			break;
		default:
			printf("Invalid input.\nPlease type again your input and make sure it is valid!\n");
		}
	}
	return 0;
}

/* ReceiveString:
description – supports recieving message + timeout handling for two usecases - waiting for opponent - 30 sec, all the rest - 15.
inputs - server ip and port, client socket
returns: 0 if failed,1 if disconnected, 2 if succeed, 3 for controled exit and 4 for malloc error*/
TransferResult_t ReceiveString(char** OutputStrPtr, SOCKET sd, MessThreadInput* param)
{
	/* Recv the the request to the server on socket sd */
	TransferResult_t RecvRes;
	char* StrBuffer = NULL;
	fd_set rset;
	int TotalStringSizeInBytes, select_val, retval = 0;
	int exit_check = -1;
	long timesec = 15;
	if (timeout_flag == 1)
		timesec = 30;
	else
		timesec = 15;
	timeout_flag = 0;
	FD_ZERO(&rset);
	FD_SET(sd, &rset);
	timeout = (timeval*)malloc(sizeof(timeval));
	if (timeout == NULL)
	{
		printf("Malloc error");
		return 4;
	}
	memset(timeout, 0, sizeof(timeval));
	timeout->tv_sec = timesec;
	select_val = select(NULL, &rset, NULL, NULL, timeout);
	if (select_val == 0)//meaning we reached timeout
	{
		printf("Timeout in recieving message from server\n");
		timeout_occured = 1;
		Connection_Loss(param->serv_ip, atoi(param->serv_port), param->username, client_socket);
		free(timeout);
		if (exit_flag == 1) {
			return 3;
		}
		return 0;
	}
	if ((OutputStrPtr == NULL) || (*OutputStrPtr != NULL))
	{
		printf("The first input to ReceiveString() must be "
			"a pointer to a char pointer that is initialized to NULL. For example:\n"
			"\tchar* Buffer = NULL;\n"
			"\tReceiveString( &Buffer, ___ )\n");
		free(timeout);
		return TRNS_FAILED;
	}

	/* The request is received in two parts. First the Length of the string (stored in
	   an int variable ), then the string itself. */

	RecvRes = ReceiveBuffer(
		(char*)(&TotalStringSizeInBytes),
		(int)(sizeof(TotalStringSizeInBytes)), // 4 bytes
		sd);

	if (RecvRes != TRNS_SUCCEEDED) return RecvRes;

	StrBuffer = (char*)malloc(TotalStringSizeInBytes * sizeof(char));

	if (StrBuffer == NULL)
		return TRNS_FAILED;

	RecvRes = ReceiveBuffer(
		(char*)(StrBuffer),
		(int)(TotalStringSizeInBytes),
		sd);

	if (RecvRes == TRNS_SUCCEEDED)
	{
		*OutputStrPtr = StrBuffer;
	}
	else
	{
		free(StrBuffer);
	}
	free(timeout);
	return RecvRes;
}

/* check_params:
description – check that all the inputs to the programe are as expected and valied
inputs - argc, argv
returns: 0 if all is correct, error  1 - otherwise*/
int check_params(int argc, char* argv[]) {
	if (!(argc == 4)) { //possible argv issues:
		printf("Wrong number of parameters of %s\n", argv[0]);
		return 1;
	}
	if (argv[1] == NULL) {
		printf("argv line is empty \n");
		return 1;
	}
	if ((strlen(argv[1]) > MAX_IP)) {
		printf("Invalid IP as input.\n");
		return 1;
	}
	if ((strlen(argv[2]) > MAX_PORT)) {
		printf("Invalid port as input.\n");
		return 1;
	}
	if ((strlen(argv[3]) > MAX_NAME)) {
		printf("Invalid username as input.\n");
		return 1;
	}
	return 0;
}

/* main_menu_show:
description – show main menu for user and handling its output. user can choose between playing with a player, the server or to quit
inputs - username, threads input for handling
returns: message to be sent - in case of malloc issue - it returns malloc*/

char* main_menu_show(char* username)
{
	char* user_input = NULL;
	char* temp_message = NULL;
	int valid_input = 0;
	user_input = (char*)malloc(16 * sizeof(char));
	if (user_input == NULL)
	{
		printf("Malloc error");
		printf("Exiting due to malloc");
		strcpy(user_input, "malloc");
		temp_message = user_input;
	}
	while (valid_input == 0)
	{
		printf("Choose what to do next:\n");
		printf("1. Play against another client\n");
		printf("2. Quit\n");
		scanf("%s", user_input);
		switch (atoi(user_input))
		{
		case 1:
			temp_message = send_message_client("VERSUS", NULL, username);
			timeout_flag = 1;
			valid_input = 1;
			break;
		case 2:
			printf("You chose to quit our great game!\n");
			temp_message = send_message_client("DISCONNECT", NULL, NULL);
			valid_input = 1;
			reconnect = 0;
			break;
		default:
			printf("Invalid input.\nPlease type again your input and make sure it is valid!\n");
		}

	}
	free(user_input);
	return temp_message;
}

void free_thread_mem(MessSplit* mess, char* send)
{
	free(mess);
	free(send);
}

/* SendRecvThread:
description – a thread that runs all the time until it fails to send and recieve data from other participant of the communication and it prints errors and free allocated memory.
inputs - recieveThreadInput struct- contains string of message, is now it's time to send a message and result value for error catching
returns: 2 if all is correct, error  is 0 or 1 depends on error type - otherwise*/
static DWORD SendRecvThread(LPVOID lpParam)
{
	TransferResult_t RecvRes = 1, TransmissionRes, SendRes = 1;
	int done = 0, mall_check = 0;
	MessSplit* SplitRcvMess = NULL;
	char* SendStr = NULL;
	MessThreadInput* thread_param = (MessThreadInput*)lpParam; //casting
	Sleep(10);
	if (NULL == lpParam)
	{
		printf("Invalid parameters of thread\n");
		thread_param->error = 1;
		return 1;
	}
	memset(&SendRes, 0, sizeof(TransferResult_t));
	memset(&RecvRes, 0, sizeof(TransferResult_t));
	SplitRcvMess = create_mess_struct();
	if (SplitRcvMess == NULL)
	{
		thread_param->error = 1;
		return 1;
	}
	while (!done)
	{
		char* AcceptedStr = NULL;
		RecvRes = ReceiveString(&AcceptedStr, client_socket, thread_param);
		if (RecvRes == TRNS_FAILED)
		{
			printf("socket error while trying to write data to socket\n");
			printf("Server might exit its program.\nClosing evertything.\nExiting...\n");
			RecvRes = 0x555;
			free_thread_mem(SplitRcvMess, SendStr);
			thread_param->error = RecvRes;
			if (AcceptedStr != NULL) {
				free(AcceptedStr);
			}
			return RecvRes;
		}
		else if (RecvRes == TRNS_DISCONNECTED)
		{
			printf("server closed connection. bye!\n");
			RecvRes = 0x555;
			free_thread_mem(SplitRcvMess, SendStr);
			thread_param->error = RecvRes;
			if (AcceptedStr != NULL) {
				free(AcceptedStr);
			}
			return RecvRes;
		}
		else if (RecvRes == 3) // free everything and go out
		{
			free_thread_mem(SplitRcvMess, SendStr);
			thread_param->error = 2;
			if (AcceptedStr != NULL) {
				free(AcceptedStr);
			}
			return 0;
		}
		else if (RecvRes == 4) //Indicates malloc error
		{
			printf("Exiting due to malloc");
			free_thread_mem(SplitRcvMess, SendStr);
			if (AcceptedStr != NULL) {
				free(AcceptedStr);
			}
			thread_param->error = 1;
			return 1;
		}
		else
		{
			//user didn't want to exit and transmission worked
			mall_check = MessageRecv(AcceptedStr, SplitRcvMess);
			if (mall_check == 1)
			{
				printf("Exiting due to malloc");
				free_thread_mem(SplitRcvMess, SendStr);
				if (AcceptedStr != NULL) {
					free(AcceptedStr);
				}
				thread_param->error = 1;
				return 1;
			}
			SendStr = handle_recieved_message_client(SplitRcvMess, thread_param->serv_ip, thread_param->serv_port, thread_param->username, thread_param, client_socket);
		}

		if (AcceptedStr == NULL) //in case we got an empty message
		{
			printf("Recieving message is failed");
			free_thread_mem(SplitRcvMess, SendStr);
			thread_param->error = RecvRes;
			return RecvRes;
		}
		if (SendStr != NULL) {
			if (strcmp(SendStr, "malloc") == 0)
			{
				printf("Exiting due to malloc");
				free_thread_mem(SplitRcvMess, SendStr);
				free(AcceptedStr);
				thread_param->error = 1;
				return 1;
			}
			SendRes = SendString(SendStr, client_socket);
			if (strcmp(SendStr, "CLIENT_DISCONNECT\n") == 0)
				done = 1;
			if (SendRes == TRNS_FAILED) {
				printf("Socket error while trying to write data to socket\n");
				printf("Server might exit its program.\nClosing evertything.\nExiting...\n");
				thread_param->error = SendRes;
				free_thread_mem(SplitRcvMess, SendStr);
				free(AcceptedStr);
				return SendRes;
			}
		}
		if ((SendStr == NULL) && (AcceptedStr != NULL) && (strncmp(AcceptedStr, "SERVER_DENIED", 14) == 0)) {
			done = 1;
		}
		free(AcceptedStr);
	}
	free_thread_mem(SplitRcvMess, SendStr);
	thread_param->error = 2;
	return 2;
}

/* Connection try:
description – try to connect a server for initiating the game
inputs - server ip and port, client socket
returns: 0 if all is correct, error  1 - otherwise*/
int Connection_try(char* serv_ip, int serv_port, char* username, SOCKET clientSocket)
{
	TransferResult_t SendRes;
	char* SendStr = NULL;//global define
	int user_input = 0;
	SOCKADDR_IN clientService; //connection function
	int connect_error = 0;
	int cleanup_res = -1;
	int valid_input = 0;

	//Create a sockaddr_in object clientService and set  values. //connection function
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr(serv_ip); //Setting the IP address to connect to (ip to change if needed
	clientService.sin_port = htons(serv_port); //Setting the port to connect to. (we can change the port if needed)


	// Call the connect function, passing the created socket and the sockaddr_in structure as parameters. 
	// Check for general errors.
	connect_error = connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService));
	if (connect_error == SOCKET_ERROR) { //connection function
		while (valid_input == 0)
		{
			printf("Failed connecting to server on %s:%d\n", serv_ip, serv_port);
			printf("Choose what to do next:\n");
			printf("1. Try to reconnect\n");
			printf("2. Exit\n");
			scanf("%s", &user_input);
			switch (user_input)
			{
			case '1':
				printf("You chose to try connect again\n");
				valid_input = 1;
				exit_flag = 0;
				reconnect = 1;
				return 1;
				break;
			case '2':
				printf("You chose to exit\n");
				printf("Exiting...\n");
				printf("ByeBye\n");
				valid_input = 1;
				exit_flag = 1;
				reconnect = 0;
				return 3;
				break;
			default:
				printf("Invalid input.\nPlease type again your input and make sure it is valid!\n");
			}
		}
	}
	else
	{
		printf("Connected to server %s:%d\n", serv_ip, serv_port);
		SendStr = send_message_client("REQUEST", NULL, username);
		SendRes = SendString(SendStr, client_socket);
		if (SendRes == TRNS_FAILED)
		{
			printf("Socket error while trying to write data to socket\n");
			printf("Server might exit its program.\nClosing evertything.\nExiting...\n");
			return SendRes;
		}
	}
	return 0;
}


int main(int argc, char* argv[])
{
	int user_input = -1, connection_check = -1, port = 0, already_cleared = 0;
	MessSplit* mess_struct = NULL; //casting
	MessThreadInput* mess_thread = NULL;
	char* message_to_send_to_server = NULL;
	HANDLE hThread = NULL;
	DWORD p_thread = NULL;
	DWORD Res;
	//for socket services
	SOCKADDR_IN clientService; //connection function
	WSADATA wsaData; //Create a WSADATA object called wsaData.
	int retval = 0, ip_server = 0, cleanup_res = -1;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//The WSADATA structure contains information about the Windows Sockets implementation.
		//Call WSAStartup and check for errors.
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
		printf("Exiting...\n");
		return 1;
	}

	//checking arguments of new client
	if (check_params(argc, argv) != 0) {
		return 1;
	}

	reconnect = 1;
	while (reconnect == 1) {
		reconnect = 0;
		mess_thread = (MessThreadInput*)malloc(sizeof(MessThreadInput));
		if (mess_thread == NULL)
		{
			printf("Malloc error");
			cleanup_res = WSACleanup();
			if (cleanup_res != 0)
			{
				printf("WSACleanup failed: %d\n", cleanup_res);
				return 1; //error
			}
			return cleanup_res;
		}
		memset(mess_thread, 0, sizeof(MessThreadInput));
		mess_thread->serv_ip = argv[1];//saving server ip
		mess_thread->serv_port = argv[2];
		port = atoi(argv[2]);//saving server port
		mess_thread->username = argv[3];//connecting relevant username

		// Create a socket for client
		client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		// Check for errors to ensure that the socket is a valid socket. If the socket call fails, it returns INVALID_SOCKET. 
		if (client_socket == INVALID_SOCKET) {
			printf("Error at socket(): %ld\n", WSAGetLastError()); // WSAGetLastError returns an error number associated with the last error that occurred.
			retval = WSACleanup();
			if (retval != 0)
			{
				printf("WSACleanup failed: %d\n", retval);
				retval = 1; //error
			}
			free(mess_thread);
			return retval;
		}

		connection_check = Connection_try(mess_thread->serv_ip, port, mess_thread->username, client_socket);
		if (connection_check == 1) { // error connecting to server, try again
			free(mess_thread);
			closesocket(client_socket);
			continue;
		}
		if (connection_check == 3) // user wishes to exit or failure to initialize socket
		{
			free(mess_thread);
			already_cleared = 1;
			closing_socket(client_socket);
			break;
		}
		else {
			hThread = CreateThreadSimple(SendRecvThread, &p_thread, (char*)mess_thread);
			Res = WaitForSingleObject(hThread, INFINITE);
			if ((mess_thread->error == 0) || (mess_thread->error == 1)) {
				retval = 1;
			}

			if (Res == WAIT_OBJECT_0) // this thread finished running
			{
				CloseHandle(hThread);
				hThread = NULL;
			}
			else {
				printf("Waiting for thread failed.\nTerminating Thread and ending program\n");
				TerminateThread(hThread, Res);
				CloseHandle(hThread);
				retval = 1;
				break;
			}
		}
	}
	free(mess_thread);
	if (already_cleared == 0) {
		retval += closing_socket(client_socket);
	}
	return retval;
}