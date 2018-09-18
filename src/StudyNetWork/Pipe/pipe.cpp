#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>



int main(void)
{
	//协程管道

	int x, y;
	if (read(STDIN_FILENO, &x, sizeof(int)) < 0)
	{
		perror("read error\n");

	}
	if (read(STDIN_FILENO, &y, sizeof(int)) < 0)
	{
		perror("read error\n");

	}

	int result = x + y;

	if (write(STDOUT_FILENO, &result, sizeof(int)) != sizeof(int))
	{
		perror("write error");
	}


}
	
	
	
	
	
	//char *cmd1[3] = { "/bin/cat", "/etc/passwd", NULL };
	//char *cmd2[3] = { "/bin/grep", "root", NULL };
	////char *cmd2[3] = { "wc", "-l", NULL };
	//	int fd[2];
	//	if (pipe(fd) < 0)
	//	{
	//		perror("pipe error");
	//		exit(1);
	//	}
	//	int i = 0;
	//	pid_t pid;
	//	for (; i < 2; i++)
	//	{
	//		pid = fork();
	//		if (pid < 0)
	//		{
	//			perror("fork error");
	//		}
	//		else if (pid == 0)//child process
	//		{
	//			if (i == 0)//第一个子进程
	//			{
	//				printf("first process create!\n");
	//				close(fd[0]);
	//				//写入数据
	//				//标准输出重定向到管道写端

	//				if (dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO)
	//				{
	//					perror("dup2 error");
	//				}
	//				close(fd[1]);
	//				//调用exec执行命令
	//				//printf("first process call execvp cmd1[0]: %s !\n",cmd1[0]);
	//				if (execvp(cmd1[0], cmd1) < 0)
	//				{
	//					perror("execp error");
	//					exit(1);
	//				}
	//				
	//				break;
	//			}
	//			if (i == 1)//secone process
	//			{   
	//				//读取数据
	//				close(fd[1]);
	//				//标准输出重定向到管道读端
	//				printf("second process call execvp cmd2[0]: %s\n", cmd2[0]);
	//				if (dup2(fd[0], STDIN_FILENO) != STDIN_FILENO)
	//				{
	//					perror("dup2 error");
	//				}
	//				close(fd[0]);
	//				if (execvp(cmd2[0], cmd2) < 0)
	//				{
	//					perror("execvp error");
	//					exit(1);
	//				}
	//				
	//				break;
	//			}
	//		}
	//		else //parant process
	//		{
	//			if (i == 1)
	//			{  
	//				close(fd[0]);
	//				close(fd[1]);
	//				wait(0);
	//				wait(0);
	//			}
	//		}
	//	}
	//}
	//
	//
	
	
	
	//int fd[2];
	//if (pipe(fd) < 0)
	//{
	//	perror("pipe error");
	//	exit(1);
	//}
	//pid_t pid;

	//if ((pid = fork()) < 0)
	//{
	//	perror("fork error");
	//	exit(1);

	//}
	//else if (pid > 0)//父进程写入数据
	//{
	//	close(fd[0]);
	//	int start = 1, end = 100;
	//	if (write(fd[1], &start, sizeof(int)) != sizeof(int))
	//	{
	//		perror("write error");
	//		exit(1);
	//	}
	//	if (write(fd[1], &end, sizeof(int)) != sizeof(int))
	//	{
	//		perror("write error");
	//		exit(1);
	//	}
	//}
	//else
	//{
	//	close(fd[1]);
	//	int start, end;
	//	if (read(fd[0], &start, sizeof(int)) < 0)
	//	{
	//		perror("read error");
	//		exit(1);
	//	}
	//	if (read(fd[0], &end, sizeof(int)) < 0)
	//	{
	//		perror("read error");
	//		exit(1);
	//	}
	//	close(fd[0]);
	//	printf("child process read start :%d,end :%d ", start, end);
	//	exit(1);
	//}
	//return 0;
//}