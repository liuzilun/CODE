#include "MySMGI.h"

#include <hash_map>
#include <string>

#include "..\Lib\fasparse.h"
#include "..\Lib\timer.h"
#include "..\Lib\log.h"
#include "IniCfg.h"
#include "ErrCode.h"

using namespace sms::smgi;
using namespace RdiClubCommon;

char* GetTime(unsigned long seconds, char *out)
{
	if (out != NULL)
	{
		//	延时seconds秒
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


/*****************************************************************************/
/*				CMySMGI::CRespTrd											 */
/*****************************************************************************/
CMySMGI::CRespTrd::CRespTrd(CMySMGI *smgi)
	: _smgi(smgi)
{
	BeginThread();
}

CMySMGI::CRespTrd::~CRespTrd(void)
{
	EndThread();
}

void CMySMGI::CRespTrd::push_back(SUBMIT_RESP &sm)
{
	_cs.Lock();
	_lst.push_back(sm);
	_cs.Unlock();
}

unsigned int CMySMGI::CRespTrd::ThreadEntry(void)
{
	while (!terminated_)
	{
		if (!_lst.empty())
		{
			_cs.Lock();
			std::list<SUBMIT_RESP> lst(_lst);
			_lst.clear();
			_cs.Unlock();

			while (!lst.empty())
			{
				SUBMIT_RESP resp = lst.front();
				lst.pop_front();

				_smgi->do_resp(resp);
			}
		}
		Sleep(10);
	}
	return 0;
}

/*****************************************************************************/
/*				CMySMGI::CDeliverTrd										 */
/*****************************************************************************/
CMySMGI::CDeliverTrd::CDeliverTrd(CMySMGI *smgi)
	: _smgi(smgi)
{
	BeginThread();
}

CMySMGI::CDeliverTrd::~CDeliverTrd(void)
{
	EndThread();
}

void CMySMGI::CDeliverTrd::push_back(DELIVER_SM &sm)
{
	_cs.Lock();
	_lst.push_back(sm);
	_cs.Unlock();
}

unsigned int CMySMGI::CDeliverTrd::ThreadEntry(void)
{
	while (!terminated_)
	{
		if (!_lst.empty())
		{
			_cs.Lock();
			std::list<DELIVER_SM> lst(_lst);
			_lst.clear();
			_cs.Unlock();

			while (!lst.empty())
			{
				DELIVER_SM deli = lst.front();
				lst.pop_front();

				_smgi->do_deliver(deli);
			}
		}
		Sleep(10);
	}
	return 0;
}

/*****************************************************************************/
/*				CMySMGI::CReportTrd											 */
/*****************************************************************************/
CMySMGI::CReportTrd::CReportTrd(CMySMGI *smgi)
	: _smgi(smgi)
{
	BeginThread();
}

CMySMGI::CReportTrd::~CReportTrd(void)
{
	EndThread();
}

void CMySMGI::CReportTrd::push_back(REPORT_SM &sm)
{
	_cs.Lock();
	_lst.push_back(sm);
	_cs.Unlock();
}

unsigned int CMySMGI::CReportTrd::ThreadEntry(void)
{
	while (!terminated_)
	{
		if (!_lst.empty())
		{
			_cs.Lock();
			std::list<REPORT_SM> lst(_lst);
			_lst.clear();
			_cs.Unlock();

			while (!lst.empty())
			{
				REPORT_SM report = lst.front();
				lst.pop_front();

				_smgi->do_report(report);
			}
		}
		Sleep(60000);
	}
	return 0;
}

/*****************************************************************************/
/*				CMySMGI::CPushTrd											 */
/*****************************************************************************/
CMySMGI::CPushTrd::CPushTrd(CMySMGI			*smgi,
							unsigned char	prio)
	: _smgi(smgi)
	, _prio(prio)
{
	BeginThread();
}

CMySMGI::CPushTrd::~CPushTrd(void)
{
	EndThread();
}

void CMySMGI::CPushTrd::push_back(SSMGLog &sm)
{
	_cs.Lock();
	_lst.push_back(sm);
	_cs.Unlock();
}

unsigned int CMySMGI::CPushTrd::ThreadEntry(void)
{
	while (!terminated_)
	{
		if (!_lst.empty())
		{
			_cs.Lock();
			std::list<SSMGLog>	lst(_lst);
			_lst.clear();
			_cs.Unlock();

			while (!lst.empty())
			{
				SSMGLog sm = lst.front();
				if (sm._status == SM_STATUS_LIMIT)
				{
					if (_smgi->can_push())
					{
						_smgi->do_submit(sm, _prio);
					}
				}
				else
				{
					_smgi->do_submit(sm, _prio);
				}
				lst.pop_front();
			}
		}
		Sleep(10);
	}
	return 0;
}

/*****************************************************************************/
/*				CMySMGI::CBusyTrd											 */
/*****************************************************************************/
CMySMGI::CBusyTrd::CBusyTrd(CMySMGI *smgi)
	: _smgi(smgi)
{
	BeginThread();
}

CMySMGI::CBusyTrd::~CBusyTrd(void)
{
	EndThread();
}

unsigned int CMySMGI::CBusyTrd::ThreadEntry(void)
{
	unsigned long count = 0;
	while (!terminated_)
	{
		int wait_cnt = 0;
		if (wait_cnt < 1800)
		{
			++wait_cnt;
			Sleep(1000);
		}
		else
		{			
			if (_smgi->_mo_cmd != NULL)
			{
				_smgi->_mo_cmd->startup();
			}

			if (_smgi->_segment != NULL)
			{
				_smgi->_segment->load();
			}

			if (_smgi->_fee != NULL)
			{
				_smgi->_fee->startup();
			}

			if (_smgi->_sms_pool != NULL)
			{
				_smgi->_sms_pool->do_load();
			}

			if (_smgi->_cmd_resp != NULL)
			{
				_smgi->_cmd_resp->init_reg_cmd();
				_smgi->_cmd_resp->init_sys_cmd();
			}
			
			wait_cnt = 0;
			Sleep(1000);
		}
	}

	return 0;
}

/*****************************************************************************/
/*				CMySMGI::CRevSubTrd											 */
/*****************************************************************************/
//	华为SP管理平台
//	命令字 + 间隔符 + 操作类型 + 间隔符 + 消息流水号 + 间隔符 + 用户手机号码 + 间隔符 + 业务代码 + 间隔符 +用户操作码（可有可无）＋附加信息（附加信息可有可无，不是必须）
//	订购通知操作命令字0001	订购应答操作码8001
//	操作类型：1--订购；操作类型：2--取消订购；
static const char	*CMD_NOTIFY				= "0001";
static const char	*CMD_NOTIFY_RESP		= "8001";
static const char	*CMD_REV_SUB			= "0003";
static const char	*CMD_REV_SUB_RESP		= "8003";
static const int	OPT_TYPE_SUBSCRIBE		= '1';
static const int	OPT_TYPE_UNSUBSCRIBE	= '2';
static const char	*SEPARATOR				= " ";

//	状态码定义
enum ESUBS_STATUS
{
	SUBS_STATUS_SUCCESS			= 0,			//	操作成功
	SUBS_STATUS_NOT_EXISTS		= 100,			//	用户手机号码不存在
	SUBS_STATUS_AUTH_FAILED		= 101,			//	用户鉴权失败
	SUBS_STATUS_USER_INVALID	= 102,			//	用户状态非法
	SUBS_STATUS_SRV_INVALID		= 107,			//	业务不存在
	SUBS_STATUS_SRV_NOT_OPEN	= 111,			//	业务尚未开通
	SUBS_STATUS_NO_SUBS			= 115,			//	用户未定购该业务
	SUBS_STATUS_DB_ERROR		= 6001,			//	内部数据库错误
	SUBS_STATUS_EXISTS			= 6002,			//	重复定购
	SUBS_STATUS_PARAM_INVALID	= 6003			//	其他消息参数不合法
};

//	订购/取消内容结构
struct SCMD_SUBSCRIBE
{
	char			_cmd_id[5];					//	命令字
	unsigned char	_opt_type;					//	操作类型
	unsigned long	_seq_id;					//	消息流水号
	char			_user_id[21];				//	用户手机号码
	char			_srv_id[10];				//	业务代码
	char			_user_opt[60];				//	用户操作码

	SCMD_SUBSCRIBE(void)
	{
		memset(this, 0, sizeof(SCMD_SUBSCRIBE));
	}
};

struct SCMD_SUBSCRIBE_RESP
{
	char			_cmd_id[5];					//	命令字
	unsigned long	_seq_id;					//	消息流水号
	unsigned long	_status;					//	状态码

	SCMD_SUBSCRIBE_RESP(void)
	{
		memset(this, 0, sizeof(SCMD_SUBSCRIBE_RESP));
	}
};

//	通过订购/取消定购短信内容提取各项数据
bool decode_subs(const char *cont, SCMD_SUBSCRIBE &subs)
{
	if (cont == NULL)
	{
		return false;
	}

	try
	{
		std::string tmp(cont);

		//	提取命令字
		strncpy(subs._cmd_id, tmp.c_str(), 4);

		//	操作类型
		subs._opt_type = (unsigned char)cont[5];

		tmp.erase(0, 7);

		const char *tmp_a = tmp.c_str();

		//	消息流水号
		size_t pos = tmp.find(SEPARATOR);
		if (pos != -1)
		{
			std::string seq_id = tmp.substr(0, pos);
			subs._seq_id = (unsigned long)atol(seq_id.c_str());
			tmp.erase(0, pos + 1);
		}

		//	用户手机号码
		pos = tmp.find(SEPARATOR);
		if (pos != -1)
		{
			std::string mobile = tmp.substr(0, pos);
			strncpy(subs._user_id, mobile.c_str(), pos);
			tmp.erase(0, pos + 1);
		}

		//	业务代码
		pos = tmp.find(SEPARATOR);
		if (pos != -1)
		{
			std::string srv_id = tmp.substr(0, pos);
			strncpy(subs._srv_id, srv_id.c_str(), pos);
			tmp.erase(0, pos + 1);
		}
		else
		{
			strncpy(subs._srv_id, tmp.c_str(), 10);
		}

		//	业务操作码可以省略
	}
	catch(...)
	{
		return false;
	}

	return true;
}

bool encode_subs(unsigned long seq_id, unsigned long status, char *cont)
{
	if (cont == NULL)
	{
		return false;
	}

	try
	{
		sprintf(cont, "%s %lu %lu", CMD_NOTIFY_RESP, seq_id, status);
	}
	catch(...)
	{
		return false;
	}

	return true;
}

bool encode_rev_subs(SCMD_SUBSCRIBE &sub, char *cont)
{
	if (cont == NULL)
	{
		return false;
	}

	try
	{
		sprintf(cont, "%s %c %lu %s %s %s", sub._cmd_id, sub._opt_type, 
			sub._seq_id, sub._user_id, sub._srv_id, sub._user_opt);
	}
	catch(...)
	{
		return false;
	}

	return true;
}

bool decode_rev_subs(const char *cont, SCMD_SUBSCRIBE_RESP &resp)
{
	if (cont == NULL)
	{
		return false;
	}

	try
	{
		std::string tmp(cont);

		//	提取命令字
		strncpy(resp._cmd_id, tmp.c_str(), 4);

		tmp.erase(0, 5);

		const char *tmp_a = tmp.c_str();

		//	消息流水号
		size_t pos = tmp.find(SEPARATOR);
		if (pos != -1)
		{
			std::string seq_id = tmp.substr(0, pos);
			resp._seq_id = (unsigned long)atol(seq_id.c_str());
			tmp.erase(0, pos + 1);
		}
		

		//	状态码
		resp._status = (unsigned long)atol(tmp.c_str());
	}
	catch(...)
	{
		return false;
	}

	return true;
}

CMySMGI::CRevSubTrd::CRevSubTrd(CMySMGI *smgi)
	: _smgi(smgi)
{
	BeginThread();
}

CMySMGI::CRevSubTrd::~CRevSubTrd(void)
{
	EndThread();
}

bool CMySMGI::CRevSubTrd::update_stat(unsigned long seq_id)
{
	SMYSQLDBLink *db = _smgi->_db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_UPDATE_REV_SUB, seq_id);
	bool ret = db->_db->Exec(sql) == TRUE;
	if (!ret)
	{
		g_log.append(Process::error, "[CRevSubTrd::update_stat]error:%s.", db->_db->LastErr());
	}
	_smgi->_db_pool->leave(*db);
	return ret;
}

unsigned int CMySMGI::CRevSubTrd::ThreadEntry(void)
{
	unsigned long count = 0;
	while (!terminated_)
	{
		if (g_ini.get()->_rev_sub == 1)
		{
			SMYSQLDBLink *db = _smgi->_db_pool->enter();
			CMYSQLRS rs;
			if (db->_db->Exec(SQL_GET_MBR_REV_SUB, &rs))
			{
				while (rs.LoadRow())
				{
					SCMD_SUBSCRIBE sub;
					strncpy(sub._cmd_id, CMD_REV_SUB, 4);
					
					sub._seq_id				= rs.AsU32(0);
					unsigned __int64 mobile = rs.AsU64(1);
					if (rs.AsU32(2) == 1)
					{
						sub._opt_type = OPT_TYPE_SUBSCRIBE;
					}
					else
					{
						sub._opt_type = OPT_TYPE_UNSUBSCRIBE;
					}
					
					strncpy(sub._srv_id, rs.AsStr(3), 10);
					
					RdiClubCommon::CFuncCommon x;
					char msisdn[MAX_ADDR_LEN + 1] = {0};
					x.do_encode_isdn(mobile, msisdn, "86");
					strncpy(sub._user_id, msisdn, MAX_ADDR_LEN);
					
					char cont[MAX_SQL_LEN] = {0};
					if (encode_rev_subs(sub, cont))
					{
						_smgi->Response(_smgi->_sp_info._misc_num, _smgi->_serv_schm->get_free_id(), 0, 
							SMG_CODING_ASCII, _smgi->_sp_info._port_num, "", cont);

						g_log.append(Process::info, "[Subs]OnRevSubs--Submit successful(Mobile:%lu,SpNum:%s,Cont:%s).",
								_smgi->_sp_info._misc_num, _smgi->_sp_info._port_num, cont);

						update_stat(sub._seq_id);
					}
					else
					{
						g_log.append(Process::error, "[Subs]OnRevSubs--Submit failed(Mobile:%lu,SpNum:%s,Cont:%s).",
								_smgi->_sp_info._misc_num, _smgi->_sp_info._port_num, cont);
					}
				}
				rs.FreeRsl();
			}
			else
			{
				g_log.append(Process::error, "[Subs]OnRevSubs--Select from db failed");
			}
			_smgi->_db_pool->leave(*db);
		}

		Sleep(30000);
	}

	return 0;
}


/*****************************************************************************/
/*				CMySMGI														 */
/*****************************************************************************/
CMySMGI::CMySMGI(unsigned long	smgi_id,
				 SSPInfo		&sp_info,
				 CMYSQLDBPool	*db_pool)
	: _db_pool(db_pool)
	, _smgi_id(smgi_id)
	, _sp_info(sp_info)

	, _resp_trd(NULL)
	, _deli_trd(NULL)
	, _report_trd(NULL)

	, _high_trd(NULL)
	, _mid_trd(NULL)
	, _low_trd(NULL)
	, _busy_trd(NULL)
	, _rev_sub_trd(NULL)

	, _mo_cmd(NULL)
	, _fee(NULL)
	, _sms_pool(NULL)
	, _segment(NULL)
	, _serv_schm(NULL)
	, _proc_sync(NULL)
	, _func_common(NULL)
	, _cmd_resp(NULL)
{
	CONNECT_INFO cn_info;
	cn_info._smg_type	= sp_info._smg_type;
	cn_info._slip_size	= sp_info._slip_win;
	cn_info._resp_wait_time = sp_info._resp_time;
	cn_info._resp_resends	= sp_info._resp_try;
	cn_info._links		= sp_info._links;
	cn_info._interval	= sp_info._mt_int;
	
	cn_info._sp_port		= sp_info._local_port;	
	cn_info._smg_port_mo	= sp_info._smg_mo_port;
	cn_info._smg_port_mt	= sp_info._smg_mt_port;
	strncpy(cn_info._smg_ip, sp_info._smg_ip, MAX_IP_LEN);
	strncpy(cn_info._login_name, sp_info._login_name, MAX_UID_LEN);
	strncpy(cn_info._login_pswd, sp_info._login_pswd, MAX_PSWD_LEN);

	strncpy(cn_info._sp_num,  sp_info._port_num, MAX_ADDR_LEN);
	strncpy(cn_info._corp_id, sp_info._corp_id, MAX_CORP_ID_LEN);
	cn_info._src_id			= sp_info._src_id;
	cn_info._seq_id			= sp_info._seq_id;
	cn_info._min_seq_id		= sp_info._min_seq_id;
	cn_info._max_seq_id		= sp_info._max_seq_id;
	cn_info._long_sm		= sp_info._is_long;
	if (_startup(cn_info))
	{
		g_log.append(Process::info, "[CMySMGI]Init SMG Interface successful.");
	}
	else
	{
		g_log.append(Process::error, "[CMySMGI]Init SMG Interface failed.");
		return;
	}

	_db_link = _db_pool->enter();

	_segment	= new RdiClubCommon::CSegment(_db_link->_db);
	_proc_sync	= new RdiClubCommon::CProcSync(_db_link->_db);
	_func_common= new RdiClubCommon::CFuncCommon();
	_mo_cmd		= new CMoCmd(_db_link->_db);
	_fee		= new CFee(_db_link->_db);
	_sms_pool	= new CSmsPool(_db_link->_db);
	_serv_schm	= new CServSchm(_db_link->_db);
	_cmd_resp	= new CCmdResp(_db_link->_db);

	_resp_trd	= new CRespTrd(this);
	_deli_trd	= new CDeliverTrd(this);
	_report_trd	= new CReportTrd(this);
	_busy_trd	= new CBusyTrd(this);
	_rev_sub_trd= new CRevSubTrd(this);

	g_log.append(Process::info, "[CMySMGI]Start tread successful.");

	_high_trd	= new CPushTrd(this, PUSH_PRIO_HIGH);
	g_log.append(Process::info, "[CMySMGI]Start High prio thread successful.");

	_mid_trd	= new CPushTrd(this, PUSH_PRIO_MID);
	g_log.append(Process::info, "[CMySMGI]Start Middle prio thread successful.");

	_low_trd	= new CPushTrd(this, PUSH_PRIO_LOW);
	g_log.append(Process::info, "[CMySMGI]Start Low prio thread successful.");	
}

CMySMGI::~CMySMGI(void)
{
	if (_mid_trd != NULL)
	{
		delete _mid_trd;
		_mid_trd = NULL;
	}

	if (_low_trd != NULL)
	{
		delete _low_trd;
		_low_trd = NULL;
	}

	if (_high_trd != NULL)
	{
		delete _high_trd;
		_high_trd = NULL;
	}

	if (_resp_trd != NULL)
	{
		delete _resp_trd;
		_resp_trd = NULL;
	}

	if (_deli_trd != NULL)
	{
		delete _deli_trd;
		_deli_trd = NULL;
	}

	if (_report_trd != NULL)
	{
		delete _report_trd;
		_report_trd = NULL;
	}

	if (_busy_trd != NULL)
	{
		delete _busy_trd;
		_busy_trd = NULL;
	}

	if (_rev_sub_trd != NULL)
	{
		delete _rev_sub_trd;
		_rev_sub_trd = NULL;
	}
}


const char *SQL_INSERT_SMG_LOG_HIGH
	= "INSERT INTO nxp_sms.smg_logh (seq_id, fin_dt, cmd_id, fee_id, port_id, mo, mt, stat, src_type, link_id, mmx_id, cont)\
	  VALUES (%lu, %I64u, %d, %d, %d, \'%s\', %I64u, %d, %d, \'%s\', %lu, \'%s\')";

bool CMySMGI::SaveMO(__int64		mobile,
					 unsigned short	cmd_id,
					 unsigned long	mmx_id,
					 const char		*sp_num,
					 const char		*link_id,
					 const char		*cont)
{
	SMYSQLDBLink *db = _db_pool->enter();
	char esc_cont[MAX_SQL_LEN] = {0};
	unsigned long len = (unsigned long)strlen(cont);
	db->_db->EscStr(esc_cont, cont, len);

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_INSERT_SMG_LOG_HIGH, 0, g_timer.DateTime(), cmd_id, 0, _smgi_id, sp_num, 
		mobile, SM_STATUS_OK, SM_SRC_TYPE_MO, link_id, mmx_id, esc_cont);
	bool ret = db->_db->Exec(sql) == TRUE;
	_db_pool->leave(*db);
	return ret;
}

