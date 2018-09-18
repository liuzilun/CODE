#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("usage:%s fifo\n", argv[0]);
		exit(1);
	}

	printf("open fifo read...\n");
	
	int fd = open(argv[1], O_RDONLY | O_NONBLOCK);
	if (fd < 0)
	{
		perror("open error");
		exit(1);

	}
	else{
		printf("open file success:%d\n ", fd);

	}
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	while (read(fd, buf, sizeof(buf)) < 0)
	{
		perror("read error");
	}
	printf("%s\n", buf);
	close(fd);
	exit(1);
}