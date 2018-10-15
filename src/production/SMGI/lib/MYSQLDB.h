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

	//	��ȡ��ǰ��ѯ�ļ�¼��
	DWORD QueryRows(void) { return sqRows; }

	//	����һ��������ϵ���һ��(MoveNext)
	BOOL LoadRow(void) { 
		return (sqRsl == NULL) 
			? FALSE : ((sqRow = mysql_fetch_row(sqRsl)) != NULL); }

	//	�ͷŽ����
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

	//	��ȡ�ֶ�ֵ
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

	MYSQL_RES*		sqRsl;			//	�����
	MYSQL_ROW		sqRow;			//	��ǰ��
	DWORD			sqRows;			//	�����
};

/***********************************************************
*
*	MYSQL Database Class
*
***********************************************************/
struct MYSQLDBS_API CDbInfo
{
	char	Host[MaxNmLen + 1];		//	��������
	char	User[MaxNmLen + 1];		//	�û�����
	char	Pswd[MaxNmLen + 1];		//	����
	char	Name[MaxNmLen + 1];		//	���ݿ�����
	char	Charset[MaxNmLen + 1];	//	�ַ���
	WORD	Port;					//	�˿�
	DWORD	Clients;				//	�ͻ��˱�־
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

	//	ִ��SQL���,����SELECT��䣬һ����CMYSQLRS
	//	���򷵻ش������2014(CR_COMMANDS_OUT_OF_SYNC)��
	BOOL Exec(const char *SqlStmt, 
			  CMYSQLRS *lpRS = NULL,
			  unsigned long SqlLen = 0);

	BOOL Open(void);						//	�������ݿ�����

	//	����Ƿ��Ѿ��������ݿ�����
	BOOL IsOpen(void) const { return sqOpen; }
	
	// �淶�ִ����������
	char* EscStr(char *DestStr, const char *SrcStr, unsigned long length)
	{
		mysql_real_escape_string(sqHnd, DestStr, SrcStr, length);
		return DestStr;
	}

	// insert���auto_incrementֵ
	DWORD InsertId(void)		{ return (DWORD)mysql_insert_id(sqHnd); }

	// UpdateӰ��ļ�¼��
	DWORD ExecRows(void)		{ return (DWORD)mysql_affected_rows(sqHnd); }
	
	//	������״̬
	const char* SvrState(void)	{ return mysql_stat(sqHnd); }
	
	//	��������Ϣ
	const char* SvrInfo(void)	{ return mysql_get_server_info(sqHnd); }

	//	�������汾
	unsigned long SvrVer(void)
	{
		return mysql_get_server_version(sqHnd);
	}
	
	// ������Ϣ
	const char* HostInfo(void)	{ return mysql_get_host_info(sqHnd); }
	
	// �ͻ�����Ϣ
	const char* CliInfo(void)	{ return mysql_get_client_info(); }
	
	// ���������Ϣ
	const char* LastErr(void)	{ return mysql_error(sqHnd); }
	
	// ����������
	unsigned int LastErrno(void) { return mysql_errno(sqHnd); }

	//	��ȡ�ַ���
	const char* CharSet(void)	{ return mysql_character_set_name(sqHnd); }
protected:
	CMYSQLDB(void) { }
	unsigned int ThreadEntry(void);

	void Close(void);						//	�ر��ѽ��������ݿ�����
private:
	MYSQL		*sqHnd;						// ���ݿ����Ӿ��
	BOOL		sqOpen;						// �Ƿ��������ݿ�

	CDbInfo		DbInfo;
	Process::CCriticalSection	db_cs;
};

#pragma pack(pop, 1)