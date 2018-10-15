#pragma once

#include "..\Lib\MYSQLDB.h"
#include "..\Lib\Sync.h"


#pragma pack (push, 1)
enum ECmdRespSys
{
	CMD_RESP_SYS_HELP	=	1,					//	�Ƿ�ָ��
	CMD_RESP_SYS_NULL	=	2,					//	���û���¼
	CMD_RESP_SYS_BUSY	=	3,					//	ϵͳ��æ
	CMD_RESP_SYS_FAIL	=	4,					//	ϵͳά�������ִ���
	CMD_RESP_SYS_AUTO	=	5,					//	���оܾ�����
};

enum ECmdRespReg
{
	CMD_RESP_REG_LIMIT	=	1,					//	�������û�
	CMD_RESP_REG_NEW	=	2,					//	�¶���СƷ
	CMD_RESP_REG_OLD	=	3,					//	�ѵǼ���ͨ��
	CMD_RESP_REG_EXIST	=	4,					//	�ѵǼ�δ���
	CMD_RESP_REG_BAN	=	5,					//	�ѵǼǲ�ͨ��
	CMD_RESP_REG_TABOO	=	6,					//	��Υ������
	CMD_RESP_REG_SHORT	=	7,					//	���ݹ���
	CMD_RESP_REG_LONG	=	8,					//	���ݹ���
	CMD_RESP_REG_PASS	=	9,					//	���ͨ����ʾ
	CMD_RESP_REG_FAIL	=	10,					//	��˾ܾ�����
	CMD_RESP_REG_OVERRUN=	11,					//	�������Ǽ�����
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
