#include "SocketFunctions.h" 
#include "Messages.h"
#include <string.h> 

void CheckThreadsStatus(PMYDATA THreadDataArguments, HANDLE** p_thread_handles, int NumberOfActiveThreadsForTheProgram) {
    DWORD wait_code;
    BOOL ret_val;
    size_t i;

    wait_code = WaitForMultipleObjects(NumberOfActiveThreadsForTheProgram, (*p_thread_handles), TRUE, TIMEOUT_IN_MILLISECONDS);

    switch (wait_code)
    {
    case WAIT_OBJECT_0 + 0:
        printf("Finished.\n");
        break;
    case WAIT_TIMEOUT:
        printf("Wait timed out.\n");
        for (i = 0; i < NumberOfActiveThreadsForTheProgram; i++)
        {
            ret_val = TerminateThread((*p_thread_handles)[i], BRUTAL_TERMINATION_CODE);
            if (false == ret_val)
            {
                printf("Error when terminating\n");
                return ERROR_CODE;
            }
        }
        break;

    default:
        printf("Error when waiting");
        return ERROR_CODE;
    }

    
    for (i = 0; i < NumberOfActiveThreadsForTheProgram; i++)
    {
        ret_val = CloseHandle((*p_thread_handles)[i]);
        if (false == ret_val)
        {
            printf("Error when closing\n");
            return ERROR_CODE;
        }
    }


}


bool CreateSocketBindItAndListen(int* Socket, struct sockaddr_in* Address, int PortNumber) {
    bool SocketProccess = true;
    int opt = NUMBER_TO_REUSE_ADDRESS;
    if (((*Socket) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        printf("Error at socket( ): %ld\n", WSAGetLastError());
       // goto server_cleanup_1;
        SocketProccess = false;
    }
    if (setsockopt((*Socket), SOL_SOCKET, SO_REUSEADDR,
        &opt, sizeof(opt)))
    {
        SocketProccess = false;
    }

    (*Address).sin_family = AF_INET;
    (*Address).sin_addr.s_addr = inet_addr(LOCAL_HOST);
    (*Address).sin_port = htons(PortNumber);
   
    if ((*Address).sin_addr.s_addr == INADDR_NONE)
    {
        printf("The string \"%s\" cannot be converted into an ip address. ending program.\n",
            LOCAL_HOST);
        //goto server_cleanup_2;
    }

    if (bind((*Socket), (struct sockaddr*)Address, sizeof(*Address)) == SOCKET_ERROR)
    {
        printf("bind( ) failed with error %ld. Ending program\n", WSAGetLastError());
        SocketProccess = false;
    }
    if (listen((*Socket), SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Failed listening on socket, error %ld.\n", WSAGetLastError());
        SocketProccess = false;
    }
    return SocketProccess;
}

SOCKET AcceptConnection(SOCKET ServerSocket) {
    SOCKET AcceptClientSocket;
    if ((AcceptClientSocket = accept(ServerSocket, NULL, NULL) == INVALID_SOCKET))
    {
        printf("Accepting connection with client failed, error %ld\n", WSAGetLastError());
        //goto
    }

    return AcceptClientSocket;

}

void AcceptChoice(PMYDATA ThreadpointerData) {
    char* ParametersToRecieve = NULL;
    Message ReceivedMessage;
    ReceivedMessage = GetRequest(ThreadpointerData->ServerSocket, &ParametersToRecieve);
    if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_VERSUS"))
    {
        //CheckForAnotherOpponent(); 15 sec
        //true send invite
        //false send SERVER_NO_OPPONENTS
        
    }
    else if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "EXIT")) {  //not exit
       
    }
}


