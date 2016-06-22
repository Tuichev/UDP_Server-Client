// SERVER.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <algorithm>
#include <set>
#include <iterator>
#include <thread>
#pragma comment (lib, "ws2_32.lib")
#define PORT 8888   


bool operator==(const sockaddr_in& left, const sockaddr_in& right) 
{
	if(  left.sin_addr.S_un.S_addr==right.sin_addr.S_un.S_addr && left.sin_port==right.sin_port)return true;
	else return false;
}

bool operator!=(const sockaddr_in& left, const sockaddr_in& right) 
{
	if(  left.sin_addr.S_un.S_addr!=right.sin_addr.S_un.S_addr || left.sin_port!=right.sin_port)return true;
	else return false;
}

bool operator < (const sockaddr_in& left, const sockaddr_in& right) 
{
	if(  left.sin_addr.S_un.S_addr<right.sin_addr.S_un.S_addr || left.sin_port<right.sin_port)return true;
	else return false;
}


int main(int argc, char* argv[])
{
	char buff[2500];
	printf("UDP Server\n");
	WSADATA WsaData;
	if (WSAStartup(0x202,&WsaData))
	{
		printf("WSAStartup error: %d\n", WSAGetLastError());
		return 0;
	}

	SOCKET my_sock;
	my_sock=socket(AF_INET,SOCK_DGRAM,0);
	if (my_sock==INVALID_SOCKET)
	{
		printf("Socket() error: %d\n",WSAGetLastError());
		WSACleanup();
		return 0;
	}
	//bind
	sockaddr_in local_addr;
	local_addr.sin_family=AF_INET;
	local_addr.sin_addr.s_addr=INADDR_ANY;
	local_addr.sin_port=htons(PORT);

	if (bind(my_sock,reinterpret_cast<sockaddr*>(&local_addr),sizeof(local_addr)))
	{
		printf("bind error: %d\n",WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		return 0;
	}

	std::set<sockaddr_in> clients_addr;
	std::set<sockaddr_in>::iterator it;
	
	while(1)
	{
		sockaddr_in client_addr;

		int client_addr_size = sizeof(client_addr);
		int bsize=recvfrom(my_sock,buff,sizeof(buff)-1,0,reinterpret_cast<sockaddr*>(&client_addr), &client_addr_size);
		if (bsize==SOCKET_ERROR)printf("recvfrom() error: %d\n",WSAGetLastError());

		clients_addr.insert(client_addr);
	
		HOSTENT *hst;
		hst=gethostbyaddr(reinterpret_cast<char*>(&client_addr.sin_addr),4,AF_INET);
		printf("%s [%s:%d]\n",(hst)?hst->h_name:"Unknown host",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

		buff[bsize]=0;
		printf("Wrote:%s\n",buff);
		printf("Number of clients: %d\n",clients_addr.size());
		for (it=clients_addr.begin(); it!=clients_addr.end(); ++it)
		{
			if(client_addr!=(*it))
			{
				printf("\n Send message to: %d  \n", ntohs(it->sin_port));
				sendto(my_sock,buff,bsize,0,(sockaddr*)(&(*it)), sizeof(*it));
			}
		}

	}
	return 0;
}

