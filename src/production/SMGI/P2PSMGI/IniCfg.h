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
	char			_srv_name[MAX_PATH];		//	服务名称
	char			_srv_desc[MAX_PATH];		//	服务描述

	unsigned long	_push_cap;					//	群发能力
	unsigned long	_cache_size;				//	缓冲大小
	unsigned long	_area_code;					//	区号
	unsigned long	_port_id;					//	端口编号
	unsigned long	_rev_sub;					//	是否启用反向订购

	//	数据库
	char			_db_host[MAX_PATH];			//	短信P2P数据库
	char			_db_uid[MAX_PATH];			//	用户名称
	char			_db_pwd[MAX_PATH];			//	用户密码
	char			_db_name[MAX_PATH];			//	数据库名称
    unsigned short  _db_port;					//  端口
	char			_db_charset[MAX_PATH];		//	字符集
	unsigned long	_db_cflag;					//	Client_Flag
	unsigned long	_db_links;					//	LINKS

	//	RPC
	char			_rpc_prot[MAX_PATH];		//	协议
	char			_rpc_port[MAX_PATH];		//	端口
	char			_rpc_ip[MAX_PATH];			//	IP地址
	unsigned long	_port_mode;					//	PUSH端口分类{0--ALL;1--收费端口;2--免费端口}

	char			_test_port[MAX_PATH];		//	测试端口 qsf

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

	SIniCfg* get_cfg(void);
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

inline SIniCfg* CIniCfg::get_cfg(void)
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

	memset(_cfg._srv_name, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "ServiceName", "", _cfg._srv_name, MAX_PATH, _file_name);

	memset(_cfg._srv_desc, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "ServiceDesc", "", _cfg._srv_desc, MAX_PATH, _file_name);

	_cfg._push_cap	= GetPrivateProfileInt("COMMON", "PushCap", 50, _file_name);
	_cfg._cache_size= GetPrivateProfileInt("COMMON", "CacheSize", 2000, _file_name);
	_cfg._area_code	= GetPrivateProfileInt("COMMON", "AreaCode", 755, _file_name);
	_cfg._port_id	= GetPrivateProfileInt("COMMON", "PortID", 0, _file_name);
	_cfg._rev_sub	= GetPrivateProfileInt("COMMON", "RevSub", 0, _file_name);
	GetPrivateProfileString("COMMON", "TestPort", "", _cfg._test_port, MAX_PATH, _file_name);

	//	DB
	memset(_cfg._db_host, 0, MAX_PATH);
	GetPrivateProfileString("DB", "Host", "127.0.0.1", _cfg._db_host, MAX_PATH, _file_name);
	memset(_cfg._db_uid, 0, MAX_PATH);
	GetPrivateProfileString("DB", "UID", "root", _cfg._db_uid, MAX_PATH, _file_name);
	memset(_cfg._db_pwd, 0, MAX_PATH);
	GetPrivateProfileString("DB", "PWD", "", _cfg._db_pwd, MAX_PATH, _file_name);
	memset(_cfg._db_name, 0, MAX_PATH);
	GetPrivateProfileString("DB", "DBName", "nxp_vms", _cfg._db_name, MAX_PATH, _file_name);
	memset(_cfg._db_charset, 0, MAX_PATH);
	GetPrivateProfileString("DB", "CharSet", "gb2312", _cfg._db_charset, MAX_PATH, _file_name);
	_cfg._db_port = GetPrivateProfileInt("DB", "Port", 3306, _file_name);
	_cfg._db_cflag= GetPrivateProfileInt("DB", "ClientFlag", 0, _file_name);
	_cfg._db_links= GetPrivateProfileInt("DB", "Links", 10, _file_name);

	//	RPC
	memset(_cfg._rpc_prot, 0, MAX_PATH);
	GetPrivateProfileString("RPC", "ProtocolSequence", "ncacn_ip_tcp", 
		_cfg._rpc_prot, MAX_PATH, _file_name);
	if (_cfg._rpc_prot[strlen(_cfg._rpc_prot) - 1] == '\\')
	{
		_cfg._rpc_prot[strlen(_cfg._rpc_prot) - 1] = '\0';
	}

	memset(_cfg._rpc_ip, 0, MAX_PATH);
	GetPrivateProfileString("RPC", "IPAddress", "127.0.0.1", _cfg._rpc_ip, 
		MAX_PATH, _file_name);
	if (_cfg._rpc_ip[strlen(_cfg._rpc_ip) - 1] == '\\')
	{
		_cfg._rpc_ip[strlen(_cfg._rpc_ip) - 1] = '\0';
	}

	memset(_cfg._rpc_port, 0, MAX_PATH);
	GetPrivateProfileString("RPC", "EndPoint", "8181", _cfg._rpc_port, 
		MAX_PATH, _file_name);
	if (_cfg._rpc_port[strlen(_cfg._rpc_port) - 1] == '\\')
	{
		_cfg._rpc_port[strlen(_cfg._rpc_port) - 1] = '\0';
	}
}

extern CIniCfg g_ini;