#include "Fee.h"

CFee::CFee(CMYSQLDB	*db)
	: _db(db)
{
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
	CMYSQLRS rs;
	if (_db->Exec(SQL_GET_FEE, &rs))
	{
		while (rs.LoadRow())
		{
			SFee x;
			unsigned short fee_id = (unsigned short)rs.AsU32(0);
			strncpy(x._srv_type, rs.AsStr(1), 10);
			strncpy(x._fee_type, rs.AsStr(2), 2);
			x._fee_val	= (unsigned short)rs.AsU32(3);
			x._ot_flag	= (unsigned char)rs.AsU32(4);
            x._prio		= (unsigned char)rs.AsU32(5);
			x._is_agent	= (unsigned char)rs.AsU32(6);
			x._is_reg	= (unsigned char)rs.AsU32(7);
			x._pay_num	= rs.AsU32(8);
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