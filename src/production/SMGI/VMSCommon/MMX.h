#include "..\Lib\MySQLDB.h"
#include "..\Lib\Sync.h"
#include <hash_map>

static const char *SQL_GET_MMX
	= "SELECT mmx_id, mmx_fee, ms_id, mmx_type, mmx_tag, mmx_file FROM %s.mmx WHERE valid=1";

#pragma pack (push, 1)
struct SMMX
{
	unsigned short	_mmx_fee;
	unsigned short	_ms_id;
	unsigned char	_mmx_type;
	char			_mmx_tag[MAX_PATH];
	char			_mmx_file[MAX_PATH];

	SMMX(void)
	{
		memset(this, 0, sizeof(SMMX));
	}

	~SMMX(void)
	{
	}
};
typedef std::hash_map<unsigned short, SMMX>	MAP_MMX;
#pragma pack (pop, 1)

class CMMX
{
public:
	CMMX(CMYSQLDB	*vms_db,
		 const char	*db_name);
	~CMMX(void);

	bool startup(void);
	void cleanup(void);

	SMMX* find(unsigned short mmx_id);
private:
	CMYSQLDB	*_vms_db;
	char		_db_name[MAX_PATH];

	Process::CCriticalSection	_cs_mmx;
	MAP_MMX						_map_mmx;
};
