/*
** Copyright (c) 2001��2005
** Shen Zhen Joint Harvest Solutions Co., Ltd.
** All rights reserved.
**
** Initial.h
** interface for the CInitial class.
** ��SMSRelay.ini�ļ��л�ȡ���ò������ṩ��Ӧ�ó�����г�ʼ��
** ����INI�����ļ�������(����·��) 
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
	//	·����Ϣ
	char			_base_dir[MAX_PATH];		//	����·��
	char			_data_dir[MAX_PATH];		//	���ݴ��·��
	char			_log_dir[MAX_PATH];			//	��־���·��

	//	SMG
	char			_sp_num[MAX_PATH];			//	�����
	char			_misc_num[MAX_PATH];		//	����ƽ̨�������
	char			_corp_id[MAX_PATH];			//	��ҵ����

	unsigned char	_smg_type;					//	��������
	unsigned short	_links;						//	������
	unsigned short	_slip_size;					//	��������
	unsigned long	_interval;					//	ÿ�����ӵ��ύ�ٶ�

	unsigned short	_resp_wait_time;			//	�����Ӧ��֮���ʱ��������ܳ���30��
	unsigned short	_resp_resends;				//	RESP����ʧ�ܣ��ط�����
	
	char			_sp_ip[MAX_PATH];			//	SP IP
	unsigned short	_sp_port;					//	SP Port
	char			_smg_ip[MAX_PATH];			//	SMG IP
	unsigned short	_smg_mo_port;				//	SMG MO Port
	unsigned short	_smg_mt_port;				//	SMG MT Port
	char			_login_name[MAX_PATH];		//	��¼����
	char			_login_pwd[MAX_PATH];		//	��¼����
	char			_src_id[MAX_PATH];			//	Դ�ڵ���

	//	Send
	unsigned short	_has_expire;				//	�Ƿ����ù���ʱ�䣬0��1��
	unsigned long	_delay_time;				//	�ͺ�ʱ��(��λ��)��ȱʡ5����

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
	//	·��
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