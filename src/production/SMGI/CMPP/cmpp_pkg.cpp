#include "cmpp_pkg.h"
#include "..\Common\MD5.h"

using namespace sms;
using namespace sms::cmpp;

UINT4 cmpp_pkg::encode_cmpp_head(char *buf)
{

	UINT4 ttl_len	= htonl(_ttl_len);
	UINT4 cmd_id	= htonl(_cmd_id);
	UINT4 seq_id	= htonl(_seq_id);

	UINT4 len = 0;
	memcpy(buf + len, &ttl_len, sizeof(ttl_len));	
	len += sizeof(ttl_len);

	memcpy(buf + len, &cmd_id, sizeof(cmd_id));
	len += sizeof(cmd_id);

	memcpy(buf + len, &seq_id, sizeof(seq_id));
	len += sizeof(seq_id);

	return len;
}

UINT4 cmpp_pkg::decode_cmpp_head(const char *buf)
{
	UINT4 len = 0;

	memcpy(&_ttl_len, buf + len, sizeof(_ttl_len));
	len += sizeof(_ttl_len);
	_ttl_len = ntohl(_ttl_len);

	memcpy(&_cmd_id, buf + len, sizeof(_cmd_id));
	len += sizeof(_cmd_id);
	_cmd_id = ntohl(_cmd_id);

	memcpy(&_seq_id, buf + len, sizeof(_seq_id));
    len += sizeof(_seq_id);
	_seq_id = ntohl(_seq_id);

	return len;
}

UINT8 cmpp_pkg::hton64(UINT8 host64)
{
	return (((host64 & UINT8(0xff00000000000000)) >> 56) |
		((host64 & UINT8(0x00ff000000000000)) >> 40) |
		((host64 & UINT8(0x0000ff0000000000)) >> 24) |
		((host64 & UINT8(0x000000ff00000000)) >>  8) |
		((host64 & UINT8(0x00000000ff000000)) <<  8) |
		((host64 & UINT8(0x0000000000ff0000)) << 24) |
		((host64 & UINT8(0x000000000000ff00)) << 40) |
		((host64 & UINT8(0x00000000000000ff)) << 56));
}

UINT8 cmpp_pkg::ntoh64(UINT8 net64)
{
	return hton64(net64);
}

//类 cmpp_active_test
cmpp_active_test::cmpp_active_test(void)
{
	_cmd_id		= MSG_TYPE::CMPP_ACTIVE_TEST;
	_ttl_len	= CMPP_HEAD_LEN;
}

UINT4 cmpp_active_test::encode(void)
{
	_buf = new char[CMPP_ACTIVE_TEST_LEN];
	return _enc_len = encode_cmpp_head(_buf);
}

UINT4 cmpp_active_test::decode(const char *buf)
{
	return decode_cmpp_head(buf);
}

//类 cmpp_connect
cmpp_connect::cmpp_connect(void)
{
	_cmd_id = MSG_TYPE::CMPP_CONNECT;
}

cmpp_connect::~cmpp_connect(void)
{
}

void cmpp_connect::set_source_addr(const char *source_addr)
{
	ZeroMemory(_source_addr, sizeof(_source_addr));
	memcpy(_source_addr, source_addr,
		min(strlen(source_addr), sizeof(_source_addr)));
}

void cmpp_connect::set_version(UINT1 version)
{
	_version = version;
}

void cmpp_connect::set_time_stamp(UINT4 time_stamp)
{
	_time_stamp = time_stamp;
}


void cmpp_connect::set_shared_secret(const char *shared_secret)
{
	ZeroMemory(_shared_secret, sizeof(_shared_secret));
	memcpy(_shared_secret, shared_secret,
		min(strlen(shared_secret), sizeof(_shared_secret)));
}

