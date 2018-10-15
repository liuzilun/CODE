#include "smgp_packet.h"
#include "..\common\md5.h"	// 	MD5Encode

using namespace sms;
using namespace sms::smgp;

/*
 *	SMGP��Ϣ����
 */
smgp_packet::smgp_packet(void)
{
}

smgp_packet::~smgp_packet(void)
{
}

void smgp::smgp_packet::encode_head(smgp_def::smgp_head &head)
{
	head.pkt_len = htonl(head.pkt_len);
	head.req_id	 = htonl(head.req_id);
	head.seq_id	 = htonl(head.seq_id);
}

void smgp::smgp_packet::decode_head(smgp_def::smgp_head & head)
{
	head.pkt_len = ntohl(head.pkt_len);
	head.req_id	 = ntohl(head.req_id);
	head.seq_id	 = ntohl(head.seq_id);
}

/*
 *	��·������Ϣ
 */
smgp_active_test::smgp_active_test(void):
			_data(NULL)
{
	_data = new smgp_def::smgp_active_test;
	
	_data->head.req_id	= smgp_def::REQ_ID::ACTIVE_TEST;
	_data->head.pkt_len	= sizeof(smgp_def::smgp_active_test);
}

smgp_active_test::~smgp_active_test(void)
{
  	release();
}

void smgp::smgp_active_test::encode(void)
{

	encode_head(_data->head);

	return;
}

void smgp::smgp_active_test::decode(const char * buf)
{
	//  ��������
	memcpy(_data, buf, sizeof(smgp_def::smgp_active_test));

	//  ת���ֽ�˳��
	_data->head.pkt_len = ntohl(_data->head.pkt_len);
	_data->head.req_id  = ntohl(_data->head.req_id);

	return;
}

