#pragma once

#include <list>
#include <hash_map>
#pragma warning (disable : 4275)
#include "..\SMGI\SMGI.h"
#include "..\SMGI\SMGDefine.h"

#include "..\Lib\WorkerThread.h"
#include "..\Lib\Sync.h"
#include "..\Lib\myfile.h"
#include "..\Lib\MYSQLDBPool.h"
#include "..\VMSCommon\SysPar.h"
#include "..\VMSCommon\SysParEx.h"
#include "..\VMSCommon\Port.h"
#include "..\VMSCommon\MoCmd.h"
#include "..\VMSCommon\Segs.h"
#include "..\VMSCommon\Fee.h"
#include "..\VMSCommon\ServSchm.h"
#include "..\VMSCommon\ContPool.h"
#include "..\VMSCommon\SMRegMng.h"

//	提交给底层到SMG
static const char *SQL_UPDATE_SMG_SUBMIT_OK
	= "UPDATE %s.smg_log SET stat=%d, seq_id=%lu, fin_dt=NOW() WHERE id=%lu";
static const char *SQL_UPDATE_SMG_SUBMIT_WAIT
	= "UPDATE %s.smg_log SET stat=%d, seq_id=%lu WHERE id=%lu";

//	收到response
static const char *SQL_UPDATE_SMG_RESP
	= "UPDATE %s.smg_log SET msg_id=%I64i WHERE seq_id=%lu";

//	收到report
static const char *SQL_UPDATE_SMG_REPORT
	= "UPDATE %s.smg_log SET fin_dt=NOW(), stat=%d, err_code=\'%s\' WHERE msg_id=%I64i";

//	记录MO/MT
static const char *SQL_INSERT_SMG_LOG
	= "INSERT INTO %s.smg_log (seq_id, fin_dt, port_ext, cmd_id, fee_id, mobile, city_id, stat, src_type, link_id, cont_id, cont)\
	  VALUES (%lu, %I64u, \'%s\', %d, %d, %lu, %d, %d, %d, \'%s\', %lu, \'%s\')";

static const char *SQL_CHECK_CAN_PUSH_STATUS
	= "SELECT %s.fc_can_push()";

static const char *SQL_CALL_DO_REPORT
	= "CALL %s.sp_do_report(%lu, %I64i)";

static const char *SQL_GET_REV_SUB
	= "SELECT a.id, a.mobile, a.opt_type, b.srv_type FROM %s.rev_sub a\
	  LEFT JOIN %s.fee b ON a.fee_id=b.fee_id WHERE a.stat=0 AND b.fee_id IS NOT NULL";

static const char *SQL_UPDATE_REV_SUB
	= "UPDATE %s.rev_sub SET stat=1 WHERE id=%lu";

static const char *SQL_UPDATE_REV_SUB_STAT
	= "UPDATE %s.rev_sub SET fin_dt=NOW(), result=%d WHERE id=%lu";

static const char *SQL_CALL_IS_FINISHED
	= "SELECT %s.fc_score_is_finished()";

enum EREMOTE_CONTROL
{
	REMOTE_CONTROL_WEB		= 0,					//	启动网站功能
	REMOTE_CONTROL_ADMIN	= 1						//	启动远程维护功能
};

enum ESM_PRIO
{
	SM_PRIO_RESP	= 0,							//	回应
	SM_PRIO_DOWN	= 1,							//	下载
	SM_PRIO_BATCH	= 2								//	批发
};

enum ESM_SRC_TYPE
{
	SM_SRC_TYPE_MO	= 0,							//	上行
	SM_SRC_TYPE_MT	= 1,							//	对称MT
	SM_SRC_TYPE_PUSH= 2,							//	PUSH作品
	SM_SRC_TYPE_WEB	= 3,							//	网站通知
	SM_SRC_TYPE_BILL= 4,							//	包月账单
	SM_SRC_TYPE_SYS	= 5,							//	系统通知
	SM_SRC_TYPE_TRIG= 6,							//	内容触发
	SM_SRC_TYPE_FWD	= 7								//	短信呼转
};

enum ESM_STATUS
{
	SM_STATUS_PENDING	= 0,						//	待发
	SM_STATUS_LIMIT		= 1,						//	限时待发
	SM_STATUS_WAITING	= 2,						//	等待
	SM_STATUS_OK		= 3,						//	完成
	SM_STATUS_FAILED	= 4,						//	失败
	SM_STATUS_CANCEL	= 5							//	取消
};

enum EDB_OPT_STATUS
{
	DB_OPT_STATUS_FAILED	= -1,					//	失败
	DB_OPT_STATUS_OK		= 0,					//	成功
	DB_OPT_STATUS_FORBID	= 1						//	禁止
};