UINT4 cmpp_connect::encode(void)
{
	_buf = new char[CMPP_CONNECT_LEN];
	memset(_buf, 0, CMPP_CONNECT_LEN);

	char *buf = _buf;
	UINT4 tmp_len;
	UINT4 len = 0;
	UINT4 time_stamp = 0;
	char tmp_str[40] = {0};
	char tmp_time_stamp[11] = {0};
	
	//构造消息头
	len = CMPP_HEAD_LEN;
	
	//构造消息体
	//source_addr
	memcpy(buf + len, _source_addr, sizeof(_source_addr));
	len += sizeof(_source_addr);

	//用于鉴别源地址。其值通过单向MD5 hash计算得出，表示如下：
	//	AuthenticatorSource =
	//	MD5（Source_Addr+9 字节的0 +shared secret+timestamp）
	//先构造加密前串

    tmp_len = 0;
	memcpy(tmp_str + tmp_len, _source_addr, sizeof(_source_addr));
	tmp_len += sizeof(_source_addr);

	memset(tmp_str + tmp_len, 0, 9);
	tmp_len += 9;

	memcpy(tmp_str + tmp_len, _shared_secret, strlen(_shared_secret));
	tmp_len += (unsigned long)strlen(_shared_secret);

	sprintf(tmp_time_stamp, "%010u", _time_stamp);  
	memcpy(tmp_str + tmp_len, tmp_time_stamp, 10);
	tmp_len += 10;

	//authenticator_source
	MD5Encode(buf + len, tmp_str, tmp_len);
	len += 16;

    //verison
	memcpy(buf + len ,&_version, sizeof(_version));
	len += sizeof(_version);
    
	//time_stamp
    time_stamp = htonl(_time_stamp);
	memcpy(buf + len, &time_stamp, sizeof(time_stamp));
	len += sizeof(time_stamp);
	
	//设置长度
	set_ttl_len(len);
	//构造消息头

	encode_cmpp_head(buf);

	return _enc_len = len;;
}

//类 cmpp_connect_resp
cmpp_connect_resp::cmpp_connect_resp(void)
{

}
cmpp_connect_resp::~cmpp_connect_resp(void)
{
}

UINT4 cmpp_connect_resp::get_status(void)
{
	return _status;
}

UINT1 cmpp_connect_resp::get_version(void)
{
	return _version;
}

UINT4 cmpp_connect_resp::decode(const char *buf)
{
	int len = 0;

	//解消息头
	decode_cmpp_head(buf);
	len = cmpp_pkg::CMPP_HEAD_LEN;

	//状态
	memcpy(&_status, buf+len, sizeof(_status));
#ifndef _CMPP2_
	_status = ntohl(_status);
#endif
	
	len += sizeof(_status);
	len += sizeof(_authenticator_ISMG);

	//最高版本
	memcpy(&_version, buf+len, sizeof(_version));
	len += sizeof(_version);

	return len;
}

// 类 cmpp_submit
void cmpp_submit::set_msg_id(UINT8 msg_id)
{
    _msg_id = hton64(msg_id);
}

void cmpp_submit::set_pk_total(UINT1 pk_total)
{
	_pk_total = pk_total;
}

void cmpp_submit::set_pk_number(UINT4 pk_number)
{
	_pk_number = (UINT1)pk_number;
}

void cmpp_submit::set_registered_delivery(UINT1 registered_delivery)
{
	_registered_delivery = registered_delivery;
}

void cmpp_submit::set_msg_level(UINT1 msg_level)
{
	_msg_level = msg_level;
}

void cmpp_submit::set_service_id(const char *service_id)
{
	ZeroMemory(&_service_id, sizeof(_service_id));
	memcpy(_service_id, service_id,
		min(strlen(service_id),sizeof(_service_id)));
}

void cmpp_submit::set_fee_user_type(UINT1 fee_user_type)
{
    _fee_user_type = fee_user_type;
}

void cmpp_submit::set_fee_terminal_id(const char *fee_terminal_id,
									  UINT1 fee_terminal_type)
{
	_fee_terminal_type = fee_terminal_type;
	ZeroMemory(_fee_terminal_id, sizeof(_fee_terminal_id));
	if(_fee_terminal_type == TERMINAL_TYPE::TRUE_NUMBER)
	{
		memcpy(_fee_terminal_id, fee_terminal_id,
			min(strlen(fee_terminal_id), sizeof(_fee_terminal_id)));
	}
	else
	{
		memcpy(_fee_terminal_id, fee_terminal_id, sizeof(_fee_terminal_id));
	}
}