bool CMySMGI::SaveMT(__int64		mobile,
					unsigned long	seq_id,
					unsigned short	fee_id,
					bool			is_rpt,
					unsigned long	mmx_id,
					const char		*sp_num,
					const char		*link_id,
					const char		*cont)
{
	bool ret = true;
	try
	{
		SMYSQLDBLink *db = _db_pool->enter();
		char esc_cont[MAX_SQL_LEN] = {0};
		unsigned long len = (unsigned long)strlen(cont);
		db->_db->EscStr(esc_cont, cont, len);

		char sql[MAX_SQL_LEN] = {0};
		if (is_rpt)
		{
			sprintf(sql, SQL_INSERT_SMG_LOG_HIGH, seq_id, g_timer.DateTime(), 0, fee_id, _smgi_id, 
				sp_num, mobile, SM_STATUS_WAITING, SM_SRC_TYPE_MT, link_id, mmx_id, esc_cont);
		}
		else
		{
			sprintf(sql, SQL_INSERT_SMG_LOG_HIGH, seq_id, g_timer.DateTime(), 0, fee_id, _smgi_id, 
				sp_num, mobile, SM_STATUS_OK, SM_SRC_TYPE_MT, link_id, mmx_id, esc_cont);
		}
		ret = db->_db->Exec(sql) == TRUE;
		_db_pool->leave(*db);
	}
	catch(...)
	{
		g_log.append(Process::error, "[CMySMGI::SaveMT]except!");
	}
	return ret;
}

