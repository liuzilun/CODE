#include "MySMGI.h"

#include <hash_map>
#include <string>

#include "..\Lib\fasparse.h"
#include "..\Lib\timer.h"
#include "..\Lib\log.h"
#include "..\Lib\RACli.h"
#include "..\Lib\MSISDN.h"
#include "..\VMSCommon\Define.h"
#include "IniCfg.h"
#include "ErrCode.h"

using namespace sms::smgi;

//	截取部分内容返回给用户
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

bool HasTelNum(const char *cont, int tel_nums)
{
	int cnt = 0;
	if (cont == NULL)
	{
		return false;
	}
	
	while (*cont)	
	{
		char tmp[3] ={0};
		unsigned char x = (unsigned char)*cont;
		if (x > 0x80)
		{
			strncpy(tmp, cont, 2);

			if (!strcmp(tmp, "１")    || !strcmp(tmp, "２") || !strcmp(tmp, "３")\
				|| !strcmp(tmp, "４") || !strcmp(tmp, "５") || !strcmp(tmp, "６")\
				|| !strcmp(tmp, "７") || !strcmp(tmp, "８") || !strcmp(tmp, "９")\
				|| !strcmp(tmp, "０") || !strcmp(tmp, "Ｏ") || !strcmp(tmp, "Ｉ"))
			{
				cnt++;
				if (cnt >= tel_nums)	
				{
					return true;
				}
			}
			else
			{
				cnt = 0;
			}
			cont = cont + 2;
	   }
	   else
	   {
			if (*cont == '1'|| *cont == '2' || *cont == '3' || *cont == '4' \
			|| *cont == '5' || *cont == '6' || *cont == '7' || *cont == '8' \
			|| *cont == '9' || *cont == '0' || *cont == 'O' || *cont == 'o' \
			|| *cont == 'i' || *cont == 'I' || *cont == '(' || *cont == ')' \
			|| *cont == '+' || *cont == '-' || *cont == '/')
			{
				cnt++;
				if (cnt >= tel_nums)	
				{
					return true;
				}
			}
			else
			{
				cnt = 0;
			}
			cont++;	
	   }
	}

	if (cnt <= tel_nums)
	{
		return false;
	}
	
	return true;
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
CMySMGI::CPushTrd::CPushTrd(CMySMGI *smgi,
							unsigned long delay_time)
	: _smgi(smgi)
	, _delay_time(delay_time)
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
						_smgi->do_submit(sm);
					}
				}
				else
				{
					_smgi->do_submit(sm);
				}
				lst.pop_front();
				//Sleep(_delay_time);
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
	, _is_busy(false)
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
		if (!get_busy())
		{
			if ((g_timer.Time_Second() >= _smgi->_sys_par_ex->get_time()->_time_kwbeg)
				&& (g_timer.Time_Second() <= _smgi->_sys_par_ex->get_time()->_time_kwend))
			{
				set_busy(true);
				Sleep(1000);
			}
		}
		else
		{
			if ((g_timer.Time_Second() >= _smgi->_sys_par_ex->get_time()->_time_kwend)
				&& (g_timer.Time_Second() <= _smgi->_sys_par_ex->get_time()->_time_kwend + 1))
			{
				if (_smgi->_sys_par != NULL)
				{
					_smgi->_sys_par->startup();
				}

				if (_smgi->_sys_par_ex != NULL)
				{
					_smgi->_sys_par_ex->startup();
				}

				if (_smgi->_mo_cmd != NULL)
				{
					_smgi->_mo_cmd->startup();
				}

				if (_smgi->_segs != NULL)
				{
					_smgi->_segs->startup();
				}

				if (_smgi->_fee != NULL)
				{
					_smgi->_fee->startup();
				}

				if (_smgi->_serv_schm != NULL)
				{
					_smgi->_serv_schm->startup();
				}

				if (_smgi->_smreg_mng != NULL)
				{
                    _smgi->_smreg_mng->startup();
				}

				if (_smgi->_cont_pool != NULL)
				{
					_smgi->_cont_pool->load();
				}

				if (_smgi->_auditing != NULL)
				{
					_smgi->_auditing->startup();
				}

				set_busy(false);
				Sleep(1000);
			}
		}
		Sleep(1000);
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

bool CMySMGI::CRevSubTrd::update_stat(unsigned long id)
{
	SMYSQLDBLink *db = _smgi->_db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_UPDATE_REV_SUB, _smgi->_db_name, id);
	bool ret = db->_db->Exec(sql) == TRUE;
	if (!ret)
	{
		g_log.append(rpt::error, "[CRevSubTrd::update_stat]error:%s.", db->_db->LastErr());
	}
	_smgi->_db_pool->leave(*db);
	return ret;
}

