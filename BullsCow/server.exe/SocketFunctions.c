#include "SocketFunctions.h" 
#include "Messages.h"
#include <string.h> 
#include <time.h>
#define MAXIMUM_GAME_RESULT_LENGHT 60
#define MAXIMUM_GAME_WIN_LENGHT 45

void CheckThreadsStatus(PMYDATA THreadDataArguments, HANDLE** p_thread_handles, int NumberOfActiveThreadsForTheProgram) {
    DWORD wait_code;
    BOOL ret_val;
    size_t i;
    if (NumberOfActiveThreadsForTheProgram>1) {
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
    else {

        wait_code = WaitForSingleObject( (*p_thread_handles), TIMEOUT_IN_MILLISECONDS);

        switch (wait_code)
        {
        case WAIT_OBJECT_0 + 0:
            printf("Finished.\n");
            break;
        case WAIT_TIMEOUT:
            printf("Wait timed out.\n");
            ret_val = TerminateThread((*p_thread_handles), BRUTAL_TERMINATION_CODE);
            if (false == ret_val)
            {
                printf("Error when terminating\n");
                return ERROR_CODE;
            }
            break;

        default:
            printf("Error when waiting");
            return ERROR_CODE;
        }

        THreadDataArguments->InUse = false;
        (*(THreadDataArguments->NumberOfConnectedPlayers))--;
        (*(THreadDataArguments->PlayerOneFinishedWriting)) = false;
        (*(THreadDataArguments->PlayerTwoFinishedWriting)) = false;

        ret_val = CloseHandle((*p_thread_handles));
        if (false == ret_val)
        {
            printf("Error when closing\n");
            return ERROR_CODE;
        }
    }
   


}

BOOL FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
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

void AcceptChoice(PMYDATA ThreadpointerData, bool* ExitCommand, bool* GameFound) {
    char* ParametersToRecieve = NULL;
    Message ReceivedMessage;
    clock_t TimeElapsed;
    
    ReceivedMessage = GetRequest(ThreadpointerData->ServerSocket);
    if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_VERSUS"))
    {
        if ((*(ThreadpointerData->NumberOfConnectedPlayers)) < 2) {
            SendRequest(ThreadpointerData->ServerSocket, "SERVER_NO_OPPONENTS\n");
            SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
        }
        else {
            TimeElapsed = clock();
            while ((((double)(clock()- TimeElapsed))/CLOCKS_PER_SEC)<=15) {
                (*(ThreadpointerData->NumberOfPlayersThatWantToPlay))++;
                if (*(ThreadpointerData->NumberOfPlayersThatWantToPlay) == 2) {
                    SendRequest(ThreadpointerData->ServerSocket, "SERVER_INVITE\n");
                    (*(ThreadpointerData->NumberOfPlayersThatWantToPlay))--;
                    (*GameFound) = true;
                    break;
                }  
                (*(ThreadpointerData->NumberOfPlayersThatWantToPlay))--;
            }
            if (!(*GameFound)) {
                SendRequest(ThreadpointerData->ServerSocket, "SERVER_NO_OPPONENTS\n");
                SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
            }
        }

            
        
        
       
    }
    else if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_DISCONNECT")) {  
        *ExitCommand = true;
    }
}

void ReadNumbersFromFile(PMYDATA ThreadpointerData, int PlayerNumber, HANDLE HandleFile, char** Line) {
    DWORD wait_res_mutex;
    BOOL release_res;
    DWORD dwBytesRead;
    wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileReadMutex, TIMEOUT_IN_MILLISECONDS);
    if (wait_res_mutex != WAIT_OBJECT_0) {
        //goto
    }
    if (PlayerNumber == 2) {
        if (HandleFile == 0)
        {
            CloseHandle(HandleFile);
            HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        }
        ReadFile(HandleFile, *Line, MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING, &dwBytesRead, NULL);
    }
    else if (PlayerNumber == 1) {
        OVERLAPPED OverLappedRead = { 0 };
        OverLappedRead.Offset = MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING;
        if (HandleFile == 0)
        {
            CloseHandle(HandleFile);
            HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

        }
        ReadFile(HandleFile, *Line, MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING, &dwBytesRead, &OverLappedRead);//maybe strcpy?
    }


    release_res = ReleaseMutex(ThreadpointerData->FileReadMutex);
    if (release_res == FALSE) {
        printf("Cant Relese Mutex\n");
        //goto
    }


}

