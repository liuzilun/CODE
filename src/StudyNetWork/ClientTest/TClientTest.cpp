// TClientTest.cpp : Defines the entry point for the console application.
//
//
//#include <winsock.h>
//#include <windows.h>
//#include <stdlib.h>
//#include <stdio.h>
//
//#pragma comment(lib,"ws2_32.lib")

#include"XTcp.h"

int main(int argc, char* argv[])
{
	
	////初始化
	//
	//WSADATA wsdata;
 //   WSAStartup(MAKEWORD(2,2),&wsdata);
	//
	////建立套接字
	//
	//SOCKET hClientsocket = socket(AF_INET,SOCK_STREAM,0);
	////绑定
	//sockaddr_in sockaddin = {0};
 //   
	//sockaddin.sin_family = AF_INET;
	//sockaddin.sin_port = htons(9908);
	//sockaddin.sin_addr.S_un.S_addr = inet_addr("192.168.1.104"); 
	//
	//if(SOCKET_ERROR == connect(hClientsocket,(SOCKADDR*)&sockaddin,sizeof(sockaddin)))
	//{
	//	printf("connect failed !:%u \n ",WSAGetLastError());
	//	closesocket(hClientsocket);
	//	WSACleanup();

	//	return -1;
	//}

 //   char buf[MAX_PATH] = {0};

	//while(TRUE)
	//{
	//	printf("请输入要发送的内容：");
	//	gets(buf);
	//	send(hClientsocket,buf,sizeof(buf),0);
	//	memset(buf,0,sizeof(buf));
	//	recv(hClientsocket,buf,sizeof(buf),0);
	//	printf("%s \n -------------------------------\n",buf);

	//}
	XTcp client;    

	client.CreateSocket();
	client.SetBlock(false);
	client.Connect("192.168.1.103", 8083,3000);

	//client.Send("client IP 1 port 8083", 32);

	//char buff[1024] = { 0 }; 

	//client.Recv(buff, sizeof(buff));

	//printf("receive message from server %s", buff);
	char buf[1024] = { 0 };
	while(true)
	{
		printf("请输入要发送的内容：");
		gets(buf);

		int len = sizeof(buf);
		client.Send(buf, sizeof(buf));
		memset(buf,0,sizeof(buf));
		client.Recv(buf, sizeof(buf));
		
		printf("%s \n -------------------------------\n",buf);

	}
	system("pause");
	return 0;
}

