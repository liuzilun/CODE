#include "cngp_packet.h"
#include "..\common\md5.h"	// 	MD5Encode

using namespace sms;
using namespace sms::cngp;

/*
 *	cngp��Ϣ����
 */
cngp_packet::cngp_packet(void)
{
}

cngp_packet::~cngp_packet(void)
{
}

void cngp::cngp_packet::encode_head(cngp_def::cngp_head &head)
{
	head.pkt_len	= htonl(head.pkt_len);
	head.req_id		= htonl(head.req_id);
	head.req_stat	= htonl(head.req_stat);
	head.seq_id		= htonl(head.seq_id);
}

void cngp::cngp_packet::decode_head(cngp_def::cngp_head & head)
{
	head.pkt_len	 = ntohl(head.pkt_len);
	head.req_id		 = ntohl(head.req_id);
	head.req_stat	 = ntohl(head.req_stat);
	head.seq_id		 = ntohl(head.seq_id);
}

void cngp::cngp_packet::encode_tlv(cngp_def::cngp_tlv &tlv)
{
	tlv.param_tag	= htons(tlv.param_tag);
	tlv.len			= htons(tlv.len);
}

/*
 *	��·������Ϣ
 */
cngp_active_test::cngp_active_test(void):
			_data(NULL)
{
	_data = new cngp_def::cngp_active_test;
	
	_data->head.req_id	= cngp_def::REQ_ID::ACTIVE_TEST;
	_data->head.pkt_len	= sizeof(cngp_def::cngp_active_test);
}

cngp_active_test::~cngp_active_test(void)
{
  	release();
}

void cngp::cngp_active_test::encode(void)
{

	encode_head(_data->head);

	return;
}

void cngp::cngp_active_test::decode(const char * buf)
{
	//  ��������
	memcpy(_data, buf, sizeof(cngp_def::cngp_active_test));

	//  ת���ֽ�˳��
	_data->head.pkt_len = ntohl(_data->head.pkt_len);
	_data->head.req_id  = ntohl(_data->head.req_id);

	return;
}

void cngp::cngp_active_test::release(void)
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
cngp_active_test_resp::cngp_active_test_resp(void):
									_data(NULL)		
{
	_data = new cngp_def::cngp_active_test_resp;

	_data->head.req_id	= cngp_def::REQ_ID::ACTIVE_TEST_RESP;
	_data->head.pkt_len	= sizeof(cngp_def::cngp_active_test_resp);
}

cngp_active_test_resp::~cngp_active_test_resp(void)
{
	release();
}

void cngp::cngp_active_test_resp::encode(void)
{
	encode_head(_data->head);
	return;
}

void cngp::cngp_active_test_resp::decode(const char *buf)
{
	memcpy(_data, buf, sizeof(cngp_def::cngp_active_test_resp));
	
	_data->head.pkt_len = ntohl(_data->head.pkt_len);
	_data->head.req_id  = ntohl(_data->head.req_id);

	return;
}

void cngp::cngp_active_test_resp::release(void)
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
cngp_exit::cngp_exit(void):_data(NULL)
{
	_data = new cngp_def::cngp_exit;

	_data->head.req_id	= cngp_def::REQ_ID::EXIT;
	_data->head.pkt_len	= sizeof(cngp_def::cngp_exit);
}

cngp_exit::~cngp_exit(void)
{
	release();
}

void cngp::cngp_exit::encode(void)
{
	encode_head(_data->head);
	return;
}

void cngp::cngp_exit::decode(const char *buf)
{
	memcpy(_data, buf, sizeof(cngp_def::cngp_exit));

	_data->head.pkt_len = ntohl(_data->head.pkt_len);
	_data->head.req_id  = ntohl(_data->head.req_id);

	return;
}

void cngp::cngp_exit::release(void)
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
cngp_exit_resp::cngp_exit_resp(void):_data(NULL)
{
	_data = new cngp_def::cngp_exit_resp;

	_data->head.req_id	= cngp_def::REQ_ID::EXIT_RESP;
	_data->head.pkt_len	= sizeof(cngp_def::cngp_exit_resp);

}

cngp_exit_resp::~cngp_exit_resp(void)
{
	release();
}

void cngp::cngp_exit_resp::encode(void)
{

	encode_head(_data->head);

	return;
}

void cngp::cngp_exit_resp::decode(const char *buf)
{
	memcpy(_data, buf, sizeof(cngp_def::cngp_exit_resp));

	_data->head.pkt_len = ntohl(_data->head.pkt_len);
	_data->head.req_id  = ntohl(_data->head.req_id);

	return;
}

void cngp::cngp_exit_resp::release(void)
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
cngp_login::cngp_login(void):_data(NULL)
{
	_data = new cngp_def::cngp_login;

	_data->head.req_id	= cngp_def::REQ_ID::LOGIN;
    _data->head.pkt_len	= sizeof(cngp_def::cngp_login);
}

cngp_login::~cngp_login(void)
{
	release();
}

void cngp::cngp_login::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

//  ����AuthenticatorClient��
UINT4 cngp::cngp_login::set_authenticator_client(void)
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

void cngp::cngp_login::encode(void)
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
cngp_login_resp::cngp_login_resp(void):_data(NULL)
{
	_data = new cngp_def::cngp_login_resp;
}

cngp_login_resp::~cngp_login_resp(void)
{
	release();
}

void cngp::cngp_login_resp::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void cngp::cngp_login_resp::decode(const char *buf)
{
	memcpy(_data, buf, sizeof(cngp_def::cngp_login_resp));
	
	decode_head(_data->head);

	return;
}

