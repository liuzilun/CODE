#pragma once

#include "..\Lib\MySQLDB.h"
#include "..\Lib\Sync.h"
#include "..\Lib\WorkerThread.h"
#include "Define.h"
#include "SysPar.h"
#include "SysParEx.h"
#include <vector>
#include <hash_map>

enum ECONT_POOL_GET_MODE
{
	CONT_POOL_GET_MODE_TRIGGER	= 0,						//	仅触发的作品
	CONT_POOL_GET_MODE_ALL		= 1							//	所有作品
};

enum ECONT_TYPE
{
	CONT_TYPE_NORMAL	= 0,								//	正常短信
	CONT_TYPE_GREEN		= 9									//	绿色短信
};

static const int JT_TYPE_GREEN	= 311;						//	绿色短信job_type

//	获取完整的内容库
static const char *SQL_GET_CONT_POOL
	= "SELECT cont_id, next_id, mmx_id, cat_id, pts_id, mo_user, iden FROM %s.cont_pool";

//	获取带触发的内容库
static const char *SQL_GET_TRIG_CONT_POOL
	= "SELECT cont_id, next_id, mmx_id, cat_id, pts_id, mo_user, iden FROM %s.cont_pool\
	  WHERE mmx_id>0 AND mmx_till>=CURDATE()";

//	获取JOB
static const char *SQL_GET_JOB
	= "SELECT a.job_id, a.jt_id, a.col_id, b.final_sp FROM %s.jobs a\
	  LEFT JOIN %s.job_type b ON a.jt_id=b.jt_id\
	  WHERE a.curr_role=0 AND a.iden_ok=0 AND b.jt_cat=2";

//	内容识别
static const char *SQL_GET_JOB_CONT
	= "SELECT id, cat_id, mmx_id, mmx_till, mo_user, cont FROM %s.job_cont\
	  WHERE job_id=%lu";

static const char *SQL_UPDATE_CONT_ID
	= "UPDATE %s.job_cont SET cont_id=%lu WHERE id=%lu";

static const char *SQL_UPDATE_JOB_FINISHED
	= "UPDATE %s.jobs SET iden_ok=1 WHERE job_id=%lu";

//	用户登记
static const char *SQL_INSERT_CONT_POOL
	= "INSERT INTO %s.cont_pool (rec_dd, cat_id, cont_type, mmx_id, mmx_till, pts_id, mo_user, cont, iden, reg_last) \
	  VALUES (%lu, %d, %d, %lu, \'%s\', %d, %lu, \'%s\', \'%s\', CURDATE())";

static const char *SQL_UPDATE_CONT_POOL
	= "UPDATE %s.cont_pool SET cat_id=%d, cont=\'%s\' WHERE cont_id=%lu";

//	赠送登记积分
static const char *SQL_INSERT_PTS_VOU
	= "INSERT INTO %s.pts_vou (mobile, pts_id, pts)\
	  SELECT %lu, pts_id, per_pts FROM pts WHERE pts_id=%d\
	  ON DUPLICATE KEY UPDATE pts_vou.pts=pts_vou.pts+VALUES(pts_vou.pts)";

static const char *SQL_CHECK_REG_PTS_LMT
	= "SELECT COUNT(*) FROM %s.pts_vou a LEFT JOIN %s.pts b ON a.pts_id=b.pts_id\
	  WHERE a.mobile=%lu AND a.pts_id=%d AND a.pts>=b.day_max";

//	下发审核通过通知
static const char *SQL_NOTIFY_AUDIT_RESULT
	= "CALL %s.sp_audit_notify(%lu, %lu, \'%s\')";

static const char *SQL_NOTIFY_AUDIT_RESULT_GREEN
	= "CALL %s.sp_audit_notify_green(%lu, %lu, \'%s\')";

#pragma pack (push, 1)
struct SContPool
{
	unsigned long	_cont_id;
	unsigned long	_next_id;
	unsigned short	_mmx_id;
	unsigned short	_cat_id;
	unsigned char	_pts_id;
	unsigned long	_mo_user;
	char			_iden[MAX_IDEN_LEN + 1];

	SContPool(void)
	{
		memset(this, 0, sizeof(SContPool));
	}

	~SContPool(void)
	{
	}
};
#pragma pack (pop, 1)

class CContPool;

class CAuditing
	: public Process::CWorkerThread
{
public:
	CAuditing(CContPool		*cont_pool,
			  CMYSQLDB		*vms_db,
			  const char	*vms_db_name);
	~CAuditing(void);

	bool startup(void);
	void cleanup(void);
	void auditing(void);

	unsigned long sync_cont(unsigned short	cat_id,
							unsigned short	mmx_id,
							unsigned long	mo_user,
							const char		*mmx_till,							
							const char		*cont,
							unsigned short	cont_type = 0);
	bool give_pts(unsigned long mobile,
				  unsigned char	pts_id);
	bool audit_notify(unsigned long	mobile,
					  unsigned long cont_id,
					  const char	*cont);
	bool audit_notify_green(unsigned long	mobile,
							unsigned long	cont_id,
							const char		*cont);
	bool check_pts_lmt(unsigned long mobile,
					   unsigned long pts_id);
protected:
	unsigned int ThreadEntry(void);
	
	bool update_cont_id(unsigned long	idx,
						unsigned long	cont_id);
private:
	CMYSQLDB	*_vms_db;
	char        _db_name[MAX_PATH];

	CContPool	*_cont_pool;
	friend class	CContPool;

	CSysPar		*_sys_par;
	CSysParEx	*_sys_par_ex;
};

class CContPool
{
public:
	CContPool(CMYSQLDB		*vms_db,
			  const char	*vms_db_name,
			  unsigned char	mode = CONT_POOL_GET_MODE_ALL);
	~CContPool(void);

	//	重载触发内容库
	bool load(unsigned char mode = CONT_POOL_GET_MODE_ALL);
	void unload(void);

	//  检查作品是否存在
	SContPool* find(const char		*cont, 
					unsigned long	len);
	
	unsigned long new_cont(unsigned short	cat_id,
						   unsigned short	mmx_id,
						   unsigned char	pts_id,
						   unsigned long	mo_user,
						   const char		*mmx_till,
						   const char		*iden,
						   const char		*cont,
						   unsigned short	cont_type = 0);
	bool update_cont(unsigned long	cont_id,
					 unsigned short	cat_id,
					 const char		*cont);
	void add_mem(SContPool &sm);
protected:	
	SContPool* find(const char		*cont);
	void sort(void);	
private:
	CMYSQLDB	*_vms_db;
	char		_vms_db_name[MAX_PATH];

	std::vector<SContPool>		_cont_pool;
	Process::CCriticalSection	_cs_cont;
};