void cmpp_submit::set_TP_pId(UINT1 TP_pId)
{
	_TP_pId = TP_pId;
}

void cmpp_submit::set_TP_udhi(UINT1 TP_udhi)
{
	_TP_udhi = TP_udhi;
}

void cmpp_submit::set_msg_src(const char *msg_src)
{
	ZeroMemory(_msg_src, sizeof(_msg_src));
	memcpy(_msg_src, msg_src,
		min(strlen(msg_src), sizeof(_msg_src)));
}

void cmpp_submit::set_fee_type(const char *fee_type)
{
	ZeroMemory(_fee_type, sizeof(_fee_type));
	memcpy(_fee_type, fee_type, 
		min(strlen(fee_type), sizeof(_fee_type)));
}

void cmpp_submit::set_fee_code(const char * fee_code)
{
	ZeroMemory(_fee_code, sizeof(_fee_code));
	memcpy(_fee_code, fee_code,
		min(strlen(fee_code), sizeof(_fee_code)));
}

void cmpp_submit::set_valid_time(const char *valid_time)
{
	ZeroMemory(_valid_time, sizeof(_valid_time));
	memcpy(_valid_time, valid_time,
		min(strlen(valid_time), sizeof(_valid_time)));
}

void cmpp_submit::set_at_time(const char *at_time)
{
	ZeroMemory(_at_time, sizeof(_at_time));
	memcpy(_at_time, at_time,
		min(strlen(at_time), sizeof(_at_time)));
}

void cmpp_submit::set_src_id(const char *src_id)
{
	ZeroMemory(_src_id, sizeof(_src_id));
	memcpy(_src_id, src_id,
		min(strlen(src_id), sizeof(_src_id)));
}

void cmpp_submit::set_dest_user_tl(UINT1 dest_user_tl)
{
	_dest_user_tl = dest_user_tl;
}

void cmpp_submit::set_dest_terminal_id(const char *dest_terminal_id, 
									   UINT1 dest_terminal_type)
{
	_dest_terminal_type = dest_terminal_type;
	ZeroMemory(_dest_terminal_id, sizeof(_dest_terminal_id));
	if(_dest_terminal_type == TERMINAL_TYPE::TRUE_NUMBER)
	{
		memcpy(_dest_terminal_id, dest_terminal_id,
			min(strlen(dest_terminal_id), sizeof(_dest_terminal_id)));
	}
	else
	{
		memcpy(_dest_terminal_id, dest_terminal_id, 
			sizeof(_dest_terminal_id));
	}
}

void cmpp_submit::set_msg_length(UINT1 msg_length)
{
	_msg_length = msg_length;
}

void cmpp_submit::set_msg_content(const char *msg_content, UINT1 msg_fmt)
{
	int len;
	ZeroMemory(_msg_content, sizeof(_msg_content));
	if(msg_fmt == MSG_FMT::ASCII)
	{
		len = (int)min(strlen(msg_content), MAX_CONTENT_LEN);
		memcpy(_msg_content, msg_content, len);
		
	}
	else
	{
		len = (int)min(strlen(msg_content), MAX_CONTENT_LEN - 20);
		memcpy(_msg_content, msg_content, len);
	}
	_msg_fmt = msg_fmt;
	set_msg_length(len);
}

//处理含有'\0'的二进制数据,UCS2编码字符
void cmpp_submit::set_msg_content(const char *buf, int len ,UINT1 msg_fmt)
{
	ZeroMemory(_msg_content, sizeof(_msg_content));
	if(len > MAX_CONTENT_LEN - 20)
	{
		len = MAX_CONTENT_LEN - 20;
	}
	memcpy(_msg_content, buf, len);
	_msg_fmt = msg_fmt;
	set_msg_length(len);
}

void cmpp_submit::set_link_id(const char *link_id)
{
	ZeroMemory(_link_id, sizeof(_link_id));
	memcpy(_link_id, link_id,
		min(strlen(link_id), sizeof(_link_id)));
}