unsigned int CMySMGI::CRevSubTrd::ThreadEntry(void)
{
	unsigned long count = 0;
	while (!terminated_)
	{
		if (g_ini.get_cfg()->_rev_sub == 1)
		{
			SMYSQLDBLink *db = _smgi->_db_pool->enter();
			char sql[MAX_SQL_LEN] = {0};
			sprintf(sql, SQL_GET_REV_SUB, _smgi->_db_name, _smgi->_db_name);
			CMYSQLRS rs;
			if (db->_db->Exec(sql, &rs))
			{
				while (rs.LoadRow())
				{
					char cont[MAX_SQL_LEN] = {0};
					SCMD_SUBSCRIBE sub;
					strncpy(sub._cmd_id, CMD_REV_SUB, 4);

					if (rs.AsU32(2) == 1)
					{
						sub._opt_type = OPT_TYPE_SUBSCRIBE;
					}
					else
					{
						sub._opt_type = OPT_TYPE_UNSUBSCRIBE;
					}
					sub._seq_id		= rs.AsU32(0);
					strncpy(sub._srv_id, rs.AsStr(3), 10);

					unsigned long mobile = rs.AsU32(1);

					unsigned short city_id = 0;
					SSegs *seg = _smgi->_segs->find(mobile);
					if (seg != NULL)
					{
						city_id = (unsigned short)seg->_city_id;
					}
					else
					{
						city_id = (unsigned short)(mobile / 100000);
					}

					CMSIsdn isdn;
					char tmp[MAX_ADDR_LEN] = {0};

					if (mobile >= 10000000 && mobile <= 99999999)
					{
						isdn.encode(tmp, mobile, (unsigned short)g_ini.get_cfg()->_area_code, false);
					}
					else if (mobile < 1000000000)
					{
						isdn.encode(tmp, mobile, 0, false);
					}
					else
					{
						isdn.encode(tmp, mobile, 0, true);
					}
					strncpy(sub._user_id, tmp, MAX_ADDR_LEN);
	                
					if (encode_rev_subs(sub, cont))
					{
						_smgi->Response(_smgi->_misc_num, city_id, _smgi->_schm_info._ord_okay, 0, SMG_CODING_ASCII, "", "", cont);
						g_log.append(rpt::info, "[Subs]OnRevSubs--Submit successful(Mobile:%lu,SpNum:%s,Cont:%s).",
								_smgi->_misc_num, _smgi->_sp_info._port_num, cont);

						update_stat(rs.AsU32(0));
					}
					else
					{
						g_log.append(rpt::error, "[Subs]OnRevSubs--Submit failed(Mobile:%lu,SpNum:%s,Cont:%s).",
								_smgi->_misc_num, _smgi->_sp_info._port_num, cont);
					}
				}
				rs.FreeRsl();
			}
			else
			{
				g_log.append(rpt::error, "[Subs]OnRevSubs--Select from db failed");
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
				 CMYSQLDBPool	*db_pool,
				 const char		*db_name,
				 CMYSQLDB		*mo_ext_db)
	: _db_pool(db_pool)
	, _smgi_id(smgi_id)
	, _sp_info(sp_info)
	, _misc_num(0)
	, _cfg_db_link(NULL)

	, _resp_trd(NULL)
	, _deli_trd(NULL)
	, _report_trd(NULL)

	, _high_trd(NULL)
	, _low_trd(NULL)
	, _bill_trd(NULL)
	, _busy_trd(NULL)

	, _rev_sub_trd(NULL)

	, _sys_par(NULL)
	, _sys_par_ex(NULL)
	, _mo_cmd(NULL)
	, _segs(NULL)
	, _fee(NULL)
	, _serv_schm(NULL)
	, _cont_pool(NULL)
	, _auditing(NULL)
	, _smreg_mng(NULL)
{

	_mo_ext_db = mo_ext_db;
	strncpy(_db_name, db_name, MAX_PATH);

	CMSIsdn isdn;
	unsigned short city_id = 0;
	unsigned char type = 0;
	_misc_num = isdn.decode(sp_info._misc_num, city_id, type);

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
	if (_startup(cn_info))
	{
		g_log.append(rpt::info, "[CMySMGI]Init SMG Interface successful.");
	}
	else
	{
		g_log.append(rpt::error, "[CMySMGI]Init SMG Interface failed.");
		return;
	}

	_cfg_db_link= _db_pool->enter();
	_audit_db_link	= _db_pool->enter();

	_sys_par	= new CSysPar(_cfg_db_link->_db, db_name);
	_sys_par_ex	= new CSysParEx(_cfg_db_link->_db, db_name);
	_mo_cmd		= new CMoCmd(_db_pool, db_name);
	_segs		= new CSegs(_cfg_db_link->_db, db_name);
	_fee		= new CFee(_cfg_db_link->_db, db_name);
	_cont_pool	= new CContPool(_cfg_db_link->_db, db_name, CONT_POOL_GET_MODE_ALL);

	_auditing	= new CAuditing(_cont_pool, _audit_db_link->_db, db_name);
	if (sp_info._is_iden == 0)
	{
		_auditing->EndThread();
	}
	
	_smreg_mng	= new CSmRegMng(_cfg_db_link->_db, db_name);
	_serv_schm	= new CServSchm(_cfg_db_link->_db, db_name);

	SServSchm *x = _serv_schm->find(SERV_ID_8SM);
	if (x != NULL)
	{
		_schm_info	= *x;
	}
	else
	{
		g_log.append(rpt::error, "[CMySMGI]Init ServSchm Info failed.");
		return;
	}

	_resp_trd	= new CRespTrd(this);
	_deli_trd	= new CDeliverTrd(this);
	_report_trd	= new CReportTrd(this);
	_busy_trd	= new CBusyTrd(this);
	_rev_sub_trd= new CRevSubTrd(this);

	g_log.append(rpt::info, "[CMySMGI]Start tread successful.");

	_high_trd	= new CPushTrd(this, _sys_par_ex->get_time()->_time_highspd);
	g_log.append(rpt::info, "[CMySMGI]Start High prio thread successful.");

	_low_trd	= new CPushTrd(this, _sys_par_ex->get_time()->_time_pushspd);
	g_log.append(rpt::info, "[CMySMGI]Start Low prio thread successful.");

	_bill_trd	= new CPushTrd(this, _sys_par_ex->get_time()->_time_billspd);
	g_log.append(rpt::info, "[CMySMGI]Start Bill prio thread successful.");
	
	bool ret = k_Startup(g_ini.get_cfg()->_rpc_prot,
						 g_ini.get_cfg()->_rpc_ip,
						 g_ini.get_cfg()->_rpc_port);
}

CMySMGI::~CMySMGI(void)
{
	if (_bill_trd != NULL)
	{
		delete _bill_trd;
		_bill_trd = NULL;
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

	k_Cleanup();

	if (_sys_par != NULL)
	{
		delete _sys_par;
		_sys_par = NULL;
	}

	if (_sys_par_ex != NULL)
	{
		delete _sys_par_ex;
		_sys_par_ex = NULL;
	}

	if (_mo_cmd != NULL)
	{
		delete _mo_cmd;
		_mo_cmd = NULL;
	}

	if (_segs != NULL)
	{
		delete _segs;
		_segs = NULL;
	}

	if (_fee != NULL)
	{
		delete _fee;
		_fee = NULL;
	}

	if (_serv_schm != NULL)
	{
		delete _serv_schm;
		_serv_schm = NULL;
	}

	if (_auditing != NULL)
	{
		delete _auditing;
		_auditing = NULL;
	}

	if (_cont_pool != NULL)
	{
		delete _cont_pool;
		_cont_pool = NULL;
	}

	if (_smreg_mng != NULL)
	{
		delete _smreg_mng;
		_smreg_mng = NULL;
	}

	_db_pool->leave(*_cfg_db_link);
	_db_pool->leave(*_audit_db_link);
}

bool CMySMGI::SaveMO(unsigned long mobile,
					 unsigned short city_id,
					 unsigned short cmd_id,
					 unsigned long	cont_id,
					 const char	*port_ext,
					 const char *link_id,
					 const char *cont)
{
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_INSERT_SMG_LOG, _db_name, 0, g_timer.DateTime(), port_ext, cmd_id,
		0, mobile, city_id, SM_STATUS_OK, SM_SRC_TYPE_MO, link_id, cont_id, cont);
	bool ret = db->_db->Exec(sql) == TRUE;
	_db_pool->leave(*db);
	return ret;
}

bool CMySMGI::SaveMT(unsigned long	mobile,
					 unsigned short	city_id,
					 unsigned long	seq_id,
					 unsigned short	fee_id,
					 bool			report,					 
					 unsigned long	cont_id,
					 const char		*port_ext,
					 const char		*link_id,
					 const char		*cont)
{
	bool ret = true;
	try
	{
		SMYSQLDBLink *db = _db_pool->enter();
		char sql[MAX_SQL_LEN] = {0};
		if (report)
		{
			sprintf(sql, SQL_INSERT_SMG_LOG, _db_name, seq_id, g_timer.DateTime(), port_ext, 0,
				fee_id, mobile, city_id, SM_STATUS_WAITING, SM_SRC_TYPE_MT, link_id, cont_id, cont);
		}
		else
		{
			sprintf(sql, SQL_INSERT_SMG_LOG, _db_name, seq_id, g_timer.DateTime(), port_ext, 0,
				fee_id, mobile, city_id, SM_STATUS_OK, SM_SRC_TYPE_MT, link_id, cont_id, cont);
		}
		ret = db->_db->Exec(sql) == TRUE;
		_db_pool->leave(*db);
	}
	catch(...)
	{
		g_log.append(rpt::error, "[CMySMGI::SaveMT]except!");
	}
	return ret;
}

void CMySMGI::Response(unsigned long	mobile,
					   unsigned short	city_id,
					   unsigned short	fee_id,
					   unsigned long	cont_id,
					   unsigned char	coding,
					   const char		*port_ext,
					   const char		*link_id,
					   const char		*cont)
{
	try
	{
		SUBMIT_SM submit;

		CMSIsdn isdn;
		char tmp[MAX_ADDR_LEN] = {0};

		if (mobile >= 10000000 && mobile <= 99999999)
		{
			isdn.encode(tmp, mobile, (unsigned short)g_ini.get_cfg()->_area_code, false);
		}
		else if (mobile < 1000000000)
		{
			isdn.encode(tmp, mobile, 0, false);
		}
		else
		{
			isdn.encode(tmp, mobile, 0, true);
		}
		strncpy(submit._mobile, tmp, MAX_ADDR_LEN);

		//	新号测试 qsf
		if (strlen(g_ini.get_cfg()->_test_port) > 0 
			&& strncmp(port_ext,  g_ini.get_cfg()->_test_port, strlen(g_ini.get_cfg()->_test_port)) == 0)
		{
			sprintf(submit._sp_num, port_ext);
		}
		else
		{
			sprintf(submit._sp_num, "%s%s", _sp_info._port_num, port_ext);
		}

		SFee *fee = _fee->find(fee_id);
		if (fee != NULL)
		{
			strncpy(submit._srv_type, fee->_srv_type, MAX_SRV_TYPE_LEN);

			if (fee->_pay_num != 0)
			{
				memset(tmp, 0, MAX_ADDR_LEN);
				isdn.encode(tmp, fee->_pay_num, 0, true);
				strncpy(submit._charge_num, tmp, MAX_ADDR_LEN);
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

		memcpy(submit._cont, cont, ::MAX_CONT_LEN);
		submit._coding = coding;
		try
		{
			unsigned long seq_id = _submit(submit, false);

			SaveMT(mobile, city_id, seq_id, fee_id, submit._report == 1, cont_id, port_ext, link_id, cont);
		}
		catch(...)
		{
			g_log.append(rpt::info, "mobile:%s,cont:%s", submit._mobile, submit._cont);
			g_log.append(rpt::error, "[CMySMGI::Response]submit to smg except!");
		}
	}
	catch(...)
	{
		g_log.append(rpt::error, "[CMySMGI::Response]except!");
	}
}

bool CMySMGI::update_submit(unsigned long sm_id,
							unsigned long seq_id,
							unsigned char stat)
{
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	if (stat == SM_STATUS_OK)
	{
		sprintf(sql, SQL_UPDATE_SMG_SUBMIT_OK, _db_name, stat, seq_id, sm_id);
	}
	else
	{
		sprintf(sql, SQL_UPDATE_SMG_SUBMIT_WAIT, _db_name, stat, seq_id, sm_id);
	}
	bool ret = db->_db->Exec(sql) == TRUE;
	if (!ret)
	{
		g_log.append(rpt::error, "[update_submit]error:%s.", db->_db->LastErr());
	}
	_db_pool->leave(*db);
	return ret;
}

bool CMySMGI::update_response(unsigned long seq_id,
							  __int64		msg_id)
{
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_UPDATE_SMG_RESP, _db_name, msg_id, seq_id);
	bool ret = db->_db->Exec(sql) == TRUE;
	if (!ret)
	{
		g_log.append(rpt::error, "[update_response]error:%s.", db->_db->LastErr());
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
	sprintf(sql, SQL_UPDATE_SMG_REPORT, _db_name, stat, err_code, msg_id);
	bool ret = db->_db->Exec(sql) == TRUE;
	if (!ret)
	{
		g_log.append(rpt::error, "[update_response]error:%s.", db->_db->LastErr());
	}
	_db_pool->leave(*db);
	return ret;
}

bool CMySMGI::call_report(unsigned long	mobile,
						  __int64		msg_id)
{
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_CALL_DO_REPORT, _db_name, mobile, msg_id);
	bool ret = db->_db->Exec(sql) == TRUE;
	_db_pool->leave(*db);
	return ret;
}

int CMySMGI::call_sp(unsigned long	mobile,
					 unsigned short city_id,
					 const char		*sql_sp,
					 unsigned char	&grp_id,
					 unsigned long	&pts)
{
	int ret = DB_OPT_STATUS_OK;
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, sql_sp, mobile, city_id);
	CMYSQLRS rs;
	if (db->_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			grp_id	= (unsigned char)rs.AsU32(0);
			pts		= rs.AsU32(1);
			ret		= (int)rs.AsU32(2);
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

int CMySMGI::query_pts(unsigned long	mobile,
					   unsigned short	city_id,
					   const char		*sql_sp,
					   unsigned char	&grp_id,
					   unsigned long	&pts,
					   unsigned long	&grp_rank,
					   unsigned long	&city_rank)
{
	int ret = DB_OPT_STATUS_OK;
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, sql_sp, mobile, city_id);
	CMYSQLRS rs;
	if (db->_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			grp_id	= (unsigned char)rs.AsU32(0);
			pts		= rs.AsU32(1);
			grp_rank= rs.AsU32(2);
			city_rank= rs.AsU32(3);
			ret		= (int)rs.AsU32(4);
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

int CMySMGI::cancel_loose(unsigned long		mobile,
						  unsigned short	city_id,
						  const char		*cont)
{
	int ret = DB_OPT_STATUS_OK;
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, "CALL %s.sp_cancel_loose(%lu, %lu, %s, @a)", _db_name, mobile, city_id, cont);
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

bool CMySMGI::dispatch(SSMGLog	&sm)
{
	if (sm._prio_id == 0 || sm._prio_id == 1)
	{
		if (_high_trd->size() >= g_ini.get_cfg()->_cache_size)
		{
			return false;
		}
		else
		{
			_high_trd->push_back(sm);
		}
	}
	else if (sm._src_type == SM_SRC_TYPE_BILL)
	{
		if (_bill_trd->size() >= g_ini.get_cfg()->_cache_size)
		{
			return false;
		}
		else
		{
			_bill_trd->push_back(sm);
		}
	}
	else
	{
		if (_low_trd->size() >= g_ini.get_cfg()->_cache_size)
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
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_CHECK_CAN_PUSH_STATUS, _db_name);
	SMYSQLDBLink *db = _db_pool->enter();
	CMYSQLRS rs;
	if (db->_db->Exec(sql, &rs))
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


void CMySMGI::do_submit(SSMGLog &sm)
{
	try
	{
		SUBMIT_SM submit;

		CMSIsdn isdn;
		char tmp[MAX_ADDR_LEN] = {0};

		if (sm._mobile >= 10000000 && sm._mobile <= 99999999)
		{
			isdn.encode(tmp, sm._mobile, (unsigned short)g_ini.get_cfg()->_area_code, false);
		}
		else
		{
			isdn.encode(tmp, sm._mobile, 0, true);
		}
		strncpy(submit._mobile, tmp, MAX_ADDR_LEN);

		if (sm._port_num != "")
		{
			strncpy(submit._sp_num, sm._port_num, MAX_ADDR_LEN);
		}
		else
		{
			strncpy(submit._sp_num, _sp_info._port_num, MAX_ADDR_LEN);
		}

		strncpy(submit._srv_type, sm._srv_type, MAX_SRV_TYPE_LEN);

		if (sm._pay_num != 0)
		{
			memset(tmp, 0, MAX_ADDR_LEN);
			isdn.encode(tmp, sm._pay_num, 0, true);
			strncpy(submit._charge_num, tmp, MAX_ADDR_LEN);
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

		memcpy(submit._cont, sm._cont, ::MAX_CONT_LEN);
#ifdef _DEBUG
		g_log.append(rpt::info, "sp_num:%s, mobile:%s, cont:%s", submit._sp_num, submit._mobile, submit._cont);
#endif
		submit._coding = SMG_CODING_GB2312;
		unsigned long seq_id = 0;
		try
		{
			seq_id = _submit(submit, false);
		}
		catch(...)
		{
			g_log.append(rpt::info, "mobile:%s,cont:%s", submit._mobile, submit._cont);
			g_log.append(rpt::error, "[CMySMGI::do_submit]submit except!");
		}

		if (seq_id != 0)
		{
			if (!update_submit(sm._sm_idx, seq_id, 
							sm._is_reg >= 1 ? SM_STATUS_WAITING : SM_STATUS_OK))
			{
				g_log.append(rpt::error, "[CMySMGI::do_submit]Update status failed.");
			}		
		}
	}
	catch(...)
	{
		g_log.append(rpt::error, "[CMySMGI::do_submit]except!");
	}
}

void CMySMGI::do_resp(SUBMIT_RESP &resp)
{
	if (!update_response(resp._seq_id, resp._msg_id))
	{
		g_log.append(rpt::error, "[CMySMGI::do_resp]update failed(seq_id:%lu,msg_id:%I64u).", 
			resp._seq_id, resp._msg_id);
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

bool CMySMGI::is_finished(void)
{
	bool ret = false;
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_CALL_IS_FINISHED, _db_name);
	SMYSQLDBLink *db = _db_pool->enter();
	CMYSQLRS rs;
	if (db->_db->Exec(sql, &rs))
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

void CMySMGI::do_deliver(DELIVER_SM &deliver)
{
	/*REPORT_SM report;
	strcpy(report._mobile, "8613312960458");
	report._msg_id = 12345;
	strcpy(report._err_code, "0");
	report._stat	= 2;
	do_report(report);*/

	try
	{
		CMSIsdn isdn;
		unsigned short city_id = 0;
		unsigned char type = 0;
		unsigned long mobile = isdn.decode(deliver._mobile, city_id, type);

		//	获取city_id
		SSegs *seg = _segs->find(mobile);
		if (seg != NULL)
		{
			city_id = (unsigned short)seg->_city_id;
		}
		else
		{
			city_id = (unsigned short)(mobile / 100000);
		}
	
		//	新号测试 qsf
		/*
		if (strlen(g_ini.get_cfg()->_test_port) > 0 
			&& strncmp(deliver._sp_num,  g_ini.get_cfg()->_test_port, strlen(g_ini.get_cfg()->_test_port)) == 0)
		{
			char test_cont[MAX_PATH] = {0};
			sprintf(test_cont, "欢迎使用新接入号测试，您发送的内容是：%s", deliver._cont); 
			test_cont[140] = '\0';

			SaveMO(mobile, city_id, 0, 0, deliver._sp_num, deliver._link_id, deliver._cont);

			Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, deliver._sp_num, deliver._link_id, test_cont);
		}
		//	判断是否采用华为网关
		else */
		if (_sys_par->get()->_misc_smg == 1
			&& _sp_info._smg_type == SMG_TYPE_SGIP_HUAWEI
			&& mobile == _misc_num)
		{
			if (0 == strncmp(deliver._cont, CMD_NOTIFY, 4))
			{
				g_log.append(rpt::info, "[Subs]OnSubsReq--Mobile:%s,SpNum:%s,Cont:%s.",
					deliver._mobile, deliver._sp_num, deliver._cont);

				//	解析内容
				SCMD_SUBSCRIBE subs;
				if (decode_subs(deliver._cont, subs))
				{
					//	用户手机号码（注意：mobile是管理平台号码，eg:10109696）
					unsigned long user_id = isdn.decode(subs._user_id, city_id, type);
					
					//	获取所在区号
					unsigned short city_id = 0;
					SSegs *seg = _segs->find(user_id);
					if (seg != NULL)
					{
						city_id = (unsigned short)seg->_city_id;
					}
					else
					{
						city_id = (unsigned short)(user_id / 100000);
					}

					//	订购
					if (subs._opt_type == OPT_TYPE_SUBSCRIBE)
					{
						SMoCmd cmd;
						if (_mo_cmd->find((unsigned char)_smgi_id, SRV_TYPE_AGREE, cmd))
						{
							SaveMO(user_id, city_id, cmd._cmd_id, 0, "", deliver._link_id, cmd._cont_cmd);
						}
						else
						{
							g_log.append(rpt::error, "Save Mo info failed(Get cmd=SRV_TYPE_AGREE failed).");
							return;
						}

						unsigned char grp_id = 0;
						unsigned long pts = 0;
						g_log.append(rpt::info, "sql:%s.", cmd._sql_sp);
						int ret = call_sp(user_id, city_id, cmd._sql_sp, grp_id, pts);
						switch (ret)
						{
						case DB_OPT_STATUS_OK:	//	返回订购成功
							{
								char cont[MAX_SQL_LEN + 1] = {0};
								encode_subs(subs._seq_id, SUBS_STATUS_SUCCESS, cont);
								Response(mobile, city_id, _schm_info._ord_okay, 0, SMG_CODING_ASCII, "", deliver._link_id, cont);
								g_log.append(rpt::info, "[Subs]OnSubsResp--Subscribe successful(Mobile:%lu,SpNum:%s,Resp_Cont:%s).",
									mobile, _sp_info._port_num, cont);
							}
							break;
						case DB_OPT_STATUS_FORBID:
							{
								char cont[MAX_SQL_LEN + 1] = {0};
								encode_subs(subs._seq_id, SUBS_STATUS_AUTH_FAILED, cont);
								Response(mobile, city_id, _schm_info._ord_fail, 0, SMG_CODING_ASCII, "", deliver._link_id, cont);
								g_log.append(rpt::error, "[Subs]OnSubsResp--Fobid join(Mobile:%lu,SpNum:%s,Resp_Cont:%s).",
										mobile, _sp_info._port_num, cont);
							}
							break;
						case DB_OPT_STATUS_FAILED:
						default:
							{
								char cont[MAX_SQL_LEN + 1] = {0};
								encode_subs(subs._seq_id, SUBS_STATUS_DB_ERROR, cont);
								Response(mobile, city_id, _schm_info._ord_fail, 0, SMG_CODING_ASCII,	"", deliver._link_id, cont);
								g_log.append(rpt::error, "[Subs]OnSubsResp--Join failed(Mobile:%lu,SpNum:%s,Resp_Cont:%s).",
									mobile, _sp_info._port_num, cont);
							}
							break;
						}
					}
					//	取消订购
					else if (subs._opt_type == OPT_TYPE_UNSUBSCRIBE)
					{
						//	保存上行原始消息
						SMoCmd cmd;
						if (_mo_cmd->find((unsigned char)_smgi_id, SRV_TYPE_EXIT, cmd))
						{
							SaveMO(user_id, city_id, cmd._cmd_id, 0, "", deliver._link_id, cmd._cont_cmd);
						}
						else
						{
							g_log.append(rpt::error, "Save Mo info failed(Get cmd=SRV_TYPE_EXIT failed).");
							return;
						}

						SMYSQLDBLink *db = _db_pool->enter();
						char sql[MAX_SQL_LEN] = {0};
						sprintf(sql, cmd._sql_sp, user_id, city_id);
						g_log.append(rpt::info, "sql:%s.", sql);
						if (!db->_db->Exec(sql))
						{
							char cont[MAX_SQL_LEN + 1] = {0};
							encode_subs(subs._seq_id, SUBS_STATUS_DB_ERROR, cont);
							Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_ASCII, "", deliver._link_id, cont);
							g_log.append(rpt::error, "[Subs]OnSubsResp--Cancel failed(Mobile:%lu,SpNum:%s,Resp_Cont:%s).",
										mobile, _sp_info._port_num, cont);
						}
						else
						{
							char cont[MAX_SQL_LEN + 1] = {0};
							encode_subs(subs._seq_id, SUBS_STATUS_SUCCESS, cont);
							Response(mobile, city_id, _schm_info._quit_id, 0, SMG_CODING_ASCII, "", deliver._link_id, cont);
							g_log.append(rpt::info, "[Subs]OnSubsResp--Cancel successful(Mobile:%lu,SpNum:%s,Resp_Cont:%s).",
										mobile, _sp_info._port_num, cont);
						}
						_db_pool->leave(*db);
					}
					else
					{
						g_log.append(rpt::error, "[Subs]OnSubsResp--unknown opt_type:%lu.", subs._opt_type);
					}
				}
				else
				{
					//	解码失败
					char cont[MAX_SQL_LEN + 1] = {0};
					encode_subs(subs._seq_id, SUBS_STATUS_PARAM_INVALID, cont);
					Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_ASCII, "", deliver._link_id, cont);
					g_log.append(rpt::error, "[Subs]OnSubsResp--decode failed,Resp_Cont:%s.", cont);

					//	保存上行原始消息
					SaveMO(mobile, city_id, 0, 0, "", deliver._link_id, deliver._cont);
				}
			}
			//	反向订购响应
			else if (0 == strncmp(deliver._cont, CMD_REV_SUB_RESP, 4))
			{
				g_log.append(rpt::info, "[Subs]OnRevSubResp--Mobile:%s,SpNum:%s,Cont:%s.",
					deliver._mobile, deliver._sp_num, deliver._cont);

				SCMD_SUBSCRIBE_RESP resp;
				if (decode_rev_subs(deliver._cont, resp))
				{
					SMYSQLDBLink *db = _db_pool->enter();
					char sql[MAX_SQL_LEN] = {0};
					sprintf(sql, SQL_UPDATE_REV_SUB_STAT, _db_name, resp._status, resp._seq_id);
					if (!db->_db->Exec(sql))
					{
						g_log.append(rpt::error, "[Subs]Update rev_sub failed--Seq_id:%lu,Result:%d.",
							resp._seq_id, resp._status);
					}
					_db_pool->leave(*db);					
				}
				else
				{
					g_log.append(rpt::error, "[Subs]OnRevSubResp--decode failed!");

					//	保存上行原始消息
					SaveMO(mobile, city_id, 0, 0, "", deliver._link_id, deliver._cont);
				}
			}
			else
			{
				g_log.append(rpt::error, "[Subs]unknown cmd code!");
			}
		}
		else
		{
			char sp_ext[::MAX_ADDR_LEN] = {0};

			//	如果是测试长号，提供完整SP号码给存储过程
			if (strlen(g_ini.get_cfg()->_test_port) > 0 
				&& strncmp(deliver._sp_num,  g_ini.get_cfg()->_test_port, strlen(g_ini.get_cfg()->_test_port)) == 0)
			{
				strcpy(sp_ext, deliver._sp_num);
			}
			else
			{
				strncpy(sp_ext, deliver._sp_num + strlen(_sp_info._port_num), MAX_ADDR_LEN);
			}

			unsigned char srv_type = 0;

			//qsf
			if ((0 != strncmp(_sp_info._port_num, deliver._sp_num, strlen(_sp_info._port_num))) 
				&& 0 != strncmp(deliver._sp_num,  g_ini.get_cfg()->_test_port, strlen(g_ini.get_cfg()->_test_port)))
			{
				SaveMO(mobile, city_id, 0, 0, sp_ext, deliver._link_id, deliver._cont);

				Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, sp_ext, 
					deliver._link_id, _sys_par_ex->get_smgi()->_smgi_help);
			}
			else
			{				
				char *trim_left = TrimLeft(deliver._cont);
				char *trim_right = TrimRight(trim_left);
				char long_sp_ext[MAX_PATH] = {0};
				SMoCmd *cmd = NULL;

				//qsf
				/*
				if (strncmp(deliver._sp_num,  g_ini.get_cfg()->_test_port, strlen(g_ini.get_cfg()->_test_port)) == 0)
				{
					strncpy(long_sp_ext, 
					cmd = _mo_cmd->match((unsigned char)_smgi_id, trim_right, sp_ext);
				}
				else
				{
					cmd = _mo_cmd->match((unsigned char)_smgi_id, trim_right, sp_ext);
				}
				*/

				cmd = _mo_cmd->match((unsigned char)_smgi_id, trim_right, sp_ext);
				if (cmd != NULL)
				{
					char cmd_ext[MAX_PATH] = {0};
					strncpy(cmd_ext, trim_right + strlen(cmd->_cont_cmd), MAX_PATH);

					switch (cmd->_cmd_type)
					{
					//	业务介绍，帮助
					case SRV_TYPE_HELP:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							SMYSQLDBLink *db = _db_pool->enter();
							char sql[MAX_SQL_LEN] = {0};
							sprintf(sql, cmd->_sql_sp, _schm_info._mtod_free, mobile, deliver._link_id);
							if (!db->_db->Exec(sql))
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);

								g_log.append(rpt::error, "[do_deliver]Exec sql=%s failed.", sql);
							}
							_db_pool->leave(*db);
						}
						break;
					//	积分计划
					case SRV_TYPE_AGREE:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							unsigned char grp_id = 0;
							unsigned long pts = 0;
							int ret = call_sp(mobile, city_id, cmd->_sql_sp, grp_id, pts);
							switch (ret)
							{
							case DB_OPT_STATUS_OK:
								{
									//	上SP管理平台
									if (strlen(_sys_par_ex->get_smgi()->_smgi_a1) != 0)
									{
										if (_sp_info._smg_type == SMG_TYPE_SGIP_OPENET 
											&& _sys_par->get()->_misc_smg == 1)
										{
											Response(mobile, city_id, _schm_info._ord_okay, 0, SMG_CODING_GB2312, 
												sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_a1);
											Response(mobile, city_id, _schm_info._push_id, 0, SMG_CODING_GB2312,
												sp_ext, "", _sys_par_ex->get_smgi()->_smgi_a1);
										}
										else
										{
											Response(mobile, city_id, _schm_info._ord_okay, 0, SMG_CODING_GB2312, 
												sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_a1);
										}
									}

									if (strlen(_sys_par_ex->get_smgi()->_smgi_a2) != 0)
									{
										if (_sp_info._smg_type == SMG_TYPE_SGIP_OPENET 
											&& _sys_par->get()->_misc_smg == 1)
										{
											Response(mobile, city_id, _schm_info._push_id, 0, SMG_CODING_GB2312,
												sp_ext, "", _sys_par_ex->get_smgi()->_smgi_a2);
										}
										else
										{
											Response(mobile, city_id, _schm_info._push_id, 0, SMG_CODING_GB2312,
												sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_a2);
										}
									}
									if (strlen(_sys_par_ex->get_smgi()->_smgi_a3) != 0)
									{
										if (_sp_info._smg_type == SMG_TYPE_SGIP_OPENET 
											&& _sys_par->get()->_misc_smg == 1)
										{
											Response(mobile, city_id, _schm_info._push_id, 0, SMG_CODING_GB2312,
												sp_ext, "", _sys_par_ex->get_smgi()->_smgi_a3);
										}
										else
										{
											Response(mobile, city_id, _schm_info._push_id, 0, SMG_CODING_GB2312,
												sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_a3);
										}
									}

									if (strlen(_sys_par_ex->get_smgi()->_smgi_a4) != 0)
									{
										if (_sp_info._smg_type == SMG_TYPE_SGIP_OPENET 
											&& _sys_par->get()->_misc_smg == 1)
										{
											Response(mobile, city_id, _schm_info._push_id, 0, SMG_CODING_GB2312,
												sp_ext, "", _sys_par_ex->get_smgi()->_smgi_a4);
										}
										else
										{
											Response(mobile, city_id, _schm_info._push_id, 0, SMG_CODING_GB2312,
												sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_a4);
										}
									}
								}
								break;
							case DB_OPT_STATUS_FORBID:
								{
									if (_sys_par->get()->_misc_smg == 1)
									{
										Response(mobile, city_id, _schm_info._ord_fail, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_aforbid);
									}
									else
									{
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_aforbid);
									}
								}
								break;
							case DB_OPT_STATUS_FAILED:
							default:
								{
									if (_sys_par->get()->_misc_smg == 1)
									{
										Response(mobile, city_id, _schm_info._ord_fail, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);
									}
									else
									{
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);
									}
								}
								break;
							}
						}
						break;
					//	免费欣赏
					case SRV_TYPE_FREE:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								unsigned char grp_id = 0;
								unsigned long pts = 0;
								int ret = call_sp(mobile, city_id, cmd->_sql_sp, grp_id, pts);
								switch (ret)
								{
									case DB_OPT_STATUS_OK:
										{
											if (pts > 0 && strlen(_sys_par_ex->get_smgi()->_smgi_ttg) != 0)
											{
												char cont[MAX_SQL_LEN] = {0};
												sprintf(cont, _sys_par_ex->get_smgi()->_smgi_ttg, pts, _sys_par->get()->_bon_show);
												Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
														sp_ext, deliver._link_id, cont);
											}
											else
											{
												if (strlen(_sys_par_ex->get_smgi()->_smgi_t1) != 0)
												{
													Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
														sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_t1);
												}

												if (strlen(_sys_par_ex->get_smgi()->_smgi_t2) != 0)
												{
													Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
														sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_t2);
												}

												if (strlen(_sys_par_ex->get_smgi()->_smgi_t3) != 0)
												{
													Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
														sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_t3);
												}

												if (strlen(_sys_par_ex->get_smgi()->_smgi_t4) != 0)
												{
													Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
														sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_t4);
												}
											}
										}
										break;
									case DB_OPT_STATUS_FORBID:
										{
											if (pts > 0)
											{
												if (strlen(_sys_par_ex->get_smgi()->_smgi_tag) != 0)
												{
													char cont[MAX_SQL_LEN] = {0};
													sprintf(cont, _sys_par_ex->get_smgi()->_smgi_tag, pts, _sys_par->get()->_bon_show);
													Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
														sp_ext, deliver._link_id, cont);
												}
											}
											else
											{
												if (strlen(_sys_par_ex->get_smgi()->_smgi_taz) != 0)
												{
													char cont[MAX_SQL_LEN] = {0};
													sprintf(cont, _sys_par_ex->get_smgi()->_smgi_taz, _sys_par->get()->_bon_show);
													Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
														sp_ext, deliver._link_id, cont);
												}
											}
										}
										break;
									case DB_OPT_STATUS_FAILED:
									default:
										{
											Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
												sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);
										}
										break;
								}
							}
						}
						break;
					//	无条件退出
					case SRV_TYPE_EXIT:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							SMYSQLDBLink *db = _db_pool->enter();
							char sql[MAX_SQL_LEN] = {0};
							sprintf(sql, cmd->_sql_sp, mobile, city_id);
							if (!db->_db->Exec(sql))
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);

								g_log.append(rpt::error, "[do_deliver]Exec sql=%s failed.", sql);
							}
							else
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312,
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_tc);
							}
							_db_pool->leave(*db);
						}
						break;
					//	挽留式退出
					case SRV_TYPE_STAY:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								unsigned char grp_id = 0;
								unsigned long pts = 0;
								int ret = call_sp(mobile, city_id, cmd->_sql_sp, grp_id, pts);
								if (grp_id <= 0)
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_ta);

								}
								else if (grp_id == 1)
								{
									if (pts == 0)
									{
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_ta);
									}
									else
									{
										char cont[MAX_SQL_LEN] = {0};
										sprintf(cont, _sys_par_ex->get_smgi()->_smgi_ta, pts);
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, cont);
									}
								}
								else if (grp_id == 2)
								{
									if (pts == 0)
									{
										char cont[MAX_SQL_LEN] = {0};
										sprintf(cont, _sys_par_ex->get_smgi()->_smgi_taaz, _sys_par->get()->_bon_show);
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, cont);
									}
									else
									{
										char cont[MAX_SQL_LEN] = {0};
										sprintf(cont, _sys_par_ex->get_smgi()->_smgi_taag, pts, _sys_par->get()->_bon_show);
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, cont);
									}
								}
								else if (grp_id == 3)
								{
									if (pts == 0)
									{
										char cont[MAX_SQL_LEN] = {0};
										sprintf(cont, _sys_par_ex->get_smgi()->_smgi_tasz);
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, cont);
									}
									else
									{
										char cont[MAX_SQL_LEN] = {0};
										sprintf(cont, _sys_par_ex->get_smgi()->_smgi_tasg, pts);
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, cont);
									}
								}
								else
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_tacfm);
								}
							}
						}
						break;
					//	0000查询定制
					case SRV_TYPE_0000:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							unsigned char grp_id = 0;
							unsigned long pts = 0;
							int ret = call_sp(mobile, city_id, cmd->_sql_sp, grp_id, pts);
							if (ret == DB_OPT_STATUS_OK)
							{
								switch (grp_id)
								{
								case USER_GROUP_T:
									{
										if (pts > 0)
										{
											char cont[MAX_SQL_LEN] = {0};
											sprintf(cont, _sys_par_ex->get_smgi()->_smgi_0000tg, pts);
											Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
												sp_ext, deliver._link_id, cont);
										}
										else
										{
											Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
												sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_0000tz);
										}
									}
									break;
								case USER_GROUP_A:
									{
										if (pts > 0)
										{
											char cont[MAX_SQL_LEN] = {0};
											sprintf(cont, _sys_par_ex->get_smgi()->_smgi_0000ag, pts, _sys_par->get()->_bon_show);
											Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
												sp_ext, deliver._link_id, cont);
										}
										else
										{
											char cont[MAX_SQL_LEN] = {0};
											sprintf(cont, _sys_par_ex->get_smgi()->_smgi_0000az, _sys_par->get()->_bon_show);
											Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
												sp_ext, deliver._link_id, cont);
										}
									}
									break;
								case USER_GROUP_S:
									{
										if (pts > 0)
										{
											char cont[MAX_SQL_LEN] = {0};
											sprintf(cont, _sys_par_ex->get_smgi()->_smgi_0000sg, pts);
											Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
												sp_ext, deliver._link_id, cont);
										}
										else
										{
											char cont[MAX_SQL_LEN] = {0};
											sprintf(cont, _sys_par_ex->get_smgi()->_smgi_0000sz);
											Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
												sp_ext, deliver._link_id, cont);
										}
									}
									break;
								default:
									{
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_null);	
									}
									break;
								}
							}
							else if (ret == DB_OPT_STATUS_FORBID)
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_null);
							}
							else
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);
							}
						}
						break;
					//	00000全部取消
					case SRV_TYPE_00000:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							SMYSQLDBLink *db = _db_pool->enter();
							char sql[MAX_SQL_LEN] = {0};
							sprintf(sql, cmd->_sql_sp, mobile, city_id);
							if (!db->_db->Exec(sql))
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);

								g_log.append(rpt::error, "[do_deliver]Exec sql=%s failed.", sql);
							}
							else
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312,
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_00000);
							}
							_db_pool->leave(*db);
						}
						break;
					//	登记自编短信
					case SRV_TYPE_REG:
						{
							unsigned long cont_id = 0;
							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							
							}
							else
							{
								unsigned char grp_id = 0;
								unsigned long pts = 0;
								int ret = call_sp(mobile, city_id, cmd->_sql_sp, grp_id, pts);
								if (ret == DB_OPT_STATUS_OK)
								{
									if (grp_id <= USER_GROUP_U)
									{
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_null);
									}
									else
									{
										std::string tmp_cont(deliver._cont);
										char proc_sm[::MAX_CONT_LEN + 1] = {0};
										unsigned long sm_len = g_fasparse.shrink(proc_sm, (char*)deliver._cont, MASK_TOTAL);
										unsigned long trunc_len = TruncSM(deliver._cont, (unsigned long)strlen(deliver._cont));

										char resp_cont[MAX_SQL_LEN + 1] = {0};
										if (sm_len < MIN_CONT_LEN)
										{
											if (grp_id == USER_GROUP_T)
											{
												int rret = cancel_loose(mobile, city_id, deliver._cont);
												if (rret == 0)
												{
													Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
														sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_auto);
												}
												else
												{
													char sm_part[TRUNC_LEN * 2 + 1] = {0};
													strncpy(sm_part, tmp_cont.c_str(), trunc_len);
													sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_short, sm_part);
													Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
														sp_ext, deliver._link_id, resp_cont);
												}
											}
											else
											{
												char sm_part[TRUNC_LEN * 2 + 1] = {0};
												strncpy(sm_part, tmp_cont.c_str(), trunc_len);
												sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_short, sm_part);
												Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
													sp_ext, deliver._link_id, resp_cont);
											}
										}
										else if ((g_fasparse.has_kword(proc_sm) != NULL) 
											|| HasTelNum(deliver._cont, 7))
										{
											char sm_part[TRUNC_LEN * 2 + 1] = {0};
											strncpy(sm_part, tmp_cont.c_str(), trunc_len);
											sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_taboo, sm_part);
											Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
													sp_ext, deliver._link_id, resp_cont);
										}
										else
										{
											SContPool *sm = _cont_pool->find(proc_sm, sm_len);
											if (sm != NULL)			
											{
												char sm_part[TRUNC_LEN * 2 + 1] = {0};
												strncpy(sm_part, tmp_cont.c_str(), trunc_len);
												sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_old, sm_part);
												Response(mobile, city_id, _schm_info._mtod_free, sm->_cont_id, SMG_CODING_GB2312, 
													sp_ext, deliver._link_id, resp_cont);

												cont_id = sm->_cont_id;
											}
											else
											{
												char iden[MAX_IDEN_LEN + 1] = {0};
												strncpy(iden, proc_sm + SIGN_LEN, MAX_IDEN_LEN);
												if (strlen(iden) == MAX_IDEN_LEN)
												{
													//	增加自动审核
													if (_sys_par_ex->get_reg()->_reg_autopass == 1)
													{
														cont_id = _auditing->sync_cont((unsigned short)_sys_par_ex->get_reg()->_reg_defcat, 
															0, mobile, 0, deliver._cont, CONT_TYPE_NORMAL);
														if (cont_id > 0)
														{
															//	赠送创作积分
															if (!_auditing->give_pts(mobile, _sys_par->get()->_pts_reg))
															{
																g_log.append(rpt::error, "[CAuditing]Give reg_pts failed(mobile:%lu, pts:%d).",
																	mobile, _sys_par->get()->_pts_reg);
															}

															//	反馈审核通过通知
															char sm_part[TRUNC_LEN * 2 + 1] = {0};
															strncpy(sm_part, tmp_cont.c_str(), trunc_len);

															char resp_cont[MAX_SQL_LEN] = {0};
															sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_accept, sm_part);
															Response(mobile, city_id, _schm_info._mtod_paid, cont_id, SMG_CODING_GB2312, 
																sp_ext, deliver._link_id, resp_cont);

															//	检查是否超过积分限制
															if (_auditing->check_pts_lmt(mobile, _sys_par->get()->_pts_reg))
															{
																if (strlen(_sys_par_ex->get_smgi()->_smgi_reg_pts_lmt) != 0)
																{
																	Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
																		sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_reg_pts_lmt);
																}
															}
														}
														else
														{
															//	登记入库失败
															char sm_part[TRUNC_LEN * 2 + 1] = {0};
															strncpy(sm_part, tmp_cont.c_str(), trunc_len);
															sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_fail, sm_part);

															Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
																sp_ext, deliver._link_id, resp_cont);
														}
													}
													else
													{
														//	成功受理，收取审核费用
														if (_smreg_mng->enroll(mobile, deliver._cont))
														{
															//	待审加入内存中
															SContPool x;
															strncpy(x._iden, iden, MAX_IDEN_LEN);
															x._mo_user = mobile;
															_cont_pool->add_mem(x);

															char sm_part[TRUNC_LEN * 2 + 1] = {0};
															strncpy(sm_part, tmp_cont.c_str(), trunc_len);
															sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_new, sm_part);
															Response(mobile, city_id, _schm_info._mtod_paid, 0, SMG_CODING_GB2312, 
																sp_ext, deliver._link_id, resp_cont);
														}
														else
														{
															//	登记入库失败
															char sm_part[TRUNC_LEN * 2 + 1] = {0};
															strncpy(sm_part, tmp_cont.c_str(), trunc_len);
															sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_fail, sm_part);

															Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
																sp_ext, deliver._link_id, resp_cont);
														}
													}
												}
												else
												{
													char sm_part[TRUNC_LEN * 2 + 1] = {0};
													strncpy(sm_part, tmp_cont.c_str(), trunc_len);
													sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_short, sm_part);
													Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
														sp_ext, deliver._link_id, resp_cont);
												}
											}
										}
									}
								}
								else if (ret == DB_OPT_STATUS_FORBID)
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_null);
								}
								else
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);
								}
							}

							SaveMO(mobile, city_id, cmd->_cmd_id, cont_id, sp_ext, deliver._link_id, deliver._cont);
						}
						break;

					//	登记绿色短信
					case SRV_TYPE_DJLS:
						{
							Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, "尊敬的客户，江苏联通“短信大赛”活动已结束，感谢您的热情参与！本活动结果将于近期公布，请留意中奖通知。详询10010");

							/*unsigned long cont_id = 0;
							if (get_busy())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								std::string tmp_cont(deliver._cont);
								char proc_sm[::MAX_CONT_LEN + 1] = {0};
								unsigned long sm_len = g_fasparse.shrink(proc_sm, (char*)deliver._cont, MASK_TOTAL);
								unsigned long trunc_len = TruncSM(deliver._cont, (unsigned long)strlen(deliver._cont));

								char resp_cont[MAX_SQL_LEN + 1] = {0};
								if (sm_len < MIN_CONT_LEN)
								{
									char sm_part[TRUNC_LEN * 2 + 1] = {0};
									strncpy(sm_part, tmp_cont.c_str(), trunc_len);
									sprintf(resp_cont, _sys_par_ex->get_lssm()->_lssm_reg_short, sm_part);
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, resp_cont);
								}
								else if (g_fasparse.has_kword(proc_sm) != NULL)
								{
									char sm_part[TRUNC_LEN * 2 + 1] = {0};
									strncpy(sm_part, tmp_cont.c_str(), trunc_len);
									sprintf(resp_cont, _sys_par_ex->get_lssm()->_lssm_reg_taboo, sm_part);
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, resp_cont);
								}
								else
								{
									SContPool *sm = _cont_pool->find(proc_sm, sm_len);
									if (sm != NULL)			
									{
										char sm_part[TRUNC_LEN * 2 + 1] = {0};
										strncpy(sm_part, tmp_cont.c_str(), trunc_len);
										sprintf(resp_cont, _sys_par_ex->get_lssm()->_lssm_reg_old, sm_part);
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, resp_cont);

										cont_id = sm->_cont_id;
									}
									else
									{
										char iden[MAX_IDEN_LEN + 1] = {0};
										strncpy(iden, proc_sm + SIGN_LEN, MAX_IDEN_LEN);
										if (strlen(iden) == MAX_IDEN_LEN)
										{
											//	成功受理，收取审核费用
											if (_smreg_mng->enroll_ls(mobile, deliver._cont))
											{
												//	待审加入内存中
												SContPool x;
												strncpy(x._iden, iden, MAX_IDEN_LEN);
												x._mo_user = mobile;
												_cont_pool->add_mem(x);

												char sm_part[TRUNC_LEN * 2 + 1] = {0};
												strncpy(sm_part, tmp_cont.c_str(), trunc_len);
												sprintf(resp_cont, _sys_par_ex->get_lssm()->_lssm_reg_new, sm_part);
												Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
													sp_ext, deliver._link_id, resp_cont);
											}
											else
											{
												Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
													sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);
											}
										}
										else
										{
											char sm_part[TRUNC_LEN * 2 + 1] = {0};
											strncpy(sm_part, tmp_cont.c_str(), trunc_len);
											sprintf(resp_cont, _sys_par_ex->get_lssm()->_lssm_reg_short, sm_part);
											Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
												sp_ext, deliver._link_id, resp_cont);
										}
									}
								}
							}*/

							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

						}
						break;

					//	查询短信排名
					case SRV_TYPE_LSPM:
						{
							Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, "尊敬的客户，江苏联通“短信大赛”活动已结束，感谢您的热情参与！本活动结果将于近期公布，请留意中奖通知。详询10010");

							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							/*if (get_busy())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								SMYSQLDBLink *db = _db_pool->enter();
								char sql[MAX_SQL_LEN] = {0};
								sprintf(sql, cmd->_sql_sp, mobile, city_id, sp_ext, deliver._link_id, cmd_ext);
								if (!db->_db->Exec(sql))
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_m_menu);

									g_log.append(rpt::error, "[GET_MORE]Exec sql=%s failed.", sql);
								}
								_db_pool->leave(*db);
							}*/
						}
						break;

					//	指定短信下载
					case SRV_TYPE_ZDXZ:
						{
							Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, "尊敬的客户，江苏联通“短信大赛”活动已结束，感谢您的热情参与！本活动结果将于近期公布，请留意中奖通知。详询10010");

							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							/*if (get_busy())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								SMYSQLDBLink *db = _db_pool->enter();
								char sql[MAX_SQL_LEN] = {0};
								sprintf(sql, cmd->_sql_sp, mobile, city_id, sp_ext, deliver._link_id, cmd_ext);
								if (!db->_db->Exec(sql))
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_m_menu);

									g_log.append(rpt::error, "[GET_MORE]Exec sql=%s failed.", sql);
								}
								_db_pool->leave(*db);
							}*/
						}
						break;

					//	查积分、排名、基金
					case SRV_TYPE_QUERY:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								unsigned char grp_id = 0;
								unsigned long pts = 0;
								unsigned long grp_rank = 0;
								unsigned long city_rank = 0;
								int ret = query_pts(mobile, city_id, cmd->_sql_sp, grp_id, pts, grp_rank, city_rank);
								if (ret == 0)
								{
									if (grp_id <= 0)
									{
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_null);
									}
									else if (grp_id == 1)
									{
										char cont[MAX_SQL_LEN] = {0};
										sprintf(cont, _sys_par_ex->get_smgi()->_smgi_jt, pts, _sys_par->get()->_bon_show);
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, cont);
									}
									else if (grp_id == 2)
									{
										char cont[MAX_SQL_LEN] = {0};
										sprintf(cont, _sys_par_ex->get_smgi()->_smgi_ja, _sys_par->get()->_bon_show, pts, grp_rank);
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, cont);
									}
									else if (grp_id == 3)
									{
										char cont[MAX_SQL_LEN] = {0};
										sprintf(cont, _sys_par_ex->get_smgi()->_smgi_js, pts, grp_rank);
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, cont);
									}
									else
									{
										char cont[MAX_SQL_LEN] = {0};
										sprintf(cont, _sys_par_ex->get_smgi()->_smgi_jo, pts, grp_rank);
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, cont);
									}
								}
								else
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_null);
								}
							}
						}
						break;
					//	免打扰
					case SRV_TYPE_FAZE:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								bool is_valid = true;
								unsigned long week_map = 0;
								for (unsigned long i = 0; i < (unsigned long)strlen(cmd_ext); ++i)
								{
									unsigned char val = cmd_ext[i];
									switch (val)
									{
									case '1':
										{
											week_map |= 0x000C;
										}
										break;
									case '2':
										{
											week_map |= 0x0030;
										}
										break;
									case '3':
										{
											week_map |= 0x00C0;
										}
										break;
									case '4':
										{
											week_map |= 0x0300;
										}
										break;
									case '5':
										{
											week_map |= 0x0C00;
										}
										break;
									case '6':
										{
											week_map |= 0x3000;
										}
										break;
									case '7':
										{
											week_map |= 0x0003;
										}
										break;
									case '0':
									case '8':
									case '9':
									default:
										is_valid = false;
										break;
									}

									if (!is_valid)
									{
										break;
									}
								}

								if (!is_valid)
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312,
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_whelp);
								}
								else
								{
									SMYSQLDBLink *db = _db_pool->enter();
									char sql[MAX_SQL_LEN] = {0};
									sprintf(sql, cmd->_sql_sp, mobile, city_id, week_map);
									if (!db->_db->Exec(sql))
									{
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
											sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);

										g_log.append(rpt::error, "[do_deliver]Exec sql=%s failed.", sql);
									}
									else
									{
										Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312,
											sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_wok);
									}
									_db_pool->leave(*db);
								}
							}
						}
						break;
					//	获取更多作品
					case SRV_TYPE_MORE:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								SMYSQLDBLink *db = _db_pool->enter();
								char sql[MAX_SQL_LEN] = {0};
								sprintf(sql, cmd->_sql_sp, mobile, city_id, sp_ext, deliver._link_id, cmd_ext);
								if (!db->_db->Exec(sql))
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_m_menu);

									g_log.append(rpt::error, "[GET_MORE]Exec sql=%s failed.", sql);
								}
								_db_pool->leave(*db);
							}
						}
						break;
					//	T100捆绑3条
					case SRV_TYPE_TOP100:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								SMYSQLDBLink *db = _db_pool->enter();
								char sql[MAX_SQL_LEN] = {0};
								sprintf(sql, cmd->_sql_sp, mobile, sp_ext, deliver._link_id);
								if (!db->_db->Exec(sql))
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_md_menu);

									g_log.append(rpt::error, "[DOWNLOAD_TOP3]Exec sql=%s failed.", sql);
								}
								_db_pool->leave(*db);
							}
						}
						break;
					//	定制栏目菜单
					case SRV_TYPE_COL_MENU:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								SMYSQLDBLink *db = _db_pool->enter();
								char sql[MAX_SQL_LEN] = {0};
								sprintf(sql, cmd->_sql_sp, _schm_info._mtod_free, mobile, deliver._link_id);
								if (!db->_db->Exec(sql))
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_md_menu);

									g_log.append(rpt::error, "[do_deliver]Exec sql=%s failed.", sql);
								}
								_db_pool->leave(*db);
							}
						}
						break;
					//	定制栏目
					case SRV_TYPE_COL:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								SMYSQLDBLink *db = _db_pool->enter();
								char sql[MAX_SQL_LEN] = {0};
								sprintf(sql, cmd->_sql_sp, _schm_info._mtod_free, mobile, cmd->_serv_id, sp_ext, deliver._link_id);
								if (!db->_db->Exec(sql))
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_md_menu);

									g_log.append(rpt::error, "[CUSTOM_COL]Exec sql=%s failed.", sql);
								}
								_db_pool->leave(*db);
							}
						}
						break;

					//	抽奖活动查询
					case SRV_TYPE_CXACT:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								SMYSQLDBLink *db = _db_pool->enter();
								char sql[MAX_SQL_LEN] = {0};
								sprintf(sql, cmd->_sql_sp, mobile, sp_ext, deliver._link_id);
								if (!db->_db->Exec(sql))
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);

									g_log.append(rpt::error, "[Query_Raffle]Exec sql=%s failed.", sql);
								}
								_db_pool->leave(*db);
							}
						}
						break;
					//	取消活动激励
					case SRV_TYPE_QXACT:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);

							if (get_busy() || !is_finished())
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								SMYSQLDBLink *db = _db_pool->enter();
								char sql[MAX_SQL_LEN] = {0};
								sprintf(sql, cmd->_sql_sp, mobile, sp_ext, deliver._link_id);
								if (!db->_db->Exec(sql))
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);

									g_log.append(rpt::error, "[do_deliver]Exec sql=%s failed.", sql);
								}
								_db_pool->leave(*db);
							}
						}
						break;			
					//	开启网站(不记录上行请求)
					case SRV_TYPE_WEB:
						{
							if (_sys_par->is_admin(mobile))
							{
								HRESULT hret = 0, rret = 0;
								rret = k_Switch(REMOTE_CONTROL_WEB, hret);
								if ((rret == S_OK) && (hret == S_OK))
								{
									g_log.append(rpt::info, "Start web control successful.");

									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312,
										_sp_info._port_num, deliver._link_id, "系统运行正常[WEB/ON]！");
								}
								else
								{
									g_log.append(rpt::error, "Start web control failed(ret:%d).", rret);

									char err[MAX_SQL_LEN] = {0};
									sprintf(err, "系统运行告警[WEB/ON](err_code=%d)！", rret);
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312,
										_sp_info._port_num, deliver._link_id, err);
								}
							}
							else
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312,
									_sp_info._port_num, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_help);
							}
						}
						break;
					//	开启远程维护(不记录上行请求)
					case SRV_TYPE_ADMIN:
						{
							if (_sys_par->is_admin(mobile))
							{
								HRESULT hret = 0, rret = 0;
								rret = k_Switch(REMOTE_CONTROL_ADMIN, hret);
								if ((rret == S_OK) && (hret == S_OK))
								{
									g_log.append(rpt::info, "Start admin control successful.");

									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312,
										_sp_info._port_num, deliver._link_id, "系统运行正常[WEB/OFF]！");
								}
								else
								{
									g_log.append(rpt::error, "Start admin control failed(ret:%d).", rret);

									char err[MAX_SQL_LEN] = {0};
									sprintf(err, "系统运行告警[WEB/OFF](err_code=%d)！", rret);
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312,
										_sp_info._port_num, deliver._link_id, err);
								}
							}
							else
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312,
									_sp_info._port_num, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_help);
							}
						}
						break;
					//	取消定制栏目
					case SRV_TYPE_COL_QX:
					//	转赠话费
					case SRV_TYPE_GIVE:
					//	调查活动
					case SRV_TYPE_VOTE:
					//	定制短信呼转
					case SRV_TYPE_DZFWD:
					//	取消短信呼转
					case SRV_TYPE_QXFWD:
					default:
						{
							SaveMO(mobile, city_id, cmd->_cmd_id, 0, sp_ext, deliver._link_id, deliver._cont);
						
							if (false)
							{
								Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
									sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_busy);
							}
							else
							{
								//SMYSQLDBLink *db = _db_pool->enter();
								//DWORD time1 = GetTickCount();
								char sql[MAX_SQL_LEN] = {0};
								sprintf(sql, cmd->_sql_sp, mobile, city_id, cmd->_cmd_id, sp_ext, 
									deliver._link_id, cmd_ext);
								if (!_mo_ext_db->Exec(sql))
								{
									Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, 
										sp_ext, deliver._link_id, _sys_par_ex->get_smgi()->_smgi_fail);

									g_log.append(rpt::error, "[OtherCommand]Exec sql=%s failed, err=%s", sql, _mo_ext_db->LastErr());
								}
								//_db_pool->leave(*db);

								//DWORD time2 = GetTickCount();
								//g_log.append(rpt::info, "time:%lu, sql:%s", time2 - time1, sql); 
							}
						}
						break;
					}
				}
				else
				{
					SaveMO(mobile, city_id, 0, 0, sp_ext, deliver._link_id, deliver._cont);

					Response(mobile, city_id, _schm_info._mtod_free, 0, SMG_CODING_GB2312, sp_ext, 
						deliver._link_id, _sys_par_ex->get_smgi()->_smgi_help);
				}
			}
		}
	}
	catch(...)
	{
		g_log.append(rpt::error, "[CMySMGI::do_deliver]except!");
	}
}

