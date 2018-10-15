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
	char			_port_num[MAX_ADDR_LEN+1];	//	�����
	__int64			_misc_num;					//	����ƽ̨�������
	unsigned char	_is_misc;					//	�Ƿ��Ϲ���ƽ̨

	unsigned char	_smg_type;					//	��������
	unsigned char	_slip_win;					//	��������
	unsigned char	_resp_time;					//	�����Ӧ��֮���ʱ��������ܳ���30��
	unsigned char	_resp_try;					//	RESP����ʧ�ܣ��ط�����
	unsigned char	_rpt_try;					//	Report�ط�����

	unsigned char	_has_expire;				//	�Ƿ����ù���ʱ�䣬0��1��
	unsigned long	_delay_time;				//	�ͺ�ʱ��(��λ��)��ȱʡ5����

	unsigned char	_links;						//	������
	unsigned short	_mt_int;					//	ÿ�����ӵ��ύ�ٶ�

	char			_local_ip[MAX_IP_LEN+1];	//	SP IP
	unsigned short	_local_port;				//	SP Port
	char			_smg_ip[MAX_IP_LEN+1];		//	SMG IP
	unsigned short	_smg_mo_port;				//	SMG MO Port
	unsigned short	_smg_mt_port;				//	SMG MT Port

	char			_login_name[MAX_UID_LEN+1];	//	��¼����
	char			_login_pswd[MAX_PSWD_LEN+1];//	��¼����

	char			_corp_id[MAX_CORP_ID_LEN+1];//	��ҵ����
	unsigned long	_src_id;					//	Դ�ڵ���

	unsigned long	_seq_id;					//	��ǰ���к�
	unsigned long	_min_seq_id;				//	��С���к�
	unsigned long	_max_seq_id;				//	������к�

	unsigned char	_is_iden;					//	�Ƿ�����ʶ��
	unsigned char	_is_long;					//	�Ƿ�֧�ֳ�����{0��1��7λ��2��6λ}

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
