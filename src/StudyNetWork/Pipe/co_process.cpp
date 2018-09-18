#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<sys/wait.h>


int main(void)
{
	int fda[2], fdb[2];

	if (pipe(fda) < 0 || pipe(fdb) < 0)
	{
		perror("pipe error");
		exit(1);
	}

	pid_t pid;
	pid = fork();
	if (pid < 0){
		perror("fork error");
		exit(1);

	}
	else if (pid == 0)
	{
		//子进程序读父进程数据,累加的结果
		close(fda[1]);
		close(fdb[0]);
		//重定向到管道A的读端
		//add程序中将从管道中读取累加参数x,y

		if (dup2(fda[0], STDIN_FILENO) != STDIN_FILENO){
			perror("son prcess dup2 STDIN_FILENO error");
		}
		
		// 重定向到输出到管道
		if (dup2(fdb[1], STDOUT_FILENO) != STDOUT_FILENO){
			perror("dup2 STDOUT_FILENO error");
		}
	
		if (execlp("./pipe", "./pipe", NULL) < 0)
		{
			perror("execlp error");
			exit(1);
		}


	}
	else{
		//parent process
		//读x,y
		//写入xy

		close(fda[0]);
		close(fdb[1]);

		int x, y;
		printf("please input ;x and y :");
		scanf("%d  %d", &x, &y);


		if (write(fda[1], &x, sizeof(int)) != sizeof(int)){
			perror("write error");
		}
		if (write(fda[1], &y, sizeof(int)) != sizeof(int)){
			perror("write error");
		}

		//读取结果
		int result = 0;
		if (read(fdb[0], &result, sizeof(int)) != sizeof(int))
		{
			perror("read error");

		}
		else
		{
			printf("add result is %d \n ", result);
		}
		close(fda[1]);
		close(fdb[0]);
		wait(0);
	}
}