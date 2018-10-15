#ifndef TIMER_H_
#define TIMER_H_

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TIMERS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TIMERS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TIMERS_EXPORTS
#define TIMERS_API __declspec(dllexport)
#else
#define TIMERS_API __declspec(dllimport)
#endif

#include <time.h>
#include "workerthread.h"

namespace Process
{
	class TIMERS_API CTimer
		: public CWorkerThread
	{
	public:
		CTimer(void);
		virtual ~CTimer();

		time_t Now(void) const;
		struct tm* Local(void);

		//	日期：格式为：20040218
		unsigned long Date(void) const;

		//	时间：格式为：120155
		unsigned long Time(void) const;
		__int64 DateTime(void) const;
		
		//	时间：格式为：YYMMDDhhmm,0604131730
		unsigned long DateTime_noCC(void) const;

		//	day
		unsigned short Day(void) const;

		//	月份：格式为:200402
		unsigned long Month(void) const;

		//	wday
		unsigned short wDay(void) const;

		//	上个月
		unsigned long Last_Month(void);

		//	时间：单位为秒＝12*3600+01*60+55
		unsigned long Time_Second(void) const;

		//	昨天的日期：格式为：20040217
		unsigned long Yesterday(void);
		unsigned long Before(int days);
		unsigned long After(int days);

		//	时间：格式为：YYMMDDhhmm,0604131730
		unsigned long Before_noCC(int days);
		unsigned long After_noCC(int days);
		unsigned long Yesterday_noCC(void);

		const char* NowToStr(char *output);
		const char* AfterToStr(int seconds, char *output);
		const char* BeforeToStr(int seconds, char *output);
	private:
		unsigned int ThreadEntry(void);
		void Update(void);
	private:
		time_t			lCurr_;
		struct tm		tmCurr_;
		unsigned long	ulDate_;
		unsigned long	ulTime_;
		unsigned long	ulMonth_;
		unsigned long	ulTime_Second_;
		__int64			i64Time_;
		unsigned long	ulTime_nocc_;
	};
}

extern Process::CTimer	g_timer;

#endif