void CMySMGI::Response(__int64			mobile,
					   unsigned short	fee_id,
					   unsigned long	mmx_id,
					   unsigned char	coding,
					   const char		*sp_num,
					   const char		*link_id,
					   const char		*cont)
{
	try
	{
		SUBMIT_SM submit;

		char mo_str[MAX_ADDR_LEN] = {0};
		RdiClubCommon::CFuncCommon func;
		if ((mobile >= 13000000000 && mobile <= 13900000000)
			|| (mobile >= 15000000000 && mobile <= 15900000000)
			|| (mobile >= 18000000000 && mobile <= 18900000000))
		{
			func.do_encode_isdn(mobile, mo_str);
		}
		else
		{
			func.do_encode_isdn(mobile, mo_str, NULL);
		}

		strncpy(submit._mobile, mo_str, MAX_ADDR_LEN);
		strncpy(submit._sp_num, sp_num, MAX_ADDR_LEN);

		SFee *fee = _fee->find(fee_id);
		if (fee != NULL)
		{
			strncpy(submit._srv_type, fee->_srv_type, MAX_SRV_TYPE_LEN);

			if ((fee->_pay_num >= 13000000000 && fee->_pay_num <= 13900000000)
				|| (fee->_pay_num >= 15000000000 && fee->_pay_num <= 15900000000)
				|| (fee->_pay_num >= 18000000000 && fee->_pay_num <= 18900000000))
			{
				func.do_encode_isdn(fee->_pay_num, submit._charge_num);
			}
			else if (fee->_pay_num > 0)
			{
				func.do_encode_isdn(fee->_pay_num, submit._charge_num, NULL);
			}

			strncpy(submit._fee_type, fee->_fee_type, MAX_FEE_TYPE_LEN);
			sprintf(submit._fee_value, "%05d", fee->_fee_val);
			submit._agent_flag	= fee->_is_agent;
			submit._mt_flag 	= fee->_ot_flag;
			submit._prio		= fee->_prio;
			submit._report		= fee->_is_reg;
		}

		if (_sp_info._has_expire == 1)
		{
			GetTime(_sp_info._delay_time, submit._expire_time);
		}

		if (link_id != NULL)
		{
			memcpy(submit._link_id, link_id, ::MAX_LINK_ID_LEN);
		}

		memcpy(submit._cont, cont, RdiClubCommon::MAX_CONT_LEN);
		submit._coding = coding;
		try
		{
			unsigned long seq_id = _submit(submit, false);

			SaveMT(mobile, seq_id, fee_id, submit._report == 1, mmx_id, sp_num, link_id, cont);
		}
		catch(...)
		{
			g_log.append(Process::info, "mobile:%s,cont:%s", submit._mobile, submit._cont);
			g_log.append(Process::error, "[CMySMGI::Response]submit to smg except!");
		}
	}
	catch(...)
	{
		g_log.append(Process::error, "[CMySMGI::Response]except!");
	}
}