void WriteToNumbersFile(PMYDATA ThreadpointerData, int PlayerNumber, HANDLE HandleFile,char* Line) {
    DWORD wait_res_mutex;
    BOOL release_res;
    DWORD dwBytesWritten;
    wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileWriteMutex, TIMEOUT_IN_MILLISECONDS);
    if (wait_res_mutex != WAIT_OBJECT_0) {
        //goto
    }
    if (PlayerNumber == 1) {
        if (HandleFile == 0)
        {
            CloseHandle(HandleFile);
            HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        }
        WriteFile(HandleFile, Line, MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING, &dwBytesWritten, NULL);
    }
    else if (PlayerNumber == 2) {
        OVERLAPPED OverLappedWrite = { 0 };
        OverLappedWrite.Offset = MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING;
        if (HandleFile == 0)
        {
            CloseHandle(HandleFile);
            HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            
        }
        WriteFile(HandleFile, Line, MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING, &dwBytesWritten, &OverLappedWrite);
    }
    

    release_res = ReleaseMutex(ThreadpointerData->FileWriteMutex);
    if (release_res == FALSE) {
        printf("Cant Relese Mutex\n");
        //goto
    }
}

void SetupTheGame(PMYDATA ThreadpointerData, int PlayerNumber, HANDLE HandleFile, Message ReceivedMessage) {
    DWORD wait_res_mutex;
    BOOL release_res;
    strcpy_s(ThreadpointerData->NumberThatIChose, MAXIMUM_GUESS_LENGHT, ReceivedMessage.Parameters[0]);
    WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile,ThreadpointerData->NumberThatIChose);
    if (PlayerNumber == 1) {
        (*(ThreadpointerData->PlayerOneFinishedWriting)) = true;
    }
    else if (PlayerNumber == 2) {
        (*(ThreadpointerData->PlayerTwoFinishedWriting)) = true;
    }
    wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileReadMutex, TIMEOUT_IN_MILLISECONDS);
    if (wait_res_mutex != WAIT_OBJECT_0) {
        //goto
    }
    while (!((*(ThreadpointerData->PlayerOneFinishedWriting)) && (*(ThreadpointerData->PlayerTwoFinishedWriting)))) {
        if ((*(ThreadpointerData->PlayerOneFinishedWriting)) && (*(ThreadpointerData->PlayerTwoFinishedWriting))) {
            release_res = ReleaseMutex(ThreadpointerData->FileReadMutex);
            if (release_res == FALSE) {
                printf("Cant Relese Mutex\n");
                //goto
            }
        }
    }
    ReadNumbersFromFile(ThreadpointerData, PlayerNumber, HandleFile, &(ThreadpointerData->NumberThatOtherChose));
    SendRequest(ThreadpointerData->ServerSocket, "SERVER_PLAYER_MOVE_REQUEST\n");
    if (PlayerNumber == 1) {
        (*ThreadpointerData->PlayerTwoFinishedWriting) = false;
    }
    else if (PlayerNumber == 2) {
        (*(ThreadpointerData->PlayerTwoFinishedWriting)) = false;
    }
    //free NumberThatOtherChose

}
void CalculateNumberOfBullsAndCows(char* Guess, char* OriginalNumber, int* NumberOfBulls, int* NumberOfCows) {
    int i = 0;
    int j = 0;
    while (i< MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING) {
        if (OriginalNumber[i]==Guess[i]) {
            (*NumberOfBulls)++;
            i++;
            j = 0;
        }
        else if (OriginalNumber[i] == Guess[j]) {
            (*NumberOfCows)++;
            i++;
            j = 0;
        }
        else {
            j++;
            if (j== MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING) {
                j = 0;
                i++;
            }
        }

    }

}

