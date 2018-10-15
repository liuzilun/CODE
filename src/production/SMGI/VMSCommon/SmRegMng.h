#pragma once

#include "..\Lib\MySQLDB.h"

static const int COL_ID_GREEN = 311;

static const char *SQL_CALL_GET_JOB_ID
	= "SELECT %s.sp_new_reg_job()";

static const char *SQL_CALL_GET_LS_JOB_ID
	= "SELECT %s.sp_new_ls_reg_job()";

static const char *SQL_INSERT_JOB_CONT
	= "INSERT INTO %s.job_cont (job_id, rec_dt, mo_user, col_id, cont) VALUES (%lu, NOW(), %lu, %lu, \'%s\')";

class CSmRegMng
{
public:
	CSmRegMng(CMYSQLDB		*vms_db,
			  const char	*db_name);
	~CSmRegMng(void);

	bool startup(void);
	void cleanup(void);

	//	用户登记
	bool enroll(unsigned long	mobile,
				const char		*cont);

	//	绿色短信用户登记
	bool enroll_ls(unsigned long	mobile,
				   const char		*cont);
private:
	unsigned long get_job_id(void);
	unsigned long get_ls_job_id(void);
private:
	CMYSQLDB	*_vms_db;
	char		_db_name[MAX_PATH];
};
