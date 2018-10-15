#include "syspar.h"
#include "Define.h"
#include <stdio.h>

CSysPar::CSysPar(CMYSQLDB	*vms_db,
				 const char	*vms_db_name)
	: _vms_db(vms_db)
{
	strncpy(_vms_db_name, vms_db_name, MAX_PATH);

	startup();
}

CSysPar::~CSysPar(void)
{
	cleanup();
}

bool CSysPar::startup(void)
{
	bool ret = true;
	Process::CAutoLock lock(&_cs);
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_GET_SYS_PAR, _vms_db_name);
	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			_par_val._t_schm_id	= (unsigned short)rs.AsU32(0);
			
			_par_val._col_m		= (unsigned short)rs.AsU32(1);
			_par_val._col_web	= (unsigned short)rs.AsU32(2);
			_par_val._col_hot	= (unsigned short)rs.AsU32(3);
			
			_par_val._jt_reg	= (unsigned short)rs.AsU32(4);
			_par_val._jt_hot	= (unsigned short)rs.AsU32(5);
			_par_val._jt_upg	= (unsigned short)rs.AsU32(6);
	
			_par_val._pts_reg	= (unsigned char)rs.AsU32(7);
			_par_val._pts_p2p	= (unsigned char)rs.AsU32(8);
			_par_val._pts_self	= (unsigned char)rs.AsU32(9);
			_par_val._pts_8sm	= (unsigned char)rs.AsU32(10);
			_par_val._pts_on8	= (unsigned char)rs.AsU32(11);
			_par_val._pts_top	= (unsigned char)rs.AsU32(12);
			_par_val._pts_topc	= (unsigned char)rs.AsU32(13);
			_par_val._pts_awd	= (unsigned char)rs.AsDbl(14);

			_par_val._alert1	= rs.AsU32(15);
			_par_val._alert2	= rs.AsU32(16);

			_par_val._proc_sync	= (unsigned char)rs.AsU32(17);
			_par_val._trg_clen	= (unsigned char)rs.AsU32(18);

			_par_val._prio_cnt	= (unsigned char)rs.AsU32(19);
			_par_val._misc_smg	= (unsigned char)rs.AsU32(20);
			_par_val._keep_bon	= (unsigned char)rs.AsU32(21);
			_par_val._keep_pts	= (unsigned char)rs.AsU32(22);

			_par_val._bon_qty	= rs.AsU32(23);
			_par_val._per_dona	= (float)rs.AsDbl(24);
			_par_val._tax_off	= (float)rs.AsDbl(25);
			_par_val._adj_rate	= (float)rs.AsDbl(26);
			_par_val._adv_rate	= (float)rs.AsDbl(27);

			_par_val._bon_min	= rs.AsU32(28);
			_par_val._bon_max	= rs.AsU32(29);
			_par_val._bon_val	= rs.AsU32(30);

			_par_val._res_rate	= (float)rs.AsDbl(31);
			_par_val._res_max	= rs.AsU32(32);
			_par_val._res_val	= rs.AsU32(33);
			_par_val._hide_res	= (unsigned char)rs.AsU32(34);

			_par_val._bon_show	= rs.AsU32(35);
			_par_val._awd_cnt	= rs.AsU32(36);
		}
		else
		{
			ret = false;
		}
		rs.FreeRsl();
	}
	else
	{
		ret = false;
	}
	return ret;
}

void CSysPar::cleanup(void)
{
}

SSysPar* CSysPar::get(void)
{
	Process::CAutoLock lock(&_cs);
	return &_par_val;
}

unsigned char CSysPar::get_proc_sync(void)
{
	unsigned char ret = PROC_STATUS_NORMAL;

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_CHECK_PROC_SYNC, _vms_db_name);

	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			ret = (unsigned char)rs.AsU32(0);
		}
		rs.FreeRsl();
	}
	return ret;
}

void CSysPar::set_proc_sync(unsigned char proc_sync)
{
	Process::CAutoLock lock(&_cs);
	_par_val._proc_sync	= proc_sync;

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_SET_PROC_SYNC, _vms_db_name, proc_sync);
	_vms_db->Exec(sql);
}

bool CSysPar::is_admin(unsigned long mobile)
{
	return _par_val._alert1 == mobile || _par_val._alert2 == mobile;
}