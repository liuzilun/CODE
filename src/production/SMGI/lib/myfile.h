#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MYFILES_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MYFILES_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MYFILES_EXPORTS
#define MYFILES_API __declspec(dllexport)
#else
#define MYFILES_API __declspec(dllimport)
#endif

#include <windows.h>
#include <string>

#pragma warning (disable : 4251)

using std::string;

namespace Process
{
	class MYFILES_API CMyFile
	{
	public:
		CMyFile(void);
		CMyFile(const char *file_name,
				DWORD open_mode = OPEN_ALWAYS,
				DWORD access = GENERIC_READ | GENERIC_WRITE,
				DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE);

		~CMyFile(void);

		void 	open(const char* file_name = NULL,
					DWORD open_mode = OPEN_ALWAYS,
					DWORD access = GENERIC_READ | GENERIC_WRITE,
					DWORD share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE);
		void 	reopen(void);
		void	close(void);

		bool 	read(void* buff, DWORD &len, DWORD offset = 0);
		bool 	read(void* buff, DWORD len, DWORD &readed, DWORD offset = 0);
		bool 	write(const void* buff, DWORD &len, DWORD offset = 0);
		bool 	append(const void* buff, DWORD &len);
		void 	erase(void);
		DWORD 	size(void);
		void 	flush(void);
		const char* get_filename(void);
	protected:
		OVERLAPPED* get_ol(DWORD offset);		
	private:
		string			_file_name;
		DWORD			_access;
		DWORD			_share_mode;
		HANDLE 	 		_hfile;
		OVERLAPPED 		_overlap;
	};

	class MYFILES_API CCacheFile 
		: public CMyFile
	{
	public:
		CCacheFile(const char* file_name, unsigned long buf_size = 65536,
					unsigned long open_mode = OPEN_ALWAYS,
					unsigned long access = GENERIC_READ | GENERIC_WRITE);

		~CCacheFile(void);

		void push(const char *buf, unsigned long len);
		void flush(void);
	private:
		char			*_buf;
		unsigned long	_max_size;
		unsigned long	_used_size;
	};
}
#pragma warning (default : 4251)