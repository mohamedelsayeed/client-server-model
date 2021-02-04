#include <iostream>
#include <string>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

void main() {
	string ipAddress = "127.0.0.1";
	int port = 54000;

	//Initialize Winsock
	WSADATA data;
	int wsResult = WSAStartup(MAKEWORD(2, 2), &data);
	if (wsResult != 0) {
		cerr << "Can't start Winsock, Err # " << wsResult << endl;
		return;
	}

	//Create Socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		cerr << "Can't create scoket, Err# " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// Fill in a hint structure
	struct sockaddr_in hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.sin_family = AF_INET;
	hints.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hints.sin_addr);

	// Connect to server
	int connResult = connect(sock, (struct sockaddr*)&hints, sizeof(hints));
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server, Err #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	//Send & Receive data
	char buff[4096];
	string userInput;

	do {
		cout << "> ";
		getline(cin, userInput);
		if (userInput.size() > 0) {
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR) {
				ZeroMemory(buff, 4096);
				int bytesReceiver = recv(sock, buff, 4096, 0);
				if (bytesReceiver > 0) {
					cout << "Server> " << string(buff, 0, bytesReceiver) << endl;
				}
			}
		}
	} while (userInput.size() > 0);
	closesocket(sock);
	WSACleanup();

}