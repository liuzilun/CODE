#include "MoCmd.h"
#include "..\Lib\log.h"

CMoCmd::CMoCmd(CMYSQLDBPool	*db_pool,
			   const char	*db_name)
	: _db_pool(db_pool)
{
	strncpy(_db_name, db_name, MAX_PATH);
	startup();
}

CMoCmd::~CMoCmd(void)
{
}

bool CMoCmd::startup(void)
{
	bool ret = true;
	cleanup();

	Process::CAutoLock lock(&_cs);
	SMYSQLDBLink *db = _db_pool->enter();

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_GET_MOCMD, _db_name);

	CMYSQLRS rs;
	if (db->_db->Exec(sql, &rs))
	{
		while (rs.LoadRow())
		{
			SMoCmd cmd;
			cmd._cmd_id		= (unsigned short)rs.AsU32(0);
			cmd._cmd_type	= (unsigned char)rs.AsU32(1);
			cmd._port_id	= (unsigned char)rs.AsU32(2);
			cmd._serv_id	= (unsigned short)rs.AsU32(3);

			strncpy(cmd._port_ext, rs.AsStr(4), MAX_SP_LEN);
			strncpy(cmd._cont_cmd, rs.AsStr(5), MAX_CMD_ID_LEN);

			cmd._loose_ext	= (unsigned char)rs.AsU32(6);
			cmd._loose_cmd	= (unsigned char)rs.AsU32(7);
			strncpy(cmd._sql_sp, rs.AsStr(8), MAX_PATH);
			_vec_cmd.push_back(cmd);
		}
		rs.FreeRsl();
	}
	else
	{
		ret = false;
	}

	_db_pool->leave(*db);
	return ret;
}

void CMoCmd::cleanup(void)
{
	_cs.Lock();
	_vec_cmd.clear();
	_cs.Unlock();
}

//	上行指令匹配
SMoCmd* CMoCmd::match(unsigned char port_id,
					  const char	*cont_cmd,
					  const char	*port_ext)
{
	Process::CAutoLock lock(&_cs);
	if ((cont_cmd == NULL) || (port_ext == NULL))
	{
		return NULL;
	}

	//	指令精确,端口精确
	for (unsigned long i = 0; i < _vec_cmd.size(); ++i)
	{
		if (_vec_cmd[i]._port_id = port_id
			&& (_stricmp(cont_cmd, _vec_cmd[i]._cont_cmd) == 0)
			&& (_stricmp(port_ext, _vec_cmd[i]._port_ext) == 0)
			&& (_vec_cmd[i]._loose_cmd == MATCH_MODE_FULL)
			&& (_vec_cmd[i]._loose_ext == MATCH_MODE_FULL))
		{
			return &_vec_cmd[i];
		}
	}
	
	//	指令精确
	for (unsigned long i = 0; i < _vec_cmd.size(); ++i)
	{
		unsigned long len = (unsigned long)strlen(_vec_cmd[i]._port_ext);
		if (_vec_cmd[i]._port_id = port_id
			&& (_stricmp(cont_cmd, _vec_cmd[i]._cont_cmd) == 0)
			&& (_strnicmp(port_ext, _vec_cmd[i]._port_ext, len) == 0)
			&& (_vec_cmd[i]._loose_cmd == MATCH_MODE_FULL)
			&& (_vec_cmd[i]._loose_ext == MATCH_MODE_LIKE))
		{
			return &_vec_cmd[i];
		}
	}
	
	//	端口精确
	for (unsigned long i = 0; i < _vec_cmd.size(); ++i)
	{
		unsigned long len = (unsigned long)strlen(_vec_cmd[i]._cont_cmd);
		if (_vec_cmd[i]._port_id = port_id
			&& (_stricmp(port_ext, _vec_cmd[i]._port_ext) == 0)
			&& (_strnicmp(cont_cmd, _vec_cmd[i]._cont_cmd, len) == 0)
			&& (_vec_cmd[i]._loose_cmd == MATCH_MODE_LIKE)
			&& (_vec_cmd[i]._loose_ext == MATCH_MODE_FULL))
		{
			return &_vec_cmd[i];
		}
	}
    
	//	都不精确
	for (unsigned long i = 0; i < _vec_cmd.size(); ++i)
	{
		unsigned long cmd_len = (unsigned long)strlen(_vec_cmd[i]._cont_cmd);
		unsigned long sp_len = (unsigned long)strlen(_vec_cmd[i]._port_ext);
		if (_vec_cmd[i]._port_id = port_id
			&& (_strnicmp(port_ext, _vec_cmd[i]._port_ext, sp_len) == 0)
			&& (_strnicmp(cont_cmd, _vec_cmd[i]._cont_cmd, cmd_len) == 0)
			&& (_vec_cmd[i]._loose_cmd == MATCH_MODE_LIKE)
			&& (_vec_cmd[i]._loose_ext == MATCH_MODE_LIKE))
		{
			return &_vec_cmd[i];
		}
	}

	return NULL;
}

SMoCmd* CMoCmd::find(unsigned char port_id,
					 unsigned char cmd_type)
{
	Process::CAutoLock lock(&_cs);
	for (unsigned long i = 0; i < _vec_cmd.size(); ++i)
	{
		//g_log.append(rpt::info, "port_id:%d, srv_type:%d, (IN)port_id:%d, cmd_type:%d", 
		//	_vec_cmd[i]._port_id, _vec_cmd[i]._cmd_type, port_id, cmd_type);

		if ((_vec_cmd[i]._port_id == port_id)
			&& (_vec_cmd[i]._cmd_type == cmd_type))
		{
			return &_vec_cmd[i];
		}
	}
	return NULL;
}

bool CMoCmd::find(unsigned char	port_id,
				  unsigned char	cmd_type,
				  SMoCmd		&cmd)
{
	bool ret = true;

	SMYSQLDBLink *db = _db_pool->enter();

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_GET_THE_CMD, _db_name, port_id, cmd_type);

	CMYSQLRS rs;
	if (db->_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			cmd._cmd_id		= (unsigned short)rs.AsU32(0);
			cmd._cmd_type	= (unsigned char)rs.AsU32(1);
			cmd._port_id	= (unsigned char)rs.AsU32(2);
			cmd._serv_id	= (unsigned short)rs.AsU32(3);

			strncpy(cmd._port_ext, rs.AsStr(4), MAX_SP_LEN);
			strncpy(cmd._cont_cmd, rs.AsStr(5), MAX_CMD_ID_LEN);

			cmd._loose_ext	= (unsigned char)rs.AsU32(6);
			cmd._loose_cmd	= (unsigned char)rs.AsU32(7);
			strncpy(cmd._sql_sp, rs.AsStr(8), MAX_PATH);
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

	_db_pool->leave(*db);
	return ret;
}