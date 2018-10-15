#pragma once

#include "..\Lib\MySQLDB.h"
#include "..\Lib\Sync.h"

static const char *SQL_GET_SYS_PAR_EX_KEY
	= "SELECT pvar FROM %s.sys_parex WHERE pkey like 'KEY_%%'";
static const char *SQL_GET_SYS_PAR_EX_EXTRA
	= "SELECT pvar FROM %s.sys_parex WHERE pkey like 'EXTRA_%%'";

static const char *SQL_GET_THE_SYS_PAR_EX
	= "SELECT pvar FROM %s.sys_parex WHERE pkey=\'%s\'";

static const char *SQL_UPDATE_SYS_PAR_STR
	= "UPDATE %s.sys_parex SET pvar=\'%s\' WHERE pkey=\'%s\'";
static const char *SQL_UPDATE_SYS_PAR_INT
	= "UPDATE %s.sys_parex SET pvar=%lu WHERE pkey=\'%s\'";

#pragma pack (push, 1)
struct SSysParTime
{
	unsigned long	_time_dsave;					//	历史数据存储周期(单位天)
	unsigned long	_time_dsile;					//	用户沉默周期(单位天)
	unsigned long	_time_canal;					//	内容分析模块启动时间(单位秒)
	unsigned long	_time_sproc;					//	日终处理模块启动时间(单位秒)
	unsigned long	_time_kwbeg;					//	业务系统开始处理时间(单位秒)
	unsigned long	_time_kwend;					//	业务系统完成处理时间(单位秒)
	unsigned long	_time_audit;					//	审核间隔时间(单位秒)
	unsigned long	_time_jobint;					//	任务间隔时长(单位秒)

	unsigned long	_time_highspd;					//	高优先级交互反馈/网站下载速度，单位为毫秒
	unsigned long	_time_billspd;					//	账单群发速度，单位为毫秒
	unsigned long	_time_pushspd;					//	低优先级群发速度，单位为毫秒
	
	SSysParTime(void)
	{
		memset(this, 0, sizeof(SSysParTime));
	}

	~SSysParTime(void)
	{
	}
};

struct SSysParBill
{
	unsigned char	_bill_isopen;					//	下账单功能是否启用{0-否，1-是}
	unsigned char	_bill_begday;					//	下包月账单日，缺省12日
	unsigned char	_bill_minday;					//	下包月账单最迟日
	unsigned char	_bill_resend;					//	每天全部重是否每天全部重下{0-否，1-是}

	SSysParBill(void)
	{
		memset(this, 0, sizeof(SSysParBill));
	}

	~SSysParBill(void)
	{
	}
};

struct SSysParRmd8
{
	unsigned char	_rmd8_isopen;					//	逢8提醒功能是否启用
	unsigned char	_rmd8_ctrl;						//	逢8送分启用选项{0未启用;1启用8;2启用88;3启用888}
	unsigned long	_rmd8_give8;					//	逢8赠送积分倍数
	unsigned long	_rmd8_give88;					//	逢88赠送积分倍数
	unsigned long	_rmd8_give888;					//	逢888赠送积分倍数

	SSysParRmd8(void)
	{
		memset(this, 0, sizeof(SSysParRmd8));
	}

	~SSysParRmd8(void)
	{
	}
};

struct SSysParReg
{
	unsigned long	_reg_autopass;					//	登记作品自动审核通过
	unsigned long	_reg_defcat;					//	登记缺省类别ID

	SSysParReg(void)
	{
		memset(this, 0, sizeof(SSysParReg));
	}

	~SSysParReg(void)
	{
	}
};

struct SSysParT100
{
	unsigned char	_t100_isopen;					//	提取最热作品功能是否启用{0-否，1-是}'),
	unsigned char	_t100_where;					//	提取条件{0按转发量，1按提取数量}'),
	unsigned long	_t100_extval;					//	提取值

