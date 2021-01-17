#include "SocketFunctions.h" 
#include "Messages.h"
#include <string.h> 
#include <time.h>
#include <sys/types.h> 
#define MAXIMUM_GAME_RESULT_LENGHT 60
#define MAXIMUM_GAME_WIN_LENGHT 45
#define SERVER_NO_OPPONENT_MASSAGE_LENGHT 20
#define INSERT_NO_OPPONENT 8
#define EMPTY_STRING " ERVER_NO_OPPONENTS"
#define INSERT_NAMES_START_OFFSET_PLAYER_ONE 38
#define INSERT_NAMES_START_OFFSET_PLAYER_TWO 58


//Wait for Threads- Release Parameters And Close or Terminate
void CheckThreadsStatus(PMYDATA THreadDataArguments, HANDLE** p_thread_handles, int NumberOfActiveThreadsForTheProgram) {
    DWORD wait_code;
    BOOL ret_val;
    size_t i;
    if (NumberOfActiveThreadsForTheProgram>1) {
        wait_code = WaitForMultipleObjects(NumberOfActiveThreadsForTheProgram, (*p_thread_handles), TRUE, INFINITE);
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
     
        closesocket(THreadDataArguments->ServerSocket);
        THreadDataArguments->InUse = false;
        if (!(THreadDataArguments->ConnectOnlyForDenied)) {
            (*(THreadDataArguments->NumberOfConnectedPlayers))--;

            (*(THreadDataArguments->PlayerOneFinishedWritingInSetup)) = false;
            (*(THreadDataArguments->PlayerTwoFinishedWritingInSetup)) = false;
            (*(THreadDataArguments->PlayerOneFinishedWritingInPlay)) = false;
            (*(THreadDataArguments->PlayerTwoFinishedWritingInPlay)) = false;

            (*(THreadDataArguments->PlayerOneFinishedReadingInSetup)) = false;
            (*(THreadDataArguments->PlayerTwoFinishedReadingInSetup)) = false;
            (*(THreadDataArguments->PlayerOneFinishedReadingInPlay)) = false;
            (*(THreadDataArguments->PlayerTwoFinishedReadingInPlay)) = false;

            if (THreadDataArguments->PlayerNumber == 1) {
                (*(THreadDataArguments->PlayerOneIsStillConnected)) = false;
            }
            if (THreadDataArguments->PlayerNumber == 2) {
                (*(THreadDataArguments->PlayerTwoIsStillConnected)) = false;
            }

            if (FileExists("GameScore.txt")) {
                remove("GameScore.txt");
                remove("../GameScore.txt");
            }
        }
       

        ret_val = CloseHandle((*p_thread_handles));
        if (false == ret_val)
        {
            printf("Error when closing\n");
            return ERROR_CODE;
        }
    }
   


}

//Check If File Exists
BOOL FileExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

