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
	char			_proc_name[MAX_PATH];		//	进程名称

	unsigned long	_push_cap;					//	群发能力
	unsigned long	_cache_size;				//	缓冲大小
	unsigned long	_area_code;					//	区号
	unsigned long	_port_id;					//	端口编号
	unsigned long	_rev_sub;					//	是否启用反向订购
	char			_sp_num_slave[MAX_PATH];	//	端口备用

	//	NXP_DBS数据库
	char			_dbs_host[MAX_PATH];			//	数据库IP
	char			_dbs_uid[MAX_PATH];				//	用户名称
	char			_dbs_pwd[MAX_PATH];				//	用户密码
	char			_dbs_name[MAX_PATH];			//	数据库名称
    unsigned short  _dbs_port;						//  端口
	char			_dbs_charset[MAX_PATH];			//	字符集
	unsigned long	_dbs_cflag;						//	Client_Flag
	unsigned long	_dbs_links;						//	数据库连接池数量

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
	SIniCfg	_ini;
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
	return &_ini;
}

inline void CIniCfg::load(void)
{
	//	路径
	memset(_ini._base_dir, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "BaseDir", "", _ini._base_dir, MAX_PATH, _file_name);
	if (_ini._base_dir[strlen(_ini._base_dir) - 1] == '\\')
	{
		_ini._base_dir[strlen(_ini._base_dir) - 1] = '\0';
	}

	memset(_ini._data_dir, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "DataDir", "", _ini._data_dir, MAX_PATH, _file_name);
	if (_ini._data_dir[strlen(_ini._data_dir) - 1] == '\\')
	{
		_ini._data_dir[strlen(_ini._data_dir) - 1] = '\0';
	}

	memset(_ini._log_dir, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "LogDir", "", _ini._log_dir, MAX_PATH, _file_name);
	if (_ini._log_dir[strlen(_ini._log_dir) - 1] == '\\')
	{
		_ini._log_dir[strlen(_ini._log_dir) - 1] = '\0';
	}

	memset(_ini._srv_name, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "ServiceName", "", _ini._srv_name, MAX_PATH, _file_name);

	memset(_ini._srv_desc, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "ServiceDesc", "", _ini._srv_desc, MAX_PATH, _file_name);

	memset(_ini._proc_name, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "ProcName", "NxpSMGI.exe", _ini._proc_name, MAX_PATH, _file_name);

	_ini._push_cap	= GetPrivateProfileInt("COMMON", "PushCap", 50, _file_name);
	_ini._cache_size= GetPrivateProfileInt("COMMON", "CacheSize", 2000, _file_name);
	_ini._area_code	= GetPrivateProfileInt("COMMON", "AreaCode", 755, _file_name);
	_ini._port_id	= GetPrivateProfileInt("COMMON", "PortID", 0, _file_name);
	_ini._rev_sub	= GetPrivateProfileInt("COMMON", "RevSub", 0, _file_name);

	memset(_ini._sp_num_slave, 0, MAX_PATH);
	GetPrivateProfileString("COMMON", "SlaveSPNum", "", _ini._sp_num_slave, MAX_PATH, _file_name);

	//	DBS
	memset(_ini._dbs_host, 0, MAX_PATH);
	GetPrivateProfileString("NXP_DBS", "Host", "127.0.0.1", _ini._dbs_host, MAX_PATH, _file_name);
	memset(_ini._dbs_uid, 0, MAX_PATH);
	GetPrivateProfileString("NXP_DBS", "UID", "root", _ini._dbs_uid, MAX_PATH, _file_name);
	memset(_ini._dbs_pwd, 0, MAX_PATH);
	GetPrivateProfileString("NXP_DBS", "PWD", "", _ini._dbs_pwd, MAX_PATH, _file_name);
	memset(_ini._dbs_name, 0, MAX_PATH);
	GetPrivateProfileString("NXP_DBS", "DBName", "nxp_vms", _ini._dbs_name, MAX_PATH, _file_name);
	memset(_ini._dbs_charset, 0, MAX_PATH);
	GetPrivateProfileString("NXP_DBS", "CharSet", "gb2312", _ini._dbs_charset, MAX_PATH, _file_name);
	_ini._dbs_port	= GetPrivateProfileInt("NXP_DBS", "Port", 3306, _file_name);
	_ini._dbs_cflag	= GetPrivateProfileInt("NXP_DBS", "ClientFlag", 0, _file_name);
	_ini._dbs_links	= GetPrivateProfileInt("NXP_DBS", "Links", 5, _file_name);
}

extern CIniCfg g_ini;