UINT4 cmpp_submit::encode(void)
{
	UINT4 len = 0;
	_buf = new char[CMPP_SUBMIT_LEN];
	char *buf = _buf;
	//跳过消息头
	len = CMPP_HEAD_LEN;
	//消息体
	
	//msg_id
    memcpy(buf + len, &_msg_id, sizeof(_msg_id));
	len += sizeof(_msg_id);
	
	//pk_total
	memcpy(buf + len, &_pk_total, sizeof(_pk_total));
	len += sizeof(_pk_total);
	
	//pk_number;
	memcpy(buf + len, &_pk_number, sizeof(_pk_number));
	len += sizeof(_pk_number);
	
	//registered_delivery
	memcpy(buf + len, &_registered_delivery, sizeof(_registered_delivery));
	len += sizeof(_registered_delivery);
	
	//msg_level
	memcpy(buf + len, &_msg_level, sizeof(_msg_level));
	len += sizeof(_msg_level);
	
	//service_id
    memcpy(buf + len, _service_id, sizeof(_service_id));
	len += sizeof(_service_id);
	
	//fee_user_type
	memcpy(buf + len, &_fee_user_type, sizeof(_fee_user_type));
	len += sizeof(_fee_user_type);
	
	//fee_terminal_id
	memcpy(buf + len, _fee_terminal_id, sizeof(_fee_terminal_id));
	len += sizeof(_fee_terminal_id);
	
#ifndef _CMPP2_
	//fee_terminal_type
	memcpy(buf + len, &_fee_terminal_type, sizeof(_fee_terminal_type));
	len += sizeof(_fee_terminal_type);
#endif    

	//tp_pid
	memcpy(buf + len, &_TP_pId, sizeof(_TP_pId));
	len += sizeof(_TP_pId);
	
	//tp_udhi
	memcpy(buf + len, &_TP_udhi, sizeof(_TP_udhi));
	len += sizeof(_TP_udhi);
	
	//msg_fmt
	memcpy(buf + len, &_msg_fmt, sizeof(_msg_fmt));
	len += sizeof(_msg_fmt);
	
	//msg_src
	memcpy(buf + len, _msg_src, sizeof(_msg_src));
	len += sizeof(_msg_src);
	
	//fee_type
	memcpy(buf + len, _fee_type, sizeof(_fee_type));
	len += sizeof(_fee_type);
	
	//fee_code
    memcpy(buf + len, _fee_code, sizeof(_fee_code));
	len += sizeof(_fee_code);
	
	//valid_time
	memcpy(buf + len, _valid_time, sizeof(_valid_time));
	len += sizeof(_valid_time);
	
	//at_time
	memcpy(buf + len, _at_time, sizeof(_at_time));
	len += sizeof(_at_time);
    
	//src_id
	memcpy(buf + len, _src_id, sizeof(_src_id));
	len += sizeof(_src_id);
	
	//dest_user_tl
	memcpy(buf + len, &_dest_user_tl, sizeof(_dest_user_tl));
	len += sizeof(_dest_user_tl);
	
	//dest_terminal_id
	memcpy(buf + len, _dest_terminal_id, sizeof(_dest_terminal_id));
	len += sizeof(_dest_terminal_id);
	
#ifndef _CMPP2_
	//dest_terminal_type
	memcpy(buf + len, &_dest_terminal_type, sizeof(_dest_terminal_type));
	len += sizeof(_dest_terminal_type);
#endif
	
	//msg_length
	memcpy(buf + len, &_msg_length, sizeof(_msg_length));
	len += sizeof(_msg_length);
	
	//msg_content
	memcpy(buf + len, &_msg_content, _msg_length);
	len += _msg_length;

	//link_id
#ifdef _CMPP2_
	memset(_link_id, 0, sizeof(_link_id));
#endif
    memcpy(buf + len, _link_id, sizeof(_link_id));
	len += sizeof(_link_id);
	//设置长度
	set_ttl_len(len);
	//构造消息头
	encode_cmpp_head(buf);

	return _enc_len = len;
}

UINT8 cmpp_submit_resp::get_msg_id(void)
{
	return _msg_id;
}

