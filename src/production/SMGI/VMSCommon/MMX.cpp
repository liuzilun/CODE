#include "mmx.h"
#include "Define.h"

CMMX::CMMX(CMYSQLDB		*vms_db,
		   const char	*db_name)
	: _vms_db(vms_db)
{
	strncpy(_db_name, db_name, MAX_PATH);

	startup();
}

CMMX::~CMMX(void)
{
	cleanup();
}

bool CMMX::startup(void)
{
	bool ret = true;
	cleanup();

	Process::CAutoLock lock(&_cs_mmx);
    char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_GET_MMX, _db_name);
	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		while (rs.LoadRow())
		{
			SMMX x;
			unsigned short mmx_id	= (unsigned short)rs.AsU32(0);
			x._mmx_fee	= (unsigned short)rs.AsU32(1);
			x._ms_id	= (unsigned short)rs.AsU32(2);
			x._mmx_type	= (unsigned char)rs.AsU32(3);
			strncpy(x._mmx_tag, rs.AsStr(4), MAX_PATH);
			strncpy(x._mmx_file, rs.AsStr(5), MAX_PATH);
			
			_map_mmx[mmx_id] = x;
		}
		rs.FreeRsl();
	}
	else
	{
		ret = false;
	}

	return ret;
}

void CMMX::cleanup(void)
{
	_cs_mmx.Lock();
	_map_mmx.clear();
	_cs_mmx.Unlock();
}

SMMX* CMMX::find(unsigned short mmx_id)
{
	Process::CAutoLock lock(&_cs_mmx);
	MAP_MMX::iterator it = _map_mmx.find(mmx_id);
	if (it != _map_mmx.end())
	{
		return &it->second;
	}
	else
	{
		return NULL;
	}
}