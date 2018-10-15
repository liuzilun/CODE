#pragma once

#include "..\Lib\MYSQLDBPool.h"
#include "..\VMSCommon\Port.h"
#include "..\Lib\WorkerThread.h"
#include "..\Lib\Sync.h"
#include "..\VMSCommon\Segs.h"
#include "MySMGI.h"
#include <hash_map>

static const char *SQL_CHECK_CAN_PUSH
	= "SELECT fc_can_push()";

//	定时加载smg_log，通过SMGI分发下去
static const char *SQL_GET_SMG_LOG_LOW
	= "SELECT a.id, a.port_ext, a.mobile, a.city_id, IF(a.pay_num>0, a.pay_num, b.pay_num), a.prio, a.src_type, \
		a.link_id, a.cont, a.stat, b.port_id, b.port, b.srv_type, b.fee_type, \
		b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM %s.smg_log a\
		LEFT JOIN %s.fee b ON a.fee_id=b.fee_id \
		WHERE (a.stat=0 OR a.stat=1) AND b.fee_id IS NOT NULL ORDER BY a.prio LIMIT 2000";

//	定时加载smg_log，通过SMGI分发下去
static const char *SQL_GET_SMG_LOG_LOW_PORT
	= "SELECT a.id, a.port_ext, a.mobile, a.city_id, IF(a.pay_num>0, a.pay_num, b.pay_num), a.prio, a.src_type, \
		a.link_id, a.cont, a.stat, b.port_id, b.port, b.srv_type, b.fee_type, \
		b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM %s.smg_log a\
		LEFT JOIN %s.fee b ON a.fee_id=b.fee_id \
		WHERE (a.stat=0 OR a.stat=1) AND b.port_id=%d ORDER BY a.prio LIMIT 2000";

static const char *SQL_UPDATE_SMG_LOG
	= "UPDATE %s.smg_log SET stat=2, city_id=%d WHERE id=%lu";
	  
static const char *SQL_GET_SMG_LOG_HIGH
	= "SELECT a.id, a.port_ext, a.mobile, a.city_id, IF(a.pay_num>0, a.pay_num, b.pay_num), a.prio, a.src_type, \
		a.link_id, a.cont, a.stat, b.port_id, b.port, b.srv_type, b.fee_type,\
		b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM %s.smg_log a\
		LEFT JOIN %s.fee b ON a.fee_id=b.fee_id \
		WHERE a.stat=0 AND b.fee_id IS NOT NULL ORDER BY a.prio LIMIT 2000";

static const char *SQL_GET_SMG_LOG_HIGH_PORT
	= "SELECT a.id, a.port_ext, a.mobile, a.city_id, IF(a.pay_num>0, a.pay_num, b.pay_num), a.prio, a.src_type, \
		a.link_id, a.cont, a.stat, b.port_id, b.port, b.srv_type, b.fee_type,\
		b.fee_val, b.ot_flag, b.prio, b.is_agent, b.is_reg FROM %s.smg_log a\
		LEFT JOIN %s.fee b ON a.fee_id=b.fee_id \
		WHERE a.stat=0 AND b.port_id=%d ORDER BY a.prio LIMIT 2000";

//	重置下发状态
static const char *SQL_RESET_SMG_LOG_STATUS
	= "UPDATE %s.smg_log SET stat=1 WHERE stat=2 AND seq_id=0 AND rec_dt>=CURDATE()";

class CSMGIDisp
	: public Process::CWorkerThread
{
public:
	CSMGIDisp(CMYSQLDBPool	*db_pool,
			  const char	*db_name,
			  CMYSQLDB		*mo_ext_db);
	~CSMGIDisp(void);

	unsigned long do_disp(void);
protected:
	unsigned int ThreadEntry(void);
private:
	bool can_push(void);
	CMySMGI* find_smgi(unsigned long port_id);
	void save_seq_id(void);

	bool reset_status(void);
private:
	CMYSQLDBPool	*_db_pool;
	char			_db_name[MAX_PATH];

	typedef std::hash_map<unsigned long, CMySMGI*>	MAP_SMGI;
	MAP_SMGI		_map_smgi;
	Process::CCriticalSection	_cs;
	CPort			*_port;

	CSegs			*_segs;
	SMYSQLDBLink	*_db_link;
};
