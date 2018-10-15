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
#include "..\Lib\ClubCommon.h"

#include "Port.h"
#include "MoCmd.h"
#include "Fee.h"
#include "SmsPool.h"
#include "ServSchm.h"
#include "CmdResp.h"

//	�ύ���ײ㵽SMG
static const char *SQL_UPDATE_SMG_SUBMIT_OK_HIGH
	= "UPDATE nxp_sms.smg_logh SET stat=%d, seq_id=%lu, fin_dt=NOW() WHERE id=%lu";
static const char *SQL_UPDATE_SMG_SUBMIT_WAIT_HIGH
	= "UPDATE nxp_sms.smg_logh SET stat=%d, seq_id=%lu, fin_dt=NOW() WHERE id=%lu";

static const char *SQL_UPDATE_SMG_SUBMIT_OK_MID
	= "UPDATE nxp_sms.smg_logm SET stat=%d, seq_id=%lu, fin_dt=NOW() WHERE id=%lu";
static const char *SQL_UPDATE_SMG_SUBMIT_WAIT_MID
	= "UPDATE nxp_sms.smg_logm SET stat=%d, seq_id=%lu, fin_dt=NOW() WHERE id=%lu";

static const char *SQL_UPDATE_SMG_SUBMIT_OK_LOW
	= "UPDATE nxp_sms.smg_logl SET stat=%d, seq_id=%lu, fin_dt=NOW() WHERE id=%lu";
static const char *SQL_UPDATE_SMG_SUBMIT_WAIT_LOW
	= "UPDATE nxp_sms.smg_logl SET stat=%d, seq_id=%lu, fin_dt=NOW() WHERE id=%lu";

//	�յ�response
static const char *SQL_UPDATE_SMG_RESP
	= "UPDATE nxp_sms.smg_log SET fin_dt=NOW(), msg_id=%I64i WHERE seq_id=%lu";

//	�յ�report
static const char *SQL_UPDATE_SMG_REPORT
	= "UPDATE nxp_sms.smg_log SET fin_dt=NOW(), stat=%d, err_code=\'%s\' WHERE msg_id=%I64i";

static const char *SQL_CHECK_CAN_PUSH_STATUS
	= "SELECT nxp_mbr.fc_smgi_can_push_sms()";

static const char *SQL_CALL_DO_REPORT
	= "CALL nxp_mbr.sp_smgi_do_report(%I64i, %I64i)";

static const char *SQL_GET_MBR_REV_SUB
	= "SELECT a.seq_id, nxp_mbr.fc_common_convert_mbr_id_to_isdn(a.mbr_id), a.opt_type, b.srv_type FROM nxp_mbr.mbr_rev_sub a\
	  LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id WHERE a.stat=0 AND b.fee_id IS NOT NULL";

static const char *SQL_UPDATE_REV_SUB
	= "UPDATE nxp_mbr.mbr_rev_sub SET stat=1 WHERE seq_id=%lu";

static const char *SQL_UPDATE_REV_SUB_STAT
	= "UPDATE nxp_mbr.mbr_rev_sub SET fin_dt=NOW(), result=%d WHERE seq_id=%lu";

static const char *SQL_GET_PORT_ID
	= "SELECT port_id, port_num FROM nxp_sms.smg_port WHERE port_num like '%\'%s\'%'";

enum PUSH_PRIO
{
	PUSH_PRIO_HIGH	= 0,
	PUSH_PRIO_MID	= 1,
	PUSH_PRIO_LOW	= 2
};

enum ESM_SRC_TYPE
{
	SM_SRC_TYPE_MO	= 0,							//	����
	SM_SRC_TYPE_MT	= 1,							//	�Գ�MT
	SM_SRC_TYPE_PUSH= 2,							//	PUSH��Ʒ
	SM_SRC_TYPE_WEB	= 3,							//	��վ֪ͨ
	SM_SRC_TYPE_BILL= 4,							//	�����˵�
	SM_SRC_TYPE_SYS	= 5,							//	ϵͳ֪ͨ
	SM_SRC_TYPE_TRIG= 6,							//	���ݴ���
	SM_SRC_TYPE_FWD	= 7								//	���ź�ת
};

enum ESM_STATUS
{
	SM_STATUS_PENDING	= 0,						//	����
	SM_STATUS_LIMIT		= 1,						//	��ʱ����
	SM_STATUS_WAITING	= 2,						//	�ȴ�
	SM_STATUS_OK		= 3,						//	���
	SM_STATUS_FAILED	= 4,						//	ʧ��
	SM_STATUS_CANCEL	= 5							//	ȡ��
};

enum EDB_OPT_STATUS
{
	DB_OPT_STATUS_FAILED	= -1,					//	ʧ��
	DB_OPT_STATUS_OK		= 0,					//	�ɹ�
	DB_OPT_STATUS_FORBID	= 1						//	��ֹ
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
	__int64			_at_dt;							//	��ʱ�·�ʱ��
	unsigned char	_port_id;						//	�˿�ID
	char			_mo[21];						//	����
	__int64			_mt;							//	Ŀ�ķ�
	__int64			_pay_num;						//	���Ѻ���
	
