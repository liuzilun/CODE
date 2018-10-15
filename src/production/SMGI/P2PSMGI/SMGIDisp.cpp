#include "SMGIDisp.h"
#include "..\Lib\Log.h"
#include "..\VMSCommon\Define.h"
#include "IniCfg.h"

CSMGIDisp::CSMGIDisp(CMYSQLDBPool	*db_pool,
					 const char		*db_name,
					 CMYSQLDB		*mo_ext_db)
	: _db_pool(db_pool)
	, _port(NULL)
	, _segs(NULL)
	, _db_link(NULL)
{
	strncpy(_db_name, db_name, MAX_PATH);
	_db_link = _db_pool->enter();
	
	_segs = new CSegs(_db_link->_db, db_name);
	_port = new CPort(_db_pool, db_name);

	//	只建立制定的端口
	if (g_ini.get_cfg()->_port_id != 0)
	{
		_port->startup(g_ini.get_cfg()->_port_id);
	}

	_cs.Lock();
	MAP_SPInfo::iterator it = _port->_sp_info.begin();
	while (it != _port->_sp_info.end())
	{
		CMySMGI *smgi = new CMySMGI(it->first, it->second, 
									_db_pool , db_name, mo_ext_db);
		_map_smgi[it->first] = smgi;
		++it;
	}
	_cs.Unlock();

	BeginThread();
}

CSMGIDisp::~CSMGIDisp(void)
{
	EndThread();

	save_seq_id();

	//	内存中数据重置stat
	reset_status();

	/*_cs.Lock();
	MAP_SMGI::iterator it = _map_smgi.begin();
	while (it != _map_smgi.end())
	{
		CMySMGI *smgi = it->second;
		if (smgi != NULL)
		{
			delete smgi;
			smgi = NULL;
		}
		++it;
	}
	_map_smgi.clear();
	_cs.Unlock();
	*/

	if (_port != NULL)
	{
		delete _port;
		_port = NULL;
	}

	if (_segs != NULL)
	{
		delete _segs;
		_segs = NULL;
	}

	_db_pool->leave(*_db_link);
}

bool CSMGIDisp::reset_status(void)
{
	SMYSQLDBLink *db = _db_pool->enter();
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_RESET_SMG_LOG_STATUS, _db_name);
	bool ret = db->_db->Exec(sql) == TRUE;
	g_log.append(rpt::info, "Reset status pending(%lu).", db->_db->ExecRows());
	_db_pool->leave(*db);
	return ret;	
}

bool CSMGIDisp::can_push(void)
{
	bool ret = false;
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_CHECK_CAN_PUSH, _db_name);
	SMYSQLDBLink *db = _db_pool->enter();
	CMYSQLRS rs;
	if (db->_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			ret = rs.AsU32(0) > 0;
		}
		rs.FreeRsl();
	}
	_db_pool->leave(*db);
	return ret;
}

void CSMGIDisp::save_seq_id(void)
{
	Process::CAutoLock lock(&_cs);
	MAP_SMGI::iterator it = _map_smgi.begin();
	while (it != _map_smgi.end())
	{
		CMySMGI *x = it->second;
		_port->save_seq_id(it->first, x->get_last_seq_id());
		++it;
	}
}

unsigned int CSMGIDisp::ThreadEntry(void)
{
	unsigned long disp_cnt = 0;
	unsigned long save_cnt = 0;

	//	每次提交2000条短信，其中每次SLEEP(10),计算次数
	unsigned long slepp_cnt = 2000 / g_ini.get_cfg()->_push_cap * 100;
	while (!terminated_)
	{
		if (disp_cnt <= slepp_cnt)
		{
			++disp_cnt;
			++save_cnt;
			Sleep(10);
		}
		else
		{
			if (save_cnt >= 30000)
			{
				save_cnt = 0;
				save_seq_id();
			}

			do_disp();
			disp_cnt= 0;
		}
	}

	return 0;
}

unsigned long CSMGIDisp::do_disp(void)
{
	char sql[MAX_SQL_LEN] = {0};
	bool is_high = true;
	
	//	采用多端口分流下发
	if (g_ini.get_cfg()->_port_id == 0)
	{
		if (can_push())
		{
			sprintf(sql, SQL_GET_SMG_LOG_LOW, _db_name, _db_name);
			is_high = false;
		}
		else
		{
			sprintf(sql, SQL_GET_SMG_LOG_HIGH, _db_name, _db_name);
		}
	}
	//	指定端口下发
	else
	{
		if (can_push())
		{
			sprintf(sql, SQL_GET_SMG_LOG_LOW_PORT, _db_name, _db_name, g_ini.get_cfg()->_port_id);
			is_high = false;
		}
		else
		{
			sprintf(sql, SQL_GET_SMG_LOG_HIGH_PORT, _db_name, _db_name, g_ini.get_cfg()->_port_id);
		}
	}

	SMYSQLDBLink *db = _db_pool->enter();
	CMYSQLRS rs;
	unsigned long count = 0;
	if (db->_db->Exec(sql, &rs))
	{
		while (rs.LoadRow())
		{
			SSMGLog x;
			x._sm_idx			= rs.AsU32(0);
			sprintf(x._port_num, "%s%s", rs.AsStr(11), rs.AsStr(1));
			x._mobile			= rs.AsU32(2);
			x._city_id			= (unsigned short)rs.AsU32(3);
			x._pay_num			= rs.AsU32(4);
			x._prio_id			= (unsigned char)rs.AsU32(5);
			x._src_type			= (unsigned char)rs.AsU32(6);
			strncpy(x._link_id, rs.AsStr(7), 21);
			strncpy(x._cont, rs.AsStr(8), 160);
			x._status			= (unsigned char)rs.AsU32(9);
			unsigned long port_id = rs.AsU32(10);
			strncpy(x._srv_type, rs.AsStr(12), 11);
			strncpy(x._fee_type, rs.AsStr(13), 2);
			x._fee_val			= (unsigned short)rs.AsU32(14);
			x._ot_flag			= (unsigned char)rs.AsU32(15);
			x._prio				= (unsigned char)rs.AsU32(16);
			x._is_agent			= (unsigned char)rs.AsU32(17);
			x._is_reg			= (unsigned char)rs.AsU32(18);
			
			CMySMGI *smgi = find_smgi(port_id);
			if (smgi != NULL)
			{
				//smgi->do_submit(x);
				if (smgi->dispatch(x))
				{
					//	获取区号
					if (x._city_id == 0)
					{
						SSegs *seg = _segs->find(x._mobile);
						if (seg != NULL)
						{
							x._city_id = (unsigned short)seg->_city_id;
						}
						else
						{
							x._city_id = (unsigned short)(x._mobile / 100000);
						}
					}

					memset(sql, 0, MAX_SQL_LEN);
					sprintf(sql, SQL_UPDATE_SMG_LOG, _db_name, x._city_id, x._sm_idx);
					if (!db->_db->Exec(sql))
					{
						g_log.append(rpt::error, "[CSMGIDisp]Update smg_log sending status failed.");
					}

					++count;
				}
			}			
		}
		rs.FreeRsl();

		if (count > 0)
		{
			g_log.append(rpt::info, "[CSMGIDisp]Submit_sm:%lu success.", count);
		}
	}
	_db_pool->leave(*db);
	return count;
}

CMySMGI* CSMGIDisp::find_smgi(unsigned long port_id)
{
	Process::CAutoLock	lock(&_cs);
	MAP_SMGI::iterator it = _map_smgi.find(port_id);
	if (it != _map_smgi.end())
	{
		return it->second;
	}
	else
	{
		return NULL;
	}
}