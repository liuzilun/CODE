#pragma once

#include "..\Lib\MYSQLDBPool.h"
#include "..\Lib\WorkerThread.h"
#include "..\Lib\Sync.h"
#include "..\Lib\ClubCommon.h"
#include "MySMGI.h"
#include <hash_map>

static const char *SQL_CHECK_CAN_PUSH
	= "SELECT nxp_mbr.fc_smgi_can_push_sms()";

//	低优先级
static const char *SQL_GET_SMG_LOG_LOW
	= "SELECT a.id, a.at_dt, a.port_id, a.mo, a.mt, IF(a.pay_num>0, a.pay_num, b.pay_num), a.src_type,	a.link_id, a.cont, a.stat,\
		b.srv_type, b.fee_type, b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM nxp_sms.smg_logl a\
		LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id \
		WHERE (a.stat=0 OR a.stat=1) AND b.fee_id IS NOT NULL LIMIT 2000";

static const char *SQL_GET_SMG_LOG_LOW_NOW
	= "SELECT a.id, a.at_dt, a.port_id, a.mo, a.mt, IF(a.pay_num>0, a.pay_num, b.pay_num), a.src_type,	a.link_id, a.cont, a.stat,\
		b.srv_type, b.fee_type, b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM nxp_sms.smg_logl a\
		LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id \
		WHERE a.stat=0 AND b.fee_id IS NOT NULL LIMIT 2000";

static const char *SQL_GET_SMG_LOG_LOW_PORT
	= "SELECT a.id, a.at_dt, a.port_id, a.mo, a.mt, IF(a.pay_num>0, a.pay_num, b.pay_num), a.src_type,	a.link_id, a.cont, a.stat,\
		b.srv_type, b.fee_type, b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM nxp_sms.smg_logl a\
		LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id \
		WHERE (a.stat=0 OR a.stat=1) AND a.port_id=%d AND b.fee_id IS NOT NULL LIMIT 2000";

static const char *SQL_GET_SMG_LOG_LOW_PORT_NOW
	= "SELECT a.id, a.at_dt, a.port_id, a.mo, a.mt, IF(a.pay_num>0, a.pay_num, b.pay_num), a.src_type,	a.link_id, a.cont, a.stat,\
		b.srv_type, b.fee_type, b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM nxp_sms.smg_logl a\
		LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id \
		WHERE a.stat=0 AND a.port_id=%d AND b.fee_id IS NOT NULL LIMIT 2000";

//	中优先级
static const char *SQL_GET_SMG_LOG_MID
	= "SELECT a.id, a.at_dt, a.port_id, a.mo, a.mt, IF(a.pay_num>0, a.pay_num, b.pay_num), a.src_type,	a.link_id, a.cont, a.stat,\
		b.srv_type, b.fee_type, b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM nxp_sms.smg_logm a\
		LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id \
		WHERE (a.stat=0 OR a.stat=1) AND b.fee_id IS NOT NULL LIMIT 2000";

static const char *SQL_GET_SMG_LOG_MID_NOW
	= "SELECT a.id, a.at_dt, a.port_id, a.mo, a.mt, IF(a.pay_num>0, a.pay_num, b.pay_num), a.src_type,	a.link_id, a.cont, a.stat,\
		b.srv_type, b.fee_type, b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM nxp_sms.smg_logm a\
		LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id \
		WHERE a.stat=0 AND b.fee_id IS NOT NULL LIMIT 2000";

static const char *SQL_GET_SMG_LOG_MID_PORT
	= "SELECT a.id, a.at_dt, a.port_id, a.mo, a.mt, IF(a.pay_num>0, a.pay_num, b.pay_num), a.src_type,	a.link_id, a.cont, a.stat,\
		b.srv_type, b.fee_type, b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM nxp_sms.smg_logm a\
		LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id \
		WHERE (a.stat=0 OR a.stat=1) AND a.port_id=%d AND b.fee_id IS NOT NULL LIMIT 2000";

static const char *SQL_GET_SMG_LOG_MID_PORT_NOW
	= "SELECT a.id, a.at_dt, a.port_id, a.mo, a.mt, IF(a.pay_num>0, a.pay_num, b.pay_num), a.src_type,	a.link_id, a.cont, a.stat,\
		b.srv_type, b.fee_type, b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM nxp_sms.smg_logm a\
		LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id \
		WHERE a.stat=0 AND a.port_id=%d AND b.fee_id IS NOT NULL LIMIT 2000";


//	高优先级
static const char *SQL_GET_SMG_LOG_HIGH
	= "SELECT a.id, a.at_dt, a.port_id, a.mo, a.mt, IF(a.pay_num>0, a.pay_num, b.pay_num), a.src_type,	a.link_id, a.cont, a.stat,\
		b.srv_type, b.fee_type, b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM nxp_sms.smg_logh a\
		LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id \
		WHERE (a.stat=0 OR a.stat=1) AND b.fee_id IS NOT NULL";

static const char *SQL_GET_SMG_LOG_HIGH_PORT
	= "SELECT a.id, a.at_dt, a.port_id, a.mo, a.mt, IF(a.pay_num>0, a.pay_num, b.pay_num), a.src_type,	a.link_id, a.cont, a.stat,\
		b.srv_type, b.fee_type, b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM nxp_sms.smg_logh a\
		LEFT JOIN nxp_mbr.fee b ON a.fee_id=b.fee_id \
		WHERE (a.stat=0 OR a.stat=1) AND a.port_id=%d AND b.fee_id IS NOT NULL";

static const char *SQL_UPDATE_SMG_LOG_HIGH
	= "UPDATE nxp_sms.smg_logh SET fin_dt=NOW(), stat=%d WHERE id=%lu";

static const char *SQL_UPDATE_SMG_LOG_MID
	= "UPDATE nxp_sms.smg_logm SET fin_dt=NOW(), stat=%d WHERE id=%lu";

static const char *SQL_UPDATE_SMG_LOG_LOW
	= "UPDATE nxp_sms.smg_logl SET fin_dt=NOW(), stat=%d WHERE id=%lu";

//	重置下发状态
static const char *SQL_RESET_SMG_LOG_STATUS
	= "UPDATE nxp_sms.smg_log SET stat=1 WHERE stat=2 AND seq_id=0 AND rec_dt>=CURDATE()";

class CSMGIDisp
	: public Process::CWorkerThread
{
public:
	CSMGIDisp(CMYSQLDBPool	*db_pool);
	~CSMGIDisp(void);

	unsigned long do_disp(unsigned char prio, bool is_now);
protected:
	unsigned int ThreadEntry(void);
private:
	bool can_push(void);
	CMySMGI* find_smgi(unsigned long port_id);
	void save_seq_id(void);

	bool reset_status(void);
private:
	CMYSQLDBPool	*_db_pool;
	
	typedef std::hash_map<unsigned long, CMySMGI*>	MAP_SMGI;
	MAP_SMGI		_map_smgi;
	Process::CCriticalSection	_cs;
	CPort			*_port;

	RdiClubCommon::CSegment	*_segs;
	SMYSQLDBLink	*_db_link;
};