bool CMySMGI::update_submit(unsigned long sm_id,
							unsigned long seq_id,
							unsigned char stat,
							unsigned char prio)
{
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	if (stat == SM_STATUS_OK)
	{
		if (prio == PUSH_PRIO_HIGH)
		{
			sprintf(sql, SQL_UPDATE_SMG_SUBMIT_OK_HIGH, stat, seq_id, sm_id);
		}
		else if (prio == PUSH_PRIO_MID)
		{
			sprintf(sql, SQL_UPDATE_SMG_SUBMIT_OK_MID, stat, seq_id, sm_id);
		}
		else
		{
			sprintf(sql, SQL_UPDATE_SMG_SUBMIT_OK_LOW, stat, seq_id, sm_id);
		}
	}
	else
	{
		if (prio == PUSH_PRIO_HIGH)
		{
			sprintf(sql, SQL_UPDATE_SMG_SUBMIT_WAIT_HIGH, stat, seq_id, sm_id);
		}
		else if (prio == PUSH_PRIO_MID)
		{
			sprintf(sql, SQL_UPDATE_SMG_SUBMIT_WAIT_MID, stat, seq_id, sm_id);
		}
		else
		{
			sprintf(sql, SQL_UPDATE_SMG_SUBMIT_WAIT_LOW, stat, seq_id, sm_id);
		}		
	}

	bool ret = db->_db->Exec(sql) == TRUE;
	if (!ret)
	{
		g_log.append(Process::error, "[update_submit]error:%s.", db->_db->LastErr());
	}
	_db_pool->leave(*db);
	return ret;
}

bool CMySMGI::update_response(unsigned long seq_id,
							  __int64		msg_id)
{
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_UPDATE_SMG_RESP, msg_id, seq_id);
	bool ret = db->_db->Exec(sql) == TRUE;
	if (!ret)
	{
		g_log.append(Process::error, "[update_response]error:%s.", db->_db->LastErr());
	}
	_db_pool->leave(*db);
	return ret;
}

bool CMySMGI::update_report(__int64			msg_id,
							unsigned char	stat,
							const char		*err_code)
{
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_UPDATE_SMG_REPORT, stat, err_code, msg_id);
	bool ret = db->_db->Exec(sql) == TRUE;
	if (!ret)
	{
		g_log.append(Process::error, "[update_response]error:%s.", db->_db->LastErr());
	}
	_db_pool->leave(*db);
	return ret;
}

bool CMySMGI::call_report(__int64	mobile,
						  __int64	msg_id)
{
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_CALL_DO_REPORT, mobile, msg_id);
	bool ret = db->_db->Exec(sql) == TRUE;
	_db_pool->leave(*db);
	return ret;
}

int CMySMGI::cancel_loose(unsigned long		mobile,
						  unsigned short	city_id,
						  const char		*cont)
{
	int ret = DB_OPT_STATUS_OK;
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, "CALL sp_cancel_loose(%lu, %lu, %s, @a)", mobile, city_id, cont);
	CMYSQLRS rs;
	if (db->_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			ret	= (int)rs.AsU32(0);
		}
		rs.FreeRsl();
	}
	else
	{
		ret = DB_OPT_STATUS_FAILED;
	}
	_db_pool->leave(*db);
	return ret;
}

bool CMySMGI::dispatch(SSMGLog	&sm, unsigned char prio)
{
	if (prio == PUSH_PRIO_HIGH)
	{
		if (_high_trd->size() >= g_ini.get()->_cache_size)
		{
			return false;
		}
		else
		{
			_high_trd->push_back(sm);
		}
	}
	else if (prio == PUSH_PRIO_MID)
	{
		if (_mid_trd->size() >= g_ini.get()->_cache_size)
		{
			return false;
		}
		else
		{
			_mid_trd->push_back(sm);
		}
	}
	else
	{
		if (_low_trd->size() >= g_ini.get()->_cache_size)
		{
			return false;
		}
		else
		{
			_low_trd->push_back(sm);
		}
	}

	return true;
}

