//*Description: this file includes most of send/recieve functions of the clients which contains additional implementations that are unique only for clients

#include "deff_main.h"
#include "SocketSendRecvTools.h"
#include "client_main.h"

//chop: -
//description – throwing last char of a string
//parameters – inputs : pt of the string
//returns:pt of chopped string
char* chop(char* str)
{
	int len = 0;
	len = strlen(str);
	if (len <= 0) {
		return str;
	}
	if (str[len - 1] == '\n') {
		str[len - 1] = '\0';
	}
	return str;
}


TransferResult_t SendBuffer(const char* Buffer, int BytesToSend, SOCKET sd)
{
	const char* CurPlacePtr = Buffer;
	int BytesTransferred;
	int RemainingBytesToSend = BytesToSend;

	while (RemainingBytesToSend > 0)
	{
		/* send does not guarantee that the entire message is sent */
		BytesTransferred = send(sd, CurPlacePtr, RemainingBytesToSend, 0);
		if (BytesTransferred == SOCKET_ERROR)
		{
			printf("send() failed, error %d\n", WSAGetLastError());
			return TRNS_FAILED;
		}

		RemainingBytesToSend -= BytesTransferred;
		CurPlacePtr += BytesTransferred; // <ISP> pointer arithmetic
	}

	return TRNS_SUCCEEDED;
}


TransferResult_t SendString(const char* Str, SOCKET sd)
{
	/* Send the the request to the server on socket sd */
	int TotalStringSizeInBytes;
	TransferResult_t SendRes;

	/* The request is sent in two parts. First the Length of the string (stored in
	   an int variable ), then the string itself. */

	TotalStringSizeInBytes = (int)(strlen(Str) + 1); // terminating zero also sent	

	SendRes = SendBuffer(
		(const char*)(&TotalStringSizeInBytes),
		(int)(sizeof(TotalStringSizeInBytes)), // sizeof(int) 
		sd);

	if (SendRes != TRNS_SUCCEEDED) return SendRes;

	SendRes = SendBuffer(
		(const char*)(Str),
		(int)(TotalStringSizeInBytes),
		sd);

	return SendRes;
}


TransferResult_t ReceiveBuffer(char* OutputBuffer, int BytesToReceive, SOCKET sd)
{
	char* CurPlacePtr = OutputBuffer;
	int BytesJustTransferred;
	int RemainingBytesToReceive = BytesToReceive;

	while (RemainingBytesToReceive > 0)
	{
		/* send does not guarantee that the entire message is sent */
		BytesJustTransferred = recv(sd, CurPlacePtr, RemainingBytesToReceive, 0);
		if (BytesJustTransferred == SOCKET_ERROR)
		{
			printf("recv() failed, error %d\n", WSAGetLastError());
			return TRNS_FAILED;
		}
		else if (BytesJustTransferred == 0)
			return TRNS_DISCONNECTED; // recv() returns zero if connection was gracefully disconnected.

		RemainingBytesToReceive -= BytesJustTransferred;
		CurPlacePtr += BytesJustTransferred; // <ISP> pointer arithmetic
	}

	return TRNS_SUCCEEDED;
}



/* create_mess_struct:
description – create struct for recieving or sending a message
inputs - none
returns: MessSplit pointer of a new struct*/
MessSplit* create_mess_struct()
{
	MessSplit* messStruct = NULL;
	messStruct = (MessSplit*)malloc(sizeof(MessSplit));
	if (messStruct == NULL)
	{
		printf("Malloc error");
		return NULL;
	}
	memset(messStruct, 0, sizeof(MessSplit));
	return messStruct;
}


/* create message_to_send:
description – creating the message to be sent by the client/server
inputs - type (based on instructions), parameters and number of parameters
returns: string of message*/