UINT4 cmpp_submit_resp::get_result(void)
{
	return _result;
}

UINT4 cmpp_submit_resp::decode(const char *buf)
{
	int len = 0;
	len = decode_cmpp_head(buf);

	//msg_id
	memcpy(&_msg_id, buf + len, sizeof(_msg_id));
	len += sizeof(_msg_id);
	_msg_id = ntoh64(_msg_id);

	//result
	memcpy(&_result, buf + len, sizeof(_result));
	len += sizeof(_result);
#ifndef _CMPP2_
	_result = ntohl(_result);
#endif	

	return len;
}

cmpp_active_test_resp::cmpp_active_test_resp(void)
{
	_cmd_id = MSG_TYPE::CMPP_ACTIVE_TEST_RESP;
}

UINT4 cmpp_active_test_resp::encode(void)
{
	_buf = new char[CMPP_ACTIVE_TEST_RESP_LEN];
	char *buf = _buf;
	int len = 0;

	//跳过消息头
	len = CMPP_HEAD_LEN;

	//消息体
	memcpy(&_reserve, buf + len, sizeof(_reserve));
	len += sizeof(_reserve);

	//设置长度
	set_ttl_len(len);
	//构造消息头
    encode_cmpp_head(buf);

	return _enc_len = len;
}

void cmpp_active_test_resp::set_reserve(UINT1 reserve)
{
	_reserve = reserve;
}

UINT4 cmpp_active_test_resp::decode(const char *buf)
{
	int len = 0;

	len = decode_cmpp_head(buf);
	memcpy(&_reserve, buf + len, sizeof(_reserve));
	len += sizeof(_reserve);

	return len;
}

UINT1 cmpp_active_test_resp::get_reserve(void)
{
	return _reserve;
}

UINT8 cmpp_deliver::get_msg_id(void)
{
	return _msg_id;
}

char* cmpp_deliver::get_dest_id(void)
{
	return (char*)_dest_id;
}

char* cmpp_deliver::get_service_id(void)
{
	return (char*)_service_id;
}

UINT1 cmpp_deliver::get_tp_pid(void)
{
	return _tp_pid;
}

UINT1 cmpp_deliver::get_tp_udhi(void)
{
	return _tp_udhi;
}

UINT1 cmpp_deliver::get_msg_fmt(void)
{
	return _msg_fmt;
}

UINT1 cmpp_deliver::get_registered_delivery(void)
{
	return _registered_delivery;
}

char* cmpp_deliver::get_src_terminal_id(void)
{
	return (char*)_src_terminal_id;
}

UINT1 cmpp_deliver::get_src_terminal_type(void)
{
	return _src_terminal_type;
}

UINT1 cmpp_deliver::get_msg_length(void)
{
	return _msg_length;
}

char* cmpp_deliver::get_msg_content(void)
{
	return (char*)_msg_content;
}

char * cmpp_deliver::get_link_id(void)
{
	return (char*)_link_id;
}