char* MakeGameResultString(int NumberOfBulls, int NumberOfCows, char* OtherGuess , char* OpponentName) {
    char GameResult[MAXIMUM_GAME_RESULT_LENGHT];
    sprintf_s(GameResult, MAXIMUM_GAME_RESULT_LENGHT, "SERVER_GAME_RESULTS:%d;%d;%s;%s\n", NumberOfBulls, NumberOfCows, OpponentName, OtherGuess);
    return GameResult;
}

char* MakeGameWinString(char* OriginalNumber, char* WinnerName) {
    char GameResult[MAXIMUM_GAME_WIN_LENGHT];
    sprintf_s(GameResult, MAXIMUM_GAME_WIN_LENGHT, "SERVER_WIN:%s;%s\n", WinnerName, OriginalNumber);
    return GameResult;
}

char* CalculateScore(int PlayerNumber,bool* PlayerOneIsTheWinner, bool* PlayerTwoIsTheWinner, char* MyGuess, char* OtherGuess,
    char* NumberThatIChose, char* NumberThatOtherChose,char* PlayerOneName, char* PlayerTwoName) {
    int NumberOfBullsForPlayerOne = 0;
    int NumberOfCowsForPlayerOne = 0;
    int NumberOfBullsForPlayerTwo = 0;
    int NumberOfCowsForPlayerTwo = 0;
    char* GameResult=NULL;
    
    if (PlayerNumber==1) {
        CalculateNumberOfBullsAndCows(MyGuess, NumberThatOtherChose,&NumberOfBullsForPlayerOne,&NumberOfCowsForPlayerOne);
        CalculateNumberOfBullsAndCows(OtherGuess, NumberThatIChose, &NumberOfBullsForPlayerTwo, &NumberOfCowsForPlayerTwo);
    }
    else if (PlayerNumber==2) {
        CalculateNumberOfBullsAndCows(MyGuess, NumberThatOtherChose, &NumberOfBullsForPlayerTwo, &NumberOfCowsForPlayerTwo);
        CalculateNumberOfBullsAndCows(OtherGuess, NumberThatIChose, &NumberOfBullsForPlayerOne, &NumberOfCowsForPlayerOne);
    }
    if (NumberOfBullsForPlayerOne== MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING) {
        *PlayerOneIsTheWinner = true;
    }
    if (NumberOfBullsForPlayerTwo == MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING) {
        *PlayerTwoIsTheWinner = true;
    }
    if (PlayerNumber == 1) {
        GameResult= MakeGameResultString(NumberOfBullsForPlayerOne, NumberOfCowsForPlayerOne, OtherGuess, PlayerTwoName);
    }
    else if (PlayerNumber == 2) {
        GameResult = MakeGameResultString(NumberOfBullsForPlayerTwo, NumberOfCowsForPlayerTwo, OtherGuess, PlayerOneName);
    }
    return GameResult;

}

