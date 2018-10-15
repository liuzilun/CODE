#include "smpp_packet.h"

using namespace sms;
using namespace sms::smpp;

PACKET_STATE smpp_protocol::decode(buffer &buff, smpp_head &head)
{
	if (buff.len < SMPP_LEN_HEAD)
	{
		return HALF_PACKET;
	}

	head.decode(buff.buf);
	if (!find_cmd(head.get_cmd_id()))
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

bool smpp_protocol::find_cmd(unsigned long cmd_id)
{
	bool ret = true;
	switch(cmd_id)
	{
	case SMPP_CMD_GENERIC_NACK:
	case SMPP_CMD_BIND_RECEIVER:
	case SMPP_CMD_BIND_RECEIVER_RESP:
	case SMPP_CMD_BIND_TRANSMITTER:
	case SMPP_CMD_BIND_TRANSMITTER_RESP:
	case SMPP_CMD_UNBIND:
	case SMPP_CMD_UNBIND_RESP:
	case SMPP_CMD_QUERY_SM:
	case SMPP_CMD_QUERY_SM_RESP:
	case SMPP_CMD_SUBMIT:
	case SMPP_CMD_SUBMIT_RESP:
	case SMPP_CMD_DELIVER:
	case SMPP_CMD_DELIVER_RESP:
	case SMPP_CMD_REPLACE_SM:
	case SMPP_CMD_REPLACE_SM_RESP:
	case SMPP_CMD_CANCEL_SM:
	case SMPP_CMD_CANCEL_SM_RESP:
	case SMPP_CMD_BIND_TRANSCEIVER:
	case SMPP_CMD_BIND_TRANSCEIVER_RESP:
	case SMPP_CMD_OUTBIND:
	case SMPP_CMD_ENQUIRE_LINK:
	case SMPP_CMD_ENQUIRE_LINK_RESP:
	case SMPP_CMD_SUBMIT_MILTI:
	case SMPP_CMD_SUBMIT_MILTI_RESP:
	case SMPP_CMD_QUERY_LAST_MSGS:
	case SMPP_CMD_QUERY_LAST_MSGS_RESP:
	case SMPP_CMD_QUERY_MSG_DETAILS:
	case SMPP_CMD_QUERY_MSG_DETAILS_RESP:	
	case SMPP_CMD_ALERT_NOTIFICATION:
	case SMPP_CMD_DATA_SM:
	case SMPP_CMD_DATA_SM_RESP:
		ret = true;
		break;
	default:
		ret = false;
		break;
	}
	return ret;
}