bool CMySMGI::can_push(void)
{
	bool ret = false;
	SMYSQLDBLink *db = _db_pool->enter();
	CMYSQLRS rs;
	if (db->_db->Exec(SQL_CHECK_CAN_PUSH_STATUS, &rs))
	{
		if (rs.LoadRow())
		{
			ret = rs.AsU32(0) > 0;
		}
		rs.FreeRsl();
	}
	_db_pool->leave(*db);
	return ret;
}


void CMySMGI::do_submit(SSMGLog &sm, unsigned char prio)
{
	try
	{
		SUBMIT_SM submit;
		char mo_str[MAX_ADDR_LEN] = {0};
		RdiClubCommon::CFuncCommon func;
		if ((sm._mt >= 13000000000 && sm._mt <= 13900000000)
			|| (sm._mt >= 15000000000 && sm._mt <= 15900000000)
			|| (sm._mt >= 18000000000 && sm._mt <= 18900000000))
		{
			func.do_encode_isdn(sm._mt, mo_str);
		}
		else
		{
			func.do_encode_isdn(sm._mt, mo_str, NULL);
		}
		strncpy(submit._mobile, mo_str, MAX_ADDR_LEN);
		strncpy(submit._sp_num, sm._mo, MAX_ADDR_LEN);

		strncpy(submit._srv_type, sm._srv_type, MAX_SRV_TYPE_LEN);

		if (sm._pay_num != 0)
		{
			memset(mo_str, 0, MAX_ADDR_LEN);
			if ((sm._pay_num >= 13000000000 && sm._pay_num <= 13900000000)
				|| (sm._pay_num >= 15000000000 && sm._pay_num <= 15900000000)
				|| (sm._pay_num >= 18000000000 && sm._pay_num <= 18900000000))
			{
				func.do_encode_isdn(sm._pay_num, mo_str);
			}
			else if (sm._pay_num > 0)
			{
				func.do_encode_isdn(sm._pay_num, mo_str, NULL);
			}
			strncpy(submit._charge_num, mo_str, MAX_ADDR_LEN);
		}

		strncpy(submit._fee_type, sm._fee_type, MAX_FEE_TYPE_LEN);
		sprintf(submit._fee_value, "%05d", sm._fee_val);
		submit._agent_flag	= sm._is_agent;
		submit._mt_flag 	= sm._ot_flag;
		submit._prio		= sm._prio;
		submit._report		= sm._is_reg;
		
		if (_sp_info._has_expire == 1)
		{
			GetTime(_sp_info._delay_time, submit._expire_time);
		}

		strncpy(submit._link_id, sm._link_id, ::MAX_LINK_ID_LEN);

		memcpy(submit._cont, sm._cont, RdiClubCommon::MAX_CONT_LEN);

		submit._coding = SMG_CODING_GB2312;
		unsigned long seq_id = 0;
		try
		{
			seq_id = _submit(submit, false);

#ifdef _DEBUG
			g_log.append(Process::info, "[do_submit]seq_id:%lu, sp_num:%s, mobile:%s, cont:%s", 
				seq_id, submit._sp_num, submit._mobile, submit._cont);
#endif

			if (seq_id != 0)
			{
				if (!update_submit(sm._sm_idx, seq_id, 
								sm._is_reg >= 1 ? SM_STATUS_WAITING : SM_STATUS_OK, prio))
				{
					g_log.append(Process::error, "[CMySMGI::do_submit]Update status failed.");
				}		
			}
		}
		catch(...)
		{
			g_log.append(Process::info, "mobile:%s,cont:%s", submit._mobile, submit._cont);
			g_log.append(Process::error, "[CMySMGI::do_submit]submit except!");
		}
	}
	catch(...)
	{
		g_log.append(Process::error, "[CMySMGI::do_submit]except!");
	}
}

void CMySMGI::do_resp(SUBMIT_RESP &resp)
{
	if (resp._resp_code == 0)
	{
		if (!update_response(resp._seq_id, resp._msg_id))
		{
			g_log.append(Process::error, "[CMySMGI::do_resp]update failed(seq_id:%lu,msg_id:%I64u).", 
				resp._seq_id, resp._msg_id);
		}
	}
	else
	{
		g_log.append(Process::error, "[CMySMGI::do_resp]submit_resp failed(seq_id:%lu,msg_id:%I64u,resp_code:%d).", 
			resp._seq_id, resp._msg_id, resp._resp_code);
	}
}

char* TrimLeft(char *in)
{
	if (in == NULL)
	{
		return NULL;
	}

	char *ptr = in;
	while ((*ptr != '\0') && (*ptr == ' '))
	{
		++ptr;
	}
	
	return ptr;
}

char* TrimRight(char *in)
{
	char *ptr = in;
	int len = (int)strlen(ptr) - 1; 
    while (len >= 0)
	{
        if (*(ptr + len) != ' ')
        {						
			*(ptr + len + 1) = '\0';
            break;
        }
        else
		{
			--len;
		}
    }
    return ptr; 
}

//	截取部分内容返回给用户
static const int TRUNC_LEN	= 20;


