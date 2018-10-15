#include "Fee.h"
#include "Define.h"

CFee::CFee(CMYSQLDB		*vms_db, 
		   const char	*db_name)
	: _vms_db(vms_db)
{
    strncpy(_db_name, db_name, MAX_PATH);
	startup();
}

CFee::~CFee(void)
{
	cleanup();
}

bool CFee::startup(void)
{
	bool ret = false;
	cleanup();

	Process::CAutoLock lock(&_cs);
    char sql[MAX_SQL_LEN] = {0};
    sprintf(sql, SQL_GET_FEE, _db_name);

	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		while (rs.LoadRow())
		{
			SFee x;
			unsigned short fee_id = (unsigned short)rs.AsU32(0);
			strncpy(x._port_num, rs.AsStr(1), 10);
			strncpy(x._srv_type, rs.AsStr(2), 10);
			strncpy(x._fee_type, rs.AsStr(3), 2);
			x._fee_val		= (unsigned short)rs.AsU32(4);
			x._ot_flag		= (unsigned char)rs.AsU32(5);
            x._prio			= (unsigned char)rs.AsU32(6);
			x._is_agent		= (unsigned char)rs.AsU32(7);
			x._is_reg		= (unsigned char)rs.AsU32(8);
			x._pay_num		= rs.AsU32(9);
			_map_fee[fee_id] = x;
		}
		rs.FreeRsl();
		ret = true;
	}
	return ret;
}

void CFee::cleanup(void)
{
	Process::CAutoLock lock(&_cs);
	_map_fee.clear();
}

SFee* CFee::find(unsigned short fee_id)
{
	Process::CAutoLock lock(&_cs);
	MAP_Fee::iterator it = _map_fee.find(fee_id);
	if (it != _map_fee.end())
	{
		return &it->second;
	}
	else
	{
		return NULL;
	}
}