#include "ContPool.h"
#include <list>
#include "..\Lib\FasParse.h"
#include "..\Lib\log.h"
#include "..\Lib\Timer.h"

static BYTE phsmap[65536];

void map_phs(char* szphs)
{
	BYTE	idx = 1;
	char*	lpc = szphs;

	while ( *lpc && (lpc - szphs < 16) )
	{
		phsmap[*((WORD*) lpc)] |= idx;
		idx <<= 1;
		++(++lpc);
	}
}

bool compare_iden(SContPool &item1, SContPool &item2)
{
	return (memcmp(item1._iden, item2._iden, MAX_IDEN_LEN) <= 0);
}

CAuditing::CAuditing(CContPool	*cont_pool,
					 CMYSQLDB	*vms_db,
					 const char	*db_name)
	: _cont_pool(cont_pool)
	, _vms_db(vms_db)
	, _sys_par(NULL)
	, _sys_par_ex(NULL)
{
	strncpy(_db_name, db_name, MAX_PATH);

	_sys_par	= new CSysPar(vms_db, db_name);
	_sys_par_ex	= new CSysParEx(vms_db, db_name);

	BeginThread();
}

CAuditing::~CAuditing(void)
{
	EndThread();

	if (_sys_par != NULL)
	{
		delete _sys_par;
		_sys_par = NULL;
	}

	if (_sys_par_ex != NULL)
	{
		delete _sys_par_ex;
		_sys_par_ex = NULL;
	}
}

bool CAuditing::startup(void)
{
	return _sys_par->startup() && _sys_par_ex->startup();
}

void CAuditing::cleanup(void)
{
	_sys_par->cleanup();
	_sys_par_ex->cleanup();
}

unsigned int CAuditing::ThreadEntry(void)
{
	unsigned long count = 0;
	while (!terminated_)
	{
		if (count <= _sys_par_ex->get_time()->_time_audit)
		{
			++count;
			Sleep(1000);
		}
		else
		{
			count = 0;
			auditing();
		}
	}

	return 0;
}

unsigned long CAuditing::sync_cont(unsigned short	cat_id,
								   unsigned short	mmx_id,
								   unsigned long	mo_user,
								   const char		*mmx_till,								   
								   const char		*cont,
								   unsigned short	cont_type)
{
	unsigned long ret = 0;
	char proc_sm[MAX_SQL_LEN] = {0};
	long sm_len = g_fasparse.shrink(proc_sm, (char*)cont, MASK_TOTAL);
	if (sm_len >= MIN_CONT_LEN)
	{
		char iden_sm[MAX_IDEN_LEN * 2 + 1] = {0};
		strncpy(iden_sm, proc_sm + SIGN_LEN, MAX_IDEN_LEN);
		if (strlen(iden_sm) == MAX_IDEN_LEN)
		{
			SContPool *sm = _cont_pool->find(proc_sm, sm_len);
			if (sm == NULL)
			{
				unsigned char pts_id = _sys_par->get()->_pts_self;
				unsigned long cont_id = _cont_pool->new_cont(cat_id, mmx_id, pts_id, 
					mo_user, mmx_till, iden_sm, cont, cont_type);

				//	内存同步识别码
				if (cont_id > 0)
				{
					SContPool new_cont;
					new_cont._cat_id	= cat_id;
					new_cont._cont_id	= cont_id;
					strncpy(new_cont._iden, iden_sm, MAX_IDEN_LEN);
					new_cont._mmx_id	= mmx_id;
					new_cont._mo_user	= mo_user;
					new_cont._next_id	= 0;
					new_cont._pts_id	= pts_id;
					_cont_pool->add_mem(new_cont);
				}
				else
				{
					g_log.append(rpt::error, "[CAuditing::sync_cont]added cont failed(mobile:%lu, cont:%s).",
						mo_user, cont);
				}

				ret = cont_id;
			}
			else
			{
				//	更新作品库的内容与类别
				if (!_cont_pool->update_cont(sm->_cont_id, cat_id, cont))
				{
					g_log.append(rpt::error, 
						"[CAuditing::sync_cont]Update cont of cont_pool failed(cont_id:%lu, cat_id:%d, cont:%s).",
						sm->_cont_id, cat_id, cont);
				}

				ret = sm->_cont_id;
			}
		}
		else
		{
			g_log.append(rpt::error, "[CAuditing::sync_cont]iden not identify(iden:%s, cont:%s).",
				iden_sm, cont);
		}
	}
	else
	{
		g_log.append(rpt::error, "[CAuditing::sync_cont]cont too short(%s).", cont);
	}
	return ret;
}

