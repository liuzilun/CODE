#pragma once

#include <vector>
#include "..\Lib\MYSQLDBPool.h"
#include "..\Lib\Sync.h"
#include "Define.h"

static const int MAX_CMD_ID_LEN = 10;

static const char *SQL_GET_MOCMD
	= "SELECT cmd_id, cmd_type, port_id, serv_id, port_ext, cont_cmd, loose_ext, loose_cmd, sql_sp FROM %s.cmd\
	  WHERE enabled=1 ORDER BY LENGTH(cont_cmd) DESC";

static const char *SQL_GET_THE_CMD
	= "SELECT cmd_id, cmd_type, port_id, serv_id, port_ext, cont_cmd, loose_ext, loose_cmd, sql_sp FROM %s.cmd\
	  WHERE port_id=%d AND cmd_type=%d";

#pragma pack (push, 1)
enum EMATCH_MODE
{
	MATCH_MODE_FULL	= 0,							//	��ȷ
	MATCH_MODE_LIKE	= 1								//	ģ��
};

enum ESRV_TYPE
{
	//	����ҵ����(0~49)
	SRV_TYPE_HELP	= 0,							//	ҵ����ܣ�����
	SRV_TYPE_AGREE	= 1,							//	���ּƻ�
	SRV_TYPE_FREE	= 2,							//	�������
	SRV_TYPE_EXIT	= 3,							//	�������˳�
	SRV_TYPE_STAY	= 4,							//	����ʽ�˳�
	SRV_TYPE_0000	= 5,							//	0000��ѯ����
	SRV_TYPE_00000	= 6,							//	00000ȫ��ȡ��
	SRV_TYPE_REG	= 7,							//	�Ǽ��Ա����
	SRV_TYPE_QUERY	= 8,							//	����֡�����������
					  
	SRV_TYPE_FAZE	= 9,							//	��ĩ����
	SRV_TYPE_MORE	= 10,							//	��ȡ������Ʒ
	SRV_TYPE_TOP100	= 11,							//	T100����3��
	SRV_TYPE_COL_MENU=12,							//	������Ŀ�˵�
	SRV_TYPE_COL	= 13,							//	������Ŀ
	SRV_TYPE_COL_QX	= 14,							//	ȡ��������Ŀ
					  
	//	���ܡ����
	SRV_TYPE_CXACT	= 50,							//	�齱���ѯ
	SRV_TYPE_QXACT	= 51,							//	ȡ���齱�����

	SRV_TYPE_GIVE	= 52,							//	ת������
	SRV_TYPE_VOTE	= 53,							//	����

	SRV_TYPE_DZFWD	= 54,							//	���ƶ��ź�ת
	SRV_TYPE_QXFWD	= 55,							//	ȡ�����ź�ת
													//	....������������

	SRV_TYPE_DJLS	= 57,							//	�Ǽ���ɫ����
	SRV_TYPE_LSPM	= 58,							//	����������ѯ
	SRV_TYPE_ZDXZ	= 59,							//	ָ����������

	//	ά������
	SRV_TYPE_WEB	= 98,							//	������վ
	SRV_TYPE_ADMIN	= 99							//	����Զ��ά��
};

struct SMoCmd
{
	unsigned short	_cmd_id;						//	ָ��ID
	unsigned char	_cmd_type;						//	ҵ������
	unsigned char	_port_id;						//	�˿�ID
	unsigned short	_serv_id;						//	����ID
	char			_cont_cmd[MAX_CMD_ID_LEN + 1];	//	ָ��
	unsigned char	_loose_cmd;						//	ָ��ƥ��ģʽ
	char			_port_ext[MAX_SP_LEN + 1];		//	�˿���չ��
	unsigned char	_loose_ext;						//	�˿�ƥ��ģʽ
	char			_sql_sp[MAX_PATH];				//	ִ��SP
};
#pragma pack (pop, 1)

class CMoCmd
{
public:
	CMoCmd(CMYSQLDBPool	*db_pool,
		   const char	*db_name);
	~CMoCmd(void);

	bool startup(void);
	void cleanup(void);

	SMoCmd* match(unsigned char	port_id,
				  const char	*cont_cmd,
				  const char	*port_ext);

	SMoCmd* find(unsigned char	port_id,
				 unsigned char	cmd_type);

	bool find(unsigned char port_id,
			  unsigned char	cmd_type,
			  SMoCmd		&cmd);
private:
	CMYSQLDBPool	*_db_pool;
	char			_db_name[MAX_PATH];
	
	std::vector<SMoCmd>			_vec_cmd;
	Process::CCriticalSection	_cs;
};