#include "MySMGI.h"

#include <hash_map>
#include <string>

#include "IniCfg.h"
#include "..\Lib\Log.h"
#include "ChineseCode.h"

using namespace sms::smgi;

//	��ȡ�������ݷ��ظ��û�
static const int TRUNC_LEN	= 20;

unsigned long TruncSM(const char *content, 
					  unsigned long len) 
{
	unsigned long ret = 0;
	const char* ptr = content;
	for (unsigned long i = 0; i < len; ++i)
	{
		unsigned char tmp = (unsigned char)*ptr;
		if (tmp <= 0x7f)
		{
			++ret;
			++ptr;
		}
		else
		{
			ret += 2;
			ptr += 2;
		}
		if (ret >= TRUNC_LEN)
		{
			break;
		}
	}
	return ret;
}

char* GetTime(unsigned long seconds, char *out)
{
	if (out != NULL)
	{
		//	��ʱseconds��
		time_t tt = time(NULL);
		struct tm x = *localtime(&tt);
		x.tm_sec += seconds;
		mktime(&x);
		try
		{
			sprintf(out, "%02d%02d%02d%02d%02d%02d032+", 
					x.tm_year%100, x.tm_mon + 1, x.tm_mday, 
					x.tm_hour, x.tm_min, x.tm_sec);
		}
		catch(...)
		{
			return NULL;
		}
	}
	return out;
}

///////////////////////////////////////////////////////////////////////////////
//							CTestSMGI
///////////////////////////////////////////////////////////////////////////////
CTestSMGI::CTestSMGI(void)
{
	CONNECT_INFO cn_info;
	
	strncpy(cn_info._sp_num, g_ini.get()->_sp_num, MAX_ADDR_LEN);
	strncpy(cn_info._corp_id, g_ini.get()->_corp_id, MAX_CORP_ID_LEN);
	
	cn_info._smg_type	= g_ini.get()->_smg_type;
	cn_info._links		= (unsigned char)g_ini.get()->_links;
	cn_info._slip_size	= (unsigned short)g_ini.get()->_slip_size;
	cn_info._interval	= (unsigned short)g_ini.get()->_interval;

	cn_info._resp_wait_time = g_ini.get()->_resp_wait_time;
	cn_info._resp_resends	= g_ini.get()->_resp_resends;
	cn_info._long_sm = 7;

	cn_info._sp_port		= g_ini.get()->_sp_port;	
	
	strncpy(cn_info._smg_ip, g_ini.get()->_smg_ip, MAX_IP_LEN);	
	cn_info._smg_port_mo	= g_ini.get()->_smg_mo_port;
	cn_info._smg_port_mt	= g_ini.get()->_smg_mt_port;

	strncpy(cn_info._login_name, g_ini.get()->_login_name, MAX_UID_LEN);
	strncpy(cn_info._login_pswd, g_ini.get()->_login_pwd, MAX_PSWD_LEN);
	
	cn_info._src_id			= (unsigned long)atol(g_ini.get()->_src_id);
	cn_info._seq_id			= 1;
	if (_startup(cn_info))
	{
		g_log.append(rpt::info, "[CMySMGI]Init SMG Interface successful.");
	}
	else
	{
		g_log.append(rpt::info, "[CMySMGI]Init SMG Interface failed.");
		return;
	}
}

CTestSMGI::~CTestSMGI(void)
{
	
}

void CTestSMGI::OnSubmitResp(SUBMIT_RESP &resp)
{
#ifdef _DEBUG
	printf("\nOnSubmitResp\nSeqID:%lu,MsgID:%I64u\n", 
		resp._seq_id, resp._msg_id);
#endif
	g_log.append(rpt::info, "[OnSubmitResp]nSeqID:%lu,MsgID:%I64u.", resp._seq_id, resp._msg_id);
}

void CTestSMGI::OnDeliver(DELIVER_SM &deliver)
{
#ifdef _DEBUG
	printf("\nOnDeliver\nMobile:%s,SPNum:%s,Coding:%d,Content:%s,LinkID:%s\n",
		deliver._mobile, deliver._sp_num, deliver._coding, deliver._cont, 
		deliver._link_id);
#endif
	g_log.append(rpt::info, "[OnDeliver]Mobile:%s,SPNum:%s,Coding:%d,Content:%s,LinkID:%s.",
		deliver._mobile, deliver._sp_num, deliver._coding, deliver._cont, deliver._link_id);

	SUBMIT_SM sm;
	strncpy(sm._mobile, deliver._mobile, MAX_ADDR_LEN);
	strncpy(sm._sp_num, g_ini.get()->_sp_num, MAX_ADDR_LEN);
	strncpy(sm._srv_type, "MFDB", MAX_SRV_TYPE_LEN);
    sprintf(sm._fee_type, "%d", 1);
	sprintf(sm._fee_value, "%05d", 0);
	sm._agent_flag	= 1;
	sm._mt_flag 	= 0;
	sm._prio		= 0;
	sm._report		= 0;

	//	add by 2005-10-14
	//	���Ӷ���Ϣ����ʱ�����
	if (g_ini.get()->_has_expire == 1)
	{
		GetTime(g_ini.get()->_delay_time, sm._expire_time);
	}

	sm._coding = 8;
	strcpy(sm._cont, "���еش���M-Zone�����й��ƶ�ͨ�ż��Ź㶫��˾�Ƴ���GSM�����ƶ��绰����Ʒ�ƣ���Ʒ����2002����ʽ�Ƴ���רΪ׷��ʱ�С�����������������Ⱥ�����ơ����еش�Ʒ�ƺ���Ҫ��Ϊ��ʱ�С����桢̽�������������ʷ���������ӱ�Ķ��Ű�����ʽ�����ṩ���ִ����Եĸ��Ի�������ָ����һ�㣬�ḻ��ʵ��ƶ��������񣬷��š��������־��ֲ��Ⱥ��������ҵ������ǰ�����ͻ�����ǰ��δ�е��ƶ�ͨ�����");
	printf("len:%d\n", strlen(sm._cont));

	if (deliver._link_id != NULL)
	{
		strncpy(sm._link_id, deliver._link_id, MAX_LINK_ID_LEN);
	}
	unsigned long seq_id = _submit(sm, true);
}

void CTestSMGI::OnReport(REPORT_SM &report)
{
#ifdef _DEBUG
	printf("\nOnReport\nMobile:%s,SeqID:%I64u,State:%d,ErrCode:%s\n",
		report._mobile, report._msg_id, report._stat, report._err_code);
#endif
}
