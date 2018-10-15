#pragma once

#include "..\Lib\MYSQLDB.h"
#include "Define.h"
#include <stdio.h>

static const char *SQL_CALL_EVENT
	= "CALL %s.sp_log_event(%d, \'%s\', \'%s\')";

enum EEVENTS_LEVEL
{
	EVENTS_LEVEL_EVENT = 0,							//	事件
	EVENTS_LEVEL_ALARM = 1							//	告警
};

class CEvents
{
public:
	CEvents(CMYSQLDB	*vms_db,
			const char	*vms_db_name);
	~CEvents(void);

	//	记录事件日志
	bool log(unsigned char	evn_alert,
			 const char		*subject,
			 const char		*details);
private:
	CMYSQLDB	*_vms_db;
	char		_vms_db_name[MAX_PATH];
};

inline CEvents::CEvents(CMYSQLDB	*vms_db,
						const char	*vms_db_name)
	: _vms_db(vms_db)
{
	strncpy(_vms_db_name, vms_db_name, MAX_PATH);
}

inline CEvents::~CEvents(void)
{
}

inline bool CEvents::log(unsigned char	evn_alert,
						 const char		*subject,
						 const char		*details)
{
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_CALL_EVENT, _vms_db_name, evn_alert, subject, details);
	return _vms_db->Exec(sql) == TRUE;
}