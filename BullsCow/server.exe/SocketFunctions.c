#include "SocketFunctions.h"

SOCKET CreateSocketBindItAndListen(int PortNumber) {
	SOCKET SocketForClient;

	if (SocketForClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) != INVALID_SOCKET) {
		SOCKADDR_IN SocketForClientAddress;
		SocketForClientAddress.sin_family = AF_INET;
		SocketForClientAddress.sin_addr.s_addr = inet_addr(LOCAL_HOST);
		SocketForClientAddress.sin_port = htons(PortNumber);
		if (bind(SocketForClient, (SOCKADDR*)&SocketForClientAddress, sizeof(SocketForClientAddress))==0) {

		}
		
	}
}