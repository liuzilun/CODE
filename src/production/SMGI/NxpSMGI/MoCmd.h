#pragma once

#include <vector>
#include "..\Lib\MYSQLDBPool.h"
#include "..\Lib\Sync.h"
#include "..\Lib\ClubCommon.h"

static const int MAX_CMD_ID_LEN = 10;

static const char *SQL_GET_MOCMD
	= "SELECT cmd_id, cmd_type, cmd_port, cmd_chr, cmd_ext, loose_ext, loose_cmd, ord_type, srv_type, serv_id, fee_id, cmd_sp FROM nxp_mbr.cmd\
	  WHERE cmd_ok=1 AND xms_type=0 ORDER BY LENGTH(cmd_ext) DESC";

static const char *SQL_GET_THE_CMD
	= "SELECT cmd_id, cmd_type, cmd_port, cmd_chr, cmd_ext, loose_ext, loose_cmd, ord_type, srv_type, serv_id, fee_id, cmd_sp FROM nxp_mbr.cmd\
	  WHERE cmd_ok=1 AND xms_type=0 AND cmd_port=%d AND ord_type=%d AND srv_type=\'%s\'";

#pragma pack (push, 1)
enum EMATCH_MODE
{
	MATCH_MODE_FULL	= 0,							//	��ȷ
	MATCH_MODE_LIKE	= 1								//	ģ��
};

enum ECMD_TYPE
{
	CMD_TYPE_STD	= 0,							//	��׼����(��̨ϵͳ��æ������ϵͳ����)
	CMD_TYPE_STD_EXT= 1,							//	��׼����(����ϵͳ��æ״̬)
	CMD_TYPE_REG_SM	= 2								//	�ǼǶ���
};

enum EORD_TYPE
{
	ORD_TYPE_NORMAL	= 0,							//	����
	ORD_TYPE_ORDER	= 1,							//	����
	ORD_TYPE_CANCEL	= 2,							//	�˶�
	ORD_TYPE_REV_SUB= 3								//	���򶩹�				
};

struct SMoCmd
{
	unsigned short	_cmd_id;						//	ָ��ID
	unsigned char	_cmd_type;						//	ҵ������
	unsigned char	_cmd_port;						//	�˿�ID
	char			_cmd_chr[RdiClubCommon::MAX_SP_LEN + 1];		//	ָ��
	unsigned char	_loose_cmd;						//	ָ��ƥ��ģʽ
	char			_cmd_ext[RdiClubCommon::MAX_SP_LEN + 1];		//	�˿���չ��
	unsigned char	_loose_ext;						//	�˿�ƥ��ģʽ

	unsigned char	_ord_type;						//	��������{0�Ƕ���,1���ƣ�2ȡ��}
	char			_srv_type[RdiClubCommon::MAX_SP_LEN + 1];	//	ҵ�����
	unsigned short	_serv_id;						//	��������ID
	unsigned short	_fee_id;						//	�ʷ�ID
	char			_cmd_sp[MAX_PATH];				//	ִ��SP
};
#pragma pack (pop, 1)

class CMoCmd
{
public:
	CMoCmd(CMYSQLDB	*db);
	~CMoCmd(void);

	bool startup(void);
	void cleanup(void);

	SMoCmd* match(unsigned char	port_id,
				  const char	*cmd_chr,
				  const char	*cmd_ext);

	SMoCmd* find(unsigned char	port_id,
				 unsigned char	cmd_type);

	bool find(unsigned char	port_id,
			  unsigned char	ord_type,
			  const char	*srv_type,
			  SMoCmd		&cmd);
private:
	CMYSQLDB	*_db;
		
	std::vector<SMoCmd>			_vec_cmd;
	Process::CCriticalSection	_cs;
};