//Create Socket, Bind And Listen
bool CreateSocketBindItAndListen(SOCKET* Socket, struct sockaddr_in* Address, int PortNumber) {
    bool ShouldGracefullyClose = false;
    bool SocketProccess = true;
    int opt = NUMBER_TO_REUSE_ADDRESS;
    if (((*Socket) = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        printf("Error at socket( ): %ld\n", WSAGetLastError());
        goto GracefullyClose;
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
        ShouldGracefullyClose = true;
        goto GracefullyClose;
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

GracefullyClose:
    if (ShouldGracefullyClose) {
        closesocket((*Socket));
    }
}

//Accept Socket Connection
SOCKET AcceptConnection(SOCKET ServerSocket) {
    SOCKET AcceptClientSocket;
    if (((AcceptClientSocket = accept(ServerSocket, NULL, NULL)) == INVALID_SOCKET))
    {
        return SOCKET_ERROR;
    }
    return AcceptClientSocket;
}

//Check if Client Wants To Play Or To Quit
void AcceptChoice(PMYDATA ThreadpointerData, bool* ExitCommand, bool* GameFound) {
    char* ParametersToRecieve = NULL;
    Message ReceivedMessage;
    clock_t TimeElapsed;
    Sleep(1000);
    ReceivedMessage = GetRequest(ThreadpointerData->ServerSocket);
    if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_VERSUS"))
    {
        (*(ThreadpointerData->NumberOfPlayersThatWantToPlay))++;
        if ((*(ThreadpointerData->NumberOfConnectedPlayers)) < 2) {
            SendRequest(ThreadpointerData->ServerSocket, "SERVER_NO_OPPONENTS\n");
            SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
            printf("Send No Opponent \n");
        }
        else {
            TimeElapsed = clock();
            while ((((double)(clock()- TimeElapsed))/CLOCKS_PER_SEC)<=15) {

               
                if (*(ThreadpointerData->NumberOfPlayersThatWantToPlay) == 2) {
                    SendRequest(ThreadpointerData->ServerSocket, "SERVER_INVITE\n");
                    (*GameFound) = true;
                    break;
                }  
               
            }
            if (!(*GameFound)) {
                SendRequest(ThreadpointerData->ServerSocket, "SERVER_NO_OPPONENTS\n");
                SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
               
            }
        }
        (*(ThreadpointerData->NumberOfPlayersThatWantToPlay))--;
    }
    else if (STRINGS_ARE_EQUAL(ReceivedMessage.MessegeType, "CLIENT_DISCONNECT")) {  
        printf("bye For %s\n", ThreadpointerData->ClientName);
        *ExitCommand = true;
    }
}

//Read From Game File
void ReadNumbersFromFile(PMYDATA ThreadpointerData, int PlayerNumber, HANDLE HandleFile, char** Line, int Length, int OffSetForPlayerOne, int OffSetForPlayerTwo) {
    DWORD wait_res_mutex;
    bool ShouldGracefullyClose = false;
    BOOL release_res;
    DWORD dwBytesRead;
    char* LineToRead=(char*)malloc((Length+1)*sizeof(char));
    OVERLAPPED OverLappedRead = { 0 };
    wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileReadMutex, TIMEOUT_IN_MILLISECONDS);
    if (wait_res_mutex != WAIT_OBJECT_0) {
        printf("player %d  mutex error\n", PlayerNumber);
        ShouldGracefullyClose = true;
        goto GracefullyClose;
    }
    if (PlayerNumber == 2) {
        OverLappedRead.Offset = OffSetForPlayerTwo;
        if (HandleFile == 0)
        {
            CloseHandle(HandleFile);
            HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        }
    }
    else if (PlayerNumber == 1) {
        OverLappedRead.Offset = OffSetForPlayerOne;
        if (HandleFile == 0)
        {
            CloseHandle(HandleFile);
            HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        }
    }
    ReadFile(HandleFile, LineToRead, Length, &dwBytesRead, &OverLappedRead);
    LineToRead[Length] = '\0';
    *Line = LineToRead;
    release_res = ReleaseMutex(ThreadpointerData->FileReadMutex);
    if (release_res == FALSE) {
        printf("Cant Relese Mutex\n");
        ShouldGracefullyClose = true;
        goto GracefullyClose;
    }

GracefullyClose:
    if (ShouldGracefullyClose) {
        CloseHandle(HandleFile);
        CheckThreadsStatus(ThreadpointerData, (ThreadpointerData->ThreadHandle), 1);
    }
}

//Write To Game File
void WriteToNumbersFile(PMYDATA ThreadpointerData, int PlayerNumber, HANDLE HandleFile,char* Line, int Length,int OffSetForPlayerOne, int OffSetForPlayerTwo) {
    DWORD wait_res_mutex;
    BOOL release_res;
    DWORD dwBytesWritten;
    bool ShouldGracefullyClose = false;
    OVERLAPPED OverLappedWrite = { 0 };
    wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileWriteMutex, TIMEOUT_IN_MILLISECONDS);
    if (wait_res_mutex != WAIT_OBJECT_0) {
        printf("player %d  mutex error\n", PlayerNumber);
        ShouldGracefullyClose = true;
        goto GracefullyClose;
    }
    if (PlayerNumber == 1) {
        OverLappedWrite.Offset = OffSetForPlayerOne;
        if (HandleFile == 0)
        {
            printf("player %d handle is 0\n", PlayerNumber);
            CloseHandle(HandleFile);
            HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        }
    }
    else if (PlayerNumber == 2) {
        OverLappedWrite.Offset = OffSetForPlayerTwo;
        if (HandleFile == 0)
        {
            printf("player %d handle is 0\n", PlayerNumber);
            CloseHandle(HandleFile);
            HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            
        }
    }
    WriteFile(HandleFile, Line, Length, &dwBytesWritten, &OverLappedWrite);
    release_res = ReleaseMutex(ThreadpointerData->FileWriteMutex);
    if (release_res == FALSE) {
        printf("Cant Relese Mutex\n");
        ShouldGracefullyClose = true;
        goto GracefullyClose;
    }

GracefullyClose:
    if (ShouldGracefullyClose) {
        CloseHandle(HandleFile);
        CheckThreadsStatus(ThreadpointerData, (ThreadpointerData->ThreadHandle), 1);
    }
}

//Pad Name With Spaces
char* PaddNameWithSpace(ClientName) {
    char* PaddedClientName = (char*)malloc(MAXIMUM_NAME_LENGHT * sizeof(char));
    strcpy_s(PaddedClientName, MAXIMUM_NAME_LENGHT, ClientName);
    while (strlen(PaddedClientName)< MAXIMUM_NAME_LENGHT-1) {
        strcat_s(PaddedClientName, MAXIMUM_NAME_LENGHT, "\t");
    }
    return PaddedClientName;
}

//Check If Opponent Still Connected
void IsOpponentStillConnected(PMYDATA ThreadpointerData, bool* OpponnetQuit) {
    if ((*(ThreadpointerData->PlayerOneIsStillConnected)) && (*(ThreadpointerData->PlayerTwoIsStillConnected))) {
        *OpponnetQuit = false;
    }
    else {
        *OpponnetQuit = true;
    }
}

//Set The Game And Write Setup Info To Game File

