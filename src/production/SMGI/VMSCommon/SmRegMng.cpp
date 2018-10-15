#include "SMRegMng.h"
#include "Define.h"
#include <stdio.h>

CSmRegMng::CSmRegMng(CMYSQLDB	*vms_db,
					 const char	*db_name)
	: _vms_db(vms_db)
{
	strncpy(_db_name, db_name, MAX_PATH);

	startup();
}

CSmRegMng::~CSmRegMng(void)
{
	cleanup();
}

unsigned long CSmRegMng::get_job_id(void)
{
	unsigned long job_id = 0;

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_CALL_GET_JOB_ID, _db_name);
	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			job_id = rs.AsU32(0);
		}
		rs.FreeRsl();
	}
	return job_id;
}

unsigned long CSmRegMng::get_ls_job_id(void)
{
	unsigned long job_id = 0;

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_CALL_GET_LS_JOB_ID, _db_name);
	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			job_id = rs.AsU32(0);
		}
		rs.FreeRsl();
	}
	return job_id;
}

bool CSmRegMng::startup(void)
{
	return true;
}

void CSmRegMng::cleanup(void)
{
}

bool CSmRegMng::enroll(unsigned long	mobile,
					   const char		*cont)
{
	char esc_cont[MAX_SQL_LEN] = {0};
	_vms_db->EscStr(esc_cont, cont, (unsigned long)strlen(cont));

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_INSERT_JOB_CONT, _db_name, get_job_id(), mobile, 0, cont);
	return _vms_db->Exec(sql) == TRUE;
}

bool CSmRegMng::enroll_ls(unsigned long	mobile,
						  const char	*cont)
{
	char esc_cont[MAX_SQL_LEN] = {0};
	_vms_db->EscStr(esc_cont, cont, (unsigned long)strlen(cont));

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_INSERT_JOB_CONT, _db_name, get_ls_job_id(), mobile, COL_ID_GREEN, cont);
	return _vms_db->Exec(sql) == TRUE;
}