enum EUSER_GROUP
{
	USER_GROUP_QXS			= -3,
	USER_GROUP_QXA			= -2,
	USER_GROUP_QXT			= -1,
	USER_GROUP_U			= 0,
	USER_GROUP_T			= 1,
	USER_GROUP_A			= 2,
	USER_GROUP_S			= 3,
};

#pragma pack (push, 1)
struct SSMGLog
{
	unsigned long	_sm_idx;						//	IDX
	unsigned long	_mobile;						//	目的方
	unsigned short	_city_id;						//	所在区号
	unsigned long	_pay_num;						//	付费号码
	char			_port_num[21];					//	接入号
	unsigned char	_prio_id;						//	优先级别ID
	char			_srv_type[10];					//	业务代码
	char			_fee_type[2];					//	计费类型
	unsigned short	_fee_val;						//	计费值
	unsigned char	_src_type;						//	来源
	unsigned char	_prio;							//	优先级
	unsigned char	_ot_flag;						//	引起下行标志
	unsigned char	_is_agent;						//	代理标志，0应收1实收
	unsigned char	_is_reg;						//	需要状态报告
	unsigned char	_status;						//	下发状态
	char			_link_id[20];					//	LINKID
	char			_cont[161];						//	短信原文

	SSMGLog(void)
	{
		memset(this, 0, sizeof(SSMGLog));
	}

	~SSMGLog(void)
	{
	}
};
#pragma pack (pop, 1)

