// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CONSOLEWATCH_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CONSOLEWATCH_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CONSOLEWATCH_EXPORTS
#define CONSOLEWATCH_API __declspec(dllexport)
#else
#define CONSOLEWATCH_API __declspec(dllimport)
#endif
#include "..\lib\MYSQLDB.h"

static const char *SQL_INSERT_SYS_STATUS = "INSERT INTO %s.SYS_STATUS(module_name, stat, subject, detail) \
											VALUES(\'%s\', %d, \'%s\',\' %s\')ON DUPLICATE KEY UPDATE module_name = VALUES(module_name), \
											stat = VALUES(stat),subject = VALUES(subject), detail = VALUES(detail), rec_dt = NOW()";

enum ESTATUS
{
	CLOSE_STAT = 0,							//	关闭
	START_STAT = 1,							//	启动
	RUNNING_STAT = 2,							//运行状态
	WARNING_STAT = 3
};

class CONSOLEWATCH_API CConsoleWatch {
public:
	CConsoleWatch(CMYSQLDB *vms_db, const char	*vms_db_name);
	
	/***************************************
	功能:更新系统状态
	参数:pkey 程序的唯一标志
		 subject 程序描述
		 detail  运行状态描述,
		 stat	 运行状态
	***************************************/
	bool report(const char* pkey, const char* subject , const char* detail, ESTATUS stat);

private:
	CMYSQLDB	*_vms_db;
	char		_vms_db_name[MAX_PATH];
	
};

