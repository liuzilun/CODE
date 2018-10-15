#include "cmdresp.h"

CCmdResp::CCmdResp(CMYSQLDB	*db)
	: _db(db)
{
	init_sys_cmd();
	init_reg_cmd();
}

CCmdResp::~CCmdResp(void)
{
}

static const char *SQL_CMD_RESP_SYS
	= "SELECT push_idx, IF(alt_last >= CURDATE(), alt_cont, rep_cont) FROM nxp_mbr.cmd_rep WHERE cmd_id=0";

bool CCmdResp::init_sys_cmd(void)
{
	Process::CAutoLock lock(&_cs);
	CMYSQLRS rs;
	if (_db->Exec(SQL_CMD_RESP_SYS, &rs))
	{
		while (rs.LoadRow())
		{
			unsigned long push_idx = rs.AsU32(0);
			switch(push_idx)
			{
			case CMD_RESP_SYS_HELP:
				{
					strncpy(_sys_resp._help_resp_cont, rs.AsStr(1), 1024);
				}
				break;
			case CMD_RESP_SYS_NULL:
				{
					strncpy(_sys_resp._null_resp_cont, rs.AsStr(1), 1024);
				}
				break;
			case CMD_RESP_SYS_BUSY:
				{
					strncpy(_sys_resp._busy_resp_cont, rs.AsStr(1), 1024);
				}
				break;
			case CMD_RESP_SYS_FAIL:
				{
					strncpy(_sys_resp._fail_resp_cont, rs.AsStr(1), 1024);
				}
				break;
			case CMD_RESP_SYS_AUTO:
				{
					strncpy(_sys_resp._auto_resp_cont, rs.AsStr(1), 1024);
				}
				break;
			default:
				{
				}
				break;
			}
		}
		rs.FreeRsl();

		return true;
	}
	else
	{
		return false;
	}
}


bool CCmdResp::init_reg_cmd(void)
{
	return true;
}
