// Client.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <thread>
#pragma comment (lib, "ws2_32.lib")
#define PORT 8888
#define SERVERADDR "127.0.0.1"

int test(SOCKET my_sock);

int main(int argc, char* argv[])
{
	char buff[2500];
	printf("UDP CHAT");
	WSADATA WsaData;
	if (WSAStartup(0x202,&WsaData))
	{
		printf("WSAStartup error: %d\n",WSAGetLastError());
		return -1;
	}

	SOCKET my_sock=socket(AF_INET, SOCK_DGRAM, 0);
	if (my_sock==INVALID_SOCKET)
	{
		printf("socket() error: %d\n",WSAGetLastError());
		WSACleanup();
		return -1;
	}

	HOSTENT *hst;
	sockaddr_in dest_addr;
	dest_addr.sin_family=AF_INET;
	dest_addr.sin_port=htons(PORT);
	if (inet_addr(SERVERADDR))
		dest_addr.sin_addr.s_addr=inet_addr(SERVERADDR);
	else
		if (hst=gethostbyname(SERVERADDR))
			dest_addr.sin_addr.s_addr=((unsigned long **)
			hst->h_addr_list)[0][0];
		else
		{
			printf("Unknown host: %d\n",WSAGetLastError());
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}

		printf("\nEnter your name:");
		std::cin.getline(buff,sizeof(buff));
		char mess[2500];
		strcpy_s(mess,buff);
		strcat_s(mess,": ");
		strcat_s(buff,", was enter to chat.");
		sendto(my_sock,buff,strlen(buff),0,reinterpret_cast<sockaddr*>(&dest_addr),sizeof(dest_addr));

		std::thread thr(test,my_sock);
		char buff_for_mess[2500];
		strcpy_s(buff_for_mess,mess);
		while(1)
		{		
			printf("You:");
			std::cin.getline(buff,sizeof(buff));
			strcat_s(mess,buff);
			sendto(my_sock,mess,strlen(mess),0,reinterpret_cast<sockaddr*>(&dest_addr),sizeof(dest_addr));
			strcpy_s(mess,buff_for_mess);
		}

		closesocket(my_sock);
		WSACleanup();
		thr.join();
		return -1;
}

int test(SOCKET my_sock)
{
	while(1){
		char buff[2500];
		sockaddr_in server_addr;
		int server_addr_size=sizeof(server_addr);
		int n=recvfrom(my_sock,buff,sizeof(buff)-1,0,reinterpret_cast<sockaddr*>(&server_addr), &server_addr_size);
		if (n==SOCKET_ERROR)
		{
			printf("recvfrom() error: %d\n",WSAGetLastError());
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}
		buff[n]=0;
		printf("\n%s",buff);
	}
}