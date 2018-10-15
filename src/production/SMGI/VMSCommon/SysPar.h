#pragma once

#include "..\Lib\MYSQLDB.h"
#include "..\Lib\Sync.h"

static const char *SQL_GET_SYS_PAR
	= "SELECT t_schm_id,col_m,col_web,col_hot,jt_reg,jt_hot,jt_upg,\
		pts_reg,pts_p2p,pts_self,pts_8sm,pts_on8,pts_top,pts_topc,pts_awd,\
		alert1,alert2,proc_sync,trg_clen,prio_cnt,misc_smg,keep_bon,\
		keep_pts,bon_qty,per_dona,tax_off,adj_rate,adv_rate,\
		bon_min,bon_max,bon_val,res_rate,res_max,res_val,hide_res,bon_show,awd_cnt\
		FROM %s.sys_par LIMIT 1";

//	实时检查状态
static const char *SQL_CHECK_PROC_SYNC
	= "SELECT proc_sync FROM %s.sys_par LIMIT 1";

//	设置状态
static const char *SQL_SET_PROC_SYNC
	= "UPDATE %s.sys_par SET proc_sync=%d";

//	进程状态
enum EPROC_STATUS
{
	PROC_STATUS_NORMAL		= 0,				//	正常
	PROC_STATUS_CDR			= 1,				//	CDR解析完毕
	PROC_STATUS_CONT_ANAL	= 2,				//	内容分析完毕
	PROC_STATUS_SCORE_PROC	= 3					//	积分计算完毕
};

#pragma pack (push, 1)
struct SSysPar
{
	unsigned short	_t_schm_id;								//	T类schm_id
	
	unsigned short	_col_m;									//	M点播栏目
	unsigned short	_col_web;								//	首页经典栏目
	unsigned short	_col_hot;								//	最热栏目

	unsigned short	_jt_reg;								//	登记任务类别
	unsigned short	_jt_hot;								//	最热任务类别
	unsigned short	_jt_upg;								//	渐进引导类别

	unsigned char	_pts_reg;								//	登记pts_id
	unsigned char	_pts_p2p;								//	普通pts_id
	unsigned char	_pts_self;								//	自发pts_id
	unsigned char	_pts_8sm;								//	接续pts_id
	unsigned char	_pts_on8;								//	逢8 pts_id
	unsigned char	_pts_top;								//	转发量前十送分pts_id(全省)
	unsigned char	_pts_topc;								//	转发量前十送分pts_id(地市)
	unsigned char	_pts_awd;								//	获奖核减

	unsigned long	_alert1;								//	告警号码1
	unsigned long	_alert2;								//	告警号码2
	
	unsigned char	_proc_sync;								//	进程同步
															//	{0正常，1话单解析完毕，2内容分析完，3积分处理完毕}
	unsigned char	_trg_clen;								//	触发内容长度

	unsigned char	_prio_cnt;								//	优先计数，进1，每天复位
	unsigned char	_misc_smg;								//	misc网关
	unsigned char	_keep_bon;								//	保留奖池（不派奖），0否1是
	unsigned char	_keep_pts;								//	保留积分（不清零），0否1是

	unsigned long	_bon_qty;								//	转发条数
	float			_per_dona;								//	每条贡献
	float			_tax_off;								//	税后比例（1-5.2%）
	float			_adj_rate;								//	调整比例**
	float			_adv_rate;								//	垫付比例**

	unsigned long	_bon_min;								//	奖池保底
	unsigned long	_bon_max;								//	奖池封顶
	unsigned long	_bon_val;								//	税前奖池

	float			_res_rate;								//	保留比率
	unsigned long	_res_max;								//	保留封顶
	unsigned long	_res_val;								//	保留金额
	unsigned char	_hide_res;								//	隐藏保留{0否，1是}

	unsigned long	_bon_show;								//	揭示奖池
	unsigned long	_awd_cnt;								//	派奖计数

	SSysPar(void)
	{
		memset(this, 0, sizeof(SSysPar));
	}

	~SSysPar(void)
	{
	}
};
#pragma pack (pop, 1)

class CSysPar
{
public:
	CSysPar(CMYSQLDB	*vms_db,
			const char	*vms_db_name);
	~CSysPar(void);

	bool startup(void);
	void cleanup(void);

	SSysPar* get(void);

	unsigned char get_proc_sync(void);
	void set_proc_sync(unsigned char proc_sync);

	bool is_admin(unsigned long mobile);
private:
	CMYSQLDB	*_vms_db;
	char		_vms_db_name[MAX_PATH];

	Process::CCriticalSection	_cs;
	SSysPar		_par_val;
};
