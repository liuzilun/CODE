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
#include <stdlib.h>
#include "XTcp.h"
#include <thread>
#include <string.h>
class TcpThread
{
public:
	void Main()
	{
		char buf[1024] = { 0 };
		for (;;)
		{
			int recvlen = client.Recv(buf, sizeof(buf)-1);
			if (recvlen <= 0) break;
			buf[recvlen] = '\0';
			if (strstr(buf, "quit") != NULL)
			{
				char re[] = "quit success!\n";
				client.Send(re, strlen(re) + 1);
				break;
			}
			int sendlen = client.Send("ok\n", 4);
			printf("recv %s\n", buf);
		}
		client.Close();
		delete this;
	}
	XTcp client;
};
#include <sys/epoll.h>
int main(int argc, char *argv[])
{
	unsigned short port = 8080;
	if (argc > 1)
	{
		port = atoi(argv[1]);
	}
	XTcp server;
	
	//create epollserver.CreateSocket();
	server.Bind(port);
	int epfd = epoll_create(256);

	//注册epoll事件
	struct epoll_event ev;
	ev.data.fd = server.sock;
	//数据接入事件|边缘检测
	ev.events=EPOLLIN|EPOLLET;
	epoll_ctl(epfd,EPOLL_CTL_ADD,server.sock,&ev);
	
	
	struct epoll_event event[20];
	char buf[1024] = {0};
	const char *msg = "HTTP/1.1 200 OK\r\nContent-Length: 1\r\n\r\nX";
	int size = strlen(msg);
	
	
	server.SetBlock(false);
	for (;;)
	{
		//检测事件
		int count = epoll_wait(epfd,event,20,500);
		if(count <=0) continue;
		for(int i = 0; i < count; i++)
		{
			//连接事件发生
			if(event[i].data.fd == server.sock)
			{
				for(;;)
				{
					XTcp client = server.Accept();
					if(client.sock<=0) break;
					
					//新注册客户端事件
					ev.data.fd = client.sock;
					ev.events=EPOLLIN|EPOLLET;
					epoll_ctl(epfd,EPOLL_CTL_ADD,client.sock,&ev);
				}
			}
			else
			{
				XTcp client ;
				client.sock = event[i].data.fd;
				client.Recv(buf,1024);
				client.Send(msg,size);
				//客户端处理完毕，清理事件
				epoll_ctl(epfd,EPOLL_CTL_DEL,client.sock,&ev);
				client.Close();
			}
		}
	}
	server.Close();
	getchar();
	return 0;
}