void SetupTheGame(PMYDATA ThreadpointerData, int PlayerNumber, HANDLE HandleFile, Message ReceivedMessage, bool* OpponnetnQuit) {
    DWORD wait_res_mutex;
    BOOL release_res;
    char* PaddedOpponentName = (char*)malloc(MAXIMUM_NAME_LENGHT * sizeof(char));
    char* PaddedClientName;
    bool ShouldGracefullyClose = false;
    rsize_t strmax = MAXIMUM_NAME_LENGHT;
    char* next_token;
   
    ThreadpointerData->NumberThatIChose = ReceivedMessage.Parameters[0];
    WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile,ThreadpointerData->NumberThatIChose, MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING
        ,0, MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING);
    PaddedClientName=PaddNameWithSpace(ThreadpointerData->ClientName);
    WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, PaddedClientName, MAXIMUM_NAME_LENGHT
       , INSERT_NAMES_START_OFFSET_PLAYER_ONE, INSERT_NAMES_START_OFFSET_PLAYER_TWO);
    WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, EMPTY_STRING, SERVER_NO_OPPONENT_MASSAGE_LENGHT, INSERT_NO_OPPONENT, INSERT_NO_OPPONENT);
    if (PlayerNumber == 1) {
        (*(ThreadpointerData->PlayerOneFinishedWritingInSetup)) = true;
    }
    else if (PlayerNumber == 2) {
        (*(ThreadpointerData->PlayerTwoFinishedWritingInSetup)) = true;
    }
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        return;
    }
    wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileReadMutex, TIMEOUT_IN_MILLISECONDS);
    if (wait_res_mutex != WAIT_OBJECT_0) {
        ShouldGracefullyClose = true;
        goto GracefullyClose;
    }
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        release_res = ReleaseMutex(ThreadpointerData->FileReadMutex);
        if (release_res == FALSE) {
            printf("Cant Relese Mutex\n");
            ShouldGracefullyClose = true;
            goto GracefullyClose;
        }
        return;
    }
    while (!((*(ThreadpointerData->PlayerOneFinishedWritingInSetup)) && (*(ThreadpointerData->PlayerTwoFinishedWritingInSetup)))) {
        IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
        if ((*OpponnetnQuit)) {
            release_res = ReleaseMutex(ThreadpointerData->FileReadMutex);
            if (release_res == FALSE) {
                printf("Cant Relese Mutex\n");
                ShouldGracefullyClose = true;
                goto GracefullyClose;
            }
            return;
        }
    }
    release_res = ReleaseMutex(ThreadpointerData->FileReadMutex);
    if (release_res == FALSE) {
        printf("Cant Relese Mutex\n");
        ShouldGracefullyClose = true;
        goto GracefullyClose;
    }
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        return;
    }
    ReadNumbersFromFile(ThreadpointerData, PlayerNumber, HandleFile, &(ThreadpointerData->NumberThatOtherChose), MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING
        , MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING,0);
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        return;
    }
    ReadNumbersFromFile(ThreadpointerData, PlayerNumber, HandleFile, &PaddedOpponentName, MAXIMUM_NAME_LENGHT
        , INSERT_NAMES_START_OFFSET_PLAYER_TWO, INSERT_NAMES_START_OFFSET_PLAYER_ONE);
    if (PlayerNumber == 1) {
        (*(ThreadpointerData->PlayerOneFinishedReadingInSetup)) = true;
    }
    else if (PlayerNumber == 2) {
        (*(ThreadpointerData->PlayerTwoFinishedReadingInSetup)) = true;
    }
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        return;
    }
    while (!((*(ThreadpointerData->PlayerOneFinishedReadingInSetup)) && (*(ThreadpointerData->PlayerTwoFinishedReadingInSetup)))) {
        IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
        if ((*OpponnetnQuit)) {
            return;
        }
    }
    char* token = strtok_s(PaddedOpponentName, "\t", &next_token);
    strcpy_s(ThreadpointerData->OpponentName, ((strlen(token) + 1) * sizeof(char)), token);
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        return;
    }
    SendRequest(ThreadpointerData->ServerSocket, "SERVER_PLAYER_MOVE_REQUEST\n");
    if (PlayerNumber == 1) {
        (*ThreadpointerData->PlayerOneFinishedWritingInSetup) = false;
        (*ThreadpointerData->PlayerOneFinishedReadingInSetup) = false;
        ThreadpointerData->PlayerOneName = ThreadpointerData->ClientName;
        ThreadpointerData->PlayerTwoName = ThreadpointerData->OpponentName;
    }
    else if (PlayerNumber == 2) {
        (*(ThreadpointerData->PlayerTwoFinishedWritingInSetup)) = false;
        (*(ThreadpointerData->PlayerTwoFinishedReadingInSetup)) = false;
        ThreadpointerData->PlayerOneName = ThreadpointerData->OpponentName;
        ThreadpointerData->PlayerTwoName = ThreadpointerData->ClientName;
    }
    //free NumberThatOtherChose and padded client name and padded opponent

GracefullyClose:
    if (ShouldGracefullyClose) {
        CloseHandle(HandleFile);
        CheckThreadsStatus(ThreadpointerData, (ThreadpointerData->ThreadHandle), 1);
    }

}

//Calculate Number Of Bulls And Cows
void CalculateNumberOfBullsAndCows(char* Guess, char* OriginalNumber, int* NumberOfBulls, int* NumberOfCows) {
    int i = 0;

    bool Guess0 = false;
    bool Guess1 = false;
    bool Guess2 = false;
    bool Guess3 = false;

    bool OriginalNumber0 = false;
    bool OriginalNumber1 = false;
    bool OriginalNumber2 = false;
    bool OriginalNumber3 = false;
    
    if ((OriginalNumber[0] == Guess[0])) {
        Guess0 = true;
        OriginalNumber0 = true;
        (*NumberOfBulls)++;
        
    }
    if ((OriginalNumber[1] == Guess[1])) {
        Guess1 = true;
        OriginalNumber1 = true;
        (*NumberOfBulls)++;
    }
    if ((OriginalNumber[2] == Guess[2])) {
        Guess2 = true;
        OriginalNumber2 = true;
        (*NumberOfBulls)++;
    }
    if ((OriginalNumber[3] == Guess[3])) {
        Guess3 = true;
        OriginalNumber3 = true;
        (*NumberOfBulls)++;
    }
    while (i< MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING) {
        if (i==0 && OriginalNumber0) {
            i++;
        }
        if (i == 1 && OriginalNumber1) {
            i++;
        }
        if (i == 2 && OriginalNumber2) {
            i++;
        }
        if (i == 3 && OriginalNumber3) {
            i++;
        }

       
        if ((OriginalNumber[i] == Guess[0]) && (!Guess0)) {
            Guess0 = true;
            if (i == 0) {
                (*NumberOfBulls)++;
            }
            else {
                (*NumberOfCows)++;
            }
            i++;

        }
        else if ((OriginalNumber[i] == Guess[1]) && (!Guess1)) {
            Guess1 = true;
            if (i == 1) {
                (*NumberOfBulls)++;
            }
            else {
                (*NumberOfCows)++;
            }
            i++;
        }
        else if ((OriginalNumber[i] == Guess[2]) && (!Guess2)) {
            Guess2 = true;
            if (i == 2) {
                (*NumberOfBulls)++;
            }
            else {
                (*NumberOfCows)++;
            }
            i++;
        }
        else if ((OriginalNumber[i] == Guess[3]) && (!Guess3)) {
            Guess3 = true;
            if (i == 3) {
                (*NumberOfBulls)++;
            }
            else {
                (*NumberOfCows)++;
            }
            i++;
        }
        else {
            i++;
        }

    }

}

