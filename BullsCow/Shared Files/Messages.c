#include "Messages.h"
#include <stdio.h>
#include <string.h>


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
	int TotalStringSizeInBytes;
	TransferResult_t SendRes;


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


TransferResult_t ReceiveString(char** OutputStrPtr, SOCKET sd)
{
	/* Recv the the request to the server on socket sd */
	int TotalStringSizeInBytes;
	TransferResult_t RecvRes;
	char* StrBuffer = NULL;

	if ((OutputStrPtr == NULL) || (*OutputStrPtr != NULL))
	{
		printf("The first input to ReceiveString() must be "
			"a pointer to a char pointer that is initialized to NULL. For example:\n"
			"\tchar* Buffer = NULL;\n"
			"\tReceiveString( &Buffer, ___ )\n");
		return TRNS_FAILED;
	}

	RecvRes = ReceiveBuffer(
		(char*)(&TotalStringSizeInBytes),
		(int)(sizeof(TotalStringSizeInBytes)),
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

	return RecvRes;
}

int CountNumberOfParameters(char* ReceivedMessageString) {
	int Count = 0;
	for (int i = 0; i <strlen(ReceivedMessageString); i++) {
		if ((ReceivedMessageString[i] == ';') || (ReceivedMessageString[i] == ':')) {
			Count++;
		}
	}
	return Count;
}

Message GetRequest(SOCKET ServerSocket) {
	TransferResult_t RecvRes;
	char* ReceivedMessageString = NULL;
	Message* ReceivedMessage;
	ReceivedMessage =(Message*) malloc(sizeof(Message));
	RecvRes = ReceiveString(&ReceivedMessageString, ServerSocket);

	if (RecvRes == TRNS_FAILED)
	{
		printf("RecvRes == TRNS_FAILED\n");
		ReceivedMessage->NumberOfParameters = 0;
		ReceivedMessage->MessegeType = "SERVER_OPPONENT_QUIT";
	}
	else if (RecvRes == TRNS_DISCONNECTED)
	{
		printf("RecvRes == TRNS_DISCONNECTED\n");
		ReceivedMessage->NumberOfParameters = 0;
		ReceivedMessage->MessegeType = "SERVER_OPPONENT_QUIT";
	}
	else
	{
		ReceivedMessage->NumberOfParameters = CountNumberOfParameters(ReceivedMessageString);
		char* next_token = NULL;
		char* token = NULL;
		token = strtok_s(ReceivedMessageString, "\n", &next_token);
		ReceivedMessage->MessegeType = strtok_s(token, ":", &next_token);
		if (ReceivedMessage->NumberOfParameters > 0) {
			for (int i = 0; i < ReceivedMessage->NumberOfParameters; i++) {
				ReceivedMessage->Parameters[i] = strtok_s(NULL, ";", &next_token);

			}
		}
	}
	
	
	
	//free(ReceivedMessageString);
	return(*ReceivedMessage);

}

void SendRequest(SOCKET ServerSocket,char* MessageToSend) {
	TransferResult_t SendRes;
	SendRes = SendString(MessageToSend, ServerSocket);

	if (SendRes == TRNS_FAILED)
	{
		printf("Service socket error while writing, closing thread.\n");
		closesocket(ServerSocket);
	}

}