UINT4 cmpp_deliver::decode(const char* buf)
{
	UINT4 len = 0;

	//解消息头
	len = decode_cmpp_head(buf);

	//解消息体
	//msg_id
	memcpy(&_msg_id, buf + len, sizeof(_msg_id));
	len += sizeof(_msg_id);
	_msg_id = ntoh64(_msg_id);

	//dest_Id
	ZeroMemory(_dest_id, sizeof(_dest_id));
    memcpy(_dest_id, buf + len, sizeof(_dest_id) - 1);
	len += (sizeof(_dest_id) - 1);

	//service_id
	ZeroMemory(_service_id, sizeof(_service_id));
	memcpy(_service_id, buf + len, sizeof(_service_id) - 1);
	len += (sizeof(_service_id) - 1);

	//tp_pid
	memcpy(&_tp_pid, buf + len, sizeof(_tp_pid));
	len += sizeof(_tp_pid);

	//tp_udhi
	memcpy(&_tp_udhi, buf + len, sizeof(_tp_udhi));
	len += sizeof(_tp_udhi);
	
	//msg_fmt
	memcpy(&_msg_fmt, buf + len, sizeof(_msg_fmt));
	len += sizeof(_msg_fmt);

	//src_terminal_id
	ZeroMemory(_src_terminal_id,sizeof(_src_terminal_id));
	memcpy(_src_terminal_id, buf + len, sizeof(_src_terminal_id) - 1);
	len += (sizeof(_src_terminal_id) - 1);

#ifndef _CMPP2_
	//src_terminal_type
	memcpy(&_src_terminal_type ,buf + len ,sizeof(_src_terminal_type));
	len += sizeof(_src_terminal_type);
#endif

	//resgistered_delivery
	memcpy(&_registered_delivery, buf + len, sizeof(_registered_delivery));
	len  += sizeof(_registered_delivery);

	//msg_length
	memcpy(&_msg_length, buf + len, sizeof(_msg_length));
	len += sizeof(_msg_length);

	//msg_content
	ZeroMemory(_msg_content, sizeof(_msg_content));
	memcpy(_msg_content, buf + len,_msg_length);
	len += _msg_length;
	_msg_content[_msg_length] = 0;

	//link_id
	ZeroMemory(_link_id,sizeof(_link_id));
	memcpy(_link_id, buf + len, sizeof(_link_id) - 1);
	len += (sizeof(_link_id) - 1);

	return len;
}

void cmpp_deliver_resp::set_msg_id(UINT8 msg_id)
{
    //转成网络字节
	_msg_id = hton64(msg_id);
}

void cmpp_deliver_resp::set_result(UINT4 result)
{
    //	转成网络字节
#ifdef _CMPP2_
	_result = (unsigned char)result;
#else
	_result = htonl(result);
#endif
}

UINT4 cmpp_deliver_resp::encode(void)
{
	UINT4 len = 0;

	_buf = new char[CMPP_DELIVER_RESP_LEN];
	char *buf = _buf;
	//跳过消息头
	len = CMPP_HEAD_LEN;
	
	//msg_id
	memcpy(buf + len, &_msg_id, sizeof(_msg_id));
	len += sizeof(_msg_id);
	
	//result
    memcpy(buf + len, &_result, sizeof(_result));
	len += sizeof(_result);

	//设置长度
	set_ttl_len(len);
	//构造消息头
	encode_cmpp_head(buf);

	return _enc_len = len;
}

cmpp_deliver_resp::cmpp_deliver_resp(void)
{
	_cmd_id = MSG_TYPE::CMPP_DELIVER_RESP;
}

cmpp_submit::cmpp_submit(void)
{
	_cmd_id = MSG_TYPE::CMPP_SUBMIT;
}

cmpp_terminate::cmpp_terminate(void)
{
	_cmd_id = MSG_TYPE::CMPP_TERMINATE;
	_ttl_len = CMPP_HEAD_LEN;
}

UINT4 cmpp_terminate::encode(void)
{
	_buf = new char[CMPP_TERMINATE];
	return _enc_len = encode_cmpp_head(_buf);
}

UINT4 cmpp_terminate::decode(const char *buf)
{
	return decode_cmpp_head(buf);
}

cmpp_terminate_resp::cmpp_terminate_resp(void)
{
	_cmd_id = MSG_TYPE::CMPP_TERMINATE_RESP;
	_ttl_len = CMPP_HEAD_LEN;
}

UINT4 cmpp_terminate_resp::encode(void)
{
	_buf = new char[CMPP_TERMINATE_RESP_LEN];
	return _enc_len = encode_cmpp_head(_buf);
}

UINT4 cmpp_terminate_resp::decode(const char *buf)
{
	return decode_cmpp_head(buf);
}

