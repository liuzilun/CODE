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
		//�ӽ����������������,�ۼӵĽ��
		close(fda[1]);
		close(fdb[0]);
		//�ض��򵽹ܵ�A�Ķ���
		//add�����н��ӹܵ��ж�ȡ�ۼӲ���x,y

		if (dup2(fda[0], STDIN_FILENO) != STDIN_FILENO){
			perror("son prcess dup2 STDIN_FILENO error");
		}
		
		// �ض���������ܵ�
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
		//��x,y
		//д��xy

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

		//��ȡ���
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