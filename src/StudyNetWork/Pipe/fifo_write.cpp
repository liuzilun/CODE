#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("usage: %s fifo\n", argv[0]);
		exit(1);
	}

	int fd = open(argv[1], O_WRONLY);
	if (fd < 0)
	{
		perror("open error");
		exit(1);
	}
	else
	{
		printf("open fifo success:%d \n", fd);

	}

	const char* s = "1234567890";
	size_t size = strlen(s);
	if (write(fd, s, size) != size)
	{
		printf("write error");
		perror("write error");
		exit(1);
	}
	close(fd);

	exit(0);
}