	SSysParT100(void)
	{
		memset(this, 0, sizeof(SSysParT100));
	}

	~SSysParT100(void)
	{
	}
};

struct SSysParIVR
{
	unsigned char	_ivr_isopen;
	unsigned long	_ivr_begtime;
	unsigned long	_ivr_endtime;
	char			_ivr_limittime[MAX_PATH];
	char			_ivr_callok[MAX_PATH];
	unsigned char	_ivr_islimitmt;
	char			_ivr_limitmt[MAX_PATH];
	char			_ivr_callfailed[MAX_PATH];

	SSysParIVR(void)
	{
		memset(this, 0, sizeof(SSysParT100));
	}

	~SSysParIVR(void)
	{
	}
};

struct SSysParLSSM
{
	char			_lssm_reg_new[MAX_PATH];		//	新短信小品
	char			_lssm_reg_old[MAX_PATH];		//	别人已登记
	char			_lssm_reg_exist[MAX_PATH];		//	已登记未审核
	char			_lssm_reg_ban[MAX_PATH];		//	已登记不通过
	char			_lssm_reg_taboo[MAX_PATH];		//	有违禁短语
	char			_lssm_reg_short[MAX_PATH];		//	内容过短
	char			_lssm_reg_accept[MAX_PATH];		//	审核通过
	char			_lssm_reg_accept2[MAX_PATH];	//	审核通过2
	char			_lssm_reg_reject[MAX_PATH];		//	审核拒绝

	SSysParLSSM(void)
	{
		memset(this, 0, sizeof(SSysParLSSM));
	}

	~SSysParLSSM(void)
	{
	}
};

struct SSysParSMGI
{
	char			_smgi_aforbid[MAX_PATH];		//	禁止加入
	char			_smgi_a1[MAX_PATH];				//	加入积分计划1
	char			_smgi_a2[MAX_PATH];				//	加入积分计划2
	char			_smgi_a3[MAX_PATH];				//	加入积分计划3
	char			_smgi_a4[MAX_PATH];				//	加入积分计划4

	char			_smgi_t1[MAX_PATH];				//	加入免费欣赏1
	char			_smgi_t2[MAX_PATH];				//	加入免费欣赏2
	char			_smgi_t3[MAX_PATH];				//	加入免费欣赏3
	char			_smgi_t4[MAX_PATH];				//	加入免费欣赏4
	char			_smgi_ttg[MAX_PATH];			//	有积分T类
	char			_smgi_tag[MAX_PATH];			//	有积分A类
	char			_smgi_taz[MAX_PATH];			//	无积分A类

	char			_smgi_tc[MAX_PATH];				//	无条件退出

	char			_smgi_ta[MAX_PATH];				//	无积分T类
	char			_smgi_tatg[MAX_PATH];			//	有积分T类
	char			_smgi_taag[MAX_PATH];			//	有积分A类
	char			_smgi_taaz[MAX_PATH];			//	无积分A类
	char			_smgi_tasg[MAX_PATH];			//	有积分S类
	char			_smgi_tasz[MAX_PATH];			//	无积分A类
	char			_smgi_tacfm[MAX_PATH];			//	其他类进行确认

	char			_smgi_help[MAX_PATH];			//	系统帮助
	char			_smgi_null[MAX_PATH];			//	未参与
	char			_smgi_busy[MAX_PATH];			//	系统繁忙
	char			_smgi_fail[MAX_PATH];			//	系统维护，出现错误
	char			_smgi_auto[MAX_PATH];			//	含有拒绝文字，自动关闭

	char			_smgi_bz1[MAX_PATH];			//	帮助回复语1
	char			_smgi_bz2[MAX_PATH];			//	帮助回复语2
	char			_smgi_bz3[MAX_PATH];			//	帮助回复语3
	char			_smgi_bz4[MAX_PATH];			//	帮助回复语4
	char			_smgi_bz5[MAX_PATH];			//	帮助回复语5
	char			_smgi_bz6[MAX_PATH];			//	帮助回复语6

