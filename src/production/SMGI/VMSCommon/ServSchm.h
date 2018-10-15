#pragma once

#include <hash_map>
#include "..\Lib\MYSQLDB.h"
#include "..\Lib\sync.h"

static const char* SQL_GET_SERV_SCHM
	= "SELECT a.serv_id, b.ord_okay, b.ord_fail, b.mtod_free, b.mtod_paid, b.rent_id, b.push_id, b.quit_id FROM %s.serv a\
	  LEFT JOIN fee_schm b ON a.defa_schm=b.schm_id WHERE b.schm_id IS NOT NULL";

#pragma warning(disable : 4103)
#pragma pack (push, 1)

enum ESERV_ID
{	
	SERV_ID_8SM		=	1,								//	8短信
	SERV_ID_FORWARD	=	2,								//	短信呼转
	SERV_ID_WEATHER	=	3,								//	天气预报
	SERV_ID_COUTRY	=	4								//	农村信息
};

struct SServSchm
{
	unsigned short	_ord_okay;							//	定制成功fee_id
	unsigned short	_ord_fail;							//	定制失败fee_id
	unsigned short	_mtod_free;							//	免费点播fee_id
	unsigned short	_mtod_paid;							//	付费点播fee_id
	unsigned short	_rent_id;							//	月租fee_id
	unsigned short	_push_id;							//	群发fee_id
	unsigned short	_quit_id;							//	退定fee_id
	
	SServSchm(void)
	{
		memset(this, 0, sizeof(SServSchm));
	}

	~SServSchm(void)
	{
	}
};
typedef std::hash_map<unsigned short, SServSchm>	MAP_ServSchm;
#pragma pack (pop, 1)

class CServSchm
{
public:
	CServSchm(CMYSQLDB	*vms_db,
			  const char *db_name);
	~CServSchm(void);

	bool startup(void);
	void cleanup(void);

	SServSchm* find(unsigned short serv_id);
private:
	CMYSQLDB	*_vms_db;
    char        _db_name[MAX_PATH];
	
	MAP_ServSchm		_map_serv;
	Process::CCriticalSection	_cs;
};