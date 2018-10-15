#include "MyService.h"
#include "..\Lib\VerInfo.h"
#include "..\Lib\Log.h"
#include "..\Lib\Timer.h"
#include "IniCfg.h"
#include "..\Lib\Encrypt.h"

CMyService::CMyService(void)
	: CNTService(g_ini.get_cfg()->_srv_name, g_ini.get_cfg()->_srv_desc)
	, _stopped(NULL)
	, _mo_ext_db(NULL)
	, _db_pool(NULL)
	, _smgi_disp(NULL)
	, _watch(NULL)
	, _watch_db_link(NULL)
{
	_stopped = CreateEvent(NULL, TRUE, FALSE, 0);
}

CMyService::~CMyService(void)
{
}

void CMyService::Run(DWORD argc, LPTSTR *argv)
{
	Christ::CVerInfo ver_info("P2PSMGI.exe");
	char product[MAX_PATH] = {0};
	char version[MAX_PATH] = {0};

	g_log.append(rpt::info, "------------------------------------------");
	g_log.append(rpt::info, "%s(%s) Started.", 
				ver_info.ProductName(product), 
				ver_info.FileVersion(version));
	
	//	report starting service to SCM
	ReportStatus(SERVICE_START_PENDING);
	
	//	do initial job
	_db_pool = new CMYSQLDBPool();
	CDbInfo link_info;
	strncpy(link_info.Host, g_ini.get_cfg()->_db_host, MaxNmLen);
	strncpy(link_info.User, g_ini.get_cfg()->_db_uid, MaxNmLen);
	
	char pwd[MAX_PATH] = {0};
	CEncrypt x;
	x.Decrypt(g_ini.get_cfg()->_db_pwd, pwd);
	strncpy(link_info.Pswd, pwd, MaxNmLen);
	
	//strncpy(link_info.Pswd, g_ini.get_cfg()->_db_pwd, MaxNmLen);

	strncpy(link_info.Name, g_ini.get_cfg()->_db_name, MaxNmLen);
	strncpy(link_info.Charset, g_ini.get_cfg()->_db_charset, MaxNmLen);
	link_info.Port		= g_ini.get_cfg()->_db_port;
	link_info.Clients	= g_ini.get_cfg()->_db_cflag;

	if (_db_pool->init(link_info, g_ini.get_cfg()->_db_links))
	{
		g_log.append(rpt::info, "Open database successful.");

		_mo_ext_db = new CMYSQLDB(link_info.Host, link_info.User, link_info.Pswd, link_info.Name, link_info.Charset, link_info.Port, link_info.Clients);
		_smgi_disp = new CSMGIDisp(_db_pool, g_ini.get_cfg()->_db_name, _mo_ext_db);
		
		_watch_db_link = _db_pool->enter();
		_watch = new CConsoleWatch(_watch_db_link->_db, g_ini.get_cfg()->_db_name);

		_watch->report(g_ini.get_cfg()->_srv_name, "短信网关接口模块", "程序运行中", START_STAT);

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
		g_log.append(rpt::fatal, "Open database failed.");
	}
			
	g_log.append(rpt::info, "%s(%s) Stopped.", 
				ver_info.ProductName(product), 
				ver_info.FileVersion(version));
	g_log.append(rpt::info, "------------------------------------------");
}

void CMyService::Stop(void)
{
	_watch->report(g_ini.get_cfg()->_srv_name, "短信网关接口模块", "程序已关闭", CLOSE_STAT);

	if (_watch != NULL)
	{
		delete _watch;
		_watch = NULL;
	}

	_db_pool->leave(*_watch_db_link);

	//	do clear job
	if (_smgi_disp != NULL)
	{
		delete _smgi_disp;
		_smgi_disp = NULL;
	}

	if (_db_pool != NULL)
	{
		delete _db_pool;
		_db_pool = NULL;
	}

	if (_mo_ext_db != NULL)
	{
		delete _mo_ext_db;
		_mo_ext_db = NULL;
	}

	if (_stopped)
	{
		SetEvent(_stopped);
	}	

	//	report stopping service to SCM
	ReportStatus(SERVICE_STOPPED);
}