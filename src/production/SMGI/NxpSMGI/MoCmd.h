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
	MATCH_MODE_FULL	= 0,							//	精确
	MATCH_MODE_LIKE	= 1								//	模糊
};

enum ECMD_TYPE
{
	CMD_TYPE_STD	= 0,							//	标准类型(后台系统繁忙，返回系统返回)
	CMD_TYPE_STD_EXT= 1,							//	标准类型(不管系统繁忙状态)
	CMD_TYPE_REG_SM	= 2								//	登记短信
};

enum EORD_TYPE
{
	ORD_TYPE_NORMAL	= 0,							//	正常
	ORD_TYPE_ORDER	= 1,							//	定制
	ORD_TYPE_CANCEL	= 2,							//	退订
	ORD_TYPE_REV_SUB= 3								//	反向订购				
};

struct SMoCmd
{
	unsigned short	_cmd_id;						//	指令ID
	unsigned char	_cmd_type;						//	业务类型
	unsigned char	_cmd_port;						//	端口ID
	char			_cmd_chr[RdiClubCommon::MAX_SP_LEN + 1];		//	指令
	unsigned char	_loose_cmd;						//	指令匹配模式
	char			_cmd_ext[RdiClubCommon::MAX_SP_LEN + 1];		//	端口扩展号
	unsigned char	_loose_ext;						//	端口匹配模式

	unsigned char	_ord_type;						//	定制类型{0非定制,1定制，2取消}
	char			_srv_type[RdiClubCommon::MAX_SP_LEN + 1];	//	业务代码
	unsigned short	_serv_id;						//	所属服务ID
	unsigned short	_fee_id;						//	资费ID
	char			_cmd_sp[MAX_PATH];				//	执行SP
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