DWORD WINAPI HandleClient(LPVOID lpParam)
{
    PMYDATA ThreadpointerData;
    ThreadpointerData = (PMYDATA)lpParam;
    bool ExitCommand = false;
    bool ClientRequestWasSent = false;
    char* ParametersToRecieve=NULL;
    Message ReceivedMessage;
    
   

    while (!ExitCommand) {
        if (!ClientRequestWasSent) {//check 15 sec

            ReceivedMessage=GetRequest(ThreadpointerData->ServerSocket, &ParametersToRecieve);
            if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_REQUEST"))
            {
                strcpy(ThreadpointerData->ClientName,ReceivedMessage.Parameters[0]);
                ClientRequestWasSent = true;
            }
            else if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "EXIT")) {  //not exit
                ExitCommand = true;
            }
            SendRequest(ThreadpointerData->ServerSocket, "SERVER_APPROVED\n");
        }
        SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
        AcceptChoice(ThreadpointerData);
        //play or quit
        closesocket(ThreadpointerData->ServerSocket);
        //relese thread and message stuff
    }
    
    
    
    
    

    




}
DWORD WINAPI DenieClient(LPVOID lpParam)
{
    PMYDATA ThreadpointerData;
    ThreadpointerData = (PMYDATA)lpParam;
    SendRequest(ThreadpointerData->ServerSocket, "SERVER_DENIED\n");
    closesocket(ThreadpointerData->ServerSocket);
}

int NumberOfActiveSockets(PMYDATA ThreadpointerDataArray) {
    int Count = 0;
    for (int i = 0; i < MAX_NUM_OF_ACTIVE_CONNECTIONS; i++) {
        if (ThreadpointerDataArray[i].InUse == true) {
            Count ++;
        
        }
    }
    return Count;
}



void HandleCommuniction(int PortNumber) {
    SOCKET ServerSocket;
    struct sockaddr_in AddressForServerSocket;
    bool ExitCommand = false;
   
    int CurrentThreadNumber=0;

    if (CreateSocketBindItAndListen(&ServerSocket,&AddressForServerSocket,PortNumber)) {
        HANDLE* p_thread_handles = (HANDLE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (MAX_NUM_OF_ACTIVE_CONNECTIONS * sizeof(HANDLE)));
        DWORD* p_thread_ids = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (MAX_NUM_OF_ACTIVE_CONNECTIONS * sizeof(DWORD)));
        PMYDATA ThreadpointerDataArray = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (MAX_NUM_OF_ACTIVE_CONNECTIONS * sizeof(MYDATA)));

        if (p_thread_handles == NULL || p_thread_ids == NULL
            || ThreadpointerDataArray == NULL) {
            printf("Malloc Or Mutex Failed\n");
            //goto

        }
        else {
            for (int i = 0; i < MAX_NUM_OF_ACTIVE_CONNECTIONS; i++) {

                ThreadpointerDataArray[i].InUse = false;
            }

            while (!ExitCommand) {
                if (ThreadpointerDataArray[CurrentThreadNumber].InUse == false) {
                    printf("Waiting for a client to connect...\n");
                    ThreadpointerDataArray[CurrentThreadNumber].InUse = true;
                    ThreadpointerDataArray[CurrentThreadNumber].ThreadNumber = CurrentThreadNumber;
                    ThreadpointerDataArray[CurrentThreadNumber].ServerSocket = AcceptConnection(ServerSocket);
                    printf("Client %d Connected.\n", CurrentThreadNumber);
                    if (NumberOfActiveSockets(ThreadpointerDataArray)< MAX_NUM_OF_ACTIVE_CONNECTIONS) {
                        (p_thread_handles)[CurrentThreadNumber] = CreateThread(NULL, 0, HandleClient, &ThreadpointerDataArray[CurrentThreadNumber], 0, NULL);
                    }
                    else {
                        (p_thread_handles)[CurrentThreadNumber] = CreateThread(NULL, 0, DenieClient, &ThreadpointerDataArray[CurrentThreadNumber], 0, NULL);
                        CheckThreadsStatus(ThreadpointerDataArray, &((p_thread_handles)[CurrentThreadNumber]), 1);
                    }


                    
                }
                CurrentThreadNumber++;
                if (CurrentThreadNumber== MAX_NUM_OF_ACTIVE_CONNECTIONS) {
                    CurrentThreadNumber = 0;
                } 
            }

            
            CheckThreadsStatus(ThreadpointerDataArray, &p_thread_handles, MAX_NUM_OF_ACTIVE_CONNECTIONS);
            HeapFree(GetProcessHeap(), NULL, ThreadpointerDataArray);
            HeapFree(GetProcessHeap(), NULL, p_thread_handles);
            HeapFree(GetProcessHeap(), NULL, p_thread_ids);

        }
    }
    else {
        printf("Cant Finish Socket Creation\n");
        //goto
    }
}