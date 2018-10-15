/*
** Copyright (c) 2001��2005
** Shen Zhen Joint Harvest Solutions Co., Ltd.
** All rights reserved.
**
** Segs.h
** class CSegs:�Ŷι�������ʶ���ͷ������շ��ֻ��������͡�������Ϣ
**		ע��ϵͳȱʡ���ṩ����ͨʹ�ã�������ƶ�ʹ����Ҫ���Ӻ�:_MOBILE_
**
** Version:	2.0
** Author:	Chris
** Time:	2005/05/28
*/

#pragma once

#include <vector>
#include "..\Lib\MYSQLDB.h"
#include "..\Lib\Sync.h"

static const char* SQL_GET_SEGS 
	= "SELECT seg_beg, seg_end, seg_ut, city_id FROM %s.city_segs \
      ORDER BY seg_beg";

#pragma warning(disable : 4103)
#pragma pack (push, 1)
enum EMOBILE_TYPE
{
	USER_TYPE_AGREE	    = 1,		//	��Լ�û�
	USER_TYPE_PREPAY	= 2 		//	Ԥ�����û�
};

struct SSegs
{
	unsigned long	_begin;			//	��ʼ����
	unsigned long	_end;			//	��������
	unsigned char	_type;			//	�û�����
	unsigned long	_city_id;		//	��������
};
#pragma pack (pop, 1)

class CSegs
{
public:
	CSegs(CMYSQLDB		*vms_db, 
		  const char	*vms_db_name);
	~CSegs(void);

	bool startup(void);
	void cleanup(void);

	SSegs* find(unsigned long mobile);
private:
	CMYSQLDB			*_vms_db;
    char                _vms_db_name[MAX_PATH];

	std::vector<SSegs>	_code;
	Process::CCriticalSection	_cs;
};