	char			_srv_type[10];					//	ҵ�����
	char			_fee_type[2];					//	�Ʒ�����
	unsigned short	_fee_val;						//	�Ʒ�ֵ
	
	unsigned char	_src_type;						//	��Դ
	unsigned char	_prio;							//	���ȼ�
	unsigned char	_ot_flag;						//	�������б�־
	unsigned char	_is_agent;						//	�����־��0Ӧ��1ʵ��
	unsigned char	_is_reg;						//	��Ҫ״̬����
	unsigned char	_status;						//	�·�״̬
	char			_link_id[20];					//	LINKID
	char			_cont[1024];					//	����ԭ��

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
		CPushTrd(CMySMGI	*smgi, unsigned char prio);
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
		unsigned char	_prio;

		Process::CCriticalSection	_cs;
		std::list<SSMGLog>			_lst;
	};
	class CBusyTrd
		: public Process::CWorkerThread
	{
	public:
		CBusyTrd(CMySMGI *smgi);
		~CBusyTrd(void);

	protected:
		unsigned int ThreadEntry(void);
	private:
		CMySMGI	*_smgi;
	};

	//	���򶩹��߳�
	class CRevSubTrd
		: public Process::CWorkerThread
	{
	public:
		CRevSubTrd(CMySMGI *smgi);
		~CRevSubTrd(void);
	protected:
		unsigned int ThreadEntry(void);

		bool update_stat(unsigned long seq_id);
	private:
		CMySMGI	*_smgi;
	};
public:
	CMySMGI(unsigned long	smgi_id,
			SSPInfo			&sp_info,
			CMYSQLDBPool	*db_pool);
	~CMySMGI(void);

	void do_resp(SUBMIT_RESP &resp);
	void do_deliver(DELIVER_SM &deliver);
	void do_report(REPORT_SM &report);
	void do_submit(SSMGLog &sm, unsigned char prio);

	unsigned long get_last_seq_id(void)
	{
		return _get_seq_id();
	}

	bool dispatch(SSMGLog &sm, unsigned char prio);

	bool can_push(void);
protected:
	void OnSubmitResp(SUBMIT_RESP &resp);
	void OnDeliver(DELIVER_SM &deliver);
	void OnReport(REPORT_SM &report);

	//	�������м�¼
	bool SaveMO(__int64			mobile,
				unsigned short	cmd_id,
				unsigned long	mmx_id,
				const char		*sp_num,
				const char		*link_id,
				const char		*cont);

	//	�������м�¼
	bool SaveMT(__int64			mobile,
				unsigned long	seq_id,
				unsigned short	fee_id,
				bool			is_rpt,
				unsigned long	mmx_id,
				const char		*sp_num,
				const char		*link_id,
				const char		*cont);
	
	//	�㲥���ж���
	void Response(__int64			mobile,
				  unsigned short	fee_id,
				  unsigned long		mmx_id,
				  unsigned char		coding,
				  const char		*sp_num,
				  const char		*link_id,
				  const char		*cont);
private:
	bool update_submit(unsigned long	sm_id,
					   unsigned long	seq_id,
					   unsigned char	stat,
					   unsigned char	prio);
	bool update_response(unsigned long	seq_id,
						 __int64		msg_id);
	bool update_report(__int64			msg_id,
					   unsigned char	stat,
					   const char		*err_code);
	bool call_report(__int64	mobile,
					 __int64	msg_id);

	int cancel_loose(unsigned long	mobile,
					 unsigned short	city_id,
					 const char		*cont);

	bool is_finished(void);
private:
	CRespTrd	*_resp_trd;
	friend class CRespTrd;

	CDeliverTrd	*_deli_trd;
	friend class CDeliverTrd;

	CReportTrd	*_report_trd;
	friend class CReportTrd;

	CPushTrd		*_high_trd;
	CPushTrd		*_mid_trd;
	CPushTrd		*_low_trd;
    friend class	CPushTrd;

	CBusyTrd		*_busy_trd;
	friend class	CBusyTrd;

	CRevSubTrd		*_rev_sub_trd;
	friend class	CRevSubTrd;
private:
	CMYSQLDBPool	*_db_pool;
	SMYSQLDBLink	*_db_link;

	unsigned long	_smgi_id;
	SSPInfo			_sp_info;

	CMoCmd			*_mo_cmd;
	CFee			*_fee;
	CSmsPool		*_sms_pool;
	CServSchm		*_serv_schm;
	CCmdResp		*_cmd_resp;

	RdiClubCommon::CSegment		*_segment;
	RdiClubCommon::CProcSync	*_proc_sync;
	RdiClubCommon::CFuncCommon	*_func_common;
};