bool CAuditing::update_cont_id(unsigned long	idx,
							   unsigned long	cont_id)
{
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_UPDATE_CONT_ID, _db_name, cont_id, idx);
	return _vms_db->Exec(sql) == TRUE;
}

bool CAuditing::give_pts(unsigned long mobile,
						 unsigned char pts_id)
{
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_INSERT_PTS_VOU, _db_name, mobile, pts_id);
	return _vms_db->Exec(sql) == TRUE;
}

bool CAuditing::check_pts_lmt(unsigned long mobile,
							  unsigned long pts_id)
{
	bool ret = false;
	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_CHECK_REG_PTS_LMT, _db_name, _db_name, mobile, pts_id);
	CMYSQLRS rs;
	if (_vms_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			if (rs.AsU32(0) > 0)
			{
				ret = true;
			}
			else
			{
				ret = false;
			}
		}
		rs.FreeRsl();
	}
	return ret;
}

//	截取部分内容返回给用户
static const int TRUNC_LEN	= 20;
unsigned long TruncCont(const char *content, 
						unsigned long len) 
{
	unsigned long ret = 0;
	const char* ptr = content;
	for (unsigned long i = 0; i < len; ++i)
	{
		unsigned char tmp = (unsigned char)*ptr;
		if (tmp <= 0x7f)
		{
			++ret;
			++ptr;
		}
		else
		{
			ret += 2;
			ptr += 2;
		}
		if (ret >= TRUNC_LEN)
		{
			break;
		}
	}
	return ret;
}

bool CAuditing::audit_notify(unsigned long	mobile,
							 unsigned long	cont_id,
							 const char		*cont)
{
	char esc_cont[MAX_SQL_LEN] = {0};
	_vms_db->EscStr(esc_cont, cont, (unsigned long)strlen(cont));

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_NOTIFY_AUDIT_RESULT, _db_name, mobile, cont_id, esc_cont);
	return _vms_db->Exec(sql) == TRUE;
}

bool CAuditing::audit_notify_green(unsigned long	mobile,
								   unsigned long	cont_id,
								   const char		*cont)
{
	char esc_cont[MAX_SQL_LEN] = {0};
	_vms_db->EscStr(esc_cont, cont, (unsigned long)strlen(cont));

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_NOTIFY_AUDIT_RESULT_GREEN, _db_name, mobile, cont_id, esc_cont);
	return _vms_db->Exec(sql) == TRUE;
}

