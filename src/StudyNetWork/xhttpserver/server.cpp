#include <stdio.h>
#include <string.h>

#include "XTcp.h"
#include <regex>
#include <sys/types.h>
#include<string>

#ifdef WIN32
typedef  int socklen_t;
#endif
#include <thread>



using namespace std;

class HttpThread
{
public:
	void TMain();
	XTcp client;


};

void HttpThread::TMain()
{
	char buff[10000] = { 0 };
	for (;;)
	{
		//接收HTTP请求
		int recvLen = client.Recv(buff, sizeof(buff) - 1);
		if (recvLen <= 0)
		{
			client.Close();
			delete this;
			return;
		}
		buff[recvLen] = '\0';
		printf("=============recv===============\n%s======================", buff);
		//GET / favicon.ico HTTP / 1.1
		//Accept: */*
		//		 Accept-Encoding: gzip, deflate
		//		 User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko
		//		 Host: 192.168.1.103:8087
		//		 DNT: 1
		//		 Connection: Keep-Alive

		//string src = buff;
		//string pattern = "^([A-Z]+) (.*+) HTTP/1";
		//regex r(pattern);
		//smatch mas;

		//regex_search(src, mas, r);

		//printf("regex len  %d \n", mas.size());

		//if (mas.size() == 0)
		//{
		//	printf("%s failed! \n", pattern.c_str());
		//	client.Close();
		///	delete this;
		//	return;
		//}
		string type = "GET";
		string path = "/";
		if (type != "GET")
		{
			client.Close();
			delete this;
			return;
		}
		string filename;
		if (path == "/")
		{
			filename = "/index.html";
		}
		string filepath = "www";
		filepath += filename;
		FILE* fp = fopen(filepath.c_str(), "rb");
		if (NULL == fp)
		{
			client.Close();
			delete this;
			return;
		}
		fseek(fp, 0, SEEK_END);
		int filesize = ftell(fp);
		fseek(fp, 0, 0);
		printf("file size is %d \n", filesize);






		//回应http客户端请求
		string rmsg = "";
		rmsg = "HTTP/1.1 200 OK\r\n";
		rmsg += "Server: XHttp\r\n";
		rmsg += "Content-Type: text/html\r\n";
		rmsg += "Content-Length: ";
		char bsize[128] = { 0 };
		sprintf(bsize, "%d", filesize);
		rmsg += bsize;
		//rmsg += 
		//rmsg += "10\r\n";
		rmsg += "\r\n\r\n";
		//rmsg += "0123456789";
		//鍙戦�佹秷鎭ご


		int sendSize = client.Send(rmsg.c_str(), rmsg.size());


		printf("===================send=============\n%s\n==================\n", rmsg.c_str());

		//发送正文
		for (;;)
		{
			int len = fread(buff, 1, sizeof(buff), fp);
			if (len <= 0)break;
			int re = client.Send(buff, len);
			if (re <= 0)
			{
				break;
			}
		}
	}
	client.Close();
	delete this;

	
	
	/*	char buf[1024] = { 0 };
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
	//client.Close();
	delete this;*/
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


		HttpThread* th = new HttpThread();
		th->client = client;
		std::thread sth(&HttpThread::TMain, th);
		sth.detach();
	}
	tcpServer.Close();

	getchar();
	return 0;
}