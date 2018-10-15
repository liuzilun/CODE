#include "MoCmd.h"
#include "..\Lib\log.h"

using namespace RdiClubCommon;

CMoCmd::CMoCmd(CMYSQLDB	*db)
	: _db(db)
{
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
	CMYSQLRS rs;
	if (_db->Exec(SQL_GET_MOCMD, &rs))
	{
		while (rs.LoadRow())
		{
			SMoCmd cmd;
			cmd._cmd_id		= (unsigned short)rs.AsU32(0);
			cmd._cmd_type	= (unsigned char)rs.AsU32(1);
			cmd._cmd_port	= (unsigned char)rs.AsU32(2);
			
			strncpy(cmd._cmd_chr, rs.AsStr(3), MAX_SP_LEN);
			strncpy(cmd._cmd_ext, rs.AsStr(4), MAX_SP_LEN);

			cmd._loose_ext	= (unsigned char)rs.AsU32(5);
			cmd._loose_cmd	= (unsigned char)rs.AsU32(6);
			
			cmd._ord_type	= (unsigned char)rs.AsU32(7);
			strncpy(cmd._srv_type, rs.AsStr(8), MAX_SP_LEN);
			cmd._serv_id	= (unsigned short)rs.AsU32(9);
			cmd._fee_id		= (unsigned short)rs.AsU32(10);
			strncpy(cmd._cmd_sp, rs.AsStr(11), MAX_PATH);

			_vec_cmd.push_back(cmd);
		}
		rs.FreeRsl();
	}
	else
	{
		ret = false;
	}
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
					  const char	*cmd_chr,
					  const char	*cmd_ext)
{
	Process::CAutoLock lock(&_cs);
	if ((cmd_chr == NULL) || (cmd_ext == NULL))
	{
		return NULL;
	}

	//	指令精确,端口精确
	for (unsigned long i = 0; i < _vec_cmd.size(); ++i)
	{
		if (_vec_cmd[i]._cmd_port == port_id
			&& (_stricmp(cmd_chr, _vec_cmd[i]._cmd_chr) == 0)
			&& (_stricmp(cmd_ext, _vec_cmd[i]._cmd_ext) == 0)
			&& (_vec_cmd[i]._loose_cmd == MATCH_MODE_FULL)
			&& (_vec_cmd[i]._loose_ext == MATCH_MODE_FULL))
		{
			return &_vec_cmd[i];
		}
	}
	
	//	指令精确
	for (unsigned long i = 0; i < _vec_cmd.size(); ++i)
	{
		unsigned long len = (unsigned long)strlen(_vec_cmd[i]._cmd_ext);
		if (_vec_cmd[i]._cmd_port == port_id
			&& (_stricmp(cmd_chr, _vec_cmd[i]._cmd_chr) == 0)
			&& (_strnicmp(cmd_ext, _vec_cmd[i]._cmd_ext, len) == 0)
			&& (_vec_cmd[i]._loose_cmd == MATCH_MODE_FULL)
			&& (_vec_cmd[i]._loose_ext == MATCH_MODE_LIKE))
		{
			return &_vec_cmd[i];
		}
	}
	
	//	端口精确
	for (unsigned long i = 0; i < _vec_cmd.size(); ++i)
	{
		unsigned long len = (unsigned long)strlen(_vec_cmd[i]._cmd_chr);
		if (_vec_cmd[i]._cmd_port == port_id
			&& (_stricmp(cmd_ext, _vec_cmd[i]._cmd_ext) == 0)
			&& (_strnicmp(cmd_chr, _vec_cmd[i]._cmd_chr, len) == 0)
			&& (_vec_cmd[i]._loose_cmd == MATCH_MODE_LIKE)
			&& (_vec_cmd[i]._loose_ext == MATCH_MODE_FULL))
		{
			return &_vec_cmd[i];
		}
	}
    
	//	都不精确
	for (unsigned long i = 0; i < _vec_cmd.size(); ++i)
	{
		unsigned long cmd_len = (unsigned long)strlen(_vec_cmd[i]._cmd_chr);
		unsigned long sp_len = (unsigned long)strlen(_vec_cmd[i]._cmd_ext);
		if (_vec_cmd[i]._cmd_port == port_id
			&& (_strnicmp(cmd_ext, _vec_cmd[i]._cmd_ext, sp_len) == 0)
			&& (_strnicmp(cmd_chr, _vec_cmd[i]._cmd_chr, cmd_len) == 0)
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
		if ((_vec_cmd[i]._cmd_port == port_id)
			&& (_vec_cmd[i]._cmd_type == cmd_type))
		{
			return &_vec_cmd[i];
		}
	}
	return NULL;
}

bool CMoCmd::find(unsigned char	port_id,
				  unsigned char	ord_type,
				  const char	*srv_type,
				  SMoCmd		&cmd)
{
	bool ret = true;

	char sql[MAX_SQL_LEN] = {0};
	sprintf(sql, SQL_GET_THE_CMD, port_id, ord_type, srv_type);

	CMYSQLRS rs;
	if (_db->Exec(sql, &rs))
	{
		if (rs.LoadRow())
		{
			cmd._cmd_id		= (unsigned short)rs.AsU32(0);
			cmd._cmd_type	= (unsigned char)rs.AsU32(1);
			cmd._cmd_port	= (unsigned char)rs.AsU32(2);
			
			strncpy(cmd._cmd_chr, rs.AsStr(3), MAX_SP_LEN);
			strncpy(cmd._cmd_ext, rs.AsStr(4), MAX_SP_LEN);

			cmd._loose_ext	= (unsigned char)rs.AsU32(5);
			cmd._loose_cmd	= (unsigned char)rs.AsU32(6);
			
			cmd._ord_type	= (unsigned char)rs.AsU32(7);
			strncpy(cmd._srv_type, rs.AsStr(8), MAX_SP_LEN);
			cmd._serv_id	= (unsigned short)rs.AsU32(9);
			cmd._fee_id		= (unsigned short)rs.AsU32(10);
			strncpy(cmd._cmd_sp, rs.AsStr(11), MAX_PATH);
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