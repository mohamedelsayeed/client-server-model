#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

void main() {

	// Initialize Winsock
	WSADATA wsData;
	int wsResult = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (wsResult != 0) {
		cerr << "Can't start Winsock, Err # " << wsResult << endl;
		return;
	}
	// Fill in a hint structure
	struct sockaddr_in hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.sin_family = AF_INET;
	hints.sin_port = htons(54000);
	hints.sin_addr.S_un.S_addr = INADDR_ANY;

/*	// Resolve the server address and port
	struct addrinfo* result = NULL;
	wsResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (wsResult != 0) {
		cerr << "getaddrinfo failed with error: " << wsResult << endl;
		WSACleanup();
		return;
	}
	*/

	//Create Socket
	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ListenSocket == INVALID_SOCKET) {
		cerr << "Can't create scoket, Err# " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// Setup the TCP listening socket
	wsResult = bind(ListenSocket, (sockaddr*)&hints, sizeof(hints));
	if (wsResult == SOCKET_ERROR) {
		cerr << "bind failed with error :" << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	wsResult = listen(ListenSocket, SOMAXCONN);
	if (wsResult == SOCKET_ERROR) {
		cerr << "listen failed with error :" << WSAGetLastError() << endl;
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
	// Accept a client socket
	struct sockaddr_in client;
	int clientsize = sizeof(client);

	SOCKET ClientSocket = accept(ListenSocket, (sockaddr*)&client, &clientsize);
	if (ClientSocket == INVALID_SOCKET) {
		cerr << "accept failed with Err# " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	char host[NI_MAXHOST];		// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e. port) the client is connect on

	ZeroMemory(host, NI_MAXHOST); 
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected on port " << service << endl;
	}

	//close server socket no longer needed
	closesocket(ListenSocket);

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;


	//receive
	/*while (true) {
		wsResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (wsResult > 0) {
			cout << "Bytes received: " << string(recvbuf, 0, wsResult) << endl;
		}
		// Echo the buffer back to the sender
		iSendResult = send(ClientSocket, recvbuf, wsResult, 0);
		if (iSendResult == SOCKET_ERROR) {
			cerr << "send failed with error :" << WSAGetLastError() << endl;
			closesocket(iSendResult);
			WSACleanup();
			return;
		}
		cout << "Bytes sent: " << string(recvbuf, 0, iSendResult) << endl;
		if (wsResult == 0) {
			cout << "Connection is closing" << endl;
		}
		else {
			cout << "recv failed with error " << WSAGetLastError << endl;
			closesocket(ClientSocket);
			WSACleanup();
		}
	}*/
	char buff[4096];

	while (true)
	{
		ZeroMemory(buff, 4096);

		// Wait for client to send data
		int bytesReceived = recv(ClientSocket, buff, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv(). Quitting" << endl;
			break;
		}

		if (bytesReceived == 0)
		{
			cout << "Client disconnected " << endl;
			break;
		}
		
		cout <<" Recieved " <<string(buff, 0, bytesReceived) << endl;

		// Echo message back to client
		send(ClientSocket, buff, bytesReceived + 1, 0);

	}
	//Shutdown connection
	wsResult = shutdown(ClientSocket, SD_SEND);
	if (ClientSocket == SOCKET_ERROR) {
		cerr << "shutdown failed with error :" << WSAGetLastError() << endl;
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}
	//clean up
	closesocket(ClientSocket);
	WSACleanup();
	return ;


}