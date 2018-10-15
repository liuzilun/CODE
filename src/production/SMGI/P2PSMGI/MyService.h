#pragma once

#include <vector>
#include "..\Lib\NTService.h"
#include "..\Lib\MYSQLDBPool.h"
#include "..\Lib\ConsoleWatch.h"
#include "SMGIDisp.h"

class CMyService
	: public Christ::CNTService
{
public:
	CMyService(void);
	~CMyService(void);

	void Run(DWORD argc, LPTSTR* argv);
	void Stop(void);
private:
	HANDLE			_stopped;
	CMYSQLDBPool	*_db_pool;
	CSMGIDisp		*_smgi_disp;
	CMYSQLDB		*_mo_ext_db;

	CConsoleWatch	*_watch;
	SMYSQLDBLink	*_watch_db_link;
};