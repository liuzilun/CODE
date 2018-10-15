#include "SmsPool.h"
#include <list>
#include "..\Lib\FasParse.h"
#include "..\Lib\log.h"
#include "..\Lib\Timer.h"
#include "..\Lib\ClubCommon.h"

using namespace RdiClubCommon;

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

bool compare_iden(SSmsPool &item1, SSmsPool &item2)
{
	return (memcmp(item1._sms_wm, item2._sms_wm, MAX_IDEN_LEN) <= 0);
}

CSmsPool::CSmsPool(CMYSQLDB	*sms_db)
	: _sms_db(sms_db)
{
	do_load();
}

CSmsPool::~CSmsPool(void)
{
	do_unload();
}

/*
	加载短信作品库到内存
*/
bool CSmsPool::do_load(void)
{
	return do_load_sms() && do_load_key() && do_load_extra();
}

/*
	加载作品
*/
bool CSmsPool::do_load_sms(void)
{
	bool ret = true;
	do_unload();

	Process::CAutoLock lock(&_cs_sms);
    CMYSQLRS rs;
	if (_sms_db->Exec(SQL_GET_SMS_BASE, &rs))
	{
		while (rs.LoadRow())
		{
			SSmsPool x;
			x._sms_id	= rs.AsU32(0);
			x._reg_cid	= rs.AsI64(1);
			x._attr_map	= (unsigned short)rs.AsU32(2);
			x._rela_id	= (unsigned char)rs.AsU32(3);
			x._segs		= (unsigned char)rs.AsU32(4);
			strncpy(x._sms_wm, rs.AsStr(5), MAX_IDEN_LEN);
		    
			_sms_pool.push_back(x);
			map_phs(x._sms_wm);
		}
		rs.FreeRsl();
		
		//	按iden进行排序
		do_sort();
	}
	else
	{
		ret = false;
	}

	return ret;
}

/*
	加载关键词
*/
bool CSmsPool::do_load_key(void)
{
	bool ret = true;
	CMYSQLRS rs;
	if (_sms_db->Exec(SQL_GET_SYS_PAREX_KEY, &rs))
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

/*
	加载排除单词
*/
bool CSmsPool::do_load_extra(void)
{
	bool ret = true;
	CMYSQLRS rs;
	if (_sms_db->Exec(SQL_GET_SYS_PAREX_EXTRA, &rs))
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

/*
	短信作品库从内存中清理出去
*/
void CSmsPool::do_unload(void)
{
	_cs_sms.Lock();
	memset(phsmap, 0, sizeof(phsmap));
	_sms_pool.clear();
	_cs_sms.Unlock();
}

/*
	按iden识别码进行排序
*/
void CSmsPool::do_sort(void)
{
	unsigned long size = (unsigned long)_sms_pool.size();
	if (size > 0)
	{
		std::list<SSmsPool> lst;

		for (unsigned long i = 0; i < size; ++i)
			lst.push_back(_sms_pool[i]);
		lst.sort(compare_iden);

		_sms_pool.clear();
		for (unsigned long i = 0; i < size; ++i)
		{
			SSmsPool x = lst.front();
			lst.pop_front();

			_sms_pool.push_back(x);
		}
	}
}

/*
	内存同步
*/
void CSmsPool::do_add_mem(SSmsPool &sm)
{
	_cs_sms.Lock();
	_sms_pool.push_back(sm);
	map_phs((char*)sm._sms_wm);
	do_sort();
	_cs_sms.Unlock();
}

/*
		登记作品
	*/
bool CSmsPool::do_register(__int64		mobile,
						   const char	*cont)
{
	char esc_cont[MAX_SQL_LEN] = {0};
	unsigned long len = (unsigned long)strlen(cont);
	_sms_db->EscStr(esc_cont, cont, len);

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_INSERT_MMX_REG, mobile, esc_cont);
	return _sms_db->Exec(sql) == TRUE;	
}

/*
	检查作品是否存在
*/ 
SSmsPool* CSmsPool::do_find(const char		*cont, 
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
					SSmsPool* ret = do_find(&cont[(lpmc - sm_maps) << 1]);
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

/*
	内容识别
*/
SSmsPool* CSmsPool::do_find(const char *cont)
{
	if (cont == NULL)
	{
		return NULL;
	}
	
	Process::CAutoLock lock(&_cs_sms);
	long start = 0;
	long end = (long)_sms_pool.size() - 1;
	long idx = 0;
	while (start <= end)
	{
		idx = (start + end) / 2;
		int result = memcmp(cont, 
							_sms_pool[idx]._sms_wm, 
							strlen((char*)_sms_pool[idx]._sms_wm));
		if (0 == result)
		{
			return &_sms_pool[idx];
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