UINT4 cmpp_deliver_report::decode(const char *buf)
{
	UINT4 len = 0;

	//msg_id
	memcpy(&_msg_id, buf + len, sizeof(_msg_id));
	len += sizeof(_msg_id);
	_msg_id = ntoh64(_msg_id);

	//stat
	ZeroMemory(_stat, sizeof(_stat));
	memcpy(_stat, buf + len, sizeof(_stat) - 1);
	len += (sizeof(_stat) - 1);
	
	//submit_time
	ZeroMemory(_submit_time, sizeof(_submit_time));
	memcpy(_submit_time, buf + len, sizeof(_submit_time) - 1);
	len += (sizeof(_submit_time) - 1);

	//done_time
	ZeroMemory(_done_time, sizeof(_done_time));
	memcpy(_done_time, buf + len, sizeof(_done_time) - 1);
	len += (sizeof(_submit_time) - 1);

	//dest_terminal_id
	ZeroMemory(_dest_terminal_id, sizeof(_dest_terminal_id));
	memcpy(_dest_terminal_id, buf + len, sizeof(_dest_terminal_id) - 1);
	len += (sizeof(_dest_terminal_id) - 1);

	//smsc_sequence
	memcpy(&_smsc_sequence, buf + len, sizeof(_smsc_sequence));
	len += sizeof(_smsc_sequence);
	_smsc_sequence = ntohl(_smsc_sequence);

	return len;
}

UINT8 cmpp_deliver_report::get_msg_id(void)
{
	return _msg_id;
}

UINT4 cmpp_deliver_report::get_smsc_sequence(void)
{
	return _smsc_sequence;
}

char* cmpp_deliver_report::get_dest_terminal_id(void)
{
	return _dest_terminal_id;
}

char* cmpp_deliver_report::get_submit_time(void)
{
	return _submit_time;
}

char* cmpp_deliver_report::get_stat(void)
{
	return _stat;
}

char* cmpp_deliver_report::get_done_time(void)
{
	return _done_time;
}

cmpp_query::cmpp_query(void)
{
	_cmd_id = MSG_TYPE::CMPP_QUERY;
}

void cmpp_query::set_query_type(UINT1 query_type)
{
	_query_type = query_type;
}

void cmpp_query::set_query_code(const char *query_code)
{
	ZeroMemory(_query_code, sizeof(_query_code));
	memcpy(_query_code, query_code,
		min(strlen(query_code), sizeof(_query_code)));
}

void cmpp_query::set_time(const char *time)
{
	ZeroMemory(_time, sizeof(_time));
	memcpy(_time, time, min(strlen(time), sizeof(_time)));
}

void cmpp_query::set_reserve(const char *reserve)
{
	ZeroMemory(_reserve ,sizeof(_reserve));
	memcpy(_reserve ,reserve,min(strlen(reserve), sizeof(_reserve)));
}

UINT4 cmpp_query::encode(void)
{
	UINT4 len = 0;
	_buf = new char[CMPP_QUERY_LEN];
	char *buf = _buf;

	//跳过信息头
	len = CMPP_HEAD_LEN;
	
	//time
	memcpy(buf + len, _time, sizeof(_time));
	len += sizeof(_time);

	//query_type
	memcpy(buf + len, &_query_type, sizeof(_query_type));
	len += sizeof(_query_type);

	//query_code
	memcpy(buf + len, _query_code, sizeof(_query_code));
	len += sizeof(_query_code);

	//reserve
	memcpy(buf + len, _reserve, sizeof(_reserve));
	len += sizeof(_reserve);

	//设置长度
	set_ttl_len(len);

	//构造消息头
    encode_cmpp_head(buf);

	return _enc_len = len;
}


char* cmpp_query_resp::get_time(void)
{
	return _time;
}

char* cmpp_query_resp::get_query_code(void)
{
	return _query_code;
}

UINT1 cmpp_query_resp::get_query_type(void)
{
	return _query_type;
}

UINT4 cmpp_query_resp::get_fl(void)
{
	return _fl;
}

UINT4 cmpp_query_resp::get_mo_fl(void)
{
	return _mo_fl;
}

UINT4 cmpp_query_resp::get_mo_scs(void)
{
	return _mo_scs;
}

UINT4 cmpp_query_resp::get_mo_wt(void)
{
	return _mo_wt;
}

UINT4 cmpp_query_resp::get_mt_tl_msg(void)
{
	return _mt_tl_msg;
}

UINT4 cmpp_query_resp::get_mt_tl_user(void)
{
	return _mt_tl_user;
}

UINT4 cmpp_query_resp::get_scs(void)
{
	return _scs;
}

