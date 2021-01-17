#ifndef MESSAGES
#define MESSAGES
#define STRINGS_ARE_EQUAL( Str1, Str2 ) ( strcmp( (Str1), (Str2) ) == 0 )
#define MAXIMUM_NUMBER_OF_PARAMETERS 5
#define MAXIMUM_NAME_LENGHT 20

typedef enum MessagesType {
	CLIENT_REQUEST,
	CLIENT_SETUP,
	CLIENT_PLAYER_MOVE,
	CLIENT_DISCONNECT,
	CLIENT_VERSUS,
	SERVER_MAIN_MENU,
	SERVER_APPROVED,
	SERVER_DENIED,
	SERVER_INVITE,
	SERVER_SETUP_REQUEST,
	SERVER_PLAYER_MOVE_REQUEST,
	SERVER_GAME_RESULTS,
	SERVER_WIN,
	SERVER_DRAW,
	SERVER_NO_OPPONENTS,
	SERVER_OPPONENT_QUIT

}MessagesType;


typedef struct{
	char *Parameters[MAXIMUM_NUMBER_OF_PARAMETERS];
	char* MessegeType;
	int NumberOfParameters;
}Message;

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")


typedef enum { TRNS_FAILED, TRNS_DISCONNECTED, TRNS_SUCCEEDED } TransferResult_t;

TransferResult_t SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd);
TransferResult_t SendString(const char* Str, SOCKET sd);
TransferResult_t ReceiveBuffer(char* OutputBuffer, int RemainingBytesToReceive, SOCKET sd);
TransferResult_t ReceiveString(char** OutputStrPtr, SOCKET sd);
Message GetRequest(SOCKET ServerSocket);
void SendRequest(SOCKET ServerSocket, char* MessageToSend);

#endif