void CAuditing::auditing(void)
{
	char job_sql[MAX_SQL_LEN] = {0};
	sprintf(job_sql, SQL_GET_JOB, _db_name, _db_name);
	CMYSQLRS job_rs;
	if (_vms_db->Exec(job_sql, &job_rs))
	{
		g_log.append(rpt::info, "[CAuditing]Pending jobs:%lu.", job_rs.QueryRows());
		while (job_rs.LoadRow())
		{
			unsigned long job_id = job_rs.AsU32(0);
			unsigned short jt_id = (unsigned short)job_rs.AsU32(1);
			unsigned short col_id= (unsigned short)job_rs.AsU32(2);
			const char *sp_name = job_rs.AsStr(3);

			//	获取本任务的内容
			unsigned long job_cont_cnt = 0;
			unsigned long sync_cnt = 0;

			char cont_sql[MAX_SQL_LEN] = {0};
			sprintf(cont_sql, SQL_GET_JOB_CONT, _db_name, job_id);
			CMYSQLRS cont_rs;
			if (_vms_db->Exec(cont_sql, &cont_rs))
			{
				job_cont_cnt = cont_rs.QueryRows();

				g_log.append(rpt::info, "[CAuditing]Pending job_cont:%lu.", job_cont_cnt);
				while (cont_rs.LoadRow())
				{
					unsigned long idx		= cont_rs.AsU32(0);
					unsigned short cat_id	= (unsigned short)cont_rs.AsU32(1);
					unsigned short mmx_id	= (unsigned short)cont_rs.AsU32(2);
					const char *mmx_till	= cont_rs.AsStr(3);
					unsigned long mo_user	= cont_rs.AsU32(4);
					const char *cont		= cont_rs.AsStr(5);

					//	绿色短信
					if (jt_id == JT_TYPE_GREEN)
					{
						//0--未分类，1--问题类别
						if (cat_id > 1)
						{
							unsigned long new_id = sync_cont(cat_id, mmx_id, mo_user, mmx_till, cont, CONT_TYPE_GREEN);
							if (new_id > 0)
							{
								if (!update_cont_id(idx, new_id))
								{
									g_log.append(rpt::error, "[CAuditing]Update cont_id failed.");
								}

								//	建立节点，下发内容
								if (mo_user > 0)
								{
									//	反馈审核通过通知
									unsigned long trunc_len = TruncCont(cont, (unsigned long)strlen(cont));
									char sm_part[TRUNC_LEN * 2 + 1] = {0};
									strncpy(sm_part, cont, trunc_len);
									
									if (strlen(_sys_par_ex->get_lssm()->_lssm_reg_accept) != 0)
									{
										char resp_cont[MAX_SQL_LEN] = {0};
										sprintf(resp_cont, _sys_par_ex->get_lssm()->_lssm_reg_accept, sm_part, new_id);
										if (!audit_notify_green(mo_user, new_id, resp_cont))
										{
											g_log.append(rpt::error, "[CAuditing]Notify audit_ok failed.");
										}
									}

									if (strlen(_sys_par_ex->get_lssm()->_lssm_reg_accept2) != 0)
									{
										if (!audit_notify_green(mo_user, new_id, _sys_par_ex->get_lssm()->_lssm_reg_accept2))
										{
											g_log.append(rpt::error, "[CAuditing]Notify audit_ok2 failed.");
										}
									}
								}				
							}
							else
							{
								g_log.append(rpt::error, "[CAuditing]Sync_cont failed(cat_id:%d, mobile:%lu, cont:%s).",
									cat_id, mo_user, cont);
							}
						}
						else
						{
							//	反馈审核未通过通知
							if (mo_user > 0)
							{
								unsigned long trunc_len = TruncCont(cont, (unsigned long)strlen(cont));
								char sm_part[TRUNC_LEN * 2 + 1] = {0};
								strncpy(sm_part, cont, trunc_len);

								char resp_cont[MAX_SQL_LEN] = {0};
								sprintf(resp_cont, _sys_par_ex->get_lssm()->_lssm_reg_reject, sm_part);
								if (!audit_notify_green(mo_user, 0, resp_cont))
								{
									g_log.append(rpt::error, "[CAuditing]Notify audit_ok failed.");
								}
							}
						}
					}
					else
					{
						//0--未分类，1--问题类别
						if (cat_id > 1)
						{
							unsigned long new_id = sync_cont(cat_id, mmx_id, mo_user, mmx_till, cont, CONT_TYPE_NORMAL);
							if (new_id > 0)
							{
								if (!update_cont_id(idx, new_id))
								{
									g_log.append(rpt::error, "[CAuditing]Update cont_id failed(idx:%lu, new_id:%lu).", idx, new_id);
								}

								//	赠送积分，建立节点，下发内容
								if (mo_user > 0)
								{
									//	赠送创作积分
									if (!give_pts(mo_user, _sys_par->get()->_pts_reg))
									{
										g_log.append(rpt::error, "[CAuditing]Give reg_pts failed(mobile:%lu, pts:%d).",
											mo_user, _sys_par->get()->_pts_reg);
									}

									//	反馈审核通过通知
									unsigned long trunc_len = TruncCont(cont, (unsigned long)strlen(cont));
									char sm_part[TRUNC_LEN * 2 + 1] = {0};
									strncpy(sm_part, cont, trunc_len);

									char resp_cont[MAX_SQL_LEN] = {0};
									sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_accept, sm_part);
									if (!audit_notify(mo_user, new_id, resp_cont))
									{
										g_log.append(rpt::error, "[CAuditing]Notify audit_ok failed(mobile:%lu, new_id:%lu, resp_cont:%s).",
											mo_user, new_id, resp_cont);
									}
								}				
							}
							else
							{
								g_log.append(rpt::error, "[CAuditing]Sync_cont failed(cat_id:%d, mobile:%lu, cont:%s).",
									cat_id, mo_user, cont);
							}
						}
						else
						{
							//	反馈审核未通过通知
							if (mo_user > 0)
							{
								unsigned long trunc_len = TruncCont(cont, (unsigned long)strlen(cont));
								char sm_part[TRUNC_LEN * 2 + 1] = {0};
								strncpy(sm_part, cont, trunc_len);

								char resp_cont[MAX_SQL_LEN] = {0};
								sprintf(resp_cont, _sys_par_ex->get_smgi()->_smgi_reg_reject, sm_part);
								if (!audit_notify(mo_user, 0, resp_cont))
								{
									g_log.append(rpt::error, "[CAuditing]Notify audit_ok failed(mobile:%lu, new_id:%lu, resp_cont:%s).",
										mo_user, 0, resp_cont);
								}
							}
						}
					}

					++sync_cnt;
				}
				cont_rs.FreeRsl();
			}

			//	设置任务识别完成
			if (job_cont_cnt == sync_cnt)
			{
				char iden_sql[MAX_SQL_LEN] = {0};
				sprintf(iden_sql, SQL_UPDATE_JOB_FINISHED, _db_name, job_id);
				if (!_vms_db->Exec(iden_sql))
				{
					g_log.append(rpt::error, "[CAuditing]Update iden_ok failed(job_id=%lu, err:%s).", 
						job_id, _vms_db->LastErr());
				}
				else
				{
					g_log.append(rpt::info, "[CAuditing]Finished jobs(job_id=%lu).", job_id);
				}

				//	执行最后的存储过程
				if (strlen(sp_name) != 0)
				{
					char sp_sql[MAX_SQL_LEN] = {0};
					sprintf(sp_sql, "CALL %s.%s(%lu, %lu, 0)", _db_name, sp_name, 
						job_id, jt_id);
					CMYSQLRS sp_rs;
					if (!_vms_db->Exec(sp_sql, &sp_rs))
					{
						g_log.append(rpt::error, "[CAuditing]CALL final_sp %s failed(%s).", sp_name, _vms_db->LastErr());
					}
					else
					{
						if (sp_rs.LoadRow())
						{
						}
						sp_rs.FreeRsl();

						g_log.append(rpt::info, "[CAuditing]CALL final_sp %s successful.", sp_name);
					}
				}
			}
			else
			{
				g_log.append(rpt::error, "[CAuditing]Sync not full(ttl_qty:%lu, sync_qty:%lu).", job_cont_cnt, sync_cnt);
			}
		}
		job_rs.FreeRsl();
	}
	else
	{
		g_log.append(rpt::error, "[CAuditing]Get Pending jobs failed(%s)", _vms_db->LastErr());
	}
}

