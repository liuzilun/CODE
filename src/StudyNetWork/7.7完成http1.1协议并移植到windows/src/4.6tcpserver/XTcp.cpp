/*********************************************************************************************
**                                                                                          **
**                                 Jiedi(China nanjing)Ltd.                                 **
**	                          版权归夏曹俊所有，此代码作为学习参考                          **                                                                            **
*********************************************************************************************/

/*****************************    联系方式   *************************************************
** 购买课程的同学可以加群398872593咨询讨论网络编程技术问题，老师一有时间就会解答您的问题
** 加群请注明购买平台和购买账号，一般一天内就会审核通过
** 如果觉得课程内容对您有帮助，希望给出五星好评，并留下您的意见，我会根据您的意见调整修改课程
** Contact       : xiacaojun@qq.com
**        博客   : http://blog.csdn.net/jiedichina
**        QQ	 : 99981441
**		  QQ群	 ：398872593
**   微信公众号  : jiedi2007
**
********************************************************************************************/
#include "XTcp.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#define socklen_t int
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define  closesocket close
#endif

#include <thread>
using namespace std;

XTcp::XTcp()
{
#ifdef WIN32
	static bool first = true;
	if (first)
	{
		first = false;
		WSADATA ws;
		WSAStartup(MAKEWORD(2, 2), &ws);
	}
#endif
}
int XTcp::CreateSocket()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		printf("create socket failed!\n");
	}
	return sock;
}
bool XTcp::Bind(unsigned short port)
{
	if (sock <= 0)
		CreateSocket();
	sockaddr_in  saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = htonl(0);
	if (::bind(sock, (sockaddr*)&saddr, sizeof(saddr)) != 0)
	{
		printf("bind port %d failed!\n", port);
		return false;
	}
	printf("bind port %d success!\n", port);
	listen(sock, 10);
	return true;
}
int XTcp::Recv(char *buf, int bufsize)
{
	return recv(sock, buf, bufsize, 0);
}
int XTcp::Send(const char *buf, int size)
{
	int s = 0;
	while (s != size)
	{
		int len = send(sock, buf + s, size - s, 0);
		if (len <= 0)break;
		s += len;
	}
	return s;
}
void XTcp::Close()
{
	if (sock <= 0) return;
	closesocket(sock);
}
XTcp XTcp::Accept()
{
	XTcp tcp;
	sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	int client = accept(sock, (sockaddr*)&caddr, &len);
	if (client <= 0) return tcp;
	printf("accept client %d\n", client);
	tcp.ip = inet_ntoa(caddr.sin_addr);
	tcp.port = ntohs(caddr.sin_port);
	tcp.sock = client;
	printf("client ip is %s,port is %d\n", tcp.ip.c_str(), tcp.port);
	return tcp;
}
XTcp::~XTcp()
{
}