//Create Result String
char* MakeGameResultString(int NumberOfBulls, int NumberOfCows, char* OtherGuess , char* OpponentName) {
    char* GameResult = (char*)malloc(MAXIMUM_GAME_RESULT_LENGHT * sizeof(char));
    sprintf_s(GameResult, MAXIMUM_GAME_RESULT_LENGHT, "SERVER_GAME_RESULTS:%d;%d;%s;%s\n", NumberOfBulls, NumberOfCows, OpponentName, OtherGuess);
    return GameResult;
}

//Make Win String
char* MakeGameWinString(char* OriginalNumber, char* WinnerName) {
    char* GameResult = (char*)malloc(MAXIMUM_GAME_WIN_LENGHT * sizeof(char));
    sprintf_s(GameResult, MAXIMUM_GAME_WIN_LENGHT, "SERVER_WIN:%s;%s\n", WinnerName, OriginalNumber);
    return GameResult;
}

//Prepare For Score Calculation And Make The Strings
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
        printf("NumberOfBulls %d,  NumberOfCows %d, MyGuess %s, NumberThatOtherChose %s\n", NumberOfBullsForPlayerOne, NumberOfCowsForPlayerOne, MyGuess, NumberThatOtherChose);
        printf("NumberOfBulls %d,  NumberOfCows %d, OtherGuess %s,NumberThatIChose %s\n", NumberOfBullsForPlayerTwo, NumberOfCowsForPlayerTwo, OtherGuess, NumberThatIChose);
    }
    else if (PlayerNumber==2) {
        CalculateNumberOfBullsAndCows(MyGuess, NumberThatOtherChose, &NumberOfBullsForPlayerTwo, &NumberOfCowsForPlayerTwo);
        CalculateNumberOfBullsAndCows(OtherGuess, NumberThatIChose, &NumberOfBullsForPlayerOne, &NumberOfCowsForPlayerOne);
        printf("NumberOfBulls %d,  NumberOfCows %d, MyGuess %s, NumberThatOtherChose %s\n", NumberOfBullsForPlayerTwo, NumberOfCowsForPlayerTwo, MyGuess, NumberThatOtherChose);
        printf("NumberOfBulls %d,  NumberOfCows %d, OtherGuess %s,NumberThatIChose %s\n", NumberOfBullsForPlayerOne, NumberOfCowsForPlayerOne, OtherGuess, NumberThatIChose);
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

// Proccess The Guess While Playing
void PlayTheGameAndProccessGuess(PMYDATA ThreadpointerData, int PlayerNumber, HANDLE HandleFile, Message ReceivedMessage, bool* WeHaveAWinner, bool* OpponnetnQuit) {
    char* OtherGuess;
    char* GameResults;
    char* WinResults;
    DWORD wait_res_mutex;
    BOOL release_res;
    bool PlayerOneIsTheWinner = false;
    bool PlayerTwoIsTheWinner = false;
    bool ShouldGracefullyClose = false;
    


    ThreadpointerData->MyGuess = ReceivedMessage.Parameters[0];
    WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, ThreadpointerData->MyGuess, MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING
        , 0, MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING);
    if (PlayerNumber == 1) {
        (*(ThreadpointerData->PlayerOneFinishedWritingInPlay)) = true;
    }
    else if (PlayerNumber == 2) {
        (*(ThreadpointerData->PlayerTwoFinishedWritingInPlay)) = true;
    }
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        return;
    }
    wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileReadMutex, TIMEOUT_IN_MILLISECONDS);
    if (wait_res_mutex != WAIT_OBJECT_0) {
        ShouldGracefullyClose = true;
        goto GracefullyClose;
    }
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        release_res = ReleaseMutex(ThreadpointerData->FileReadMutex);
        if (release_res == FALSE) {
            printf("Cant Relese Mutex\n");
            ShouldGracefullyClose = true;
            goto GracefullyClose;
        }
        return;
    }
    while (!((*(ThreadpointerData->PlayerOneFinishedWritingInPlay)) && (*(ThreadpointerData->PlayerTwoFinishedWritingInPlay)))) {
        IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
        if ((*OpponnetnQuit)) {
            release_res = ReleaseMutex(ThreadpointerData->FileReadMutex);
            if (release_res == FALSE) {
                printf("Cant Relese Mutex\n");
                ShouldGracefullyClose = true;
                goto GracefullyClose;
            }
            return;
        }
    }
    release_res = ReleaseMutex(ThreadpointerData->FileReadMutex);
    if (release_res == FALSE) {
        printf("Cant Relese Mutex\n");
        ShouldGracefullyClose = true;
        goto GracefullyClose;
    }
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        return;
    }
    ReadNumbersFromFile(ThreadpointerData, PlayerNumber, HandleFile, &(ThreadpointerData->OtherGuess), MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING, MAXIMUM_GUESS_LENGHT_WITH_OUT_END_STRING,0);
    if (PlayerNumber == 1) {
        (*(ThreadpointerData->PlayerOneFinishedReadingInPlay)) = true;
    }
    else if (PlayerNumber == 2) {
        (*(ThreadpointerData->PlayerTwoFinishedReadingInPlay)) = true;
    }
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        return;
    }
    while (!((*(ThreadpointerData->PlayerOneFinishedReadingInPlay)) && (*(ThreadpointerData->PlayerTwoFinishedReadingInPlay)))) {
        IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
        if ((*OpponnetnQuit)) {
            return;
        }
    }
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        return;
    }
    GameResults = CalculateScore(PlayerNumber,&PlayerOneIsTheWinner, &PlayerTwoIsTheWinner, ThreadpointerData->MyGuess, ThreadpointerData->OtherGuess,
        ThreadpointerData->NumberThatIChose, ThreadpointerData->NumberThatOtherChose, ((ThreadpointerData->PlayerOneName)), ((ThreadpointerData->PlayerTwoName)));
    IsOpponentStillConnected(ThreadpointerData, OpponnetnQuit);
    if ((*OpponnetnQuit)) {
        return;
    }
    SendRequest(ThreadpointerData->ServerSocket, GameResults);
    if ((PlayerOneIsTheWinner) && (PlayerTwoIsTheWinner)) {
        *WeHaveAWinner = true;
        SendRequest(ThreadpointerData->ServerSocket, "SERVER_DRAW\n");
       
    }
    else if ((!PlayerOneIsTheWinner) && (PlayerTwoIsTheWinner)) {
        *WeHaveAWinner = true;
        WinResults = MakeGameWinString(ThreadpointerData->NumberThatOtherChose, ((ThreadpointerData->PlayerTwoName)));
        SendRequest(ThreadpointerData->ServerSocket, WinResults);
       
    }
    else if ((PlayerOneIsTheWinner) && (!PlayerTwoIsTheWinner)) {
        *WeHaveAWinner = true;
        WinResults = MakeGameWinString(ThreadpointerData->NumberThatOtherChose, ((ThreadpointerData->PlayerOneName)));
        SendRequest(ThreadpointerData->ServerSocket, WinResults);
        
    }
    else {
        *WeHaveAWinner = false;
        SendRequest(ThreadpointerData->ServerSocket, "SERVER_PLAYER_MOVE_REQUEST\n");
    }
    if (PlayerNumber == 1) {
        (*(ThreadpointerData->PlayerOneFinishedWritingInPlay)) = false;
        (*(ThreadpointerData->PlayerOneFinishedReadingInPlay)) = false;
    }
    else if (PlayerNumber == 2) {
        (*(ThreadpointerData->PlayerTwoFinishedWritingInPlay)) = false;
        (*(ThreadpointerData->PlayerTwoFinishedReadingInPlay)) = false;
    }