UINT4 cmpp_query_resp::get_wt(void)
{
	return _wt;
}

UINT4 cmpp_query_resp::decode(const char * buf)
{
	UINT4 len = 0;

	//解消息头
	len = decode_cmpp_head(buf);

	//解消息体
	//	Time
	memcpy(_time, buf + len, sizeof(_time) - 1);
	len += (sizeof(_time) - 1);

	//Query_Type
	memcpy(&_query_type, buf + len, sizeof(_query_type));
	len += sizeof(_query_type);

    //Query_Code
	memcpy(_query_code, buf + len, sizeof(_query_code) - 1);
	len += (sizeof(_query_code) - 1);

	//MT_TLMsg
	memcpy(&_mt_tl_msg, buf + len, sizeof(_mt_tl_msg));
	len += sizeof(_mt_tl_msg);
	_mt_tl_msg = ntohl(_mt_tl_msg);

	//MT_Tlusr
	memcpy(&_mt_tl_user, buf + len, sizeof(_mt_tl_user));
	len += sizeof(_mt_tl_user);
	_mt_tl_user = ntohl(_mt_tl_user);

    //MT_Scs
	memcpy(&_scs, buf + len, sizeof(_scs));
	len += sizeof(_scs);
	_scs = ntohl(_scs);

	//MT_WT
	memcpy(&_wt, buf + len, sizeof(_wt));
	len += sizeof(_wt);
	_wt = ntohl(_wt);

	//MT_FL
	memcpy(&_fl, buf + len, sizeof(_fl));
	len += sizeof(_fl);
	_fl = ntohl(_fl);

	//MO_Scs
	memcpy(&_mo_scs, buf + len, sizeof(_mo_scs));
	len += sizeof(_mo_scs);
	_mo_scs = ntohl(_mo_scs);

	//MO_WT
	memcpy(&_mo_wt, buf + len, sizeof(_mo_wt));
	len += sizeof(_mo_wt);
	_mo_wt = ntohl(_mo_wt);

	//MO_FL    
	memcpy(&_mo_fl, buf + len, sizeof(_mo_fl));
	len += sizeof(_mo_fl);
	_mo_fl = ntohl(_mo_fl);

	return len;
}

cmpp_cancel::cmpp_cancel(void)
{
	_cmd_id = MSG_TYPE::CMPP_CANCEL;
}

UINT4 cmpp_cancel::encode(void)
{
	UINT4 len = 0;

	char * buf;
	_buf = new char[CMPP_CANCEL_LEN];
	buf = _buf;

	//跳过消息头
	len = CMPP_HEAD_LEN;

    //消息体
	memcpy(buf + len, &_msg_id, sizeof(_msg_id));
	len += sizeof(_msg_id);

	//设置长度
	set_ttl_len(len);

	//构造消息头
	encode_cmpp_head(buf);
	
	_enc_len = len;
	return _enc_len;
}

void cmpp_cancel::set_msg_id(UINT8 msg_id)
{
	_msg_id = hton64(msg_id);
}

UINT4 cmpp_cancel_resp::get_success_id(void)
{
	return _success_id;
}

UINT4 cmpp_cancel_resp::decode(const char *buf)
{
	UINT4 len = 0;

	//解消息头
	len = decode_cmpp_head(buf);
	//解消息体

	//result
	memcpy(&_success_id, buf + len ,sizeof(_success_id));
	len += sizeof(_success_id);
#ifndef _CMPP2_
	_success_id = htonl(_success_id);
#endif

	return len;
}

cmpp_seq::cmpp_seq(UINT4 seq_id)
	: _seq_id(seq_id)
{
}

cmpp_seq::~cmpp_seq(void)
{
}

void cmpp_seq::set_seq(cmpp_pkg &cmpp_package)
{
	cmpp_package.set_seq_id((unsigned long)::InterlockedIncrement((LPLONG)&_seq_id));
}

void cmpp::cmpp_seq::set_seq(cmpp_pkg *cmpp_package)
{
	cmpp_package->set_seq_id((unsigned long)::InterlockedIncrement((LPLONG)&_seq_id));
}