/*
 *	�ύ��Ϣ
 */
cngp_submit::cngp_submit(void):_data(NULL),
							   _enc_len(0)
{
	_data = new cngp_def::cngp_submit;

	_data->head.req_id = cngp_def::REQ_ID::SUBMIT;

	#ifdef _TLV
		_data->prot_id.param_tag = PROT_ID;
		_data->prot_id.len		 = 1;
		_data->prot_id.value	 = 0;
	#endif

}

cngp_submit::~cngp_submit(void)
{
	release();
}
void cngp::cngp_submit::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void cngp::cngp_submit::encode(void)
{

	#ifdef _TLV
		encode_tlv(_data->prot_id);

		memcpy(_data->msg_content + _data->msg_len, 
			&_data->prot_id, 
			sizeof(_data->prot_id));
	#endif

	//  ��������ĳ���
	_enc_len = sizeof(cngp_def::cngp_submit) 
				- sizeof(_data->msg_content) 
				+ _data->msg_len;

	_data->head.pkt_len = _enc_len;
	
	//	���ó���֮���ٱ���
	encode_head(_data->head);

	return;
}

/*
 *	�ύ��Ӧ��Ϣ
 */
cngp_submit_resp::cngp_submit_resp(void):_data(NULL),
										 _msg_id(0)
{
	_data = new cngp_def::cngp_submit_resp;
}

cngp_submit_resp::~cngp_submit_resp(void)
{
	release();
}

void cngp::cngp_submit_resp::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void cngp::cngp_submit_resp::decode(const char *buf)
{
	memcpy(_data, buf, sizeof(cngp_def::cngp_submit_resp));

	decode_head(_data->head);
  	
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
cngp_deliver::cngp_deliver(void):_data(NULL),
								 _msg_id(0)
{
	_data = new cngp_def::cngp_deliver;
}

cngp_deliver::~cngp_deliver(void)
{
	release();
}

void cngp::cngp_deliver::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void cngp::cngp_deliver::decode(const char *buf, UINT4 len)
{

	memcpy(_data, buf, len);
	
	decode_head(_data->head);

	_msg_id = bcd2int(_data->msg_id.seq_id, sizeof(_data->msg_id.seq_id));

	return;
}

/*
 *	���ͻ�Ӧ��Ϣ
 */
cngp_deliver_resp::cngp_deliver_resp(void):_data(NULL)
{
	_data = new cngp_def::cngp_deliver_resp;
    
	_data->head.req_id	= cngp_def::REQ_ID::DELIVER_RESP;
	_data->head.pkt_len = sizeof(cngp_def::cngp_deliver_resp);

	#ifdef _TLV
		_data->cong_stat.param_tag	= CONG_STAT;
		_data->cong_stat.len		= 1;
		_data->cong_stat.value		= 0;
	#endif

}

cngp_deliver_resp::~cngp_deliver_resp(void)
{
	release();
}

void cngp::cngp_deliver_resp::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

void cngp::cngp_deliver_resp::encode(void)
{
	encode_head(_data->head);

	#ifdef _TLV
		encode_tlv(_data->cong_stat);
	#endif	

	return;
}

/*
 *	״̬����
 */
cngp_report::cngp_report(void):_data(NULL),
							   _msg_id(0),	
							   _stat(0)
{
	_data = new cngp_def::cngp_report;

	memset(_err_code, 0, sizeof(_err_code));
}

cngp_report::~cngp_report(void)
{
	release();
}

void cngp::cngp_report::release(void)
{
	if (_data != NULL)
	{
		delete _data;
		_data = NULL;
	}
}

//  ״̬����
UINT4 cngp::cngp_report::get_stat_code(const char * stat)
{
	UINT4 i_stat = STAT_UNKNOW;
	if(strlen(stat) > 0)
	{
		if(strstr(STAT0, stat) != NULL)
		{
			i_stat = STAT0_VALUE;
		}
		else
		{
			if(strstr(STAT1, stat) != NULL)
			{
				i_stat = STAT1_VALUE;
			}
			else
			{
				if(strstr(STAT2, stat) != NULL)
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

void cngp::cngp_report::decode(const char * buf,UINT4 len)
{
	char tmp_err[MAX_ERR_CODE_LEN + 1] = {0};

	_msg_id = bcd2int(_data->msg_id.seq_id, sizeof(_data->msg_id.seq_id));

	memcpy(_data, buf, len);
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
cngp::cngp_head::cngp_head(void):_data(NULL)
{
	_data = new cngp_def::cngp_head;
}

cngp::cngp_head::~cngp_head(void)
{
    release();
}

void cngp::cngp_head::release(void)
{
	if (_data != NULL) 
	{
		delete _data;
		_data = NULL;
	}
}

void cngp::cngp_head::decode(const char *buf)
{	
	memcpy(_data, buf, sizeof(cngp_def::cngp_head));
	decode_head(*_data);
}

/*
 *	cngp ���кŲ�����
 */
cngp::cngp_seq::cngp_seq(UINT4 seed):_seed(seed)
{

}

cngp::cngp_seq::~cngp_seq(void)
{

}

void cngp::cngp_seq::set_seq(cngp_packet &cngp_pkt)
{
	m_lock.Lock();
	cngp_pkt.set_seq_id(_seed);
	_seed++;
	m_lock.Unlock();
}

void cngp::cngp_seq::set_seq(cngp_packet *cngp_pkt)
{
	m_lock.Lock();
	cngp_pkt->set_seq_id(_seed);
	_seed++;
	m_lock.Unlock();
}
