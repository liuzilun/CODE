#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MYSQLDBS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MYSQLDBS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MYSQLDBS_EXPORTS
#define MYSQLDBS_API __declspec(dllexport)
#else
#define MYSQLDBS_API __declspec(dllimport)
#endif

#pragma warning(disable : 4103)
#pragma warning(disable : 4251)
#pragma pack(push, 1)

#ifndef _WINSOCKAPI_
#include <winsock2.h>
#else
#define _WINSOCK2API_
#endif
#include <windows.h>

#include <stdlib.h>
#include <mysql.h>
#include <errmsg.h>
#include "workerthread.h"
#include "sync.h"

#define MaxNmLen		128
#define StmtLen			2048
#define	MaxSleepTimes	30
#define	PerSleepTime	1000
#define MaxRetryTimes	5
#define RetrySleepTime	100

#define MYSQL_VER_40	40000
#define MYSQL_VER_41	40100
#define MYSQL_VER_50	50000

static const char	*RST_NULL	= "";

/***********************************************************
*
*	MYSQL Result Set Class
*
***********************************************************/

class CMYSQLDB;

class MYSQLDBS_API CMYSQLRS
{
public:
	CMYSQLRS(void) 
		: sqRsl(NULL)
		, sqRow(NULL)
		, sqRows(0)	
	{
	}

	virtual ~CMYSQLRS(void) { FreeRsl(); }

	//	获取当前查询的记录数
	DWORD QueryRows(void) { return sqRows; }

	//	检索一个结果集合的下一行(MoveNext)
	BOOL LoadRow(void) { 
		return (sqRsl == NULL) 
			? FALSE : ((sqRow = mysql_fetch_row(sqRsl)) != NULL); }

	//	释放结果集
	void FreeRsl(void) {
		if (sqRsl != NULL)
		{
			try
			{
				mysql_free_result(sqRsl);
			}
			catch(...)
			{
			}
		}

		sqRsl = NULL;
		sqRow = NULL;
		sqRows= 0;
	}

	//	获取字段值
	const char*	AsStr(int Fidx)	{
		return (sqRow[Fidx] == NULL) ? RST_NULL : sqRow[Fidx];
	}

	double	AsDbl(int Fidx)	{
		return (sqRow[Fidx] == NULL) ? 0 : atof(sqRow[Fidx]);
	}

	long	AsI32(int Fidx)	{ 
		return (sqRow[Fidx] == NULL) ? 0 : atol(sqRow[Fidx]); 
	}

	DWORD	AsU32(int Fidx)	{ 
		return (sqRow[Fidx] == NULL) ? 0 : (DWORD)atol(sqRow[Fidx]); 
	}

	__int64	AsI64(int Fidx)	{ 
		return (sqRow[Fidx] == NULL) ? 0 : _atoi64(sqRow[Fidx]); 
	}

	DWORD64	AsU64(int Fidx)	{
		return (sqRow[Fidx] == NULL) ? 0 : (DWORD64)_atoi64(sqRow[Fidx]); 
	}
private:
	friend class	CMYSQLDB;

	MYSQL_RES*		sqRsl;			//	结果集
	MYSQL_ROW		sqRow;			//	当前行
	DWORD			sqRows;			//	结果数
};

/***********************************************************
*
*	MYSQL Database Class
*
***********************************************************/
struct MYSQLDBS_API CDbInfo
{
	char	Host[MaxNmLen + 1];		//	主机名称
	char	User[MaxNmLen + 1];		//	用户名称
	char	Pswd[MaxNmLen + 1];		//	密码
	char	Name[MaxNmLen + 1];		//	数据库名称
	char	Charset[MaxNmLen + 1];	//	字符集
	WORD	Port;					//	端口
	DWORD	Clients;				//	客户端标志
};

class MYSQLDBS_API CMYSQLDB 
	: public Process::CWorkerThread
{
public:
	CMYSQLDB(const char	*szHost,
			 const char	*szUser,
			 const char	*szPswd,
			 const char	*szName, 
			 const char	*szCharset = "gb2312",
			 WORD Port = 3306,
			 DWORD Clients = 0);
	virtual ~CMYSQLDB(void);

	//	执行SQL语句,对于SELECT语句，一定加CMYSQLRS
	//	否则返回错误代码2014(CR_COMMANDS_OUT_OF_SYNC)。
	BOOL Exec(const char *SqlStmt, 
			  CMYSQLRS *lpRS = NULL,
			  unsigned long SqlLen = 0);

	BOOL Open(void);						//	建立数据库连接

	//	检查是否已经建立数据库连接
	BOOL IsOpen(void) const { return sqOpen; }
	
	// 规范字串的特殊符号
	char* EscStr(char *DestStr, const char *SrcStr, unsigned long length)
	{
		mysql_real_escape_string(sqHnd, DestStr, SrcStr, length);
		return DestStr;
	}

	// insert后的auto_increment值
	DWORD InsertId(void)		{ return (DWORD)mysql_insert_id(sqHnd); }

	// Update影响的记录数
	DWORD ExecRows(void)		{ return (DWORD)mysql_affected_rows(sqHnd); }
	
	//	服务器状态
	const char* SvrState(void)	{ return mysql_stat(sqHnd); }
	
	//	服务器信息
	const char* SvrInfo(void)	{ return mysql_get_server_info(sqHnd); }

	//	服务器版本
	unsigned long SvrVer(void)
	{
		return mysql_get_server_version(sqHnd);
	}
	
	// 主机信息
	const char* HostInfo(void)	{ return mysql_get_host_info(sqHnd); }
	
	// 客户机信息
	const char* CliInfo(void)	{ return mysql_get_client_info(); }
	
	// 最近错误信息
	const char* LastErr(void)	{ return mysql_error(sqHnd); }
	
	// 最近错误代码
	unsigned int LastErrno(void) { return mysql_errno(sqHnd); }

	//	获取字符集
	const char* CharSet(void)	{ return mysql_character_set_name(sqHnd); }
protected:
	CMYSQLDB(void) { }
	unsigned int ThreadEntry(void);

	void Close(void);						//	关闭已建立的数据库连接
private:
	MYSQL		*sqHnd;						// 数据库连接句柄
	BOOL		sqOpen;						// 是否连接数据库

	CDbInfo		DbInfo;
	Process::CCriticalSection	db_cs;
};

#pragma pack(pop, 1)