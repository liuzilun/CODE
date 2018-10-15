#include "P2PLog.h"
#include "Define.h"
#include <string>

CP2PLog::CP2PLog(CMYSQLDB		*vms_db,
				 const char		*vms_db_name,
				 const char		*data_dir)
	: _vms_db(vms_db)
	, _p2p_file(NULL)
	, _ppc_file(NULL)
{
    strncpy(_vms_db_name, vms_db_name, MAX_PATH);
	strncpy(_data_dir, data_dir, MAX_PATH);

	memset(_p2p_file_name, 0, MAX_PATH);
	sprintf(_p2p_file_name, "%s\\%s", _data_dir, P2PLOG_FILE_NAME);

	vms_db->EscStr(_p2p_load_path, _p2p_file_name, 
		(unsigned long)strlen(_p2p_file_name));

	memset(_ppc_file_name, 0, MAX_PATH);
	sprintf(_ppc_file_name, "%s\\%s", _data_dir, PPCLOG_FILE_NAME);

	vms_db->EscStr(_ppc_load_path, _ppc_file_name,
		(unsigned long)strlen(_ppc_file_name));
}

CP2PLog::~CP2PLog(void)
{
	if (_p2p_file != NULL)
	{
		delete _p2p_file;
		_p2p_file = NULL;
	}

	if (_ppc_file != NULL)
	{
		delete _ppc_file;
		_ppc_file = NULL;
	}
}

bool CP2PLog::startup(void)
{
	return create_p2p() && create_ppc();
}

bool CP2PLog::cleanup(void)
{
	if (_p2p_file != NULL)
	{
		delete _p2p_file;
		_p2p_file = NULL;
	}

	if (_ppc_file != NULL)
	{
		delete _ppc_file;
		_ppc_file = NULL;
	}

	//	采用CSV文件格式进行加载到数据库
	char p2p_sql[MAX_SQL_LEN] = {0};
	sprintf(p2p_sql, SQL_LOAD_P2PLOG, _p2p_load_path, _vms_db_name);

	char ppc_sql[MAX_SQL_LEN] = {0};
	sprintf(ppc_sql, SQL_LOAD_PPCLOG, _ppc_load_path, _vms_db_name);

	return _vms_db->Exec(p2p_sql) && _vms_db->Exec(ppc_sql);
}

bool CP2PLog::create_p2p(void)
{
	bool ret = true;
	try
	{
		_p2p_file = new File::CCacheFile(_p2p_file_name,
											MAX_CACHE_SIZE,
											CREATE_ALWAYS,
											GENERIC_WRITE);
	}
	catch(...)
	{
		ret = false;
	}
	return ret;
}


bool CP2PLog::create_ppc(void)
{
	bool ret = true;
	try
	{
		_ppc_file = new File::CCacheFile(_ppc_file_name,
										 MAX_CACHE_SIZE,
										 CREATE_ALWAYS,
										 GENERIC_WRITE);
	}
	catch(...)
	{
		ret = false;
	}
	return ret;
}

void CP2PLog::p2ptofile(SP2PLog &sm)
{
	if (_p2p_file == NULL)
	{
		create_p2p();
	}

	if (_p2p_file != NULL)
	{
		char cont[MAX_SQL_LEN] = {0};			
		sprintf(cont, "%I64u,%lu,%lu,%lu,%d,%d,%d,%d\n", 
			sm._dt, sm._mo, sm._mt, sm._cont_id, sm._cont_len, sm._mt_out,
			sm._mo_city, sm._pts_id);
		_p2p_file->push(cont, (unsigned long)strlen(cont));
	}
}

void CP2PLog::ppctofile(SP2PLog &sm)
{
	if (_ppc_file == NULL)
	{
		create_ppc();
	}

	if (_ppc_file != NULL)
	{
		char cont[MAX_SQL_LEN] = {0};			
		sprintf(cont, "%I64u,%lu,%lu,%lu,%d,%d,%d\n", 
			sm._dt, sm._mo, sm._mt, sm._cont_id, sm._mt_out, sm._mo_city, sm._pts_id);
		_ppc_file->push(cont, (unsigned long)strlen(cont));
	}
}