void CMySMGI::do_report(REPORT_SM &report)
{
	try
	{
		CMSIsdn isdn;
		unsigned short city_id = 0;
		unsigned char type = 0;
		unsigned long mobile = isdn.decode(report._mobile, city_id, type);

		//	短信下发状态转换
		unsigned char stat = SM_STATUS_OK;
		switch (report._stat)
		{
		case 0:
			stat = SM_STATUS_OK;
			break;
		case 1:
			//stat = SM_STATUS_WAITING;
			//break;
		case 2:
		case 255:
		default:
			stat = SM_STATUS_FAILED;
			break;
		}

#ifdef _DEBUG
		g_log.append(rpt::info, "OnReport--Mobile:%s,State:%d,ErrCode:%s.",
				report._mobile, stat, report._err_code);
#endif
			
		//	记录状态报告信息
		if (!update_report(report._msg_id, stat, report._err_code))
		{
			g_log.append(rpt::error, 
				"[CMySMGI::do_report]Update report failed--msg_id:%I64i,stat:%d,err_code:%s.",
				report._msg_id, stat, report._err_code);
		}

		//	成功才计费
		if (stat == SM_STATUS_OK)
		{
			if (!call_report(mobile, report._msg_id))
			{
				g_log.append(rpt::error, "Call sp_do_report failed(mobile:%s, msg_id:%I64i).",
					report._mobile, report._msg_id);
			}
		}
	}
	catch(...)
	{
		g_log.append(rpt::error, "[CMySMGI::do_report]except!");
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
	_deli_trd->push_back(deliver);
}

void CMySMGI::OnReport(REPORT_SM &report)
{
#ifdef _DEBUG
	printf("\nOnReport\nMobile:%s,SeqID:%I64u,State:%d,ErrCode:%s\n",
		report._mobile, report._msg_id, report._stat, report._err_code);
#endif
	_report_trd->push_back(report);
}

///////////////////////////////////////////////////////////////////////////////
//							CTestSMGI
///////////////////////////////////////////////////////////////////////////////
/*CTestSMGI::CTestSMGI(void)
{
	CONNECT_INFO cn_info;
	cn_info._smg_type	= g_ini.get()->_smg_type;
	cn_info._slip_size	= (unsigned short)g_ini.get()->_slip_windows;
	cn_info._resp_wait_time = g_ini.get()->_resp_wait_time;
	cn_info._resp_resends	= g_ini.get()->_resp_resends;
	cn_info._links		= (unsigned char)g_ini.get()->_links;
	cn_info._interval	= (unsigned short)g_ini.get()->_mt_interval;
	
	cn_info._sp_port		= g_ini.get()->_sp_port;	
	cn_info._smg_port_mo	= g_ini.get()->_smg_mo_port;
	cn_info._smg_port_mt	= g_ini.get()->_smg_mt_port;
	strncpy(cn_info._smg_ip, g_ini.get()->_smg_ip, MAX_IP_LEN);	
	strncpy(cn_info._login_name, g_ini.get()->_login_name, MAX_UID_LEN);
	strncpy(cn_info._login_pswd, g_ini.get()->_login_pwd, MAX_PSWD_LEN);

	strncpy(cn_info._sp_num, g_ini.get()->_sp_num, MAX_ADDR_LEN);
	strncpy(cn_info._corp_id, g_ini.get()->_corp_id, MAX_CORP_ID_LEN);
	cn_info._src_id			= (unsigned long)atol(g_ini.get()->_src_id);
	cn_info._seq_id			= g_ini.get()->_seq_id;
	cn_info._min_seq_id		= g_ini.get()->_min_seq_id;
	cn_info._max_seq_id		= g_ini.get()->_max_seq_id;
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
}

void CTestSMGI::OnDeliver(DELIVER_SM &deliver)
{
#ifdef _DEBUG
	printf("\nOnDeliver\nMobile:%s,SPNum:%s,Coding:%d,Content:%s,LinkID:%s\n",
		deliver._mobile, deliver._sp_num, deliver._coding, deliver._cont, 
		deliver._link_id);
#endif

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
	//	增加短消息过期时间参数
	if (g_ini.get()->_has_expire == 1)
	{
		GetTime(g_ini.get()->_delay_time, sm._expire_time);
	}

	strncpy(sm._cont, "系统正在维护中，请稍候再试，不便之处，敬请见谅。", MAX_CONT_LEN);

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
}*/