GracefullyClose:
    if (ShouldGracefullyClose) {
        CloseHandle(HandleFile);
        CheckThreadsStatus(ThreadpointerData, (ThreadpointerData->ThreadHandle), 1);
    }
    

}

//Check If Opponent Quit Using The File
void CheckIfOpponentQuit(PMYDATA ThreadpointerData, int PlayerNumber, HANDLE HandleFile, bool* OpponnetnQuit) {
    char* Line = (char*)malloc(SERVER_NO_OPPONENT_MASSAGE_LENGHT * sizeof(char));
    ReadNumbersFromFile(ThreadpointerData, PlayerNumber, HandleFile, &Line, SERVER_NO_OPPONENT_MASSAGE_LENGHT, INSERT_NO_OPPONENT, INSERT_NO_OPPONENT);
    if (STRINGS_ARE_EQUAL(Line, "SERVER_NO_OPPONENTS"))
    {
        *OpponnetnQuit = true;

    }
    //free line
}

//Remove Game File
void RemoveFile() {
    if ((((remove("GameScore.txt") != 0))&&(remove("../GameScore.txt") != 0))) {
        printf("Unable to delete the file\n");
    }
    else {
        printf("File Deleted\n");
    }
}


//Start The Actual Game 
void StartGame(PMYDATA ThreadpointerData,int PlayerNumber, HANDLE HandleFile, bool* ExitCommand) {
    Message* ReceivedMessage=(Message*)malloc(sizeof(Message));
    SendRequest(ThreadpointerData->ServerSocket, "SERVER_SETUP_REQUEST\n");
    bool WeHaveAWinner = false;
    bool OpponnetnQuit = false;
    DWORD wait_res_mutex;
    BOOL release_res;
    
    
    IsOpponentStillConnected(ThreadpointerData, &OpponnetnQuit);
    if ((!OpponnetnQuit)) {
        *ReceivedMessage = GetRequest(ThreadpointerData->ServerSocket);
    }
    else {
        SendRequest(ThreadpointerData->ServerSocket, "SERVER_OPPONENT_QUIT\n");
        SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
        WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, "SERVER_NO_OPPONENTS", SERVER_NO_OPPONENT_MASSAGE_LENGHT, INSERT_NO_OPPONENT, INSERT_NO_OPPONENT);
        return;
    }
    IsOpponentStillConnected(ThreadpointerData, &OpponnetnQuit);
    if ((!OpponnetnQuit) ) {
        if (STRINGS_ARE_EQUAL(ReceivedMessage->MessegeType, "CLIENT_SETUP"))//wait? if wrong massage?
        {
            IsOpponentStillConnected(ThreadpointerData, &OpponnetnQuit);
            SetupTheGame(ThreadpointerData, PlayerNumber, HandleFile, *ReceivedMessage, &OpponnetnQuit);
            IsOpponentStillConnected(ThreadpointerData, &OpponnetnQuit);
            while ((!WeHaveAWinner) && (!OpponnetnQuit)) {

                CheckIfOpponentQuit(ThreadpointerData, PlayerNumber, HandleFile, &OpponnetnQuit);
                IsOpponentStillConnected(ThreadpointerData, &OpponnetnQuit);
                if (OpponnetnQuit) {
                    SendRequest(ThreadpointerData->ServerSocket, "SERVER_OPPONENT_QUIT\n");
                    SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
                    WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, "SERVER_NO_OPPONENTS", SERVER_NO_OPPONENT_MASSAGE_LENGHT, INSERT_NO_OPPONENT, INSERT_NO_OPPONENT);
                    return;
                }
                IsOpponentStillConnected(ThreadpointerData, &OpponnetnQuit);
                if ((!OpponnetnQuit)) {
                    *ReceivedMessage = GetRequest(ThreadpointerData->ServerSocket);
                }
                else {
                    SendRequest(ThreadpointerData->ServerSocket, "SERVER_OPPONENT_QUIT\n");
                    SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
                    WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, "SERVER_NO_OPPONENTS", SERVER_NO_OPPONENT_MASSAGE_LENGHT, INSERT_NO_OPPONENT, INSERT_NO_OPPONENT);
                    return;
                }
                IsOpponentStillConnected(ThreadpointerData, &OpponnetnQuit);
                if ((!OpponnetnQuit)) {
                    if (STRINGS_ARE_EQUAL(ReceivedMessage->MessegeType, "CLIENT_PLAYER_MOVE"))
                    {
                        PlayTheGameAndProccessGuess(ThreadpointerData, PlayerNumber, HandleFile, *ReceivedMessage, &WeHaveAWinner, &OpponnetnQuit);
                        IsOpponentStillConnected(ThreadpointerData, &OpponnetnQuit);
                        if ((STRINGS_ARE_EQUAL(ReceivedMessage->MessegeType, "SERVER_OPPONENT_QUIT")) ) {
                            SendRequest(ThreadpointerData->ServerSocket, "SERVER_OPPONENT_QUIT\n");
                            SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
                            WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, "SERVER_NO_OPPONENTS", SERVER_NO_OPPONENT_MASSAGE_LENGHT
                                , INSERT_NO_OPPONENT, INSERT_NO_OPPONENT);
                            *ExitCommand = true;
                            return;
                        }
                    }
                    else if (STRINGS_ARE_EQUAL(ReceivedMessage->MessegeType, "SERVER_OPPONENT_QUIT")) {
                        SendRequest(ThreadpointerData->ServerSocket, "SERVER_OPPONENT_QUIT\n");
                        SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
                        WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, "SERVER_NO_OPPONENTS", SERVER_NO_OPPONENT_MASSAGE_LENGHT
                            , INSERT_NO_OPPONENT, INSERT_NO_OPPONENT);
                        *ExitCommand = true;
                        return;
                    }
                   
                }
                else {
                    SendRequest(ThreadpointerData->ServerSocket, "SERVER_OPPONENT_QUIT\n");
                    SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
                    WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, "SERVER_NO_OPPONENTS", SERVER_NO_OPPONENT_MASSAGE_LENGHT, INSERT_NO_OPPONENT, INSERT_NO_OPPONENT);
                    return;
                }
            }
        }
        else if (STRINGS_ARE_EQUAL(ReceivedMessage->MessegeType, "SERVER_OPPONENT_QUIT")) {
            if (PlayerNumber == 1) {
                (*(ThreadpointerData->PlayerTwoIsStillConnected)) = false;
            }
            if (PlayerNumber == 2) {
                (*(ThreadpointerData->PlayerOneIsStillConnected)) = false;
            }
            SendRequest(ThreadpointerData->ServerSocket, "SERVER_OPPONENT_QUIT\n");
            SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
            WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, "SERVER_NO_OPPONENTS", SERVER_NO_OPPONENT_MASSAGE_LENGHT
                , INSERT_NO_OPPONENT, INSERT_NO_OPPONENT);
            *ExitCommand = true;
            return;
        }
        ///free massage
    }
    else {
        if (PlayerNumber == 1) {
            (*(ThreadpointerData->PlayerTwoIsStillConnected)) = false;
        }
        if (PlayerNumber == 2) {
            (*(ThreadpointerData->PlayerOneIsStillConnected)) = false;
        }
        SendRequest(ThreadpointerData->ServerSocket, "SERVER_OPPONENT_QUIT\n");
        SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");
        WriteToNumbersFile(ThreadpointerData, PlayerNumber, HandleFile, "SERVER_NO_OPPONENTS", SERVER_NO_OPPONENT_MASSAGE_LENGHT, INSERT_NO_OPPONENT, INSERT_NO_OPPONENT);
        return;
    }

   
}


