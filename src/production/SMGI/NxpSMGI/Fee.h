#pragma once

#include <hash_map>
#include "..\Lib\MYSQLDB.h"
#include "..\Lib\sync.h"

static const char* SQL_GET_FEE
	= "SELECT fee_id, srv_type, fee_type, fee_val, ot_flag, prio, is_agent, is_reg, pay_num FROM nxp_mbr.fee ORDER BY fee_id";

#pragma warning(disable : 4103)
#pragma pack (push, 1)
struct SFee
{
	char			_srv_type[10];			//	ҵ�����
	char			_fee_type[2];			//	�Ʒ�����
	unsigned short	_fee_val;				//	�Ʒ�ֵ����λΪ��
	unsigned char	_ot_flag;				//	���б�־
    unsigned char	_prio;					//	���ȼ�
	unsigned char   _is_agent;				//  ���շѱ�־
	unsigned char	_is_reg;				//	�Ƿ���Ҫ״̬����
    unsigned long	_pay_num;				//  ���Ѻ���
};
typedef std::hash_map<unsigned short, SFee>	MAP_Fee;
#pragma pack (pop, 1)

class CFee
{
public:
	CFee(CMYSQLDB	*db);
	~CFee(void);

	bool startup(void);
	void cleanup(void);

	SFee* find(unsigned short fee_id);
private:
	CMYSQLDB	*_db;

	MAP_Fee		_map_fee;
	Process::CCriticalSection	_cs;
};