void smgp::smgp_active_test::release(void)
{
	if(_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

/*
 *	��·���Ի�Ӧ��Ϣ
 */
smgp_active_test_resp::smgp_active_test_resp(void):
									_data(NULL)		
{
	_data = new smgp_def::smgp_active_test_resp;

	_data->head.req_id	= smgp_def::REQ_ID::ACTIVE_TEST_RESP;
	_data->head.pkt_len	= sizeof(smgp_def::smgp_active_test_resp);
}

smgp_active_test_resp::~smgp_active_test_resp(void)
{
	release();
}

void smgp::smgp_active_test_resp::encode(void)
{
	encode_head(_data->head);
	return;
}

void smgp::smgp_active_test_resp::decode(const char *buf)
{
	memcpy(_data, buf, sizeof(smgp_def::smgp_active_test_resp));
	
	_data->head.pkt_len = ntohl(_data->head.pkt_len);
	_data->head.req_id  = ntohl(_data->head.req_id);

	return;
}

void smgp::smgp_active_test_resp::release(void)
{
	if(_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

/*
 *	�˳���Ϣ
 */
smgp_exit::smgp_exit(void):_data(NULL)
{
	_data = new smgp_def::smgp_exit;

	_data->head.req_id	= smgp_def::REQ_ID::EXIT;
	_data->head.pkt_len	= sizeof(smgp_def::smgp_exit);
}

smgp_exit::~smgp_exit(void)
{
	release();
}

void smgp::smgp_exit::encode(void)
{
	encode_head(_data->head);
	return;
}

void smgp::smgp_exit::decode(const char *buf)
{
	memcpy(_data, buf, sizeof(smgp_def::smgp_exit));

	_data->head.pkt_len = ntohl(_data->head.pkt_len);
	_data->head.req_id  = ntohl(_data->head.req_id);

	return;
}

void smgp::smgp_exit::release(void)
{
	if(_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

/*
 *	�˳���Ӧ��Ϣ
 */
smgp_exit_resp::smgp_exit_resp(void):_data(NULL)
{
	_data = new smgp_def::smgp_exit_resp;

	_data->head.req_id	= smgp_def::REQ_ID::EXIT_RESP;
	_data->head.pkt_len	= sizeof(smgp_def::smgp_exit_resp);

}

smgp_exit_resp::~smgp_exit_resp(void)
{
	release();
}

void smgp::smgp_exit_resp::encode(void)
{

	encode_head(_data->head);

	return;
}

void smgp::smgp_exit_resp::decode(const char *buf)
{
	memcpy(_data, buf, sizeof(smgp_def::smgp_exit_resp));

	_data->head.pkt_len = ntohl(_data->head.pkt_len);
	_data->head.req_id  = ntohl(_data->head.req_id);

	return;
}

void smgp::smgp_exit_resp::release(void)
{
	if(_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

/*
 *	    ��¼������Ϣ
 */
smgp_login::smgp_login(void):_data(NULL)
{
	_data = new smgp_def::smgp_login;

	_data->head.req_id	= smgp_def::REQ_ID::LOGIN;
    _data->head.pkt_len	= sizeof(smgp_def::smgp_login);
}

smgp_login::~smgp_login(void)
{
	release();
}

void smgp::smgp_login::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

//  ����AuthenticatorClient��
UINT4 smgp::smgp_login::set_authenticator_client(void)
{
	UINT4 len = 0;

	char time_stamp[MAX_TIME_STAMP_LEN+1] = {0};
	char md5_src[MAX_MD5_SRC_LEN] = {0};  

	memcpy(md5_src + len, _data->client_id, strlen(_data->client_id));
	len += (UINT4)strlen(_data->client_id);

	//  7���ֽڵ�0x00
	len += 7; 

	memcpy(md5_src + len, _shared_secret, strlen(_shared_secret));
	len += (UINT4)strlen(_shared_secret);

	sprintf(time_stamp, "%010u", _data->time_stamp);
	memcpy(md5_src + len, time_stamp, strlen(time_stamp));
	len += (UINT4)strlen(time_stamp);

	//  MD5����
	MD5Encode(_data->authenticator_client, md5_src, len);

	return 16;
}

void smgp::smgp_login::encode(void)
{
	
	//  ͷ��
	encode_head(_data->head);

	//  ������ܴ�
	set_authenticator_client();

	//  time_stamp
	_data->time_stamp = htonl(_data->time_stamp);

	return;
}

/*
 *		��¼��Ӧ��Ϣ
 */
smgp_login_resp::smgp_login_resp(void):_data(NULL)
{
	_data = new smgp_def::smgp_login_resp;
}

smgp_login_resp::~smgp_login_resp(void)
{
	release();
}

void smgp::smgp_login_resp::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void smgp::smgp_login_resp::decode(const char *buf)
{
	memcpy(_data, buf, sizeof(smgp_def::smgp_login_resp));
	
	decode_head(_data->head);

	_data->status = ntohl(_data->status);

	return;
}

/*
 *	   ��ѯ��Ϣ
 */
smgp_query::smgp_query(void):_data(NULL)
{
	_data = new smgp_def::smgp_query;

	_data->head.req_id	= smgp_def::REQ_ID::QUERY;
	_data->head.pkt_len = sizeof(smgp_def::smgp_query);
}

smgp_query::~smgp_query(void)
{
	release();
}


void smgp::smgp_query::encode(void)
{
	encode_head(_data->head);
	return;
}

void smgp::smgp_query::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

/*
 *	��ѯ��Ӧ��Ϣ
 */
smgp_query_resp::smgp_query_resp(void):_data(NULL)
{
	_data = new smgp_def::smgp_query_resp;
}

smgp_query_resp::~smgp_query_resp(void)
{
	release();
}

void smgp::smgp_query_resp::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void smgp::smgp_query_resp::decode(const char * buf)
{
	memcpy(_data, buf, sizeof(smgp_def::smgp_query_resp));
	//  ��ͷ
	decode_head(_data->head);
	//  ����
	_data->mo_fl		= ntohl(_data->mo_fl);
	_data->mo_scs		= ntohl(_data->mo_scs);
	_data->mo_wt		= ntohl(_data->mo_wt);
	_data->mt_fl		= ntohl(_data->mt_fl);
	_data->mt_scs		= ntohl(_data->mt_scs);
	_data->mt_tl_msg	= ntohl(_data->mt_tl_msg);
	_data->mt_tl_usr	= ntohl(_data->mt_tl_usr);
	_data->mt_wt		= ntohl(_data->mt_wt);

	return;
}

/*
 *	�ύ��Ϣ
 */
smgp_submit::smgp_submit(void):_data(NULL),
							   _enc_len(0)
{
	_data = new smgp_def::smgp_submit;

	_data->head.req_id = smgp_def::REQ_ID::SUBMIT;

#ifdef _SMGP3_
	_data->link_id_tag = htons(0x0003);
	_data->link_id_len = htons(20);

	_data->submit_msg_type_tag = htons(0x000B);
	_data->submit_msg_type_len = htons(1);
	_data->submit_msg_type = 0;
	
	_data->sp_deal_result_tag = htons(0x000C);
	_data->sp_deal_result_len = htons(1);
	_data->sp_deal_result = 0;
#endif
}

smgp_submit::~smgp_submit(void)
{
	release();
}
void smgp::smgp_submit::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void smgp::smgp_submit::encode(void)
{
	//  ��������ĳ���
	_enc_len = sizeof(smgp_def::smgp_submit) 
				- sizeof(_data->msg_content) 
				+ _data->msg_len;

	_data->head.pkt_len = _enc_len;

	encode_head(_data->head);

#ifdef _SMGP3_
	memcpy(_data->msg_content + _data->msg_len + MAX_RESERVE_LEN, &_data->link_id_tag, 34);
#endif

	return;
}

/*
 *	�ύ��Ӧ��Ϣ
 */
smgp_submit_resp::smgp_submit_resp(void):_data(NULL),
										 _msg_id(0)
{
	_data = new smgp_def::smgp_submit_resp;
}

smgp_submit_resp::~smgp_submit_resp(void)
{
	release();
}

void smgp::smgp_submit_resp::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void smgp::smgp_submit_resp::decode(const char *buf)
{
	memcpy(_data, buf, sizeof(smgp_def::smgp_submit_resp));

	decode_head(_data->head);
    
    _data->status = ntohl(_data->status);
	
	_msg_id = bcd2int(_data->msg_id.seq_id, sizeof(_data->msg_id.seq_id));

	return;
}

//  ��bcd��ת��һ������(static����)
UINT8 bcd2int(const unsigned char src_str[], UINT4 src_len)
{
	UINT8 result = 0, power = 0, tmp_result = 0,ui_base = 0;
	unsigned int i = 0, j= 0;
	unsigned char num = 0, base = 0;

	//  32λ������BCD�벻���ܳ���5λ
	for (i = 0; i< src_len; i++)
	{
		num = src_str[i];
		//  ����λ,
		base = num & 0xF0;
		base = base >> 4;
		//	����
		j++;
		power = (UINT8)pow(10, (2 * src_len - j));
		tmp_result = ((UINT8)base) * power;
		result += tmp_result;	

		//  ����λ
		base = num & 0x0F;
		//	����
		j++;
		power = (UINT8)pow(10, (2 * src_len - j));
		result += base * power;
	}
	return  result;
}

/*
 *	������Ϣ
 */
smgp_deliver::smgp_deliver(void):_data(NULL),
								 _msg_id(0)
{
	_data = new smgp_def::smgp_deliver;
}

smgp_deliver::~smgp_deliver(void)
{
	release();
}

void smgp::smgp_deliver::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void smgp::smgp_deliver::decode(const char *buf, UINT4 len)
{

	int tlv_len = 0;
	int done_len = 0;
	UINT2 field_len = 0;
	char err[100] = {0};

	UINT2 tlv_type =0;
	char *p = 0;

	memcpy(_data, buf, len);
	
	decode_head(_data->head);

	_msg_id = bcd2int(_data->msg_id.seq_id, sizeof(_data->msg_id.seq_id));

#ifdef _SMGP3_
	/*�ƶ�resve�ֶκ�����*/
	tlv_len = _data->head.pkt_len - (81 + _data->msg_length + MAX_RESERVE_LEN);
	p = _data->msg_content + _data->msg_length + MAX_RESERVE_LEN;


	while (done_len < tlv_len)
	{
/*
		sprintf(err, "%d", *p);
		OutputDebugString(err);
		p++;
		done_len++;
*/

		memcpy(&tlv_type, p, 2);
		tlv_type = htons(tlv_type);
		p = p + 2;
		done_len += 2;

		memcpy(&field_len, p, 2);
		field_len = htons(field_len);
		p = p + 2;
		done_len += 2;

		switch(tlv_type)
		{
		case 3:
			memcpy(_data->link_id, p, field_len);
			break;
		case 11:
			memcpy(&_data->submit_msg_type, p, field_len);
			break;
		case 12:
			memcpy(&_data->sp_deal_result, p, field_len);
			break;
		default:
			break;
		}
		p = p + field_len;
		done_len += field_len;

		sprintf(err, "tlv_tag = %d, field_len = %d, tlv_len = %d, done_len = %d", tlv_type, field_len, tlv_len, done_len);
		OutputDebugString(err);
		
	}
	//memcpy(_data->reserve + MAX_RESERVE_LEN, _data->msg_content + _data->msg_length + MAX_RESERVE_LEN, 24 + 5 + 5);
/*
	_data->link_id_tag = ntohs(_data->link_id_tag);
	_data->link_id_len = ntohs(_data->link_id_len);

	_data->submit_msg_type_tag = ntohs(_data->submit_msg_type_tag);
	_data->submit_msg_type_len = ntohs(_data->submit_msg_type_len);

	_data->sp_deal_result_tag = ntohs(_data->sp_deal_result_tag);
	_data->sp_deal_result_len = ntohs(_data->sp_deal_result_len);
*/
#endif

	return;
}

/*
 *	���ͻ�Ӧ��Ϣ
 */
smgp_deliver_resp::smgp_deliver_resp(void):_data(NULL)
{
	_data = new smgp_def::smgp_deliver_resp;
    
	_data->head.req_id	= smgp_def::REQ_ID::DELIVER_RESP;
	_data->head.pkt_len = sizeof(smgp_def::smgp_deliver_resp);
}

smgp_deliver_resp::~smgp_deliver_resp(void)
{
	release();
}

void smgp::smgp_deliver_resp::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void smgp::smgp_deliver_resp::encode(void)
{
	encode_head(_data->head);

	_data->status = htonl(_data->status);

	return;
}

/*
 *	״̬����
 */
smgp_report::smgp_report(void):_data(NULL),
							   _msg_id(0),	
							   _stat(0)
{
	_data = new smgp_def::smgp_report;

	memset(_err_code, 0, sizeof(_err_code));
}

smgp_report::~smgp_report(void)
{
	release();
}

void smgp::smgp_report::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

//  ״̬����
UINT4 smgp::smgp_report::get_stat_code(const char * stat)
{
	UINT4 i_stat = STAT_UNKNOW;
	if(strlen(stat) > 0)
	{
		if(strstr(stat, STAT0) != NULL)
		{
			i_stat = STAT0_VALUE;
		}
		else
		{
			if(strstr(stat, STAT1) != NULL)
			{
				i_stat = STAT1_VALUE;
			}
			else
			{
				if(strstr(stat, STAT2) != NULL)
				{
					i_stat = STAT2_VALUE;
				}
				else
				{
					i_stat = STAT_UNKNOW;
				}
			}
		}
	}
	return i_stat;
}

void smgp::smgp_report::decode(const char * buf,UINT4 len)
{
	char tmp_err[MAX_ERR_CODE_LEN + 1] = {0};

	memcpy(_data, buf, len);
	_msg_id = bcd2int(_data->msg_id.seq_id, sizeof(_data->msg_id.seq_id));

	//   �淶�ַ���
	memcpy(tmp_err, _data->stat, sizeof(_data->stat));
	//   ����
	_stat = get_stat_code(tmp_err);
	
	memcpy(_err_code, _data->err, sizeof(_data->err));

	return;
}

/*
 *	��Ϣͷ
 */
smgp::smgp_head::smgp_head(void):_data(NULL)
{
	_data = new smgp_def::smgp_head;
}

smgp::smgp_head::~smgp_head(void)
{
    release();
}

void smgp::smgp_head::release(void)
{
	if (_data != NULL) 
	{
		delete _data;
		_data = NULL;
	}
}

void smgp::smgp_head::decode(const char *buf)
{	
	memcpy(_data, buf, sizeof(smgp_def::smgp_head));
	decode_head(*_data);
}

/*
 *	smgp ���кŲ�����
 */
smgp::smgp_seq::smgp_seq(UINT4 seed):_seed(seed)
{

}

smgp::smgp_seq::~smgp_seq(void)
{

}

void smgp::smgp_seq::set_seq(smgp_packet &smgp_pkt)
{
	m_lock.Lock();
	smgp_pkt.set_seq_id(_seed);
	_seed++;
	m_lock.Unlock();
}

void smgp::smgp_seq::set_seq(smgp_packet *smgp_pkt)
{
	m_lock.Lock();
	smgp_pkt->set_seq_id(_seed);
	_seed++;
	m_lock.Unlock();
}