// Check If There Is A File Game, And Give Each Player A Number
void CheckGameFile(PMYDATA ThreadpointerData, bool* ExitCommand, bool* GameFound) {
    DWORD wait_res_mutex;
    BOOL release_res;
    bool ShouldGracefullyClose = false;
    HANDLE HandleFile;
    int PlayerNumber;
    wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileWriteMutex, TIMEOUT_IN_MILLISECONDS);
    if (wait_res_mutex != WAIT_OBJECT_0) {
        printf("Mutex Error\n");
        ShouldGracefullyClose = true;
        goto GracefullyClose;
    }
    if (!FileExists("GameScore.txt")) {
        HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        (*(ThreadpointerData->PlayerOneFileInUse)) = true;
        
        release_res = ReleaseMutex(ThreadpointerData->FileWriteMutex);
        if (release_res == FALSE) {
            printf("Cant Relese Mutex\n");
            ShouldGracefullyClose = true;
            goto GracefullyClose;
        }
        PlayerNumber = 1;
        (*(ThreadpointerData->PlayerOneIsStillConnected)) = true;
        ThreadpointerData->PlayerNumber = 1;
        StartGame(ThreadpointerData, PlayerNumber, HandleFile, ExitCommand);
        CloseHandle(HandleFile);
        (*(ThreadpointerData->PlayerOneFileInUse)) = false;
        *GameFound = false;
        wait_res_mutex = WaitForSingleObject(ThreadpointerData->FileWriteMutex, TIMEOUT_IN_MILLISECONDS);
        if (wait_res_mutex != WAIT_OBJECT_0) {
            printf("Mutex Error\n");
            ShouldGracefullyClose = true;
            goto GracefullyClose;
        }
        release_res = ReleaseMutex(ThreadpointerData->FileWriteMutex);
        if (release_res == FALSE) {
            printf("Cant Relese Mutex\n");
            ShouldGracefullyClose = true;
            goto GracefullyClose;
        }
        
    }
    else {
        HandleFile = CreateFile("GameScore.txt", GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        (*(ThreadpointerData->PlayerTwoFileInUse)) = true;
        release_res = ReleaseMutex(ThreadpointerData->FileWriteMutex);
        if (release_res == FALSE) {
            printf("Cant Relese Mutex\n");
            ShouldGracefullyClose = true;
            goto GracefullyClose;
        }
        PlayerNumber = 2;
        (*(ThreadpointerData->PlayerTwoIsStillConnected)) = true;
        ThreadpointerData->PlayerNumber = 2;
        StartGame(ThreadpointerData, PlayerNumber, HandleFile, ExitCommand);
        CloseHandle(HandleFile);
        (*(ThreadpointerData->PlayerTwoFileInUse)) = false;
        *GameFound = false;
    }

    if (PlayerNumber == 1) {
        while ((*(ThreadpointerData->PlayerTwoFileInUse)) || (*(ThreadpointerData->PlayerOneFileInUse))) {

        }
        RemoveFile();

    }

GracefullyClose:
    if (ShouldGracefullyClose) {
        CheckThreadsStatus(ThreadpointerData, (ThreadpointerData->ThreadHandle), 1);
    }

}

