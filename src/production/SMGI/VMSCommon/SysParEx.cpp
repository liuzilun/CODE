#include "sysparex.h"
#include "../Lib/Fasparse.h"
#include "../Lib/Log.h"
#include "Define.h"
#include <stdio.h>

CSysParEx::CSysParEx(CMYSQLDB	*vms_db,
					 const char	*vms_db_name)
	: _vms_db(vms_db)
{
	strncpy(_vms_db_name, vms_db_name, MAX_PATH);
	startup();
}

CSysParEx::~CSysParEx(void)
{
	cleanup();
}

bool CSysParEx::startup(void)
{
	Process::CAutoLock lock(&_cs);
	return load_key() && load_extra() && load_time()
		&& load_bill() && load_rmd8() && load_t100()
		&& load_smgi() && load_ivr() && load_lssm() && load_reg();
}

void CSysParEx::cleanup(void)
{
}

bool CSysParEx::load_key(void)
{
	bool ret = true;
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_GET_SYS_PAR_EX_KEY, _vms_db_name);

	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		while (rs.LoadRow())
		{
			g_fasparse.add_kword((char*)rs.AsStr(0));
		}
		rs.FreeRsl();
	}
	else
	{
		ret = false;
	}
	return ret;
}

bool CSysParEx::load_extra(void)
{
	bool ret = true;
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_GET_SYS_PAR_EX_EXTRA, _vms_db_name);

	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		while (rs.LoadRow())
		{
			g_fasparse.set_extra((char*)rs.AsStr(0));
		}
		rs.FreeRsl();
	}
	else
	{
		ret = false;
	}
	return ret;
}

bool CSysParEx::load_time(void)
{
	_par_time._time_dsave	= get_val_int("TIME_DSAVE");
	_par_time._time_dsile	= get_val_int("TIME_DSILE");
	_par_time._time_canal	= get_val_int("TIME_CANAL");
	_par_time._time_sproc	= get_val_int("TIME_SPROC");
	_par_time._time_kwbeg	= get_val_int("TIME_KWBEG");
	_par_time._time_kwend	= get_val_int("TIME_KWEND");
	_par_time._time_audit	= get_val_int("TIME_AUDIT");
	_par_time._time_jobint	= get_val_int("TIME_JOBINT");
	_par_time._time_highspd	= get_val_int("TIME_HIGHSPD");
	_par_time._time_billspd	= get_val_int("TIME_BILLSPD");
	_par_time._time_pushspd	= get_val_int("TIME_PUSHSPD");

	return true;
}

