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
	unsigned long	_time_dsave;					//	��ʷ���ݴ洢����(��λ��)
	unsigned long	_time_dsile;					//	�û���Ĭ����(��λ��)
	unsigned long	_time_canal;					//	���ݷ���ģ������ʱ��(��λ��)
	unsigned long	_time_sproc;					//	���մ���ģ������ʱ��(��λ��)
	unsigned long	_time_kwbeg;					//	ҵ��ϵͳ��ʼ����ʱ��(��λ��)
	unsigned long	_time_kwend;					//	ҵ��ϵͳ��ɴ���ʱ��(��λ��)
	unsigned long	_time_audit;					//	��˼��ʱ��(��λ��)
	unsigned long	_time_jobint;					//	������ʱ��(��λ��)

	unsigned long	_time_highspd;					//	�����ȼ���������/��վ�����ٶȣ���λΪ����
	unsigned long	_time_billspd;					//	�˵�Ⱥ���ٶȣ���λΪ����
	unsigned long	_time_pushspd;					//	�����ȼ�Ⱥ���ٶȣ���λΪ����
	
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
	unsigned char	_bill_isopen;					//	���˵������Ƿ�����{0-��1-��}
	unsigned char	_bill_begday;					//	�°����˵��գ�ȱʡ12��
	unsigned char	_bill_minday;					//	�°����˵������
	unsigned char	_bill_resend;					//	ÿ��ȫ�����Ƿ�ÿ��ȫ������{0-��1-��}

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
	unsigned char	_rmd8_isopen;					//	��8���ѹ����Ƿ�����
	unsigned char	_rmd8_ctrl;						//	��8�ͷ�����ѡ��{0δ����;1����8;2����88;3����888}
	unsigned long	_rmd8_give8;					//	��8���ͻ��ֱ���
	unsigned long	_rmd8_give88;					//	��88���ͻ��ֱ���
	unsigned long	_rmd8_give888;					//	��888���ͻ��ֱ���

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
	unsigned long	_reg_autopass;					//	�Ǽ���Ʒ�Զ����ͨ��
	unsigned long	_reg_defcat;					//	�Ǽ�ȱʡ���ID

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
	unsigned char	_t100_isopen;					//	��ȡ������Ʒ�����Ƿ�����{0-��1-��}'),
	unsigned char	_t100_where;					//	��ȡ����{0��ת������1����ȡ����}'),
	unsigned long	_t100_extval;					//	��ȡֵ

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
	char			_lssm_reg_new[MAX_PATH];		//	�¶���СƷ
	char			_lssm_reg_old[MAX_PATH];		//	�����ѵǼ�
	char			_lssm_reg_exist[MAX_PATH];		//	�ѵǼ�δ���
	char			_lssm_reg_ban[MAX_PATH];		//	�ѵǼǲ�ͨ��
	char			_lssm_reg_taboo[MAX_PATH];		//	��Υ������
	char			_lssm_reg_short[MAX_PATH];		//	���ݹ���
	char			_lssm_reg_accept[MAX_PATH];		//	���ͨ��
	char			_lssm_reg_accept2[MAX_PATH];	//	���ͨ��2
	char			_lssm_reg_reject[MAX_PATH];		//	��˾ܾ�

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
	char			_smgi_aforbid[MAX_PATH];		//	��ֹ����
	char			_smgi_a1[MAX_PATH];				//	������ּƻ�1
	char			_smgi_a2[MAX_PATH];				//	������ּƻ�2
	char			_smgi_a3[MAX_PATH];				//	������ּƻ�3
	char			_smgi_a4[MAX_PATH];				//	������ּƻ�4

	char			_smgi_t1[MAX_PATH];				//	�����������1
	char			_smgi_t2[MAX_PATH];				//	�����������2
	char			_smgi_t3[MAX_PATH];				//	�����������3
	char			_smgi_t4[MAX_PATH];				//	�����������4
	char			_smgi_ttg[MAX_PATH];			//	�л���T��
	char			_smgi_tag[MAX_PATH];			//	�л���A��
	char			_smgi_taz[MAX_PATH];			//	�޻���A��

	char			_smgi_tc[MAX_PATH];				//	�������˳�

	char			_smgi_ta[MAX_PATH];				//	�޻���T��
	char			_smgi_tatg[MAX_PATH];			//	�л���T��
	char			_smgi_taag[MAX_PATH];			//	�л���A��
	char			_smgi_taaz[MAX_PATH];			//	�޻���A��
	char			_smgi_tasg[MAX_PATH];			//	�л���S��
	char			_smgi_tasz[MAX_PATH];			//	�޻���A��
	char			_smgi_tacfm[MAX_PATH];			//	���������ȷ��

	char			_smgi_help[MAX_PATH];			//	ϵͳ����
	char			_smgi_null[MAX_PATH];			//	δ����
	char			_smgi_busy[MAX_PATH];			//	ϵͳ��æ
	char			_smgi_fail[MAX_PATH];			//	ϵͳά�������ִ���
	char			_smgi_auto[MAX_PATH];			//	���оܾ����֣��Զ��ر�

	char			_smgi_bz1[MAX_PATH];			//	�����ظ���1
	char			_smgi_bz2[MAX_PATH];			//	�����ظ���2
	char			_smgi_bz3[MAX_PATH];			//	�����ظ���3
	char			_smgi_bz4[MAX_PATH];			//	�����ظ���4
	char			_smgi_bz5[MAX_PATH];			//	�����ظ���5
	char			_smgi_bz6[MAX_PATH];			//	�����ظ���6

	char			_smgi_0000tg[MAX_PATH];			//	�л���T��
	char			_smgi_0000tz[MAX_PATH];			//	�޻���T��
	char			_smgi_0000ag[MAX_PATH];			//	�л���A��
	char			_smgi_0000az[MAX_PATH];			//	�޻���A��
	char			_smgi_0000sg[MAX_PATH];			//	�л���S��
	char			_smgi_0000sz[MAX_PATH];			//	�޻���S��

	char			_smgi_00000[MAX_PATH];			//	ȫ��ȡ��

	char			_smgi_ja[MAX_PATH];				//	A���û�
	char			_smgi_jt[MAX_PATH];				//	T���û�
	char			_smgi_js[MAX_PATH];				//	S���û�
	char			_smgi_jo[MAX_PATH];				//	Other���û�

	char			_smgi_wok[MAX_PATH];			//	Wָ��
	char			_smgi_whelp[MAX_PATH];			//	Wָ�����

	char			_smgi_reg_rej[MAX_PATH];		//	�������û�
	char			_smgi_reg_new[MAX_PATH];		//	�¶���СƷ
	char			_smgi_reg_old[MAX_PATH];		//	�����ѵǼ�
	char			_smgi_reg_exist[MAX_PATH];		//	�ѵǼ�δ���
	char			_smgi_reg_ban[MAX_PATH];		//	�ѵǼǲ�ͨ��
	char			_smgi_reg_taboo[MAX_PATH];		//	��Υ������
	char			_smgi_reg_short[MAX_PATH];		//	���ݹ���
	char			_smgi_reg_fail[MAX_PATH];		//	�Ǽ�ʧ��
	char			_smgi_reg_accept[MAX_PATH];		//	���ͨ��
	char			_smgi_reg_reject[MAX_PATH];		//	��˾ܾ�
	char			_smgi_reg_pts_lmt[MAX_PATH];	//	��������֪ͨ

	char			_smgi_m_menu[MAX_PATH];			//	M�˵�
	char			_smgi_md_menu[MAX_PATH];		//	MD�˵�

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
	//	���عؼ���
	bool load_key(void);

	//	�����ų���
	bool load_extra(void);

	//	������չ
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