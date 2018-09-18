#include "mstdio.h"

#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include <string.h>
#include<memory.h>
#include<malloc.h>
#include<unistd.h>
#include<fcntl.h>

#define BUFFER_LEN 4096


MYFILE* mfopen(const char* const pathname, const char* const mode)
{
	int fd;
	if (!strcmp(mode, "r")){
		fd = open(pathname, O_RDONLY);
	}
	else if (!strcmp(mode, "w")){
		fd = open(pathname, O_WRONLY|O_CREAT|O_TRUNC,0777);
	}
	else if (!strcmp(mode, "a")){
		fd = open(pathname, O_WRONLY | O_CREAT | O_APPEND, 0777);
	}
	else{
		return NULL;
	}
	if (fd < 0) return NULL;
	return mfdopen(fd, mode);
}
int mfclose(MYFILE* fp)
{
	mfflush(fp);
	int res = close(fp->_fd);
	free(fp->_buffer);
	free(fp);
	return res;
}
void mfflush(MYFILE* fp)
{
	if (fp->_mode = READ){
		fp->_nextc = fp->_buffer;
		fp->_left = 0;
	}
	else{
		write(fp->_fd, fp->_buffer,(BUFFER_LEN-(fp->_left)));

		fp->_nextc = fp->_buffer;

		fp->_left = BUFFER_LEN;

	}
}
MYFILE* mfdopen(int fd, const char* const mode)
{
	MYFILE* fp = (MYFILE*)malloc(sizeof(MYFILE));
	assert(fp != NULL);
	fp->_buffer = (char*)malloc(BUFFER_LEN);
	fp->_fd = fd;
	fp->_nextc = fp->_buffer;

	if (!strcmp(mode, "r")){
		fp->_mode = READ;
		fp->_left = 0;
	}
	if (!strcmp(mode, "w")){
		fp->_mode = WRITE;
		fp->_left = BUFFER_LEN;
	}
	if (!strcmp(mode, "a")){
		fp->_mode = APPEND;
		fp->_left = BUFFER_LEN;
	}
	return fp;
}

int mfgetc(MYFILE* fp)
{
	assert(fp->_mode == READ);
	if (fp->_left == 0){
		ssize_t size = read(fp->_fd, fp->_buffer, BUFFER_LEN);
		assert(size >= 0);
		if (size == 0)return MEOF;
		fp->_nextc = fp->_buffer;
		fp->_left = size;
	}
	char c = *(fp->_nextc);
	fp->_nextc++;
	fp->_left--;
	return c;
}
int mfputc(int character, MYFILE* fp)
{
	assert(fp->_mode == WRITE || fp->_mode == APPEND);


	//缓存满就缓存写入文件
	if (fp->_left == 0){
		if (write(fp->_fd, fp->_buffer, BUFFER_LEN)){
			return 0;
		}
		fp->_nextc = fp->_buffer;
		fp->_left = BUFFER_LEN;

	}
	//将字符写入到缓存指定位置
	*(fp->_nextc) = (char)character;
	fp->_nextc++;
	fp->_left--;
	return 1;
}
int mungetc(int character, MYFILE * fp)
{

}
char* mfgets(char* buff, MYFILE* fp)
{
}
int mfputs(char* buff, MYFILE* fp)
{
}

size_t mfread(void* buff, size_t size, size_t counter, MYFILE* fp)
{
}

size_t mfwrite(void *buff, size_t size, size_t counter, MYFILE* fp)
{
}




int main(int argc, char* argv[])
{
	MYFILE* fp1 = mfopen("/etc/passwd", "r");
	assert(fp1 != NULL);

	MYFILE* fp2 = mfopen("mypasswd", "w");
	assert(fp2 != NULL);

	char c;
	while ((c = mfgetc(fp1)) != MEOF){
		mfputc(c, fp2);

	}
	mfclose(fp1);
	mfclose(fp2);


	return 0;
}