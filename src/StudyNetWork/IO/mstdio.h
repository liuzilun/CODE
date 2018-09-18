#ifndef __MSTDIO_H__
#define __MSTDIO_H__
#define MEOF    -1

#include<sys/types.h>
enum mode{READ,WRITE,APPEND};
typedef struct
{
	int    _fd;
	char*  _buffer;
	char*  _nextc;
	int    _mode;
	off_t  _left;
}MYFILE;


extern MYFILE* mfopen(const char* const tpathname, const char* const mode);
extern int mfclose(MYFILE* fp);
extern void mfflush(MYFILE* fp);
extern MYFILE* mfdopen(int fd, const char* const mode);


extern int mfgetc(MYFILE* fp);
extern int mfputc(int character, MYFILE* fp);
extern int mungetc(int character, MYFILE * fp);
extern char* mfgets(char* buff, MYFILE* fp);
extern int mfputs(char* buff, MYFILE* fp);

extern size_t mfread(void* buff, size_t size, size_t counter, MYFILE* fp);

extern size_t mfwrite(void *buff, size_t size, size_t counter, MYFILE* fp);





















#endif