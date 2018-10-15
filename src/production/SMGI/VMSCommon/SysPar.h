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

//	ʵʱ���״̬
static const char *SQL_CHECK_PROC_SYNC
	= "SELECT proc_sync FROM %s.sys_par LIMIT 1";

//	����״̬
static const char *SQL_SET_PROC_SYNC
	= "UPDATE %s.sys_par SET proc_sync=%d";

//	����״̬
enum EPROC_STATUS
{
	PROC_STATUS_NORMAL		= 0,				//	����
	PROC_STATUS_CDR			= 1,				//	CDR�������
	PROC_STATUS_CONT_ANAL	= 2,				//	���ݷ������
	PROC_STATUS_SCORE_PROC	= 3					//	���ּ������
};

#pragma pack (push, 1)
struct SSysPar
{
	unsigned short	_t_schm_id;								//	T��schm_id
	
	unsigned short	_col_m;									//	M�㲥��Ŀ
	unsigned short	_col_web;								//	��ҳ������Ŀ
	unsigned short	_col_hot;								//	������Ŀ

	unsigned short	_jt_reg;								//	�Ǽ��������
	unsigned short	_jt_hot;								//	�����������
	unsigned short	_jt_upg;								//	�����������

	unsigned char	_pts_reg;								//	�Ǽ�pts_id
	unsigned char	_pts_p2p;								//	��ͨpts_id
	unsigned char	_pts_self;								//	�Է�pts_id
	unsigned char	_pts_8sm;								//	����pts_id
	unsigned char	_pts_on8;								//	��8 pts_id
	unsigned char	_pts_top;								//	ת����ǰʮ�ͷ�pts_id(ȫʡ)
	unsigned char	_pts_topc;								//	ת����ǰʮ�ͷ�pts_id(����)
	unsigned char	_pts_awd;								//	�񽱺˼�

	unsigned long	_alert1;								//	�澯����1
	unsigned long	_alert2;								//	�澯����2
	
	unsigned char	_proc_sync;								//	����ͬ��
															//	{0������1����������ϣ�2���ݷ����꣬3���ִ������}
	unsigned char	_trg_clen;								//	�������ݳ���

	unsigned char	_prio_cnt;								//	���ȼ�������1��ÿ�츴λ
	unsigned char	_misc_smg;								//	misc����
	unsigned char	_keep_bon;								//	�������أ����ɽ�����0��1��
	unsigned char	_keep_pts;								//	�������֣������㣩��0��1��

	unsigned long	_bon_qty;								//	ת������
	float			_per_dona;								//	ÿ������
	float			_tax_off;								//	˰�������1-5.2%��
	float			_adj_rate;								//	��������**
	float			_adv_rate;								//	�渶����**

	unsigned long	_bon_min;								//	���ر���
	unsigned long	_bon_max;								//	���طⶥ
	unsigned long	_bon_val;								//	˰ǰ����

	float			_res_rate;								//	��������
	unsigned long	_res_max;								//	�����ⶥ
	unsigned long	_res_val;								//	�������
	unsigned char	_hide_res;								//	���ر���{0��1��}

	unsigned long	_bon_show;								//	��ʾ����
	unsigned long	_awd_cnt;								//	�ɽ�����

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
