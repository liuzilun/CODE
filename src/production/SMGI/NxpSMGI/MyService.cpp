#include "MyService.h"
#include "..\Lib\VerInfo.h"
#include "..\Lib\Log.h"
#include "..\Lib\Timer.h"
#include "IniCfg.h"

CMyService::CMyService(void)
	: CNTService(g_ini.get()->_srv_name, g_ini.get()->_srv_desc)
	, _stopped(NULL)
	, _db_pool(NULL)
	, _smgi_disp(NULL)
{
	_stopped = CreateEvent(NULL, TRUE, FALSE, 0);
}

CMyService::~CMyService(void)
{
}

void CMyService::Run(DWORD argc, LPTSTR *argv)
{
	Process::CVerInfo ver_info(g_ini.get()->_proc_name);
	char product[MAX_PATH] = {0};
	char version[MAX_PATH] = {0};

	g_log.append(Process::info, "------------------------------------------");
	g_log.append(Process::info, "%s(%s) Started.", 
				ver_info.ProductName(product), 
				ver_info.FileVersion(version));
	
	//	report starting service to SCM
	ReportStatus(SERVICE_START_PENDING);
	
	//	do initial job
	_db_pool = new CMYSQLDBPool();
	CDbInfo link_info;
	strncpy(link_info.Host, g_ini.get()->_dbs_host, MaxNmLen);
	strncpy(link_info.User, g_ini.get()->_dbs_uid, MaxNmLen);
	
	RdiClubCommon::CFuncCommon x;
	char dbs_pwd[MAX_PATH] = {0};
	x.do_decode_pwd(g_ini.get()->_dbs_pwd, dbs_pwd);
	strncpy(link_info.Pswd, dbs_pwd, MaxNmLen);
	
	strncpy(link_info.Name, g_ini.get()->_dbs_name, MaxNmLen);
	strncpy(link_info.Charset, g_ini.get()->_dbs_charset, MaxNmLen);
	link_info.Port		= g_ini.get()->_dbs_port;
	link_info.Clients	= g_ini.get()->_dbs_cflag;
	if (_db_pool->init(link_info, g_ini.get()->_dbs_links))
	{
		g_log.append(Process::info, "Open database successful.");

		_smgi_disp = new CSMGIDisp(_db_pool);
		
		//	报告运行状态
		char module_name[MAX_PATH] = {0};
		strncpy(module_name, g_ini.get()->_srv_name, RdiClubCommon::MAX_MODULE_NAME_LEN);

		SMYSQLDBLink *db_link = _db_pool->enter();
		RdiClubCommon::CDBCommon x(db_link->_db, g_ini.get()->_dbs_name);
		x.do_rpt_run(RdiClubCommon::MODULE_RUN_STAT_WORKING, module_name, "短信网关接口模块", "程序启动完毕");
		_db_pool->leave(*db_link);

		//	report running service to SCM
		ReportStatus(SERVICE_RUNNING);

		WaitForSingleObject(_stopped, INFINITE);
		if (_stopped)
		{
			CloseHandle(_stopped);
		}
	}
	else
	{
		g_log.append(Process::fatal, "Open database failed.");
	}
			
	g_log.append(Process::info, "%s(%s) Stopped.", 
				ver_info.ProductName(product), 
				ver_info.FileVersion(version));
	g_log.append(Process::info, "------------------------------------------");
}

void CMyService::Stop(void)
{
	//	报告运行状态
	char module_name[MAX_PATH] = {0};
	strncpy(module_name, g_ini.get()->_srv_name, RdiClubCommon::MAX_MODULE_NAME_LEN);

	SMYSQLDBLink *db_link = _db_pool->enter();
	RdiClubCommon::CDBCommon x(db_link->_db, g_ini.get()->_dbs_name);
	x.do_rpt_run(RdiClubCommon::MODULE_RUN_STAT_CLOSED, module_name, "短信网关接口模块", "程序关闭完毕");
	_db_pool->leave(*db_link);

	//	do clear job
	if (_smgi_disp != NULL)
	{
		delete _smgi_disp;
		_smgi_disp = NULL;
	}

	/*if (_db_pool != NULL)
	{
		delete _db_pool;
		_db_pool = NULL;
	}*/

	if (_stopped)
	{
		SetEvent(_stopped);
	}	

	//	report stopping service to SCM
	ReportStatus(SERVICE_STOPPED);
}