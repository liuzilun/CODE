#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MYSQLDBPOOLS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MYSQLDBPOOLS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MYSQLDBPOOLS_EXPORTS
#define MYSQLDBPOOLS_API __declspec(dllexport)
#else
#define MYSQLDBPOOLS_API __declspec(dllimport)
#endif

#pragma warning (disable : 4251)

#include <hash_map>
#include "MySQLDB.h"

static const unsigned long MAX_DB_LINKS = 30;

struct MYSQLDBPOOLS_API SMYSQLDBLink
{
	int			_id;
	bool		_used;
	CMYSQLDB	*_db;	
};

class MYSQLDBPOOLS_API CMYSQLDBPool
{
public:
	CMYSQLDBPool(void);
	~CMYSQLDBPool(void);

	bool init(CDbInfo &info, int link_cnt = 1);
	bool clear(void);

	SMYSQLDBLink* enter(void);
	void leave(SMYSQLDBLink &link);
private:
	bool grow(int link_cnt);
	bool resize(int add_links);
private:
	typedef std::hash_map<unsigned long, SMYSQLDBLink>	MAP_DB;
	MAP_DB _map_db;
	Process::CCriticalSection	_cs_db;
	CDbInfo	_db_info;
};
