#pragma once

#include "..\Lib\MySQLDB.h"
#include "..\Lib\Sync.h"
#include "..\Lib\ClubCommon.h"
#include <vector>
#include <hash_map>

//	��ȡ�������ݿ�
static const char *SQL_GET_SMS_BASE
	= "SELECT mmx_id, reg_cid, attr_map, rela_id, segs, wmark FROM nxp_mmx.mmx_base \
	  WHERE mmx_type=0 AND mmx_sort=0";

static const char *SQL_GET_SYS_PAREX_KEY
	= "SELECT pvar FROM nxp_mbr.sys_parex WHERE pkey like 'KEY_%%'";
static const char *SQL_GET_SYS_PAREX_EXTRA
	= "SELECT pvar FROM nxp_mbr.sys_parex WHERE pkey like 'EXTRA_%%'";

//	�Ǽ���Ʒ
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
		���ض�����Ʒ�⵽�ڴ�
	*/
	bool do_load(void);

	/*
		������Ʒ����ڴ��������ȥ
	*/
	void do_unload(void);

	/*
		�����Ʒ�Ƿ����
	*/ 
	SSmsPool* do_find(const char	*cont, 
					  unsigned long	len);

	/*
		�Ǽ���Ʒ
	*/
	bool do_register(__int64	mobile,
					 const char	*cont);

	/*
		�ڴ�ͬ��
	*/
	void do_add_mem(SSmsPool &sm);
protected:
	/*
		������Ʒ
	*/
	bool do_load_sms(void);

	/*
		���عؼ���
	*/
	bool do_load_key(void);

	/*
		�����ų�����
	*/
	bool do_load_extra(void);

	/*
		��idenʶ�����������
	*/
	void do_sort(void);

	/*
		����ʶ��
	*/
	SSmsPool* do_find(const char *cont);

private:
	CMYSQLDB	*_sms_db;

	std::vector<SSmsPool>		_sms_pool;
	Process::CCriticalSection	_cs_sms;
};
