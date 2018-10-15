#include "SMGIDisp.h"
#include "..\Lib\Log.h"
#include "..\Lib\Fasparse.h"
#include "IniCfg.h"

using namespace RdiClubCommon;

CSMGIDisp::CSMGIDisp(CMYSQLDBPool	*db_pool)
	: _db_pool(db_pool)
	, _port(NULL)
	, _segs(NULL)
	, _db_link(NULL)
{
	_db_link = _db_pool->enter();
	
	_segs = new CSegment(_db_link->_db);
	_port = new CPort(_db_link->_db);

	//	只建立制定的端口
	if (g_ini.get()->_port_id != 0)
	{
		_port->startup(g_ini.get()->_port_id);
	}

	_cs.Lock();
	MAP_SPInfo::iterator it = _port->_sp_info.begin();
	while (it != _port->_sp_info.end())
	{
		CMySMGI *smgi = new CMySMGI(it->first, it->second, _db_pool);
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
	bool ret = db->_db->Exec(SQL_RESET_SMG_LOG_STATUS) == TRUE;
	g_log.append(Process::info, "Reset status pending(%lu).", db->_db->ExecRows());
	_db_pool->leave(*db);
	return ret;	
}

bool CSMGIDisp::can_push(void)
{
	bool ret = false;
	SMYSQLDBLink *db = _db_pool->enter();
	CMYSQLRS rs;
	if (db->_db->Exec(SQL_CHECK_CAN_PUSH, &rs))
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
	unsigned long slepp_cnt = 2000 / g_ini.get()->_push_cap * 100;
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

			if (can_push())
			{
				do_disp(PUSH_PRIO_HIGH, false);
				if (do_disp(PUSH_PRIO_MID, false) < 2000)
				{
					do_disp(PUSH_PRIO_LOW, false);
				}
			}
			else
			{
				do_disp(PUSH_PRIO_HIGH, true);
				if (do_disp(PUSH_PRIO_MID, true) < 2000)
				{
					do_disp(PUSH_PRIO_LOW, true);
				}
			}

			disp_cnt= 0;
		}
	}

	return 0;
}

unsigned long CSMGIDisp::do_disp(unsigned char prio, bool is_now)
{
	char sql[MAX_SQL_LEN] = {0};
	
	if (prio == PUSH_PRIO_HIGH)
	{
		//	采用多端口分流下发
		if (g_ini.get()->_port_id == 0)
		{
			sprintf(sql, SQL_GET_SMG_LOG_HIGH);
		}
		//	指定端口下发
		else
		{
			sprintf(sql, SQL_GET_SMG_LOG_HIGH_PORT, g_ini.get()->_port_id);
		}

	}
	else if (prio == PUSH_PRIO_MID)
	{
		//	采用多端口分流下发
		if (g_ini.get()->_port_id == 0)
		{
			if (is_now)
			{
				sprintf(sql, SQL_GET_SMG_LOG_MID_NOW);
			}
			else
			{
				sprintf(sql, SQL_GET_SMG_LOG_MID);
			}
		}
		//	指定端口下发
		else
		{
			if (is_now)
			{
				sprintf(sql, SQL_GET_SMG_LOG_MID_PORT_NOW, g_ini.get()->_port_id);
			}
			else
			{
				sprintf(sql, SQL_GET_SMG_LOG_MID_PORT, g_ini.get()->_port_id);
			}
		}
	}
	else
	{
		//	采用多端口分流下发
		if (g_ini.get()->_port_id == 0)
		{
			if (is_now)
			{
				sprintf(sql, SQL_GET_SMG_LOG_LOW_NOW);
			}
			else
			{
				sprintf(sql, SQL_GET_SMG_LOG_LOW);
			}
		}
		//	指定端口下发
		else
		{
			if (is_now)
			{
				sprintf(sql, SQL_GET_SMG_LOG_LOW_PORT_NOW, g_ini.get()->_port_id);
			}
			else
			{
				sprintf(sql, SQL_GET_SMG_LOG_LOW_PORT, g_ini.get()->_port_id);
			}
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
			x._at_dt			= rs.AsI64(1);
			x._port_id			= (unsigned char)rs.AsU32(2);
			strncpy(x._mo, rs.AsStr(3), 21);
			x._mt				= rs.AsI64(4);
			x._pay_num			= rs.AsI64(5);
			x._src_type			= (unsigned char)rs.AsU32(6);
			strncpy(x._link_id, rs.AsStr(7), 21);
			strncpy(x._cont, rs.AsStr(8), 1024);
			x._status			= (unsigned char)rs.AsU32(9);
			strncpy(x._srv_type, rs.AsStr(10), 11);
			strncpy(x._fee_type, rs.AsStr(11), 2);
			x._fee_val			= (unsigned short)rs.AsU32(12);
			x._ot_flag			= (unsigned char)rs.AsU32(13);
			x._prio				= (unsigned char)rs.AsU32(14);
			x._is_agent			= (unsigned char)rs.AsU32(15);
			x._is_reg			= (unsigned char)rs.AsU32(16);
			
			//	含有关键词，拦截该短信，不进行下发，设置状态=5
			if (g_fasparse.has_kword(x._cont))
			{
				if (prio == PUSH_PRIO_HIGH)
				{						
					sprintf(sql, SQL_UPDATE_SMG_LOG_HIGH, SM_STATUS_CANCEL, x._sm_idx);
				}
				else if (prio == PUSH_PRIO_MID)
				{
					sprintf(sql, SQL_UPDATE_SMG_LOG_MID, SM_STATUS_CANCEL, x._sm_idx);
				}
				else
				{
					sprintf(sql, SQL_UPDATE_SMG_LOG_LOW, SM_STATUS_CANCEL, x._sm_idx);
				}

				if (!db->_db->Exec(sql))
				{
					g_log.append(Process::error, "[CSMGIDisp]Update smg_log sending status failed.");
				}
				//++count;
			}
			else
			{
				CMySMGI *smgi = find_smgi(x._port_id);
				if (smgi != NULL)
				{
					if (smgi->dispatch(x, prio))
					{
						memset(sql, 0, MAX_SQL_LEN);
						if (prio == PUSH_PRIO_HIGH)
						{						
							sprintf(sql, SQL_UPDATE_SMG_LOG_HIGH, SM_STATUS_WAITING, x._sm_idx);
						}
						else if (prio == PUSH_PRIO_MID)
						{
							sprintf(sql, SQL_UPDATE_SMG_LOG_MID, SM_STATUS_WAITING, x._sm_idx);
						}
						else
						{
							sprintf(sql, SQL_UPDATE_SMG_LOG_LOW, SM_STATUS_WAITING, x._sm_idx);
						}

						if (!db->_db->Exec(sql))
						{
							g_log.append(Process::error, "[CSMGIDisp]Update smg_log sending status failed.");
						}

						++count;
					}
				}
			}
		}
		rs.FreeRsl();

		if (count > 0)
		{
			g_log.append(Process::info, "[CSMGIDisp]Submit_sm:%lu success.", count);
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