CContPool::CContPool(CMYSQLDB		*vms_db,
					 const char		*vms_db_name,
					 unsigned char	mode)
	: _vms_db(vms_db)
{
	strncpy(_vms_db_name, vms_db_name, MAX_PATH);

	load(mode);
}

CContPool::~CContPool(void)
{
	unload();
}

bool CContPool::load(unsigned char mode)
{
	bool ret = true;
	unload();

	Process::CAutoLock lock(&_cs_cont);
    char sql[MAX_SQL_LEN] = {0};
	switch (mode)
	{
	case CONT_POOL_GET_MODE_TRIGGER:
		{
			sprintf(sql, SQL_GET_TRIG_CONT_POOL, _vms_db_name);
			CMYSQLRS rs;
			if (_vms_db->Exec(sql, &rs))
			{
				while (rs.LoadRow())
				{
					SContPool x;
					x._cont_id	= rs.AsU32(0);
					x._next_id	= rs.AsU32(1);
					x._mmx_id	= (unsigned short)rs.AsU32(2);
					x._cat_id	= (unsigned short)rs.AsU32(3);
					x._pts_id	= (unsigned char)rs.AsU32(4);
					x._mo_user	= rs.AsU32(5);
					strncpy(x._iden, rs.AsStr(6), MAX_IDEN_LEN);
		            
					_cont_pool.push_back(x);
					map_phs(x._iden);
				}
				rs.FreeRsl();
				sort();
			}
			else
			{
				ret = false;
			}
		}
		break;
	case CONT_POOL_GET_MODE_ALL:
	default:
		{
			sprintf(sql, SQL_GET_CONT_POOL, _vms_db_name);
			CMYSQLRS rs;
			if (_vms_db->Exec(sql, &rs))
			{
				while (rs.LoadRow())
				{
					SContPool x;
					x._cont_id	= rs.AsU32(0);
					x._next_id	= rs.AsU32(1);
					x._mmx_id	= (unsigned short)rs.AsU32(2);
					x._cat_id	= (unsigned short)rs.AsU32(3);
					x._pts_id	= (unsigned char)rs.AsU32(4);
					x._mo_user	= rs.AsU32(5);
					strncpy(x._iden, rs.AsStr(6), MAX_IDEN_LEN);
		            
					_cont_pool.push_back(x);
					map_phs(x._iden);
				}
				rs.FreeRsl();
				sort();
			}
			else
			{
				ret = false;
			}
		}
		break;
	}	

	return ret;
}

