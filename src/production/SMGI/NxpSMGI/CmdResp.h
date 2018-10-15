#pragma once

#include "..\Lib\MYSQLDB.h"
#include "..\Lib\Sync.h"


#pragma pack (push, 1)
enum ECmdRespSys
{
	CMD_RESP_SYS_HELP	=	1,					//	非法指令
	CMD_RESP_SYS_NULL	=	2,					//	无用户记录
	CMD_RESP_SYS_BUSY	=	3,					//	系统繁忙
	CMD_RESP_SYS_FAIL	=	4,					//	系统维护，出现错误
	CMD_RESP_SYS_AUTO	=	5,					//	含有拒绝文字
};

enum ECmdRespReg
{
	CMD_RESP_REG_LIMIT	=	1,					//	受限制用户
	CMD_RESP_REG_NEW	=	2,					//	新短信小品
	CMD_RESP_REG_OLD	=	3,					//	已登记且通过
	CMD_RESP_REG_EXIST	=	4,					//	已登记未审核
	CMD_RESP_REG_BAN	=	5,					//	已登记不通过
	CMD_RESP_REG_TABOO	=	6,					//	有违禁短语
	CMD_RESP_REG_SHORT	=	7,					//	内容过短
	CMD_RESP_REG_LONG	=	8,					//	内容过长
	CMD_RESP_REG_PASS	=	9,					//	审核通过提示
	CMD_RESP_REG_FAIL	=	10,					//	审核拒绝接受
	CMD_RESP_REG_OVERRUN=	11,					//	超过最大登记数量
};

struct SCmdRespSys
{
	char	_help_resp_cont[1024];
	char	_null_resp_cont[1024];
	char	_busy_resp_cont[1024];
	char	_fail_resp_cont[1024];
	char	_auto_resp_cont[1024];
};

struct SCmdRespReg
{
	//
};

#pragma pack (pop, 1)

class CCmdResp
{
public:
	CCmdResp(CMYSQLDB	*db);
	~CCmdResp(void);

	bool init_sys_cmd(void);
	bool init_reg_cmd(void);

	const char* get_cmd_resp_help(void)
	{
		Process::CAutoLock lock(&_cs);
		return _sys_resp._help_resp_cont;
	}

	const char* get_cmd_resp_null(void)
	{
		Process::CAutoLock lock(&_cs);
		return _sys_resp._null_resp_cont;
	}

	const char* get_cmd_resp_busy(void)
	{
		Process::CAutoLock lock(&_cs);
		return _sys_resp._busy_resp_cont;
	}

	const char* get_cmd_resp_fail(void)
	{
		Process::CAutoLock lock(&_cs);
		return _sys_resp._fail_resp_cont;	
	}

	const char* get_cmd_resp_auto(void)
	{
		Process::CAutoLock lock(&_cs);
		return _sys_resp._auto_resp_cont;	
	}
	
private:
	CMYSQLDB	*_db;

	Process::CCriticalSection	_cs;

	SCmdRespSys		_sys_resp;
	SCmdRespReg		_reg_resp;
};