void CMySMGI::do_deliver(DELIVER_SM &deliver)
{
#ifdef _DEBUG
	g_log.append(Process::info, "OnDeliver--Mobile:%s,SpNum:%s,LinkID:%s,Cont:%s.",
		deliver._mobile, deliver._sp_num, deliver._link_id, deliver._cont);
#endif
	try
	{
		RdiClubCommon::CFuncCommon func;
		unsigned char net_type = 0;
		__int64 mobile = func.do_decode_isdn(deliver._mobile, net_type);

		//	判断是否采用华为网关
		if (_sp_info._is_misc == 1
			&& _sp_info._smg_type == SMG_TYPE_SGIP_HUAWEI
			&& mobile == _sp_info._misc_num)
		{
			if (0 == strncmp(deliver._cont, CMD_NOTIFY, 4))
			{
				g_log.append(Process::info, "[Subs]OnSubsReq--Mobile:%s,SpNum:%s,Cont:%s.",
					deliver._mobile, deliver._sp_num, deliver._cont);

				//	解析内容
				SCMD_SUBSCRIBE subs;
				if (decode_subs(deliver._cont, subs))
				{
					//	用户手机号码（注意：mobile是管理平台号码，eg:10109696）
					__int64 user_id  = func.do_decode_isdn(subs._user_id, net_type);

					//	订购
					if (subs._opt_type == OPT_TYPE_SUBSCRIBE)
					{
						SMoCmd cmd;
						if (_mo_cmd->find((unsigned char)_smgi_id, ORD_TYPE_ORDER, subs._srv_id, cmd))
						{
							SaveMO(user_id, cmd._cmd_id, 0, deliver._sp_num, deliver._link_id, cmd._cmd_chr);
						}
						else
						{
							g_log.append(Process::error, 
								"Save Mo info failed(Get port_id=%d, ord_type=%d, srv_type=%s failed).", 
								_smgi_id, ORD_TYPE_ORDER, subs._srv_id);
							return;
						}

						SServSchm *schm = _serv_schm->find(cmd._serv_id);

						CSegment::SSegment *seg = _segment->do_find_by_mobile(user_id);
						unsigned short area_id = 0;
						unsigned char network = 0;
						if (seg != NULL)
						{
							area_id = seg->_area_id;
							network = seg->_seg_net;
						}

						g_log.append(Process::info, "sql:%s.", cmd._cmd_sp);
						SMYSQLDBLink *db = _db_pool->enter();
						char sql[MAX_SQL_LEN] = {0};
						sprintf(sql, cmd._cmd_sp, user_id, area_id, network, _smgi_id, cmd._cmd_id, cmd._serv_id,
							deliver._sp_num, deliver._link_id, deliver._cont);
						if (db->_db->Exec(sql))
						{
							char cont[MAX_SQL_LEN + 1] = {0};
							encode_subs(subs._seq_id, SUBS_STATUS_SUCCESS, cont);
							Response(mobile, schm != NULL ? schm->_psh_fee : _serv_schm->get_free_id(), 0, 
								SMG_CODING_ASCII, deliver._sp_num, deliver._link_id, cont);
							g_log.append(Process::info, "[Subs]OnSubsResp--Subscribe successful(Mobile:%lu,SpNum:%s,Resp_Cont:%s).",
								mobile, _sp_info._port_num, cont);
						}
						else
						{
							char cont[MAX_SQL_LEN + 1] = {0};
							encode_subs(subs._seq_id, SUBS_STATUS_DB_ERROR, cont);
							Response(mobile, schm != NULL ? schm->_per_fee : _serv_schm->get_free_id(), 0, 
								SMG_CODING_ASCII, deliver._sp_num, deliver._link_id, cont);
							g_log.append(Process::error, "[Subs]OnSubsResp--Subscribe failed(Mobile:%lu,SpNum:%s,Resp_Cont:%s).",
								mobile, _sp_info._port_num, cont);
						}
						_db_pool->leave(*db);
					}
					//	取消订购
					else if (subs._opt_type == OPT_TYPE_UNSUBSCRIBE)
					{
						//	保存上行原始消息
						SMoCmd cmd;
						if (_mo_cmd->find((unsigned char)_smgi_id, ORD_TYPE_CANCEL, subs._srv_id, cmd))
						{
							SaveMO(user_id, cmd._cmd_id, 0, deliver._sp_num, deliver._link_id, cmd._cmd_chr);
						}
						else
						{
							g_log.append(Process::error, 
								"Save Mo info failed(Get port_id=%d, ord_type=%d, srv_type=%s failed).", 
								_smgi_id, ORD_TYPE_CANCEL, subs._srv_id);
							return;
						}

						SServSchm *schm = _serv_schm->find(cmd._serv_id);

						CSegment::SSegment *seg = _segment->do_find_by_mobile(user_id);
						unsigned short area_id = 0;
						unsigned char network = 0;
						if (seg != NULL)
						{
							area_id = seg->_area_id;
							network = seg->_seg_net;
						}

						g_log.append(Process::info, "sql:%s.", cmd._cmd_sp);
						SMYSQLDBLink *db = _db_pool->enter();
						char sql[MAX_SQL_LEN] = {0};
						sprintf(sql, cmd._cmd_sp, user_id, area_id, network, _smgi_id, cmd._cmd_id, cmd._serv_id,
							deliver._sp_num, deliver._link_id, deliver._cont);
						if (db->_db->Exec(sql))
						{
							char cont[MAX_SQL_LEN + 1] = {0};
							encode_subs(subs._seq_id, SUBS_STATUS_SUCCESS, cont);
							Response(mobile, schm != NULL ? schm->_psh_fee : _serv_schm->get_free_id(), 0, 
								SMG_CODING_ASCII, deliver._sp_num, deliver._link_id, cont);
							g_log.append(Process::info, "[Subs]OnSubsResp--Cancel successful(Mobile:%lu,SpNum:%s,Resp_Cont:%s).",
										mobile, _sp_info._port_num, cont);
						}
						else
						{
							char cont[MAX_SQL_LEN + 1] = {0};
							encode_subs(subs._seq_id, SUBS_STATUS_DB_ERROR, cont);
							Response(mobile, schm != NULL ? schm->_per_fee : _serv_schm->get_free_id(), 0, 
								SMG_CODING_ASCII, deliver._sp_num, deliver._link_id, cont);
							g_log.append(Process::error, "[Subs]OnSubsResp--Cancel failed(Mobile:%lu,SpNum:%s,Resp_Cont:%s).",
										mobile, _sp_info._port_num, cont);
						}							
						_db_pool->leave(*db);
					}
					else
					{
						g_log.append(Process::error, "[Subs]OnSubsResp--unknown opt_type:%lu.", subs._opt_type);
					}
				}
				else
				{
					//	解码失败
					char cont[MAX_SQL_LEN + 1] = {0};
					encode_subs(subs._seq_id, SUBS_STATUS_PARAM_INVALID, cont);
					Response(mobile, _serv_schm->get_free_id(), 0, SMG_CODING_ASCII, deliver._sp_num, deliver._link_id, cont);
					g_log.append(Process::error, "[Subs]OnSubsResp--decode failed,Resp_Cont:%s.", cont);

					//	保存上行原始消息
					SaveMO(mobile, 0, 0, deliver._sp_num, deliver._link_id, deliver._cont);
				}
			}
			//	反向订购响应
			else if (0 == strncmp(deliver._cont, CMD_REV_SUB_RESP, 4))
			{
				g_log.append(Process::info, "[Subs]OnRevSubResp--Mobile:%s,SpNum:%s,Cont:%s.",
					deliver._mobile, deliver._sp_num, deliver._cont);

				SCMD_SUBSCRIBE_RESP resp;
				if (decode_rev_subs(deliver._cont, resp))
				{
					SMYSQLDBLink *db = _db_pool->enter();
					char sql[MAX_SQL_LEN] = {0};
					sprintf(sql, SQL_UPDATE_REV_SUB_STAT, resp._status, resp._seq_id);
					if (!db->_db->Exec(sql))
					{
						g_log.append(Process::error, "[Subs]Update rev_sub failed--Seq_id:%lu,Result:%d.",
							resp._seq_id, resp._status);
					}
					_db_pool->leave(*db);					
				}
				else
				{
					g_log.append(Process::error, "[Subs]OnRevSubResp--decode failed!");

					//	保存上行原始消息
					SaveMO(mobile, 0, 0, deliver._sp_num, deliver._link_id, deliver._cont);
				}
			}
			else
			{
				g_log.append(Process::error, "[Subs]unknown cmd code!");
			}
		}
		else
		{
			char sp_ext[::MAX_ADDR_LEN] = {0};

			//	Master port
			if (0 == strncmp(_sp_info._port_num, deliver._sp_num, strlen(_sp_info._port_num)))
			{
				strncpy(sp_ext, deliver._sp_num + strlen(_sp_info._port_num), MAX_ADDR_LEN);
			}
			//	Slave Port
			else if (0 == strncmp(g_ini.get()->_sp_num_slave, deliver._sp_num, strlen(g_ini.get()->_sp_num_slave)))
			{
				strncpy(sp_ext, deliver._sp_num + strlen(g_ini.get()->_sp_num_slave), MAX_ADDR_LEN);
			}
			else
			{
				SaveMO(mobile, 0, 0, _sp_info._port_num, deliver._link_id, deliver._cont);
				return;
			}
			
			unsigned char srv_type = 0;
			{	
				char *trim_left = TrimLeft(deliver._cont);
				char *trim_right = TrimRight(trim_left);
				SMoCmd *cmd = _mo_cmd->match((unsigned char)_smgi_id, trim_right, sp_ext);
				if (cmd != NULL)
				{
					char cmd_ext[MAX_PATH] = {0};
					strncpy(cmd_ext, trim_right + strlen(cmd->_cmd_chr), MAX_PATH);

					switch (cmd->_cmd_type)
					{
					//	登记自编短信
					case CMD_TYPE_REG_SM:
						{
							unsigned long mmx_id = 0;
							if (!_proc_sync->do_check_status_normal())
							{
								Response(mobile, cmd->_fee_id, 0, SMG_CODING_GB2312, deliver._sp_num,
									deliver._link_id, _cmd_resp->get_cmd_resp_busy());
							}
							else
							{
								SMYSQLDBLink *db = _db_pool->enter();
								char sql[MAX_SQL_LEN] = {0};
								RdiClubCommon::CSegment::SSegment *seg = _segment->do_find_by_mobile(mobile);
								unsigned short area_id = 0;
								unsigned char network = 0;
								if (seg != NULL)
								{	
									area_id = seg->_area_id;
									network = seg->_seg_net;
								}
								
								std::string tmp_cont(deliver._cont);
								char proc_sm[RdiClubCommon::MAX_CONT_LEN + 1] = {0};
								unsigned long sm_len = g_fasparse.shrink(proc_sm, (char*)deliver._cont, MASK_TOTAL);
								unsigned long trunc_len = _func_common->get_trunc_len(deliver._cont, TRUNC_LEN);
								
								char sm_part[255] = {0};
								strncpy(sm_part, tmp_cont.c_str(), trunc_len);
								if (sm_len < 40)
								{											
									sprintf(sql, cmd->_cmd_sp, CMD_RESP_REG_SHORT, mobile, area_id, network, cmd->_cmd_port,
											cmd->_cmd_id, cmd->_serv_id, deliver._sp_num, deliver._link_id, 0, sm_part);
									if (!db->_db->Exec(sql))
									{
										Response(mobile, cmd->_fee_id, 0, SMG_CODING_GB2312, deliver._sp_num,
											deliver._link_id, _cmd_resp->get_cmd_resp_fail());
										
										g_log.append(Process::error, "[CMD_TYPE_REG_SM]%s failed(err:%s).", sql, db->_db->LastErr());
									}
								}
								else if ((g_fasparse.has_kword(proc_sm) != NULL) 
									|| _func_common->has_tel_num(deliver._cont, 7))
								{
									sprintf(sql, cmd->_cmd_sp, CMD_RESP_REG_TABOO, mobile, area_id, network, cmd->_cmd_port,
											cmd->_cmd_id, cmd->_serv_id, deliver._sp_num, deliver._link_id, 0, sm_part);

									if (!db->_db->Exec(sql))
									{
										Response(mobile, cmd->_fee_id, 0, SMG_CODING_GB2312, deliver._sp_num,
											deliver._link_id, _cmd_resp->get_cmd_resp_fail());
										
										g_log.append(Process::error, "[CMD_TYPE_REG_SM]%s failed(err:%s).", sql, db->_db->LastErr());
									}
								}
								else
								{
									SSmsPool *sm = _sms_pool->do_find(proc_sm, sm_len);
									if (sm != NULL)			
									{
										sprintf(sql, cmd->_cmd_sp, CMD_RESP_REG_OLD, mobile, area_id, network, cmd->_cmd_port,
											cmd->_cmd_id, cmd->_serv_id, deliver._sp_num, deliver._link_id, 0, sm_part);
										if (!db->_db->Exec(sql))
										{
											Response(mobile, cmd->_fee_id, 0, SMG_CODING_GB2312, deliver._sp_num,
												deliver._link_id, _cmd_resp->get_cmd_resp_fail());
											
											g_log.append(Process::error, "[CMD_TYPE_REG_SM]%s failed(err:%s).", sql, db->_db->LastErr());
										}
										
										mmx_id = sm->_sms_id;
									}
									else
									{
										if (_sms_pool->do_register(mobile, deliver._cont))
										{
											//	待审加入内存中
											char iden[MAX_IDEN_LEN + 1] = {0};
											strncpy(iden, proc_sm + 10, MAX_IDEN_LEN);
											if (strlen(iden) == MAX_IDEN_LEN)
											{
												SSmsPool x;
												strncpy(x._sms_wm, iden, MAX_IDEN_LEN);
												x._reg_cid = mobile;
												_sms_pool->do_add_mem(x);

												sprintf(sql, cmd->_cmd_sp, CMD_RESP_REG_NEW, mobile, area_id, network, cmd->_cmd_port,
													cmd->_cmd_id, cmd->_serv_id, deliver._sp_num, deliver._link_id, 0, sm_part);
												if (!db->_db->Exec(sql))
												{
													Response(mobile, cmd->_fee_id, 0, SMG_CODING_GB2312, deliver._sp_num,
														deliver._link_id, _cmd_resp->get_cmd_resp_fail());
													
													g_log.append(Process::error, "[CMD_TYPE_REG_SM]%s failed(err:%s).", sql, db->_db->LastErr());
												}
											}
											else
											{
												sprintf(sql, cmd->_cmd_sp, CMD_RESP_REG_SHORT, mobile, area_id, network, cmd->_cmd_port,
													cmd->_cmd_id, cmd->_serv_id, deliver._sp_num, deliver._link_id, 0, sm_part);
												if (!db->_db->Exec(sql))
												{
													Response(mobile, cmd->_fee_id, 0, SMG_CODING_GB2312, deliver._sp_num,
														deliver._link_id, _cmd_resp->get_cmd_resp_fail());
													
													g_log.append(Process::error, "[CMD_TYPE_REG_SM]%s failed(err:%s).", sql, db->_db->LastErr());
												}
											}
										}
										else
										{
											Response(mobile, cmd->_fee_id, 0, SMG_CODING_GB2312, deliver._sp_num,
												deliver._link_id, _cmd_resp->get_cmd_resp_fail());
										}
									}
								}

								_db_pool->leave(*db);
							}

							SaveMO(mobile, cmd->_cmd_id, mmx_id, deliver._sp_num, deliver._link_id, deliver._cont);
						}
						break;

					//	标准接口(检查系统繁忙)
					case CMD_TYPE_STD:
						{
							SaveMO(mobile, cmd->_cmd_id, 0, deliver._sp_num, deliver._link_id, deliver._cont);
						
							if (!_proc_sync->do_check_status_normal())
							{
								Response(mobile, cmd->_fee_id, 0, SMG_CODING_GB2312, deliver._sp_num,
									deliver._link_id, _cmd_resp->get_cmd_resp_busy());
							}
							else
							{											
								SMYSQLDBLink *db = _db_pool->enter();
								
								char esc_cont[MAX_SQL_LEN] = {0};
								unsigned long len = (unsigned long)strlen(deliver._cont);
								db->_db->EscStr(esc_cont, deliver._cont, len);

								char sql[MAX_SQL_LEN] = {0};
								RdiClubCommon::CSegment::SSegment *seg = _segment->do_find_by_mobile(mobile);
								if (seg != NULL)
								{						
									sprintf(sql, cmd->_cmd_sp, mobile, seg->_area_id, seg->_seg_net, cmd->_cmd_port,
										cmd->_cmd_id, cmd->_serv_id, deliver._sp_num, deliver._link_id, esc_cont);
								}
								else
								{
									sprintf(sql, cmd->_cmd_sp, mobile, 0, 0, cmd->_cmd_port,
										cmd->_cmd_id, cmd->_serv_id, deliver._sp_num, deliver._link_id, esc_cont);		
								}

								if (!db->_db->Exec(sql))
								{
									Response(mobile, cmd->_fee_id, 0, SMG_CODING_GB2312, 
										deliver._sp_num, deliver._link_id, _cmd_resp->get_cmd_resp_fail());

									g_log.append(Process::error, "[CMD_TYPE_STD]%s failed(err:%s).", sql, db->_db->LastErr());
								}
								_db_pool->leave(*db);
							}
						}
						break;

					//	标准接口(不检查系统繁忙)
					case CMD_TYPE_STD_EXT:
					default:
						{
							SaveMO(mobile, cmd->_cmd_id, 0, deliver._sp_num, deliver._link_id, deliver._cont);
															
							SMYSQLDBLink *db = _db_pool->enter();

							char esc_cont[MAX_SQL_LEN] = {0};
							unsigned long len = (unsigned long)strlen(deliver._cont);
							db->_db->EscStr(esc_cont, deliver._cont, len);

							char sql[MAX_SQL_LEN] = {0};
							RdiClubCommon::CSegment::SSegment *seg = _segment->do_find_by_mobile(mobile);
							if (seg != NULL)
							{						
								sprintf(sql, cmd->_cmd_sp, mobile, seg->_area_id, seg->_seg_net, cmd->_cmd_port,
									cmd->_cmd_id, cmd->_serv_id, deliver._sp_num, deliver._link_id, esc_cont);
							}
							else
							{
								sprintf(sql, cmd->_cmd_sp, mobile, 0, 0, cmd->_cmd_port,
									cmd->_cmd_id, cmd->_serv_id, deliver._sp_num, deliver._link_id, esc_cont);		
							}

							if (!db->_db->Exec(sql))
							{
								Response(mobile, cmd->_fee_id, 0, SMG_CODING_GB2312, 
									deliver._sp_num, deliver._link_id, _cmd_resp->get_cmd_resp_fail());

								g_log.append(Process::error, "[CMD_TYPE_STD]%s failed(err:%s).", sql, db->_db->LastErr());
							}
							_db_pool->leave(*db);
						}
						break;
					}
				}
				else
				{
					SaveMO(mobile, 0, 0, deliver._sp_num, deliver._link_id, deliver._cont);

					/*
					Response(mobile, _serv_schm->get_free_id(), 0, SMG_CODING_GB2312, deliver._sp_num, 
						deliver._link_id, _cmd_resp->get_cmd_resp_help());*/

					//	增加未知指令存储过程
					SMYSQLDBLink *db = _db_pool->enter();

					char esc_cont[MAX_SQL_LEN] = {0};
					unsigned long len = (unsigned long)strlen(deliver._cont);
					db->_db->EscStr(esc_cont, deliver._cont, len);

					char sql[MAX_SQL_LEN] = {0};
					RdiClubCommon::CSegment::SSegment *seg = _segment->do_find_by_mobile(mobile);
					if (seg != NULL)
					{						
						sprintf(sql, "CALL nxp_mbr.sp_smgi_call_cmd_unknown(%I64u, %lu, %lu, %lu, %lu, %lu, '%s', '%s', '%s')",
							mobile, seg->_area_id, seg->_seg_net, _smgi_id, 0, 0, deliver._sp_num, deliver._link_id, esc_cont);
					}
					else
					{
						sprintf(sql, "CALL nxp_mbr.sp_smgi_call_cmd_unknown(%I64u, %lu, %lu, %lu, %lu, %lu, '%s', '%s', '%s')",
							mobile, 0, 0, _smgi_id, 0, 0, deliver._sp_num, deliver._link_id, esc_cont);
					}					

					if (!db->_db->Exec(sql))
					{
						Response(mobile, _serv_schm->get_free_id(), 0, SMG_CODING_GB2312, 
							deliver._sp_num, "", _cmd_resp->get_cmd_resp_fail());

						g_log.append(Process::error, "[CMD_TYPE_STD]%s failed(err:%s).", sql, db->_db->LastErr());
					}
					_db_pool->leave(*db);
				}
			}
		}
	}
	catch(...)
	{
		g_log.append(Process::error, "[CMySMGI::do_deliver]except!");
	}
}

