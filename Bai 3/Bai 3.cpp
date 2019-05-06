// Bai 3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <string>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

WIN32_FIND_DATAA DATA;
DWORD WINAPI ClientThread(LPVOID);
void CreateSendBuffer();
bool checkPath(char * buffer);
void getPath(char *path);
void repstr(char *str, char c);
void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);

char sendBuffer[1024 * 5];
char WindowPath[128];
char URLPath[128];
char lastPath[128];

int main()
{
	std::cout << "Hello World!\n";
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	int port = 9000;
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);
	printf("Khoi dong server thanh cong\n");

	while (true) {
		SOCKET client = accept(listener, NULL, NULL);
		printf("%d connected\n", client);
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
	return 0;
}
DWORD WINAPI ClientThread(LPVOID lpParameter) {
	SOCKET ClientSocket = *(SOCKET *)lpParameter;
	char recvBuffer[1024];
	while (true) {
		int ret = recv(ClientSocket, recvBuffer, sizeof(recvBuffer), 0);
		if (ret <= 0) {
			cout << ClientSocket << " disconnected cause ret < 0\n";
			break;
		}
		recvBuffer[ret] = 0;

		bool correctPath = checkPath(recvBuffer);

		printf("Current URL path: %s\n", URLPath);
		const char *header = "HTTP/1.1 200 OK\nContent-Type: text/html; charset = utf - 8\r\n\r\n";
		CreateSendBuffer();
		printf("Current Window path: %s\n", WindowPath);
		if (correctPath == true) {
			send(ClientSocket, header, strlen(header), 0);
			send(ClientSocket, sendBuffer, strlen(sendBuffer), 0);
		}
		else {
			char msg[1024] = "<html><body><h1>Sai duong dan</h1></body></html>";
			send(ClientSocket, header, strlen(header), 0);
			send(ClientSocket, msg, strlen(msg), 0);
			cout << ClientSocket << " disconnected cause wrong message\n";
			break;
		}
		cout << ClientSocket << " after 1 while loop\n";
	}
	closesocket(ClientSocket);
	return 0;
}

bool checkPath(char *buffer) {
	char cmd[8];
	char path[128];
	char type[16];
	int ret = sscanf(buffer, "%s %s %s", cmd, path, type);
	if (ret == 3) {
		if (strcmp(cmd, "GET") == 0) {
			if (strcmp(type, "HTTP/1.1") == 0) {

				getPath(path);
				return true;
			}
			else return false;
		}
		else return false;
	}
	else return false;
}

void getPath(char *path) {
	//Chuyen "%20" sang dau cach
	string pathString(path);
	ReplaceStringInPlace(pathString, "%20", " ");
	memset(path, 0, sizeof(path));
	strcpy(path, pathString.c_str());

	memset(URLPath, 0, sizeof(URLPath));
	strcat(URLPath, path);

	//Chuyen / sang \\
	repstr(path, '\\');
	memset(WindowPath, 0, sizeof(WindowPath));
	strcat(WindowPath, "C:");
	strcat(WindowPath, path);
	if (strcmp(path, "\\") == 0) {
		strcat(WindowPath, "*.*");
	}
	else
		strcat(WindowPath, "\\*.*");

}

void CreateSendBuffer() {
	memset(sendBuffer, 0, sizeof(sendBuffer));

	const char *boldStartTag = "<b>";
	const char *boldEndTag = "</b>";
	const char *italicStartTag = "<i>";
	const char *italicEndTag = "</i>";
	const char *lineBreakTag = "<br>";
	const char *attributeEndTag = "</a>";

	strcat(sendBuffer, "<html><body>");
	HANDLE h = FindFirstFileA(WindowPath, &DATA);

	strcat(sendBuffer, "Index: ");
	strcat(sendBuffer, WindowPath);
	strcat(sendBuffer, lineBreakTag);
	while (FindNextFileA(h, &DATA)) {
		if (DATA.dwFileAttributes &
			FILE_ATTRIBUTE_DIRECTORY) {
			strcat(sendBuffer, boldStartTag);
			strcat(sendBuffer, "<a href=\"");
			strcat(sendBuffer, URLPath);
			strcat(sendBuffer, DATA.cFileName);
			strcat(sendBuffer, "/");
			strcat(sendBuffer, "\">");
			strcat(sendBuffer, DATA.cFileName);
			strcat(sendBuffer, attributeEndTag);
			strcat(sendBuffer, boldEndTag);
			strcat(sendBuffer, lineBreakTag);
		}
		else {
			//Tao duong dan cho file
			strcat(sendBuffer, italicStartTag);
			strcat(sendBuffer, "<a href=\"");
			strcat(sendBuffer, URLPath);
			strcat(sendBuffer, DATA.cFileName);
			strcat(sendBuffer, "\">");
			strcat(sendBuffer, DATA.cFileName);
			strcat(sendBuffer, attributeEndTag);
			strcat(sendBuffer, italicEndTag);
			strcat(sendBuffer, lineBreakTag);
		}
	};

	strcat(sendBuffer, "</body></html>");
}

void repstr(char *str, char c) {
	char *p = str;
	while (*p != NULL) {
		if (*p == '/') {
			*p = c;
		}
		p++;
	}
}

void ReplaceStringInPlace(std::string& subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}
