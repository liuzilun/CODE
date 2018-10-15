#pragma once

#include "..\Lib\MYSQLDBPool.h"
#include "..\Lib\Sync.h"
#include "..\SMGI\SMGDefine.h"
#include <hash_map>

static const char *SQL_GET_PORT
	= "SELECT port_id, port_num, misc_num, smg_type, slip_win, resp_time, resp_try, rpt_try,\
	  have_expire, delay_time, links, mt_int, local_ip, local_port, smg_ip, smg_mo_port, smg_mt_port,\
	  login_name, login_pswd, corp_id, src_id, min_seq_id, max_seq_id, seq_id, is_iden, is_long, is_misc FROM nxp_sms.smg_port";

static const char *SQL_GET_THE_PORT
	= "SELECT port_id, port_num, misc_num, smg_type, slip_win, resp_time, resp_try, rpt_try,\
	  have_expire, delay_time, links, mt_int, local_ip, local_port, smg_ip, smg_mo_port, smg_mt_port,\
	  login_name, login_pswd, corp_id, src_id, min_seq_id, max_seq_id, seq_id, is_iden, is_long, is_misc FROM nxp_sms.smg_port\
	  WHERE port_id=%d";

static const char *SQL_UPDATE_SEQ_ID
	= "UPDATE nxp_sms.smg_port SET seq_id=%lu WHERE port_id=%d";

using namespace sms::smgi;

#pragma pack (push, 1)
struct SSPInfo
{
	char			_port_num[MAX_ADDR_LEN+1];	//	接入号
	__int64			_misc_num;					//	管理平台接入号码
	unsigned char	_is_misc;					//	是否上管理平台

	unsigned char	_smg_type;					//	网关类型
	unsigned char	_slip_win;					//	滑动窗口
	unsigned char	_resp_time;					//	命令及其应答之间的时间间隔最大不能超过30秒
	unsigned char	_resp_try;					//	RESP返回失败，重发次数
	unsigned char	_rpt_try;					//	Report重发次数

	unsigned char	_has_expire;				//	是否启用过期时间，0否，1是
	unsigned long	_delay_time;				//	滞后时间(单位秒)，缺省5分钟

	unsigned char	_links;						//	连接数
	unsigned short	_mt_int;					//	每个连接的提交速度

	char			_local_ip[MAX_IP_LEN+1];	//	SP IP
	unsigned short	_local_port;				//	SP Port
	char			_smg_ip[MAX_IP_LEN+1];		//	SMG IP
	unsigned short	_smg_mo_port;				//	SMG MO Port
	unsigned short	_smg_mt_port;				//	SMG MT Port

	char			_login_name[MAX_UID_LEN+1];	//	登录名称
	char			_login_pswd[MAX_PSWD_LEN+1];//	登录密码

	char			_corp_id[MAX_CORP_ID_LEN+1];//	企业代码
	unsigned long	_src_id;					//	源节点码

	unsigned long	_seq_id;					//	当前序列号
	unsigned long	_min_seq_id;				//	最小序列号
	unsigned long	_max_seq_id;				//	最大序列号

	unsigned char	_is_iden;					//	是否内容识别
	unsigned char	_is_long;					//	是否支持长短信{0否，1是7位，2是6位}

	SSPInfo(void)
	{
		memset(this, 0, sizeof(SSPInfo));
	}

	~SSPInfo(void)
	{
	}
};
typedef std::hash_map<unsigned long, SSPInfo>	MAP_SPInfo;

class CPort
{
public:
	CPort(CMYSQLDB	*db);
	~CPort(void);

	bool startup(unsigned long port_id);
	void cleanup(void);

	SSPInfo* find(unsigned long port_id);
	bool save_seq_id(unsigned long port_id, 
					 unsigned long seq_id);
	unsigned long size(void);
public:
	MAP_SPInfo	_sp_info;
	Process::CCriticalSection	_cs_info;
private:
	CMYSQLDB	*_db;
};