void PlayTheGameAndProccessGuess(PMYDATA ThreadpointerData, int PlayerNumber, HANDLE HandleFile, Message ReceivedMessage, bool* WeHaveAWinner) {
    char* OtherGuess;
    char* GameResults;
    char* WinResults;
    DWORD wait_res_mutex;
    BOOL release_res;
    bool PlayerOneIsTheWinner = false;
    bool PlayerTwoIsTheWinner = false;

    strcpy_s(ThreadpointerData->MyGuess, MAXIMUM_GUESS_LENGHT, ReceivedMessage.Parameters[0]);
    WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, ThreadpointerData->MyGuess);
    if (PlayerNumber == 1) {
        (*(ThreadpointerData->PlayerOneFinishedWriting)) = true;
    }
    else if (PlayerNumber == 2) {
        (*(ThreadpointerData->PlayerTwoFinishedWriting)) = true;
    }
    wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileReadMutex, TIMEOUT_IN_MILLISECONDS);
    if (wait_res_mutex != WAIT_OBJECT_0) {
        //goto
    }
    while (!((*(ThreadpointerData->PlayerOneFinishedWriting)) && (*(ThreadpointerData->PlayerTwoFinishedWriting)))) {
        if ((*(ThreadpointerData->PlayerOneFinishedWriting)) && (*(ThreadpointerData->PlayerTwoFinishedWriting))) {
            release_res = ReleaseMutex(ThreadpointerData->FileReadMutex);
            if (release_res == FALSE) {
                printf("Cant Relese Mutex\n");
                //goto
            }
        }
    }
    ReadNumbersFromFile(ThreadpointerData, PlayerNumber, HandleFile, &(ThreadpointerData->OtherGuess));
    GameResults = CalculateScore(PlayerNumber,&PlayerOneIsTheWinner, &PlayerTwoIsTheWinner, ThreadpointerData->MyGuess, ThreadpointerData->OtherGuess,
        ThreadpointerData->NumberThatIChose, ThreadpointerData->NumberThatOtherChose, (*(ThreadpointerData->PlayerOneName)), (*(ThreadpointerData->PlayerTwoName)));
    SendRequest(ThreadpointerData->ServerSocket, GameResults);
    if (PlayerOneIsTheWinner && PlayerTwoIsTheWinner) {
        *WeHaveAWinner = true;
        SendRequest(ThreadpointerData->ServerSocket, "SERVER_DRAW\n");
    }
    else if (!PlayerOneIsTheWinner && PlayerTwoIsTheWinner) {
        *WeHaveAWinner = true;
        WinResults = MakeGameWinString(ThreadpointerData->NumberThatOtherChose, (*(ThreadpointerData->PlayerTwoName)));
        SendRequest(ThreadpointerData->ServerSocket, WinResults);
    }
    else if (PlayerOneIsTheWinner && !PlayerTwoIsTheWinner) {
        *WeHaveAWinner = true;
        WinResults = MakeGameWinString(ThreadpointerData->NumberThatOtherChose, (*(ThreadpointerData->PlayerOneName)));
        SendRequest(ThreadpointerData->ServerSocket, WinResults);
    }
    else {
        *WeHaveAWinner = false;
        SendRequest(ThreadpointerData->ServerSocket, "SERVER_PLAYER_MOVE_REQUEST\n");
        if (PlayerNumber == 1) {
            (*(ThreadpointerData->PlayerOneFinishedWriting)) = false;
        }
        else if (PlayerNumber == 2) {
            (*(ThreadpointerData->PlayerTwoFinishedWriting)) = false;
        }
    }

}

void StartGame(PMYDATA ThreadpointerData,int PlayerNumber, HANDLE HandleFile, bool* ExitCommand) {
    Message ReceivedMessage;
    SendRequest(ThreadpointerData->ServerSocket, "SERVER_SETUP_REQUEST\n");
    ReceivedMessage = GetRequest(ThreadpointerData->ServerSocket);
    bool WeHaveAWinner = false;
    bool OpponnetnQuit = false;
    DWORD wait_res_mutex;
    BOOL release_res;
    
    if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_SETUP"))//wait? if wrong massage?
    {
        SetupTheGame(ThreadpointerData, PlayerNumber, HandleFile, ReceivedMessage);
        while (!WeHaveAWinner && !OpponnetnQuit) {
            //check from file if opponent has quit
            ReceivedMessage = GetRequest(ThreadpointerData->ServerSocket);
            if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_PLAYER_MOVE"))
            {
                PlayTheGameAndProccessGuess(ThreadpointerData, PlayerNumber, HandleFile, ReceivedMessage, &WeHaveAWinner);

            }
            else if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_DISCONNECT")) {
                //write file server no opponent
                ExitCommand = true;
            }
            if (OpponnetnQuit) {
                SendRequest(ThreadpointerData->ServerSocket, "SERVER_OPPONENT_QUIT\n");
                SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
            }
        }
    }
    else if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_DISCONNECT")) {
        ExitCommand = true;
    }
    
}



