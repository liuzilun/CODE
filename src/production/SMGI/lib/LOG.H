#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LOGGERS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LOGGERS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LOGGERS_EXPORTS
#define LOGGERS_API __declspec(dllexport)
#else
#define LOGGERS_API __declspec(dllimport)
#endif

#pragma warning(push, 3)
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#pragma warning(pop)

#include "sync.h"

#pragma warning(disable : 4251)

namespace Process
{
	enum LOGGERS_API rpt_level
	{
		debug	= 0,
		info	= 1,
		warn	= 2,
		error	= 3,
		fatal	= 4
	};

	class LOGGERS_API layout
	{
	public:
		layout(void) {}
		virtual ~layout(void) {}

		virtual bool format(int level, const std::string &source, 
			const std::string &message, std::string& out) = 0;
	protected:
		const std::string grade(int level);
	};

	class LOGGERS_API simplelayout 
		: public layout
	{
	public:
		simplelayout(const std::string &fmt = "%d %t | %s | %e | %m") : format_(fmt) {}

		bool format(int level, const std::string &source, 
			const std::string &message, std::string &out);
	private:
		std::string	format_;
	};

	class LOGGERS_API logimpl
	{
	public:
		logimpl(int level, layout *l = NULL);
		virtual ~logimpl(void);

		bool append(int level, const std::string &message);
		bool source(const std::string &src);
	protected:
		virtual bool do_append(const std::string &message) = 0;
	private:
		layout		*layout_;
		std::string	source_;
		int			level_;
	};

	class LOGGERS_API log
	{
	public:
		log(const std::string &source = "unknown");
		~log(void);

		bool add(logimpl *impl);
		bool append(int level, const std::string &message);
		bool append(int level, char *fmt, ...);
		bool append(int level, int lasterr = 0);
	private:
		std::vector<logimpl* >		logs_;
		std::string					source_;
		Process::CCriticalSection	sync_;
	};

	class LOGGERS_API plainfile
		: public logimpl
	{
	public:
		plainfile(int level, const std::string &file = "unknown", layout *l = NULL);
		~plainfile(void);
	protected:
		bool do_append(const std::string &message);
	private:
		std::ofstream	*ofs_;
	};

	class LOGGERS_API debugger 
		: public logimpl
	{
	public:
		debugger(int level, layout* l = 0) 
			: logimpl(level, l) {}
		~debugger(void) {}

	protected:
		bool do_append(const std::string &message);
	};

	class LOGGERS_API console 
		: public logimpl
	{
	public:
		console(int level, layout* l = 0) 
			: logimpl(level, l) {}
		~console(void)
		{
			std::cout << std::flush;
		}
	protected:
		bool do_append(const std::string &message);
	};

	class LOGGERS_API journal
		: public logimpl
	{
	public:
		journal(int level, const std::string& file = "unknown", layout* l = 0);
		~journal(void);
		bool do_append(const std::string& message);
	private:
		void createfile(void);
	private:
		std::string		filename_;
		std::ofstream*	ofs_;
		int				day_;
	};
}
#pragma warning(default : 4251)

extern Process::log	g_log;