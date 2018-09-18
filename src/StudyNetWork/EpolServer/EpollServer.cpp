#include <stdio.h>
#include <string.h>

#include "XTcp.h"

#ifdef WIN32
typedef  int socklen_t;
#endif
#include <thread>
#include<sys/epoll.h>


using namespace std;

class TcpThread
{
public:
	void TMain();
	XTcp client;
};

void TcpThread::TMain()
{
	char buf[1024] = { 0 };
	for (;;)
	{
		int lent = client.Recv(buf, sizeof(buf) - 1);
		if (lent <= 0)
		{
			break;
		}
		buf[lent] = '\0';
		if (strstr(buf, "quit") != NULL)
		{
			char re[] = "quit success!";
			client.Send(re, strlen(re) + 1);
			break;
		}
		int sendlen = client.Send("OK\n", 3);

		printf("recv %s \n ", buf);
	}
	client.Close();
	delete this;
}
int main(int argc, char*argv[])
{


	unsigned short port = 8099;
	if (argc >1)
	{
		port = atoi(argv[1]);
	}

	XTcp tcpServer;

	tcpServer.CreateSocket();
	tcpServer.Bind(port);

	//create epoll
	int epfd = epoll_create(256);

	//epoll_ev
	epoll_event ev;
	ev.data.fd = tcpServer.sock;
	ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, tcpServer.sock, &ev);
	epoll_event event[20];
	char buf[1024] = { 0 };
	const char* msg = "Http/1.1 200 OK\r\nContent_Length: 1\r\n\r\nX";
	int size = strlen(msg);
		for (;;)
		{
			int count = epoll_wait(epfd,event, 20, 500);
			//printf("epoll_wait count is %d",count);
			if (count <= 0)
			{
				continue;
			}

			for (int i = 0; i < count; i++)
			{
				
				if (event[i].data.fd == tcpServer.sock)
				{
					printf("new connection add to set");
					XTcp client = tcpServer.Accept();
					ev.data.fd = client.sock;
					ev.events = EPOLLIN | EPOLLET;
					epoll_ctl(epfd, EPOLL_CTL_ADD, client.sock, &ev);

				}
				else
				{
					printf("new event from client recv data");
					XTcp client;
					client.sock = event[i].data.fd;
					memset(buf, 0, sizeof(buf));
					int lent = client.Recv(buf, 1024);

					printf("receive data from client %s,长度 %d\n", buf, lent);
					int len = client.Send(msg, size);
					if (len <= 0)
					{
						printf("发送数据失败！ %s,长度 %d", msg, len);
					}

					printf("发送数据 %s,长度 %d\n", msg, len);
					epoll_ctl(epfd, EPOLL_CTL_DEL, client.sock, &ev);
					client.Close();

				}
			}

			//TcpThread* th = new TcpThread();
			//th->client = client;
			//std::thread sth(&TcpThread::TMain, th);
			//sth.detach();
		}
	tcpServer.Close();

	getchar();
	return 0;
}