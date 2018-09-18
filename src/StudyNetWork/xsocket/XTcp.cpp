// XTcp.cpp: implementation of the XTcp class.
//
//////////////////////////////////////////////////////////////////////

#include "XTcp.h"

#include <string.h>

#ifdef WIN32

#include <winsock.h>
#include <windows.h>
#include <stdlib.h>

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define closesocket close
#endif      
#include <stdio.h>

#ifdef WIN32
typedef  int socklen_t;
#endif

using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XTcp::XTcp()
{
#ifdef WIN32
	static bool first =true;
	if (first)
	{
		first =true;
		WSADATA ws;
	    WSAStartup(MAKEWORD(2,2),&ws);
	}

#endif
}

XTcp::~XTcp()
{

}
int XTcp::CreateSocket()
{
	printf("create socket first !\n");
	if (sock = 0)
	{
		printf("socket <=0 !\n");
		CreateSocket();
	}
	
	sock = socket(AF_INET,SOCK_STREAM,0);
	printf("create socket =%d  !\n",sock);
	if(sock <=0)
	{
		printf("create socket failed !\n");
		return -1;
	}
	return 0;
}
bool XTcp::Bind(unsigned short port)
{

	//bind
	sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htons(0); 
	
	printf("bind sock is %d port %d \n",sock,port);

	if(0!=::bind(sock,(sockaddr*)&saddr,sizeof(saddr)))
	{
		printf("bind port %d failed \n",port);
		return false;
	}
	printf("bind port %d success !\n",port);

	//listen
	listen(sock,10);

	return true;
}

XTcp XTcp::Accept()
{ 
	XTcp tcp;
	sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	
	int client = accept(sock,(sockaddr*)&caddr,&len);

	tcp.sock = client;
	
	if (client<=0)
	{
		return tcp;
	}
	//accept
	printf("accept client %d\n ");
	char * ip = inet_ntoa(caddr.sin_addr);
	strcpy(tcp.ip, ip);
	tcp.port = ntohs(caddr.sin_port);
    printf("client ip %s port  %d \n",tcp.ip,tcp.port);

    return tcp;

}
void XTcp::Close()
{
   if (sock <= 0)
   {
	  return;
   }
    closesocket(sock);
}
int XTcp::Recv(char* buf,int bufsize)
{
    return recv(sock,buf,bufsize,0);
}
int XTcp::Send(const char* buf,int size)
{
    int s = 0;
	while (s!=size)
	{
		int len =send(sock,buf+s,size-s,0);
		if (len <= 0)
		{
			break;
		}
		s+=len;  
	}
   return s;

}
bool XTcp::Connect(const char* ip, unsigned short port,int timeout)
{
	if (sock <= 0)
	{
		CreateSocket();
	}

	sockaddr_in saddr;
	saddr.sin_family = AF_INET; 
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);
	SetBlock(false);
	  
	fd_set set;
	if (connect(sock, (sockaddr*)&saddr, sizeof(saddr)) != 0)
	{
		FD_ZERO(&set);
		FD_SET(sock, &set);
		timeval tm;

		tm.tv_sec = 0;
		tm.tv_usec = timeout * 1000;
		if (select(sock + 1, 0, &set, 0, &tm) <= 0)
		{

			printf("connect timeout or error\n");
			return false;
		}
	}
	SetBlock(true);
	printf("connect %s:%d success! %s\n", ip, port, strerror(errno));
	return true;
}

bool XTcp::SetBlock(bool isblock){
	if (sock <= 0)
	{
		return false;
	}
	#ifdef WIN32
		unsigned long ul = 0;
		if (!isblock) ul = 1;
		ioctlsocket(sock, FIONBIO, &ul);
	#else
	int flags = fcntl(sock,F_GETFL,0);
	if (flags < 0) return false;

	if (isblock)
	{
		flags = flags&~O_NONBLOCK;
	}
	else
	{
		flags = flags | O_NONBLOCK;
	}
	if(fcntl(sock, F_SETFL, flags)!=0)return false;
	return true; 
#endif
}
