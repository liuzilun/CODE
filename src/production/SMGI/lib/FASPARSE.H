#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FASPARSES_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FASPARSES_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef FASPARSES_EXPORTS
#define FASPARSES_API __declspec(dllexport)
#else
#define FASPARSES_API __declspec(dllimport)
#endif

#include <windows.h>

#pragma warning (disable : 4103)
#pragma pack(1)

#define MASK_DELI	0x01
#define MASK_DIGI	0x02
#define MASK_LTTR	0x04
#define MASK_SPEC	0x08

#define MASK_EXTRA	0x10
#define MASK_TOTAL	0x1F

namespace Process
{
	class FASPARSES_API CFasParse
	{
	public:
		CFasParse(void);
		virtual ~CFasParse(void);

		void	set_extra(char* szsrc);
		void	reset(void);

		//	把内存块转换成hex字串
		char*	memhex(char* szhex, void* lpmem, int len);

		//	跳过中英文分隔符分拆参数
		long	parser(char* szsrc, char* phs[], long phs_num, BYTE skip_map=MASK_SPEC);

		//	压缩短信, 全角字母数字转半角大写, 并剔除分隔符
		long	shrink(char* szshk, char* szsrc, BYTE skip_map = MASK_DELI | MASK_SPEC);

		DWORD	sm_nid(char* szsrc);

		//	将作品识别码转换成数值
		DWORD	str2ul(const char *iden);
		unsigned __int64 str2int64(const char *iden);

		void	add_kword(char* szkw);
		char*	has_kword(char* szsrc);

		char*	MD5(char* digest, const char* plain_text);
		char*	MD5_32(char* digest32, const char* plain_text);
		DWORD	MD5_CRC(const char* plain_text);

	private:
		void	set_map(char* szsrc, char mask);

		char*	find_kws(const char* szkw);
		void	sort_kws(void);
	};
}

extern Process::CFasParse	g_fasparse;