class CMySMGI
	: public CSMGI
{
	class CRespTrd
		: public Process::CWorkerThread
	{
	public:
		CRespTrd(CMySMGI *smgi);
		~CRespTrd(void);

		void push_back(SUBMIT_RESP &sm);
	protected:
		unsigned int ThreadEntry(void);
	private:
		CMySMGI	*_smgi;

		Process::CCriticalSection	_cs;
		std::list<SUBMIT_RESP>		_lst;
	};

	class CDeliverTrd
		: public Process::CWorkerThread
	{
	public:
		CDeliverTrd(CMySMGI *smgi);
		~CDeliverTrd(void);

		void push_back(DELIVER_SM &sm);
	protected:
		unsigned int ThreadEntry(void);
	private:
		CMySMGI	*_smgi;

		Process::CCriticalSection	_cs;
		std::list<DELIVER_SM>		_lst;
	};

	class CReportTrd
		: public Process::CWorkerThread
	{
	public:
		CReportTrd(CMySMGI *smgi);
		~CReportTrd(void);

		void push_back(REPORT_SM &sm);
	protected:
		unsigned int ThreadEntry(void);
	private:
		CMySMGI	*_smgi;

		Process::CCriticalSection	_cs;
		std::list<REPORT_SM>		_lst;
	};

	class CPushTrd
		: public Process::CWorkerThread
	{
	public:
		CPushTrd(CMySMGI		*smgi,
				 unsigned long	delay_time);
		~CPushTrd(void);

		void push_back(SSMGLog &sm);
		unsigned long size(void)
		{
			unsigned long ret = 0;
			_cs.Lock();
			ret = (unsigned long)_lst.size();
			_cs.Unlock();

			return ret;
		}
	protected:
		unsigned int ThreadEntry(void);
	private:
		CMySMGI			*_smgi;
		unsigned long	_delay_time;

		Process::CCriticalSection	_cs;
		std::list<SSMGLog>			_lst;
	};
	
	class CBusyTrd
		: public Process::CWorkerThread
	{
	public:
		CBusyTrd(CMySMGI *smgi);
		~CBusyTrd(void);

		bool get_busy(void)
		{
			Process::CAutoLock lock(&_cs_busy);
			return _is_busy;
		}
	protected:
		unsigned int ThreadEntry(void);
	private:
		bool	_is_busy;
		Process::CCriticalSection _cs_busy;

		void set_busy(bool busy)
		{
			Process::CAutoLock lock(&_cs_busy);
			_is_busy = busy;
		}
	private:
		CMySMGI	*_smgi;
	};

	//	反向订购线程
	class CRevSubTrd
		: public Process::CWorkerThread
	{
	public:
		CRevSubTrd(CMySMGI *smgi);
		~CRevSubTrd(void);
	protected:
		unsigned int ThreadEntry(void);

		bool update_stat(unsigned long id);
	private:
		CMySMGI	*_smgi;
	};
public:
	CMySMGI(unsigned long	smgi_id,
			SSPInfo			&sp_info,
			CMYSQLDBPool	*db_pool,
			const char		*db_name,
			CMYSQLDB		*mo_ext_db);
	~CMySMGI(void);

	void do_resp(SUBMIT_RESP &resp);
	void do_deliver(DELIVER_SM &deliver);
	void do_report(REPORT_SM &report);
	void do_submit(SSMGLog &sm);

	unsigned long get_last_seq_id(void)
	{
		return _get_seq_id();
	}

	bool dispatch(SSMGLog &sm);

	bool can_push(void);
protected:
	void OnSubmitResp(SUBMIT_RESP &resp);
	void OnDeliver(DELIVER_SM &deliver);
	void OnReport(REPORT_SM &report);

	//	保存上行记录
	bool SaveMO(unsigned long	mobile,
				unsigned short	city_id,
				unsigned short	cmd_id,
				unsigned long	cont_id,
				const char		*port_ext,
				const char		*link_id,
				const char		*cont);

	//	保存下行记录
	bool SaveMT(unsigned long	mobile,
				unsigned short	city_id,
				unsigned long	seq_id,
				unsigned short	fee_id,
				bool			report,
				unsigned long	cont_id,
				const char		*port_ext,
				const char		*link_id,
				const char		*cont);
	
	//	点播下行短信
	void Response(unsigned long		mobile,
				  unsigned short	city_id,
				  unsigned short	fee_id,
				  unsigned long		cont_id,
				  unsigned char		coding,
				  const char		*port_ext,
				  const char		*link_id,
				  const char		*cont);
private:
	bool update_submit(unsigned long	sm_id,
					   unsigned long	seq_id,
					   unsigned char	stat);
	bool update_response(unsigned long	seq_id,
						 __int64		msg_id);
	bool update_report(__int64			msg_id,
					   unsigned char	stat,
					   const char		*err_code);
	bool call_report(unsigned long	mobile,
					 __int64		msg_id);

	int call_sp(unsigned long	mobile,
				unsigned short	city_id,
				const char		*sql_sp,
				unsigned char	&grp_id,
				unsigned long	&pts);

	int query_pts(unsigned long		mobile,
				  unsigned short	city_id,
				  const char		*sql_sp,
				  unsigned char		&grp_id,
				  unsigned long		&pts,
				  unsigned long		&grp_rank,
				  unsigned long		&city_rank);

	int cancel_loose(unsigned long	mobile,
					 unsigned short	city_id,
					 const char		*cont);

	bool get_busy(void)
	{
		return _busy_trd->get_busy();
	}

	bool is_finished(void);
private:
	CRespTrd	*_resp_trd;
	friend class CRespTrd;

	CDeliverTrd	*_deli_trd;
	friend class CDeliverTrd;

	CReportTrd	*_report_trd;
	friend class CReportTrd;

	CPushTrd		*_high_trd;
	CPushTrd		*_low_trd;
	CPushTrd		*_bill_trd;
    friend class	CPushTrd;

	CBusyTrd		*_busy_trd;
	friend class	CBusyTrd;

	CRevSubTrd		*_rev_sub_trd;
	friend class	CRevSubTrd;
private:
	CMYSQLDBPool	*_db_pool;
	char			_db_name[MAX_PATH];

	unsigned long	_smgi_id;
	SSPInfo			_sp_info;
	unsigned long	_misc_num;

	CSysPar			*_sys_par;
	CSysParEx		*_sys_par_ex;
	CMoCmd			*_mo_cmd;
	CSegs			*_segs;
	CFee			*_fee;
	CServSchm		*_serv_schm;
	CContPool		*_cont_pool;
	CAuditing		*_auditing;
	CSmRegMng		*_smreg_mng;
	SMYSQLDBLink	*_cfg_db_link;
	SMYSQLDBLink	*_audit_db_link;

	//	8短信资费信息
	SServSchm		_schm_info;

	//	上行专用数据连接
	CMYSQLDB		*_mo_ext_db;
};

//	流量测试SMGI
class CTestCapSMGI
	: public CSMGI
{
public:
	CTestCapSMGI(void);
	~CTestCapSMGI(void);
protected:
	void OnSubmitResp(SUBMIT_RESP &resp);
	void OnDeliver(DELIVER_SM &deliver);
	void OnReport(REPORT_SM &report);
};

//	维护SMGI
class CTestSMGI
	: public CSMGI
{
public:
	CTestSMGI(void);
	~CTestSMGI(void);
protected:
	void OnSubmitResp(SUBMIT_RESP &resp);
	void OnDeliver(DELIVER_SM &deliver);
	void OnReport(REPORT_SM &report);
};