//Get Client And Handle His Requests. 
//From Here We Start Every Thing For The Player
DWORD WINAPI HandleClient(LPVOID lpParam)
{
    PMYDATA ThreadpointerData;
    ThreadpointerData = (PMYDATA)lpParam;
    bool ExitCommand = false;
    bool ClientRequestWasSent = false;
    bool GameFound = false;
    Message ReceivedMessage;
    u_long Mode = 0;
    ioctlsocket(ThreadpointerData->ServerSocket, FIONBIO, &Mode);
   

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
            printf("Game Was Found For %s\n", ThreadpointerData->ClientName);
            CheckGameFile(ThreadpointerData, &ExitCommand, &GameFound);
            SendRequest(ThreadpointerData->ServerSocket, "SERVER_MAIN_MENU\n");

        }
        
       
        
    }
    if (ExitCommand) {
        printf("Closing thread %d for Client %s\n", ThreadpointerData->ThreadNumber, ThreadpointerData->ClientName);
        free(ReceivedMessage.MessegeType);
        CheckThreadsStatus(ThreadpointerData, (ThreadpointerData->ThreadHandle), 1);
        
    }
    
    
    
    
    

    




}

//Denie Client
DWORD WINAPI DenieClient(LPVOID lpParam)
{
    PMYDATA ThreadpointerData;
    ThreadpointerData = (PMYDATA)lpParam;
    u_long Mode = 0;
    ioctlsocket(ThreadpointerData->ServerSocket, FIONBIO, &Mode);
    SendRequest(ThreadpointerData->ServerSocket, "SERVER_DENIED\n");
    Sleep(2000);
    CheckThreadsStatus(ThreadpointerData, (ThreadpointerData->ThreadHandle), 1);
}

//Count Active Client Threads
int NumberOfActiveSockets(PMYDATA ThreadpointerDataArray) {
    int Count = 0;
    for (int i = 0; i < MAX_NUM_OF_ACTIVE_CONNECTIONS; i++) {
        if (ThreadpointerDataArray[i].InUse == true) {
            Count ++;
        
        }
    }
    return Count;
}