void CheckGameFile(PMYDATA ThreadpointerData, bool* ExitCommand) {
    DWORD wait_res_mutex;
    BOOL release_res;
    wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileWriteMutex, TIMEOUT_IN_MILLISECONDS);
    if (wait_res_mutex != WAIT_OBJECT_0) {
        //goto
    }
    if (!FileExists("GameScore.txt")) {
        HANDLE HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
        release_res = ReleaseMutex(ThreadpointerData->FileWriteMutex);
        if (release_res == FALSE) {
            printf("Cant Relese Mutex\n");
            //goto
        }
        int PlayerNumber = 1;
        strcpy_s((*(ThreadpointerData->PlayerOneName)), MAXIMUM_NAME_LENGHT, ThreadpointerData->ClientName);
        StartGame(ThreadpointerData, PlayerNumber, HandleFile, ExitCommand);
        CloseHandle(HandleFile);
        wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileWriteMutex, TIMEOUT_IN_MILLISECONDS);
        if (wait_res_mutex != WAIT_OBJECT_0) {
            //goto
        }
        if (remove("GameScore.txt") != 0) {
            printf("Unable to delete the file");
            //goto
        }
        release_res = ReleaseMutex(ThreadpointerData->FileWriteMutex);
        if (release_res == FALSE) {
            printf("Cant Relese Mutex\n");
            //goto
        }
        
    }
    else {
        wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileWriteMutex, TIMEOUT_IN_MILLISECONDS);
        HANDLE HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        release_res = ReleaseMutex(ThreadpointerData->FileWriteMutex);
        if (release_res == FALSE) {
            printf("Cant Relese Mutex\n");
            //goto
        }
        int PlayerNumber = 2;
        strcpy_s((*(ThreadpointerData->PlayerTwoName)), MAXIMUM_NAME_LENGHT, ThreadpointerData->ClientName);
        StartGame(ThreadpointerData, PlayerNumber, HandleFile, ExitCommand);
        CloseHandle(HandleFile);
    }

}