char* create_message_to_send(char* type, char** parameters, int num)
{
	char* full_message = NULL;
	int i = 0;
	full_message = (char*)malloc(MAX_PATHS * sizeof(char));
	if (full_message == NULL)
	{
		printf("Malloc error");
		return NULL;
	}
	if (num == 0)
		sprintf(full_message, "CLIENT_%s\n", type);
	else
	{
		sprintf(full_message, "CLIENT_%s:", type);
		for (num != 1; --num;)
		{
			sprintf(full_message, "%s%s;", full_message, parameters[i]);
			i++;
		}
		sprintf(full_message, "%s%s\n", full_message, parameters[i]);
	}
	return full_message;
}

/* param_type:
description – decoding message type and returns number of parameters to analyze
inputs - message type
returns: number of parameters of specific message type*/
int param_type(char* Type)
{
	if ((strcmp(Type, "SERVER_MAIN_MENU") == 0) || (strcmp(Type, "SERVER_APPROVED") == 0) || (strcmp(Type, "SERVER_SETUP_REQUEST") == 0) || (strcmp(Type, "SERVER_PLAYER_MOVE_REQUEST") == 0) || (strcmp(Type, "SERVER_GAME_OVER_MENU") == 0) || (strcmp(Type, "SERVER_NO_OPPONENT") == 0))
	{
		return 0;
	}
	else if ((strcmp(Type, "SERVER_DENIED") == 0) || (strcmp(Type, "SERVER_INVITE") == 0) || (strcmp(Type, "SERVER_OPPONENT_QUIT") == 0) )
	{
		return 1;
	}
	else if ((strcmp(Type, "SERVER_GAME_RESULTS") == 0))
		return 4;
	else
		printf("Message type is not valid\n");
	return -1;
}


/* MessageRecv:
description – parsing incoming message from client/server
inputs - message, MessSplit struct
returns: 0 if correct, 1 otherwise */

int MessageRecv(char* message, MessSplit* MessageStruct)
{
	char* messType = NULL;
	int numParam = 0;
	int type_par_num = 0;
	int mess_len = 0;
	char* mess = NULL;
	char* PtmessParam = NULL;
	messType = strtok(message, ":");
	mess_len = strlen(messType);
	if (messType[mess_len - 1] == '\n')
	{
		messType = chop(message);
		if (messType == NULL)
			return 1;
	}
	else
	{
		//PtmessParam = mess + mess_len + 1;
		type_par_num = param_type(messType);
		while (numParam != type_par_num)
		{
			PtmessParam = strtok(NULL, ";");
			MessageStruct->parameters[numParam] = PtmessParam;
			numParam++;
		}
		numParam = numParam - 1;
		MessageStruct->parameters[numParam] = chop(MessageStruct->parameters[numParam]);
	}
	MessageStruct->parameter_num = type_par_num;
	MessageStruct->type = messType;
	return 0;
}

/*send_message_client:
description – building the actual message to be sent by client
inputs - message type, player move (if needed) and username (if needed)
returns: 0 if worked, 1 otherwise*/

char* send_message_client(char* type, char* player_move, char* username)
{
	char* Param[1] = { NULL };
	char* message_to_send_to_server = NULL;

	if (strcmp(type, "REQUEST") == 0)
	{
		Param[0] = username;
		message_to_send_to_server = create_message_to_send("REQUEST", Param, 1);
	}
	else if (strcmp(type, "MAIN_MENU") == 0)
		message_to_send_to_server = create_message_to_send("MAIN_MENU", NULL, 0);
	
	else if (strcmp(type, "VERSUS") == 0)
		message_to_send_to_server = create_message_to_send("VERSUS", NULL, 0);
	else if (strcmp(type, "PLAYER_MOVE") == 0)
	{
		Param[0] = player_move;
		message_to_send_to_server = create_message_to_send("PLAYER_MOVE", Param, 1);
	}
	else if (strcmp(type, "REPLAY") == 0)
		message_to_send_to_server = create_message_to_send("REPLAY", NULL, 0);
	else if (strcmp(type, "DISCONNECT") == 0)
		message_to_send_to_server = create_message_to_send("DISCONNECT", NULL, 0);
	//printf("%s", message_to_send_to_server);
	return message_to_send_to_server;
}