void CContPool::unload(void)
{
	_cs_cont.Lock();
	memset(phsmap, 0, sizeof(phsmap));
	_cont_pool.clear();
	_cs_cont.Unlock();
}

void CContPool::sort(void)
{
	unsigned long size = (unsigned long)_cont_pool.size();
	if (size > 0)
	{
		std::list<SContPool> lst;

		for (unsigned long i = 0; i < size; ++i)
			lst.push_back(_cont_pool[i]);
		lst.sort(compare_iden);

		_cont_pool.clear();
		for (unsigned long i = 0; i < size; ++i)
		{
			SContPool x = lst.front();
			lst.pop_front();

			_cont_pool.push_back(x);
		}
	}
}

void CContPool::add_mem(SContPool &sm)
{
	_cs_cont.Lock();
	_cont_pool.push_back(sm);
	map_phs((char*)sm._iden);
	sort();
	_cs_cont.Unlock();
}

unsigned long CContPool::new_cont(unsigned short	cat_id,
								  unsigned short	mmx_id,
								  unsigned char		pts_id,
								  unsigned long		mo_user,
								  const char		*mmx_till,
								  const char		*iden,
								  const char		*cont,
								  unsigned short	cont_type)
{
	char esc_cont[MAX_SQL_LEN] = {0};
	_vms_db->EscStr(esc_cont, cont, (unsigned long)strlen(cont));

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_INSERT_CONT_POOL, _vms_db_name, g_timer.Date(), cat_id, cont_type, mmx_id,
		mmx_till == NULL ? "1900-01-01" : mmx_till, pts_id, mo_user, esc_cont, iden == NULL ? "" : iden);
	if (_vms_db->Exec(sql))
	{
		return _vms_db->InsertId();
	}
	else
	{
		return 0;
	}
}

bool CContPool::update_cont(unsigned long	cont_id,
							unsigned short	cat_id,
							const char		*cont)
{
	char esc_cont[MAX_SQL_LEN] = {0};
	_vms_db->EscStr(esc_cont, cont, (unsigned long)strlen(cont));

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_UPDATE_CONT_POOL, _vms_db_name, cat_id, esc_cont, cont_id);
	return _vms_db->Exec(sql) == TRUE;
}

SContPool* CContPool::find(const char		*cont, 
						   unsigned long	len)
{
	if (cont == NULL)
	{
		return NULL;
	}

	char*	lpcs = const_cast<char*>(cont);
	char*	lpce = lpcs + len;

	BYTE	sm_maps[256] = {0};
	BYTE*	lpms = sm_maps;
	BYTE*	lpme = sm_maps;
	BYTE*	lpmc = NULL;
	BYTE*	lpme_canquit = NULL;
	BYTE	midx = 1;

	while (lpcs < lpce)
	{
		*(lpme++) = *(phsmap + *((WORD*)lpcs++));

		if (*lpcs)
		{
			++lpcs;
		}
	}
		
	lpme_canquit = lpme - 8;
	
	while (lpms < lpme)
	{
		if (lpmc)
		{
			if (*lpms & (midx <<=1))
			{
				if (midx == 0x80)
				{
					SContPool* ret = find(&cont[(lpmc - sm_maps) << 1]);
					if (ret != NULL)
					{
						return ret;
					}
					else
					{
						lpms = lpmc;
						lpmc = NULL;
						midx = 1;
					}
				}
			}
			else
			{
				lpms = lpmc;
				lpmc = NULL;
				midx = 1;
			}
		}
		else
		{
			if (*lpms & 1)
			{
                lpmc = lpms;
			}
			else 
			if (lpms >= lpme_canquit)
			{
				return NULL;
			}
		}
		++lpms;
	}
	
	return NULL;
}

SContPool* CContPool::find(const char *cont)
{
	if (cont == NULL)
	{
		return NULL;
	}
	
	Process::CAutoLock lock(&_cs_cont);
	long start = 0;
	long end = (long)_cont_pool.size() - 1;
	long idx = 0;
	while (start <= end)
	{
		idx = (start + end) / 2;
		int result = memcmp(cont, 
							_cont_pool[idx]._iden, 
							strlen((char*)_cont_pool[idx]._iden));
		if (0 == result)
		{
			return &_cont_pool[idx];
		}
		else
		if (result > 0)
		{
			start = idx + 1;
		}
		else
		{
			end = idx - 1;			
		}
	}

	return NULL;
}