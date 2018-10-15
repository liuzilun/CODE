#include "sgip_packet.h"
#include <time.h>

using namespace sms;
using namespace sms::sgip;

PACKET_STATE sgip_protocol::decode(buffer &buff, sgip_head &head)
{
	if (buff.len < SGIP_LEN_HEAD)
	{
		return HALF_PACKET;
	}

	head.decode(buff.buf);
	if (!find_command(head.get_cmd_id()))
	{
		return INVALID_PACKET;
	}

	if (head.get_cmd_id() == 0)
	{
		return INVALID_PACKET;
	}

	if (buff.len < head.get_pkt_len())
	{
		return HALF_PACKET;
	}

	return WHOLE_PACKET;
}

bool sgip_protocol::find_command(unsigned long cmd_id)
{
	switch (cmd_id)
	{
    case SGIP_CMD_BIND:
	case SGIP_CMD_BIND_RESP:
	case SGIP_CMD_UNBIND:
	case SGIP_CMD_UNBIND_RESP:
	case SGIP_CMD_SUBMIT:
	case SGIP_CMD_SUBMIT_RESP:
	case SGIP_CMD_DELIVER:
	case SGIP_CMD_DELIVER_RESP:
	case SGIP_CMD_REPORT:
	case SGIP_CMD_REPORT_RESP:
	case SGIP_CMD_ADDSP:
	case SGIP_CMD_ADDSP_RESP:
	case SGIP_CMD_MODIFYSP:
	case SGIP_CMD_MODIFYSP_RESP:
	case SGIP_CMD_DELETESP:
	case SGIP_CMD_DELETESP_RESP:
	case SGIP_CMD_QUERYROUTE:
	case SGIP_CMD_QUERYROUTE_RESP:
	case SGIP_CMD_ADDTELESEG:
	case SGIP_CMD_ADDTELESEG_RESP:
	case SGIP_CMD_MODIFYTELESEG:
	case SGIP_CMD_MODIFYTELESEG_RESP:
	case SGIP_CMD_DELETETELESEG:
	case SGIP_CMD_DELETETELESEG_RESP:
	case SGIP_CMD_ADDSMG:
	case SGIP_CMD_ADDSMG_RESP:
	case SGIP_CMD_MODIFYSMG:
	case SGIP_CMD_MODIFYSMG_RESP:
	case SGIP_CMD_DELETESMG:
	case SGIP_CMD_DELETESMG_RESP:
	case SGIP_CMD_CHECKUSER:
	case SGIP_CMD_CHECKUSER_RESP:
	case SGIP_CMD_USERRPT:
	case SGIP_CMD_USERRPT_RESP:
	case SGIP_CMD_TRACE:
	case SGIP_CMD_TRACE_RESP:
	case SGIP_CMD_KEEPALIVE:
	case SGIP_CMD_KEEPALIVE_RESP:
		return true;
	default:
		return false;
	}
}