bool CSysParEx::load_smgi(void)
{
	//	A
	char val[MAX_PATH] = {0};
	strncpy(_par_smgi._smgi_aforbid, get_val_str("SMGI_AFORBID", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_a1, get_val_str("SMGI_A1", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_a2, get_val_str("SMGI_A2", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_a3, get_val_str("SMGI_A3", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_a4, get_val_str("SMGI_A4", val), MAX_PATH);

	//	T
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_t1, get_val_str("SMGI_T1", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_t2, get_val_str("SMGI_T2", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_t3, get_val_str("SMGI_T3", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_t4, get_val_str("SMGI_T4", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_ttg, get_val_str("SMGI_TTG", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_tag, get_val_str("SMGI_TAG", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_taz, get_val_str("SMGI_TAZ", val), MAX_PATH);

	//	TA
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_ta, get_val_str("SMGI_TA", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_tatg, get_val_str("SMGI_TATG", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_taag, get_val_str("SMGI_TAAG", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_taaz, get_val_str("SMGI_TAAZ", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_tasg, get_val_str("SMGI_TASG", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_tasz, get_val_str("SMGI_TASZ", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_tacfm, get_val_str("SMGI_TACFM", val), MAX_PATH);

	//	TC
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_tc, get_val_str("SMGI_TC", val), MAX_PATH);

	//	BZ/H/160
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_help, get_val_str("SMGI_HELP", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_null, get_val_str("SMGI_NULL", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_busy, get_val_str("SMGI_BUSY", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_fail, get_val_str("SMGI_FAIL", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_auto, get_val_str("SMGI_AUTO", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_bz1, get_val_str("SMGI_BZ1", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_bz2, get_val_str("SMGI_BZ2", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_bz3, get_val_str("SMGI_BZ3", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_bz4, get_val_str("SMGI_BZ4", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_bz5, get_val_str("SMGI_BZ5", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_bz6, get_val_str("SMGI_BZ6", val), MAX_PATH);

	//	0000
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_0000tg, get_val_str("SMGI_0000TG", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_0000tz, get_val_str("SMGI_0000TZ", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_0000ag, get_val_str("SMGI_0000AG", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_0000az, get_val_str("SMGI_0000AZ", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_0000sg, get_val_str("SMGI_0000SG", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_0000sz, get_val_str("SMGI_0000SZ", val), MAX_PATH);
	
	//	00000
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_00000, get_val_str("SMGI_00000", val), MAX_PATH);

	//	J
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_ja, get_val_str("SMGI_JA", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_jt, get_val_str("SMGI_JT", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_js, get_val_str("SMGI_JS", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_jo, get_val_str("SMGI_JO", val), MAX_PATH);

	//	W
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_wok, get_val_str("SMGI_WOK", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_whelp, get_val_str("SMGI_WHELP", val), MAX_PATH);

	//	登记作品
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_rej, get_val_str("SMGI_REG_REJ", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_new, get_val_str("SMGI_REG_NEW", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_old, get_val_str("SMGI_REG_OLD", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_exist, get_val_str("SMGI_REG_EXIST", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_ban, get_val_str("SMGI_REG_BAN", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_taboo, get_val_str("SMGI_REG_TABOO", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_short, get_val_str("SMGI_REG_SHORT", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_fail, get_val_str("SMGI_REG_FAIL", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_accept, get_val_str("SMGI_REG_ACCEPT", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_reject, get_val_str("SMGI_REG_REJECT", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_reg_pts_lmt, get_val_str("SMGI_REG_PTS_LMT", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_m_menu, get_val_str("SMGI_M_MENU", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_smgi._smgi_md_menu, get_val_str("SMGI_MD_MENU", val), MAX_PATH);

	return true;
}

bool CSysParEx::load_bill(void)
{
	return true;
}

bool CSysParEx::load_rmd8(void)
{
	return true;
}

bool CSysParEx::load_t100(void)
{
	_par_t100._t100_isopen	= (unsigned char)get_val_int("T100_ISOPEN");
	_par_t100._t100_where	= (unsigned char)get_val_int("T100_WHERE");
	_par_t100._t100_extval	= get_val_int("T100_EXTVAL");
	return true;
}

bool CSysParEx::load_reg(void)
{
	_par_reg._reg_autopass	= get_val_int("REG_AUTOPASS");
	_par_reg._reg_defcat	= get_val_int("REG_DEFCAT");

	return true;
}

bool CSysParEx::load_ivr(void)
{
	char val[MAX_PATH] = {0};
	_par_ivr._ivr_isopen	= (unsigned char)get_val_int("IVR_ISOPEN");
	_par_ivr._ivr_begtime	= get_val_int("IVR_BEGTIME");
	_par_ivr._ivr_endtime	= get_val_int("IVR_ENDTIME");

	memset(val, 0, MAX_PATH);
	strncpy(_par_ivr._ivr_limittime, get_val_str("IVR_LIMITTIME", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_ivr._ivr_callok, get_val_str("IVR_CALLOK", val), MAX_PATH);

	_par_ivr._ivr_islimitmt	= (unsigned char)get_val_int("IVR_ISLIMITMT");

	memset(val, 0, MAX_PATH);
	strncpy(_par_ivr._ivr_limitmt, get_val_str("IVR_LIMITMT", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_ivr._ivr_callfailed, get_val_str("IVR_CALLFAILED", val), MAX_PATH);

	return true;
}

bool CSysParEx::load_lssm(void)
{
	//	登记作品
	char val[MAX_PATH] = {0};
	strncpy(_par_lssm._lssm_reg_new, get_val_str("LSSM_REG_NEW", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_lssm._lssm_reg_old, get_val_str("LSSM_REG_OLD", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_lssm._lssm_reg_exist, get_val_str("LSSM_REG_EXIST", val), MAX_PATH);
	
	memset(val, 0, MAX_PATH);
	strncpy(_par_lssm._lssm_reg_ban, get_val_str("LSSM_REG_BAN", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_lssm._lssm_reg_taboo, get_val_str("LSSM_REG_TABOO", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_lssm._lssm_reg_short, get_val_str("LSSM_REG_SHORT", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_lssm._lssm_reg_accept, get_val_str("LSSM_REG_ACCEPT", val), MAX_PATH);

	memset(val, 0, MAX_PATH);
	strncpy(_par_lssm._lssm_reg_reject, get_val_str("LSSM_REG_REJECT", val), MAX_PATH);
	
	return true;
}

unsigned long CSysParEx::get_val_int(const char *par_name)
{
	CMYSQLRS rs;
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_GET_THE_SYS_PAR_EX, _vms_db_name, par_name);
	if (_vms_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			return rs.AsU32(0);
		}
		else
		{
			g_log.append(rpt::error, "[CSysParEx]par_name[%s] no exist.", par_name);
		}
	}
	else
	{
		g_log.append(rpt::error, "[CSysParEx]Exec sql=%s failed.", sql);
	}
	return 0;
}

const char* CSysParEx::get_val_str(const char *par_name, char *val)
{
	CMYSQLRS rs;
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_GET_THE_SYS_PAR_EX, _vms_db_name, par_name);
	if (_vms_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			strncpy(val, rs.AsStr(0), MAX_PATH);
		}
		else
		{
			g_log.append(rpt::error, "[CSysParEx]par_name[%s] no exist.", par_name);
		}
		rs.FreeRsl();
	}
	else
	{
		g_log.append(rpt::error, "[CSysParEx]Exec sql=%s failed.", sql);
	}

	return val;
}

bool CSysParEx::set_val_int(const char *par_name, unsigned long par_val)
{
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_UPDATE_SYS_PAR_INT, _vms_db_name, par_val, par_name);
	return _vms_db->Exec(sql) == TRUE;
}

SSysParTime* CSysParEx::get_time(void)
{
	Process::CAutoLock lock(&_cs);
	return &_par_time;
}

SSysParBill* CSysParEx::get_bill(void)
{
	Process::CAutoLock lock(&_cs);
	return &_par_bill;
}

SSysParRmd8* CSysParEx::get_rmd8(void)
{
	Process::CAutoLock lock(&_cs);
	return &_par_rmd8;
}

SSysParT100* CSysParEx::get_t100(void)
{
	Process::CAutoLock lock(&_cs);
	return &_par_t100;
}

SSysParSMGI* CSysParEx::get_smgi(void)
{
	Process::CAutoLock lock(&_cs);
	return &_par_smgi;
}

SSysParIVR* CSysParEx::get_ivr(void)
{
	Process::CAutoLock lock(&_cs);
	return &_par_ivr;
}

SSysParLSSM* CSysParEx::get_lssm(void)
{
	Process::CAutoLock lock(&_cs);
	return &_par_lssm;
}

SSysParReg* CSysParEx::get_reg(void)
{
	Process::CAutoLock lock(&_cs);
	return &_par_reg;
}