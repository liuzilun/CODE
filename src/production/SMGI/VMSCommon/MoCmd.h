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
	MATCH_MODE_FULL	= 0,							//	精确
	MATCH_MODE_LIKE	= 1								//	模糊
};

enum ESRV_TYPE
{
	//	基本业务类(0~49)
	SRV_TYPE_HELP	= 0,							//	业务介绍，帮助
	SRV_TYPE_AGREE	= 1,							//	积分计划
	SRV_TYPE_FREE	= 2,							//	免费欣赏
	SRV_TYPE_EXIT	= 3,							//	无条件退出
	SRV_TYPE_STAY	= 4,							//	挽留式退出
	SRV_TYPE_0000	= 5,							//	0000查询定制
	SRV_TYPE_00000	= 6,							//	00000全部取消
	SRV_TYPE_REG	= 7,							//	登记自编短信
	SRV_TYPE_QUERY	= 8,							//	查积分、排名、基金
					  
	SRV_TYPE_FAZE	= 9,							//	周末免扰
	SRV_TYPE_MORE	= 10,							//	获取更多作品
	SRV_TYPE_TOP100	= 11,							//	T100捆绑3条
	SRV_TYPE_COL_MENU=12,							//	定制栏目菜单
	SRV_TYPE_COL	= 13,							//	定制栏目
	SRV_TYPE_COL_QX	= 14,							//	取消定制栏目
					  
	//	功能、活动类
	SRV_TYPE_CXACT	= 50,							//	抽奖活动查询
	SRV_TYPE_QXACT	= 51,							//	取消抽奖活动激励

	SRV_TYPE_GIVE	= 52,							//	转赠话费
	SRV_TYPE_VOTE	= 53,							//	调查活动

	SRV_TYPE_DZFWD	= 54,							//	定制短信呼转
	SRV_TYPE_QXFWD	= 55,							//	取消短信呼转
													//	....定制其他服务

	SRV_TYPE_DJLS	= 57,							//	登记绿色短信
	SRV_TYPE_LSPM	= 58,							//	短信排名查询
	SRV_TYPE_ZDXZ	= 59,							//	指定短信下载

	//	维护管理
	SRV_TYPE_WEB	= 98,							//	开启网站
	SRV_TYPE_ADMIN	= 99							//	开启远程维护
};

struct SMoCmd
{
	unsigned short	_cmd_id;						//	指令ID
	unsigned char	_cmd_type;						//	业务类型
	unsigned char	_port_id;						//	端口ID
	unsigned short	_serv_id;						//	服务ID
	char			_cont_cmd[MAX_CMD_ID_LEN + 1];	//	指令
	unsigned char	_loose_cmd;						//	指令匹配模式
	char			_port_ext[MAX_SP_LEN + 1];		//	端口扩展号
	unsigned char	_loose_ext;						//	端口匹配模式
	char			_sql_sp[MAX_PATH];				//	执行SP
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