#include "Segs.h"
#include "Define.h"

CSegs::CSegs(CMYSQLDB   *vms_db, 
             const char *vms_db_name)
	: _vms_db(vms_db)
{
    strncpy(_vms_db_name, vms_db_name, MAX_PATH);
	startup();
}

CSegs::~CSegs(void)
{
	cleanup();
}

bool CSegs::startup(void)
{
	bool ret = true;
	
	cleanup();
	
	Process::CAutoLock lock(&_cs);
    
    char sql[MAX_SQL_LEN] = {0};
    sprintf(sql, SQL_GET_SEGS, _vms_db_name);

	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		while (rs.LoadRow())
		{
			SSegs x;
			x._begin	= rs.AsU32(0);
			x._end		= rs.AsU32(1);
			x._type		= (unsigned char)rs.AsU32(2);
			x._city_id	= rs.AsU32(3);
			_code.push_back(x);
		}
		rs.FreeRsl();
	}
	else
	{
		ret = false;
	}
	return ret;
}

void CSegs::cleanup(void)
{
	_cs.Lock();
	_code.clear();
	_cs.Unlock();
}

//	’€∞Î≤È’“À„∑®
SSegs* CSegs::find(unsigned long mobile)
{
	Process::CAutoLock lock(&_cs);
	long start	= 0;
	long end	= (long)_code.size() - 1;
	long idx	= 0;
	while (start <= end)
	{
		idx = (start + end) / 2;
		if (mobile >= _code[idx]._begin)
		{
			if (mobile <= _code[idx]._end)
			{
				return &_code[idx];
			}
			else
			{
				start = idx + 1;
			}
		}
		else
		{
			end = idx - 1;			
		}
	}
	return NULL;
}