	char			_smgi_0000tg[MAX_PATH];			//	有积分T类
	char			_smgi_0000tz[MAX_PATH];			//	无积分T类
	char			_smgi_0000ag[MAX_PATH];			//	有积分A类
	char			_smgi_0000az[MAX_PATH];			//	无积分A类
	char			_smgi_0000sg[MAX_PATH];			//	有积分S类
	char			_smgi_0000sz[MAX_PATH];			//	无积分S类

	char			_smgi_00000[MAX_PATH];			//	全部取消

	char			_smgi_ja[MAX_PATH];				//	A类用户
	char			_smgi_jt[MAX_PATH];				//	T类用户
	char			_smgi_js[MAX_PATH];				//	S类用户
	char			_smgi_jo[MAX_PATH];				//	Other类用户

	char			_smgi_wok[MAX_PATH];			//	W指令
	char			_smgi_whelp[MAX_PATH];			//	W指令帮助

	char			_smgi_reg_rej[MAX_PATH];		//	受限制用户
	char			_smgi_reg_new[MAX_PATH];		//	新短信小品
	char			_smgi_reg_old[MAX_PATH];		//	别人已登记
	char			_smgi_reg_exist[MAX_PATH];		//	已登记未审核
	char			_smgi_reg_ban[MAX_PATH];		//	已登记不通过
	char			_smgi_reg_taboo[MAX_PATH];		//	有违禁短语
	char			_smgi_reg_short[MAX_PATH];		//	内容过短
	char			_smgi_reg_fail[MAX_PATH];		//	登记失败
	char			_smgi_reg_accept[MAX_PATH];		//	审核通过
	char			_smgi_reg_reject[MAX_PATH];		//	审核拒绝
	char			_smgi_reg_pts_lmt[MAX_PATH];	//	积分限制通知

	char			_smgi_m_menu[MAX_PATH];			//	M菜单
	char			_smgi_md_menu[MAX_PATH];		//	MD菜单

	SSysParSMGI(void)
	{
		memset(this, 0, sizeof(SSysParSMGI));
	}

	~SSysParSMGI(void)
	{
	}
};
#pragma pack (pop, 1)

class CSysParEx
{
public:
	CSysParEx(CMYSQLDB		*vms_db,
			  const char	*vms_db_name);
	~CSysParEx(void);

	bool startup(void);
	void cleanup(void);

	SSysParTime* get_time(void);
	SSysParBill* get_bill(void);
	SSysParRmd8* get_rmd8(void);
	SSysParT100* get_t100(void);
	SSysParSMGI* get_smgi(void);
	SSysParIVR*  get_ivr(void);
	SSysParLSSM* get_lssm(void);
	SSysParReg*  get_reg(void);
private:
	//	加载关键词
	bool load_key(void);

	//	加载排除字
	bool load_extra(void);

	//	加载扩展
	bool load_ext(void);

	//
	bool load_time(void);
	bool load_bill(void);
	bool load_rmd8(void);
	bool load_t100(void);
	bool load_smgi(void);
	bool load_ivr(void);
	bool load_lssm(void);
	bool load_reg(void);
private:
	unsigned long get_val_int(const char *par_name);
	const char* get_val_str(const char *par_name, char *val);
	bool set_val_int(const char *par_name, unsigned long par_val);
private:
	CMYSQLDB	*_vms_db;
	char		_vms_db_name[MAX_PATH];

	Process::CCriticalSection	_cs;
	SSysParTime	_par_time;
	SSysParBill	_par_bill;
	SSysParRmd8	_par_rmd8;
	SSysParT100	_par_t100;
	SSysParSMGI	_par_smgi;
	SSysParIVR	_par_ivr;
	SSysParLSSM	_par_lssm;
	SSysParReg	_par_reg;
};