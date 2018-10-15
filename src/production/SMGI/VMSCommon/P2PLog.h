#pragma once

#include <stdio.h>
#include "..\Lib\MYSQLDB.h"
#include "..\Lib\Sync.h"
#include "..\Lib\Myfile.h"
#include "..\Lib\Timer.h"

static const char *P2PLOG_FILE_NAME 
	= "P2PLog.dat";

static const char *PPCLOG_FILE_NAME 
	= "PPCLog.dat";

static const char *SQL_LOAD_P2PLOG
	= "LOAD DATA LOCAL INFILE \'%s\' INTO TABLE %s.p2p_log \
	  FIELDS TERMINATED BY \',\' ENCLOSED BY \'\"\' \
	  LINES TERMINATED BY \'\\n\' \
	  (dt, mo, mt, cont_id, cont_len, mt_out, mo_city, pts_id)";

static const char *SQL_LOAD_PPCLOG
	= "LOAD DATA LOCAL INFILE \'%s\' INTO TABLE %s.ppc_log \
	  FIELDS TERMINATED BY \',\' ENCLOSED BY \'\"\' \
	  LINES TERMINATED BY \'\\n\' \
	  (dt, mo, mt, cont_id, mt_out, mo_city, pts_id)";

#pragma warning (disable : 4103)
#pragma pack (push, 1)
struct SP2PLog
{
	unsigned __int64 _dt;					//	发起日期、时间
	unsigned long	_mo;					//	发起方
	unsigned long	_mt;					//	接收方
	unsigned long	_cont_id;				//	内容ID
	
	unsigned char	_cont_len;				//	内容长度
	unsigned char	_mt_out;				//	mt是否外网用户{0否，1是}

	unsigned short	_mo_city;				//	mo区号
	unsigned char	_pts_id;				//	积分ID
	
	SP2PLog(void)
	{
		memset(this, 0, sizeof(SP2PLog));
	}

	~SP2PLog(void)
	{
	}
};
#pragma pack (pop, 1)

class CP2PLog
{
public:
	CP2PLog(CMYSQLDB	*vms_db, 
            const char	*vms_db_name,
			const char	*data_dir);
	~CP2PLog(void);

	bool startup(void);
	bool cleanup(void);

	//	保存到文件
	void p2ptofile(SP2PLog &sm);
	void ppctofile(SP2PLog &sm);
private:
	//	创建临时文件
	bool create_p2p(void);
	bool create_ppc(void);

	//	加载文件到数据库中
	bool load_from_file(void);
private:
	CMYSQLDB    *_vms_db;
	char        _vms_db_name[MAX_PATH];

	File::CCacheFile	*_p2p_file;
	File::CCacheFile	*_ppc_file;
	char		_data_dir[MAX_PATH];
	char		_p2p_file_name[MAX_PATH];
	char		_p2p_load_path[MAX_PATH];
	char		_ppc_file_name[MAX_PATH];
	char		_ppc_load_path[MAX_PATH];
};