DWORD WINAPI HandleClient(LPVOID lpParam)
{
    PMYDATA ThreadpointerData;
    ThreadpointerData = (PMYDATA)lpParam;
    bool ExitCommand = false;
    bool ClientRequestWasSent = false;
    bool GameFound = false;
    Message ReceivedMessage;
    
   

    while (!ExitCommand) {
        if (!ClientRequestWasSent) {//check 15 sec

            ReceivedMessage=GetRequest(ThreadpointerData->ServerSocket);
            if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_REQUEST"))
            {
                strcpy_s(ThreadpointerData->ClientName, MAXIMUM_NAME_LENGHT, ReceivedMessage.Parameters[0]);
                ClientRequestWasSent = true;
                SendRequest(ThreadpointerData->ServerSocket, "SERVER_APPROVED\n");
                SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
            }
            else if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_DISCONNECT")) {  
                ExitCommand = true;
            }  
        }
        AcceptChoice(ThreadpointerData, &ExitCommand, &GameFound);
        if (GameFound) {
            CheckGameFile(ThreadpointerData, &ExitCommand);
            SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");

        }
        
       
        
    }
    if (ExitCommand) {

        closesocket(ThreadpointerData->ServerSocket);
        free(ReceivedMessage.MessegeType);
        CheckThreadsStatus(ThreadpointerData, (ThreadpointerData->ThreadHandle), 1);
        
    }
    
    
    
    
    

    




}
DWORD WINAPI DenieClient(LPVOID lpParam)
{
    PMYDATA ThreadpointerData;
    ThreadpointerData = (PMYDATA)lpParam;
    SendRequest(ThreadpointerData->ServerSocket, "SERVER_DENIED\n");
    closesocket(ThreadpointerData->ServerSocket);
    CheckThreadsStatus(ThreadpointerData, (ThreadpointerData->ThreadHandle), 1);
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
    int NumberOfPlayersThatWantToPlay = 0;
    int NumberOfConnectedPlayers = 0;
    bool PlayerOneFinishedWriting = false;
    bool PlayerTwoFinishedWriting = false;
    char PlayerOneName[MAXIMUM_GUESS_LENGHT];
    char PlayerTwoName[MAXIMUM_NAME_LENGHT];
    
    if (CreateSocketBindItAndListen(&ServerSocket,&AddressForServerSocket,PortNumber)) {
        HANDLE* p_thread_handles = (HANDLE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (MAX_NUM_OF_ACTIVE_CONNECTIONS * sizeof(HANDLE)));
        DWORD* p_thread_ids = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (MAX_NUM_OF_ACTIVE_CONNECTIONS * sizeof(DWORD)));
        PMYDATA ThreadpointerDataArray = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (MAX_NUM_OF_ACTIVE_CONNECTIONS * sizeof(MYDATA)));
        HANDLE FileWriteMutex = CreateMutex(NULL, FALSE, NULL);
        HANDLE FileReadMutex = CreateMutex(NULL, FALSE, NULL);
        HANDLE CanReadSemaphore = CreateSemaphore(NULL, 0, MAX_NUM_OF_PLAYERS, NULL);
        

        if (p_thread_handles == NULL || p_thread_ids == NULL
            || ThreadpointerDataArray == NULL || FileWriteMutex == NULL || FileReadMutex == NULL) {
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
                    NumberOfConnectedPlayers++;
                    ThreadpointerDataArray[CurrentThreadNumber].ThreadNumber = CurrentThreadNumber;
                    ThreadpointerDataArray[CurrentThreadNumber].NumberOfPlayersThatWantToPlay = &NumberOfPlayersThatWantToPlay;
                    ThreadpointerDataArray[CurrentThreadNumber].NumberOfConnectedPlayers = &NumberOfConnectedPlayers;
                    ThreadpointerDataArray[CurrentThreadNumber].FileWriteMutex = FileWriteMutex;
                    ThreadpointerDataArray[CurrentThreadNumber].FileReadMutex = FileReadMutex;
                    ThreadpointerDataArray[CurrentThreadNumber].PlayerOneFinishedWriting = &PlayerOneFinishedWriting;
                    ThreadpointerDataArray[CurrentThreadNumber].PlayerTwoFinishedWriting = &PlayerTwoFinishedWriting;
                    ThreadpointerDataArray[CurrentThreadNumber].PlayerOneName = &PlayerOneName;
                    ThreadpointerDataArray[CurrentThreadNumber].PlayerOneName = &PlayerTwoName;
                    ThreadpointerDataArray[CurrentThreadNumber].ThreadHandle = &((p_thread_handles)[CurrentThreadNumber]);
                    ThreadpointerDataArray[CurrentThreadNumber].ServerSocket = AcceptConnection(ServerSocket);
                    printf("Client %d Connected.\n", CurrentThreadNumber);
                    if (NumberOfActiveSockets(ThreadpointerDataArray)< MAX_NUM_OF_ACTIVE_CONNECTIONS) {
                        (p_thread_handles)[CurrentThreadNumber] = CreateThread(NULL, 0, HandleClient, &ThreadpointerDataArray[CurrentThreadNumber], 0, NULL);
                    }
                    else {
                        (p_thread_handles)[CurrentThreadNumber] = CreateThread(NULL, 0, DenieClient, &ThreadpointerDataArray[CurrentThreadNumber], 0, NULL);
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