#include <stdlib.h>
#include "XTcp.h"
#include <thread>
#include <string.h>
#include <string>
#include <regex>
using namespace std;
class HttpThread
{
public:
	void Main()
	{
		char buf[10000] = {0};
		for(;;)
		{
			//接受http客户端请�?
			int recvLen = client.Recv(buf,sizeof(buf)-1);
			if(recvLen <= 0)
			{
				Close();
				return ;
			}
			buf[recvLen] = '\0';
			printf("=======recv=========\n%s===================\n",buf);

			
		//GET /index.html HTTP/1.1
		//Host: 192.168.3.69
		//User-Agent: Mozilla/5.0 (Windows NT 10.0; WOW64; rv:51.0) Gecko/20100101 Fi
		//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
		//Accept-Language: zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3
		//Accept-Encoding: gzip, deflate
		//DNT: 1
		//Connection: keep-alive
		//Upgrade-Insecure-Requests: 1
			/*string src = buf;
			string pattern = "^([A-Z]+) (.+) HTTP/1";
			regex r(pattern);
			smatch mas;
			regex_search(src,mas,r);
			if(mas.size() == 0)
			{
				printf("%s failed!\n",pattern.c_str());
				Close();
				return ;
			}*/
			string type = "GET";
			string path = "/";
			if(type != "GET")
			{
				Close();
				return ;
			}
			string filename = path;
			if(path == "/")
			{
				filename = "/index.html";
			}

			string filepath = "www";
			filepath += filename;
			FILE *fp = fopen(filepath.c_str(),"rb");
			if(fp == NULL)
			{
				Close();
				return;
			}
			//获取文件大小
			fseek(fp,0,SEEK_END);
			int filesize = ftell(fp);
			fseek(fp,0,0);
			printf("file size is %d\n",filesize);

			//回应http GET请求
			//消息�?
			string rmsg = "";
			rmsg = "HTTP/1.1 200 OK\r\n";
			rmsg += "Server: XHttp\r\n";
			rmsg += "Content-Type: text/html\r\n";
			rmsg += "Content-Length: ";
			char bsize[128] = {0};
			sprintf(bsize,"%d",filesize);
			rmsg += bsize ;
			//rmsg += 
			//rmsg += "10\r\n";
			rmsg += "\r\n\r\n";
			//rmsg += "0123456789";
			//发送消息头
			int sendSize = client.Send(rmsg.c_str(),rmsg.size());
			printf("sendsize = %d\n",sendSize);
			printf("=======send=========\n%s\n=============\n",rmsg.c_str());

			//发送正�?
			for(;;)
			{
				int len = fread(buf,1,sizeof(buf),fp);
				if(len <=0)break;
				int re = client.Send(buf,len);
				if(re<=0)break;
			}
		}
		Close();
	}
	void Close()
	{
		client.Close();
		delete this;
	}
	XTcp client;
};

int main(int argc, char *argv[])
{
	unsigned short port = 8080;
	if (argc > 1)
	{
		port = atoi(argv[1]);
	}
	XTcp server;
	server.CreateSocket();
	server.Bind(port);
	for (;;)
	{
		XTcp client = server.Accept();
		HttpThread *th = new HttpThread();
		th->client = client;
		std::thread sth(&HttpThread::Main, th);
		sth.detach();
	}
	server.Close();
	getchar();
	return 0;
}