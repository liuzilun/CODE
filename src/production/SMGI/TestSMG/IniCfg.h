/*
** Copyright (c) 2001－2005
** Shen Zhen Joint Harvest Solutions Co., Ltd.
** All rights reserved.
**
** Initial.h
** interface for the CInitial class.
** 从SMSRelay.ini文件中获取配置参数，提供给应用程序进行初始化
** 输入INI配置文件的名称(包含路径) 
**
** Version: 2.0
** Author:  Chris
** Time:	2005/06/02
** Desc:	
*/

#pragma once

#include <windows.h>
#include <stdio.h>
#include <hash_map>

#pragma warning (disable : 4103)
#pragma pack (push, 1)
struct SIniCfg
{
	//	路径信息
	char			_base_dir[MAX_PATH];		//	基本路径
	char			_data_dir[MAX_PATH];		//	数据存放路径
	char			_log_dir[MAX_PATH];			//	日志存放路径

	//	SMG
	char			_sp_num[MAX_PATH];			//	接入号
	char			_misc_num[MAX_PATH];		//	管理平台接入号码
	char			_corp_id[MAX_PATH];			//	企业代码

	unsigned char	_smg_type;					//	网关类型
	unsigned short	_links;						//	连接数
	unsigned short	_slip_size;					//	滑动窗口
	unsigned long	_interval;					//	每个连接的提交速度

	unsigned short	_resp_wait_time;			//	命令及其应答之间的时间间隔最大不能超过30秒
	unsigned short	_resp_resends;				//	RESP返回失败，重发次数
	
	char			_sp_ip[MAX_PATH];			//	SP IP
	unsigned short	_sp_port;					//	SP Port
	char			_smg_ip[MAX_PATH];			//	SMG IP
	unsigned short	_smg_mo_port;				//	SMG MO Port
	unsigned short	_smg_mt_port;				//	SMG MT Port
	char			_login_name[MAX_PATH];		//	登录名称
	char			_login_pwd[MAX_PATH];		//	登录密码
	char			_src_id[MAX_PATH];			//	源节点码

	//	Send
	unsigned short	_has_expire;				//	是否启用过期时间，0否，1是
	unsigned long	_delay_time;				//	滞后时间(单位秒)，缺省5分钟

	SIniCfg(void)
	{
		memset(this, 0, sizeof(SIniCfg));
	}

	~SIniCfg(void)
	{
	}
};

#pragma pack (pop, 1)

class CIniCfg
{
public:
	CIniCfg();
	CIniCfg(const char *file_name);
	~CIniCfg(void);

	void set_file_name(const char *file_name);
	void set_seq_id(unsigned long smgi_id, 
					unsigned long seq_id);

	SIniCfg* get(void);
	void load(void);
private:
	char	_file_name[MAX_PATH];
	SIniCfg		_cfg;
};

inline CIniCfg::CIniCfg(void)
{
	memset(_file_name, 0, MAX_PATH);
}

inline CIniCfg::CIniCfg(const char *file_name)
{
	memset(_file_name, 0, MAX_PATH);
	strncpy(_file_name, file_name, MAX_PATH);
	load();
}

inline CIniCfg::~CIniCfg(void)
{
}

inline void CIniCfg::set_file_name(const char *file_name)
{
	memset(_file_name, 0, MAX_PATH);
	strncpy(_file_name, file_name, MAX_PATH);
	load();
}

inline SIniCfg* CIniCfg::get(void)
{ 
	return &_cfg;
}

inline void CIniCfg::load(void)
{
	//	路径
	memset(_cfg._base_dir, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "BaseDir", "", _cfg._base_dir, MAX_PATH, _file_name);
	if (_cfg._base_dir[strlen(_cfg._base_dir) - 1] == '\\')
	{
		_cfg._base_dir[strlen(_cfg._base_dir) - 1] = '\0';
	}

	memset(_cfg._data_dir, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "DataDir", "", _cfg._data_dir, MAX_PATH, _file_name);
	if (_cfg._data_dir[strlen(_cfg._data_dir) - 1] == '\\')
	{
		_cfg._data_dir[strlen(_cfg._data_dir) - 1] = '\0';
	}

	memset(_cfg._log_dir, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "LogDir", "", _cfg._log_dir, MAX_PATH, _file_name);
	if (_cfg._log_dir[strlen(_cfg._log_dir) - 1] == '\\')
	{
		_cfg._log_dir[strlen(_cfg._log_dir) - 1] = '\0';
	}

	//	SMG
	memset(_cfg._sp_num, 0, MAX_PATH);
	GetPrivateProfileString("SMG", "SpNum", "", _cfg._sp_num, MAX_PATH, _file_name);
	memset(_cfg._misc_num, 0, MAX_PATH);
	GetPrivateProfileString("SMG", "MiscNum", "", _cfg._misc_num, MAX_PATH, _file_name);
	memset(_cfg._corp_id, 0, MAX_PATH);
	GetPrivateProfileString("SMG", "CorpID", "", _cfg._corp_id, MAX_PATH, _file_name);

	_cfg._smg_type		= GetPrivateProfileInt("SMG", "SMGType", 1, _file_name);
	_cfg._links			= GetPrivateProfileInt("SMG", "Links", 1, _file_name);
	_cfg._slip_size		= GetPrivateProfileInt("SMG", "SlipSize", 32, _file_name);
	_cfg._interval		= GetPrivateProfileInt("SMG", "Interval", 0, _file_name);

	_cfg._resp_wait_time= GetPrivateProfileInt("SMG", "RespWaitTime", 30, _file_name);
	_cfg._resp_resends	= GetPrivateProfileInt("SMG", "RespResends", 3, _file_name);
	
	_cfg._has_expire	= GetPrivateProfileInt("SMG", "HasExpire", 0, _file_name);
	_cfg._delay_time	= GetPrivateProfileInt("SMG", "DelayTime", 0, _file_name);

	memset(_cfg._sp_ip, 0, MAX_PATH);
	GetPrivateProfileString("SMG", "SPIP", "127.0.0.1", _cfg._sp_ip, MAX_PATH, _file_name);
	_cfg._sp_port		= GetPrivateProfileInt("SMG", "SPPort", 8801, _file_name);

	memset(_cfg._smg_ip, 0, MAX_PATH);
	GetPrivateProfileString("SMG", "SMGIP", "127.0.0.1", _cfg._smg_ip, MAX_PATH, _file_name);

	_cfg._smg_mo_port	= GetPrivateProfileInt("SMG", "SMGMOPort", 8801, _file_name);
	_cfg._smg_mt_port	= GetPrivateProfileInt("SMG", "SMGMTPort", 8801, _file_name);
	
	memset(_cfg._login_name, 0, MAX_PATH);
	GetPrivateProfileString("SMG", "LoginName", "", _cfg._login_name, MAX_PATH, _file_name);
	memset(_cfg._login_pwd, 0, MAX_PATH);
	GetPrivateProfileString("SMG", "LoginPwd", "", _cfg._login_pwd, MAX_PATH, _file_name);	
	
	memset(_cfg._src_id, 0, MAX_PATH);
	GetPrivateProfileString("SMG", "SrcID", "", _cfg._src_id, MAX_PATH, _file_name);
}

extern CIniCfg g_ini;