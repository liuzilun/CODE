#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;
int main(int argc, char *argv[])
{
	unsigned short port = 8080;
	if (argc > 1)
	{
		port = atoi(argv[1]);
	}

	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == -1)
	{
		printf("create socket failed!\n");
	}

	sockaddr_in  saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	sendto(sock,"12345",6,0,(struct sockaddr *)&saddr,sizeof(saddr));  
	sockaddr_in  sin;
	socklen_t sin_len = sizeof(sin);
	memset(&sin,0,sizeof(sin));
	char buf[1024] = {0};
	recvfrom(sock,buf,sizeof(buf),0,(struct sockaddr *)&sin,&sin_len);  
    printf("Response from server:%s\n",buf);  
	close(sock);
}