//Create Threads For Client
void HandleCommuniction(int PortNumber) {
    SOCKET ServerSocket;
    struct sockaddr_in AddressForServerSocket;
    u_long Mode = 1;
    bool ExitCommand = false;
    bool ShouldGracefullyClose = false;
    char user_input_string[MAXIMUM_NUMBER_OF_PARAMETERS] = { NULL };

    int CurrentThreadNumber=0;
    int NumberOfPlayersThatWantToPlay = 0;
    int NumberOfConnectedPlayers = 0;
    int NumberOfPlayersThatCanConnect = 0;

    bool PlayerOneFinishedWritingInSetup = false;
    bool PlayerTwoFinishedWritingInSetup = false;
    bool PlayerOneFinishedReadingInSetup = false;
    bool PlayerTwoFinishedReadingInSetup = false;
    bool PlayerOneFinishedWritingInPlay = false;
    bool PlayerTwoFinishedWritingInPlay = false;
    bool PlayerOneFinishedReadingInPlay = false;
    bool PlayerTwoFinishedReadingInPlay = false;

    bool PlayerOneFileInUse = false;
    bool PlayerTwoFileInUse = false;
    bool PlayerOneIsStillConnected = false;
    bool PlayerTwoIsStillConnected = false;


    char* PlayerOneName=(char*)malloc(MAXIMUM_NAME_LENGHT * sizeof(char));//free
    char* PlayerTwoName = (char*)malloc(MAXIMUM_NAME_LENGHT * sizeof(char));//free
    
    if (CreateSocketBindItAndListen(&ServerSocket,&AddressForServerSocket,PortNumber)) {
        ioctlsocket(ServerSocket, FIONBIO, &Mode);
        HANDLE* p_thread_handles = (HANDLE*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (MAX_NUM_OF_ACTIVE_CONNECTIONS * sizeof(HANDLE)));
        DWORD* p_thread_ids = (DWORD*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (MAX_NUM_OF_ACTIVE_CONNECTIONS * sizeof(DWORD)));
        PMYDATA ThreadpointerDataArray = (PMYDATA)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (MAX_NUM_OF_ACTIVE_CONNECTIONS * sizeof(MYDATA)));
        HANDLE FileWriteMutex = CreateMutex(NULL, FALSE, NULL);
        HANDLE FileReadMutex = CreateMutex(NULL, FALSE, NULL);
        HANDLE CanReadSemaphore = CreateSemaphore(NULL, 0, MAX_NUM_OF_PLAYERS, NULL);
        

        if (p_thread_handles == NULL || p_thread_ids == NULL
            || ThreadpointerDataArray == NULL || FileWriteMutex == NULL || FileReadMutex == NULL) {
            printf("Malloc Or Mutex Failed\n");
            HeapFree(GetProcessHeap(), NULL, ThreadpointerDataArray);
            HeapFree(GetProcessHeap(), NULL, p_thread_handles);
            HeapFree(GetProcessHeap(), NULL, p_thread_ids);
            ShouldGracefullyClose = true;
            goto GracefullyClose;

        }
        else {
            for (int i = 0; i < MAX_NUM_OF_ACTIVE_CONNECTIONS; i++) {

                ThreadpointerDataArray[i].InUse = false;
            }

            while (!ExitCommand) {//check for exit
                
                if (ThreadpointerDataArray[CurrentThreadNumber].InUse == false) {
                    while (((ThreadpointerDataArray[CurrentThreadNumber].ServerSocket = AcceptConnection(ServerSocket)) == SOCKET_ERROR)&& (!ExitCommand)) {
                        if (NumberOfActiveSockets(ThreadpointerDataArray) == 0) {
                            printf("Type exit to Exit...\nType Anything Else To Accept Connection\nbuffer size is %d dont abuse\n", MAXIMUM_NUMBER_OF_PARAMETERS);
                            scanf_s("%s", user_input_string, MAXIMUM_NUMBER_OF_PARAMETERS);
                            if (STRINGS_ARE_EQUAL(user_input_string, "exit")) {
                                ExitCommand = true;
                            }
                            printf("Waiting for a client to connect...\n");
                            printf("Waiting For New Client to Accept Socket\n");
                        }
                    }
                    if (!ExitCommand) {
                        printf("Client %d Connected.\n", CurrentThreadNumber);
                        ThreadpointerDataArray[CurrentThreadNumber].InUse = true;
                        ThreadpointerDataArray[CurrentThreadNumber].ThreadHandle = &((p_thread_handles)[CurrentThreadNumber]);
                        if (NumberOfActiveSockets(ThreadpointerDataArray) < MAX_NUM_OF_ACTIVE_CONNECTIONS) {
                            ThreadpointerDataArray[CurrentThreadNumber].ConnectOnlyForDenied = false;
                            NumberOfConnectedPlayers++;
                            ThreadpointerDataArray[CurrentThreadNumber].ThreadNumber = CurrentThreadNumber;
                            ThreadpointerDataArray[CurrentThreadNumber].PlayerNumber = 0;
                            ThreadpointerDataArray[CurrentThreadNumber].NumberOfPlayersThatWantToPlay = &NumberOfPlayersThatWantToPlay;
                            ThreadpointerDataArray[CurrentThreadNumber].NumberOfConnectedPlayers = &NumberOfConnectedPlayers;
                            ThreadpointerDataArray[CurrentThreadNumber].FileWriteMutex = FileWriteMutex;
                            ThreadpointerDataArray[CurrentThreadNumber].FileReadMutex = FileReadMutex;

                            ThreadpointerDataArray[CurrentThreadNumber].PlayerOneFinishedWritingInSetup = &PlayerOneFinishedWritingInSetup;
                            ThreadpointerDataArray[CurrentThreadNumber].PlayerTwoFinishedWritingInSetup = &PlayerTwoFinishedWritingInSetup;
                            ThreadpointerDataArray[CurrentThreadNumber].PlayerOneFinishedReadingInSetup = &PlayerOneFinishedReadingInSetup;
                            ThreadpointerDataArray[CurrentThreadNumber].PlayerTwoFinishedReadingInSetup = &PlayerTwoFinishedReadingInSetup;

                            ThreadpointerDataArray[CurrentThreadNumber].PlayerOneFinishedWritingInPlay = &PlayerOneFinishedWritingInPlay;
                            ThreadpointerDataArray[CurrentThreadNumber].PlayerTwoFinishedWritingInPlay = &PlayerTwoFinishedWritingInPlay;
                            ThreadpointerDataArray[CurrentThreadNumber].PlayerOneFinishedReadingInPlay = &PlayerOneFinishedReadingInPlay;
                            ThreadpointerDataArray[CurrentThreadNumber].PlayerTwoFinishedReadingInPlay = &PlayerTwoFinishedReadingInPlay;

                            ThreadpointerDataArray[CurrentThreadNumber].PlayerOneIsStillConnected = &PlayerOneIsStillConnected;
                            ThreadpointerDataArray[CurrentThreadNumber].PlayerTwoIsStillConnected = &PlayerTwoIsStillConnected;

                            ThreadpointerDataArray[CurrentThreadNumber].PlayerOneFileInUse = &PlayerOneFileInUse;
                            ThreadpointerDataArray[CurrentThreadNumber].PlayerTwoFileInUse = &PlayerTwoFileInUse;
                            (p_thread_handles)[CurrentThreadNumber] = CreateThread(NULL, 0, HandleClient, &ThreadpointerDataArray[CurrentThreadNumber], 0, NULL);
                        }
                        else {
                            ThreadpointerDataArray[CurrentThreadNumber].ConnectOnlyForDenied = true;
                            (p_thread_handles)[CurrentThreadNumber] = CreateThread(NULL, 0, DenieClient, &ThreadpointerDataArray[CurrentThreadNumber], 0, NULL);
                        }
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
        ShouldGracefullyClose = true;
        goto GracefullyClose;
    }
GracefullyClose:
    if (ShouldGracefullyClose) {
        closesocket(ServerSocket);
    }
}