void CMySMGI::do_report(REPORT_SM &report)
{
	try
	{
		RdiClubCommon::CFuncCommon func;
		unsigned char net_type = 0;
		__int64 mobile = func.do_decode_isdn(report._mobile, net_type);

		//	短信下发状态转换
		unsigned char stat = SM_STATUS_OK;
		switch (report._stat)
		{
		case 0:
			stat = SM_STATUS_OK;
			break;
		case 1:
		case 2:
		case 255:
		default:
			stat = SM_STATUS_FAILED;
			break;
		}

#ifdef _DEBUG
		g_log.append(Process::info, "OnReport--Mobile:%s,State:%d,ErrCode:%s.",
				report._mobile, stat, report._err_code);
#endif
			
		//	记录状态报告信息
		if (!update_report(report._msg_id, stat, report._err_code))
		{
			g_log.append(Process::error, 
				"[CMySMGI::do_report]Update report failed--msg_id:%I64i,stat:%d,err_code:%s.",
				report._msg_id, stat, report._err_code);
		}

		//	成功才计费
		if (stat == SM_STATUS_OK)
		{
			if (!call_report(mobile, report._msg_id))
			{
				g_log.append(Process::error, "Call sp_do_report failed(mobile:%s, msg_id:%I64i).",
					report._mobile, report._msg_id);
			}
		}
	}
	catch(...)
	{
		g_log.append(Process::error, "[CMySMGI::do_report]except!");
	}
}

void CMySMGI::OnSubmitResp(SUBMIT_RESP &resp)
{
#ifdef _DEBUG
	printf("\nOnSubmitResp\nSeqID:%lu,MsgID:%I64u\n", 
		resp._seq_id, resp._msg_id);
#endif
	do_resp(resp);
	//_resp_trd->push_back(resp);
}

void CMySMGI::OnDeliver(DELIVER_SM &deliver)
{
#ifdef _DEBUG
	printf("\nOnDeliver\nMobile:%s,SPNum:%s,Coding:%d,Content:%s,LinkID:%s\n",
		deliver._mobile, deliver._sp_num, deliver._coding, deliver._cont, 
		deliver._link_id);
#endif
	do_deliver(deliver);
	//_deli_trd->push_back(deliver);
}

void CMySMGI::OnReport(REPORT_SM &report)
{
#ifdef _DEBUG
	printf("\nOnReport\nMobile:%s,SeqID:%I64u,State:%d,ErrCode:%s\n",
		report._mobile, report._msg_id, report._stat, report._err_code);
#endif
	_report_trd->push_back(report);
}