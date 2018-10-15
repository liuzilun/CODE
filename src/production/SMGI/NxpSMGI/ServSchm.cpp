#include "servschm.h"

CServSchm::CServSchm(CMYSQLDB	*db)
	: _db(db)
	, _free_fee(0)
{
	startup();
}

CServSchm::~CServSchm(void)
{
	cleanup();
}

static const char *SQL_GET_SERV_SCHM
	= "SELECT a.serv_id, a.schm_id, b.mth_fee, b.psh_fee, b.per_fee, b.xtr_fee FROM nxp_mbr.serv a\
	  LEFT JOIN nxp_mbr.schm b ON a.schm_id=b.schm_id WHERE b.schm_id IS NOT NULL";

static const char *SQL_GET_FREE_FEE_ID
	= "SELECT free_id FROM nxp_mbr.sys_par LIMIT 1";

bool CServSchm::startup(void)
{
	cleanup();

	Process::CAutoLock lock(&_cs_schm);

	CMYSQLRS rs;
	if (_db->Exec(SQL_GET_FREE_FEE_ID, &rs))
	{
		if (rs.LoadRow())
		{
			_free_fee = (unsigned short)rs.AsU32(0);
		}
		rs.FreeRsl();
	}
	
	if (_db->Exec(SQL_GET_SERV_SCHM, &rs))
	{
		while (rs.LoadRow())
		{
			SServSchm x;
			x._schm_id	= (unsigned short)rs.AsU32(1);
			x._mth_fee	= (unsigned short)rs.AsU32(2);
			x._psh_fee	= (unsigned short)rs.AsU32(3);
			x._per_fee	= (unsigned short)rs.AsU32(4);
			x._xtr_fee	= (unsigned short)rs.AsU32(5);

			_map_schm[(unsigned short)rs.AsU32(0)] = x;
		}
		rs.FreeRsl();

		return true;
	}
	else
	{
		return false;
	}
}

void CServSchm::cleanup(void)
{
	Process::CAutoLock lock(&_cs_schm);
	_map_schm.clear();
}

SServSchm* CServSchm::find(unsigned short serv_id)
{
	Process::CAutoLock lock(&_cs_schm);
	MAP_ServSchm::iterator it = _map_schm.find(serv_id);
	if (it != _map_schm.end())
	{
		return &it->second;
	}
	else
	{
		return NULL;
	}
}