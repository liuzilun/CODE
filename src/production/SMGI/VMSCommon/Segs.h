/*
** Copyright (c) 2001－2005
** Shen Zhen Joint Harvest Solutions Co., Ltd.
** All rights reserved.
**
** Segs.h
** class CSegs:号段管理，用来识别发送方、接收方手机号码类型、地市信息
**		注意系统缺省是提供给联通使用，如果给移动使用需要增加宏:_MOBILE_
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
	USER_TYPE_AGREE	    = 1,		//	合约用户
	USER_TYPE_PREPAY	= 2 		//	预付费用户
};

struct SSegs
{
	unsigned long	_begin;			//	起始号码
	unsigned long	_end;			//	截至号码
	unsigned char	_type;			//	用户类型
	unsigned long	_city_id;		//	地市区号
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