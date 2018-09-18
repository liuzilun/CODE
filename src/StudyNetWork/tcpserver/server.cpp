#include <stdio.h>
#include <string.h>

#include "XTcp.h"

#ifdef WIN32
typedef  int socklen_t;
#endif
#include <thread>


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

	for (;;)
	{

		XTcp client = tcpServer.Accept();


		TcpThread* th = new TcpThread();
		th->client = client;
		std::thread sth(&TcpThread::TMain, th);
		sth.detach();
	}
	tcpServer.Close();

	getchar();
	return 0;
}