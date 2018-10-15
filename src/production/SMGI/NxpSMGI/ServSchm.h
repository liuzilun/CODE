#pragma once

#include <hash_map>
#include "..\Lib\Sync.h"
#include "..\Lib\MYSQLDB.h"

#pragma pack (push, 1)
struct SServSchm
{
	unsigned short	_schm_id;							//	�ʷѷ���ID
	unsigned short	_mth_fee;							//	����fee_id
	unsigned short	_psh_fee;							//	Ⱥ��fee_id
	unsigned short	_per_fee;							//	����fee_id
	unsigned short	_xtr_fee;							//	����fee_id
};

typedef std::hash_map<unsigned short, SServSchm>	MAP_ServSchm;
#pragma pack (pop, 1)

class CServSchm
{
public:
	CServSchm(CMYSQLDB	*db);
	~CServSchm(void);

	bool startup(void);
	void cleanup(void);

	SServSchm* find(unsigned short serv_id);

	unsigned short get_free_id(void)
	{
		Process::CAutoLock lock(&_cs_schm);
		return _free_fee;
	}
private:
	CMYSQLDB	*_db;

	Process::CCriticalSection	_cs_schm;
	MAP_ServSchm				_map_schm;
	unsigned short				_free_fee;		//	���fee_id
};
