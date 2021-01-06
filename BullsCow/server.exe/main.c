/* File Header: Comments:
* Authors- Saar Zaidfunden -205892334, Jonathan Johansson-308034107
* Project- IOS_EX4 -
Description- 
*/

#include "SocketFunctions.h"
int main(int argc, char* argv[]) {
	if (argc != 2) {
		printf("Number of arguments is not 2!");
	}
	else {
		int PortNumber = atoi(argv[1]);
		WSADATA wsa_data;
		int  WSA_Result;
		SOCKET SocketForClient;
		// Initialize Winsock
		WSA_Result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
		if (WSA_Result != 0) {
			printf("WSAStartup failed: %d\n", WSA_Result);
			return 1;
		}
		else {
			HandleCommuniction(PortNumber);
			
			// Deinitialize Winsock
			WSA_Result = WSACleanup();
			if (WSA_Result != 0) {
				printf("WSACleanup failed: %d\n", WSA_Result);
				return 1;
			}

		}

	}
	



	return 0;
}