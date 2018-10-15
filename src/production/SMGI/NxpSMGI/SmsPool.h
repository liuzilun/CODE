#pragma once

#include "..\Lib\MySQLDB.h"
#include "..\Lib\Sync.h"
#include "..\Lib\ClubCommon.h"
#include <vector>
#include <hash_map>

//	获取短信内容库
static const char *SQL_GET_SMS_BASE
	= "SELECT mmx_id, reg_cid, attr_map, rela_id, segs, wmark FROM nxp_mmx.mmx_base \
	  WHERE mmx_type=0 AND mmx_sort=0";

static const char *SQL_GET_SYS_PAREX_KEY
	= "SELECT pvar FROM nxp_mbr.sys_parex WHERE pkey like 'KEY_%%'";
static const char *SQL_GET_SYS_PAREX_EXTRA
	= "SELECT pvar FROM nxp_mbr.sys_parex WHERE pkey like 'EXTRA_%%'";

//	登记作品
static const char *SQL_INSERT_MMX_REG
	= "INSERT INTO nxp_mmx.mmx_reg (reg_cid, reg_dd, mmx_type, site_id, cont) \
	  SELECT %I64u, CURDATE(), 0, site_id, \'%s\' FROM nxp_mbr.sys_par";

#pragma pack (push, 1)
struct SSmsPool
{
	unsigned long	_sms_id;
	__int64			_reg_cid;
	unsigned short	_attr_map;
	unsigned char	_rela_id;
	unsigned char	_segs;
	char			_sms_wm[RdiClubCommon::MAX_IDEN_LEN + 1];

	SSmsPool(void)
	{
		memset(this, 0, sizeof(SSmsPool));
	}

	~SSmsPool(void)
	{
	}
};
#pragma pack (pop, 1)

class CSmsPool
{
public:
	CSmsPool(CMYSQLDB *sms_db);
	~CSmsPool(void);

	/*
		加载短信作品库到内存
	*/
	bool do_load(void);

	/*
		短信作品库从内存中清理出去
	*/
	void do_unload(void);

	/*
		检查作品是否存在
	*/ 
	SSmsPool* do_find(const char	*cont, 
					  unsigned long	len);

	/*
		登记作品
	*/
	bool do_register(__int64	mobile,
					 const char	*cont);

	/*
		内存同步
	*/
	void do_add_mem(SSmsPool &sm);
protected:
	/*
		加载作品
	*/
	bool do_load_sms(void);

	/*
		加载关键词
	*/
	bool do_load_key(void);

	/*
		加载排除单词
	*/
	bool do_load_extra(void);

	/*
		按iden识别码进行排序
	*/
	void do_sort(void);

	/*
		内容识别
	*/
	SSmsPool* do_find(const char *cont);

private:
	CMYSQLDB	*_sms_db;

	std::vector<SSmsPool>		_sms_pool;
	Process::CCriticalSection	_cs_sms;
};
