// Baitap1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// 127.0.0.1:9000

#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

WIN32_FIND_DATAA DATA;
DWORD WINAPI ClientThread(LPVOID);
void CreateSendBuffer();
bool checkPath(char * buffer);


char sendBuffer[1024];

int main()
{
	std::cout << "Hello World!\n";
	int port = 9000;
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);
	printf("Khoi dong server thanh cong\n");

	CreateSendBuffer();

	while (true) {
		SOCKET client = accept(listener, NULL, NULL);
		printf("New client connected: %d\n", client);
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
	return 0;
}

DWORD WINAPI ClientThread(LPVOID lpParameter) {
	SOCKET ClientSocket = *(SOCKET *)lpParameter;
	char recvBuffer[1024];
	while (true) {
		int ret = recv(ClientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (ret < 0) {
			cout << ClientSocket << " disconnected" << endl;
			break;
		}
		recvBuffer[ret] = 0;
		bool correctPath = checkPath(recvBuffer);
		const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset = utf - 8\r\n\r\n";
		if (correctPath == true) {
			send(ClientSocket, header, strlen(header), 0);
			send(ClientSocket, sendBuffer, strlen(sendBuffer), 0);
		}
		else {
			char msg[1024] = "<html><body><h1>Sai duong dan</h1></body></html>";
			send(ClientSocket, header, strlen(header), 0);
			send(ClientSocket, msg, strlen(msg), 0);
			closesocket(ClientSocket);
			cout << ClientSocket << " disconnected" << endl;
		}

		closesocket(ClientSocket);

		cout << ClientSocket << " disconnected" << endl;
	}

	return 0;
}

void CreateSendBuffer() {
	const char *boldStartTag = "<b>";
	const char *boldEndTag = "</b>";
	const char *italicStartTag = "<i>";
	const char *italicEndTag = "</b>";
	const char *lineBreakTag = "<br>";

	strcat(sendBuffer, "<html><body>");
	HANDLE h = FindFirstFileA("C:\\*.*", &DATA);
	do {
		if (DATA.dwFileAttributes &
			FILE_ATTRIBUTE_DIRECTORY) {
			strcat(sendBuffer, boldStartTag);
			strcat(sendBuffer, DATA.cFileName);
			strcat(sendBuffer, boldEndTag);
			strcat(sendBuffer, lineBreakTag);
		}
		else {
			strcat(sendBuffer, italicStartTag);
			strcat(sendBuffer, DATA.cFileName);
			strcat(sendBuffer, italicEndTag);
			strcat(sendBuffer, lineBreakTag);
		}
	} while (FindNextFileA(h, &DATA));

	strcat(sendBuffer, "</body></html>");
	cout << "Khoi tao trang web thanh cong" << endl;
}

bool checkPath(char *buffer) {
	char cmd[8];
	char path[16];
	char type[16];
	int ret = sscanf(buffer, "%s %s %s", cmd, path, type);
	if (ret == 3) {
		if (strcmp(cmd, "GET") == 0) {
			if (strcmp(type, "HTTP/1.1") == 0) {
				if (strcmp(path, "/") == 0) {
					return true;
				}
				else return false;
			}
			else return false;
		}
		else return false;
	}
	else return false;
}