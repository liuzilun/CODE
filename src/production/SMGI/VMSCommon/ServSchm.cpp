#include "servschm.h"
#include "Define.h"

CServSchm::CServSchm(CMYSQLDB	*vms_db, 
					 const char	*db_name)
	: _vms_db(vms_db)
{
    strncpy(_db_name, db_name, MAX_PATH);
	startup();
}

CServSchm::~CServSchm(void)
{
	cleanup();
}

bool CServSchm::startup(void)
{
	bool ret = false;
	cleanup();

	Process::CAutoLock lock(&_cs);
    char sql[MAX_SQL_LEN] = {0};
    sprintf(sql, SQL_GET_SERV_SCHM, _db_name);

	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		while (rs.LoadRow())
		{
			SServSchm x;
			unsigned short serv_id = (unsigned short)rs.AsU32(0);
			x._ord_okay		= (unsigned short)rs.AsU32(1);
			x._ord_fail		= (unsigned short)rs.AsU32(2);
			x._mtod_free	= (unsigned short)rs.AsU32(3);
			x._mtod_paid	= (unsigned short)rs.AsU32(4);
			x._rent_id		= (unsigned short)rs.AsU32(5);
			x._push_id		= (unsigned short)rs.AsU32(6);
			x._quit_id		= (unsigned short)rs.AsU32(7);
			_map_serv[serv_id] = x;
		}
		rs.FreeRsl();
		ret = true;
	}
	return ret;
}

void CServSchm::cleanup(void)
{
	Process::CAutoLock lock(&_cs);
	_map_serv.clear();
}

SServSchm* CServSchm::find(unsigned short serv_id)
{
	Process::CAutoLock lock(&_cs);
	MAP_ServSchm::iterator it = _map_serv.find(serv_id);
	if (it != _map_serv.end())
	{
		return &it->second;
	}
	else
	{
		return NULL;
	}
}
