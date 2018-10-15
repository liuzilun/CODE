#include "smgp.h"


using namespace sms;
using namespace sms::smgp;


CSmgp::CSmgp(void)
	: _high_index(0)
	, _low_first(0)
	, _low_count(0)
	, _low_index(0)
	, _client_idx(0)
	, _seq(NULL)

{
}

CSmgp::~CSmgp(void)
{
	Cleanup();
}

//	��ʼ��
//	���ò���,���������̵߳�
bool CSmgp::Startup(CONNECT_INFO &info)
{
	memcpy(&_conn_info, &info, sizeof(CONNECT_INFO));
	
	//	�������кŲ�����
	_seq = new smgp_seq(_conn_info._seq_id);

	//	��������
	CreateAllConns();
	
	//	ͳ�����Ӽ�����Ϣ
	GetConnLevel();

	return true;
}

/*
�������ӵĹ���
1.���ֻ�ܿ�һ������,��ô��,��,����,��������һ������
2.����ܿ���������
	һ��������(���н���)0��,һ��������ͨ
3.������Կ�3����3�����ϵ�����
	 ����0��,����1��,������ͨ����
*/
void CSmgp::CreateAllConns(void)
{
	int i = 0, created_count = 0;
	//	һ������
	if (_conn_info._links == 1)
	{
		CreateConn(0, CONN_LOW, smgp_def::LOGIN_MODE::MO_MT);
	}
	//	��������
	if (_conn_info._links == 2)
	{
		//	������������������
		switch (_conn_info._smg_type) 
		{
		//	SMGP��׼
		case SMGP_STD:	
			{
				//	����0
				CreateConn(0, CONN_HIGH, smgp_def::LOGIN_MODE::MO_MT);

				Sleep(200);

				//	��ͨ1
				CreateConn(1, CONN_LOW, smgp_def::LOGIN_MODE::MT);
			}
			break;
		default:
		#ifdef _DEBUG
			printf("�������ʹ�!\n");
		#endif
			break;
		}//switch
	}//if 2

	//3����������
	if (_conn_info._links >= 3)
	{
		created_count = 0;

		//	������������������
		switch (_conn_info._smg_type) 
		{
		case SMGP_STD:
			{
				//	����0
				CreateConn(0, CONN_LOW, smgp_def::LOGIN_MODE::MO_MT);
				created_count++;

				Sleep(200);

				//	����
				CreateConn(1, CONN_HIGH, smgp_def::LOGIN_MODE::MO_MT);
				created_count++;
			}
			break;
		default:
		#ifdef _DEBUG
			printf("�������ʹ�\n!");
		#endif			
			break;

		}// switch
		
		//	����������ͨ����
		for (i = created_count; i < _conn_info._links; i++)	
		{
			Sleep(200);
			CreateConn(i, CONN_LOW, smgp_def::LOGIN_MODE::MO_MT);
		}
	}// if 3
}

//������:
//	    CreateConn
//����:
//		����һ������
//����:
//    conn_id          ���ӱ�ʶ,��0��ʼ,����
//    conn_level	   ����  0 - ��ͨ  1 - ����
//	  conn_type 	   ����  0 - ���л�����, 1-����

void CSmgp::CreateConn(unsigned int conn_id,
					   unsigned int conn_level, 
					   unsigned int conn_type)
{
	CClient *client = NULL;

	_conn.id	= conn_id;
	_conn.level = conn_level;
	_conn.type  = conn_type;

	//	����Clientʵ��
	client = new CClient(this, _seq, &_conn_info, &_conn);		

	//	�����������
	_clients.push_back(client);
}

//����������Ϣ��ֵ(��,�����ȼ�)
void CSmgp::GetConnLevel(void)
{
	unsigned int i = 0;

	//	������
	_high_index = -1;
	for (i = 0; i< _clients.size(); i++)
	{
		if (_clients[i]->get_level() == CONN_HIGH 
			&& _clients[i]->get_type() != smgp_def::LOGIN_MODE::MO)
		{
			_high_index = i;
			break;  
		}
	}

	//	������
	_low_first = -1;
	_low_count = 0;

	for (i = 0; i< _clients.size(); i++)
	{
		if (_clients[i]->get_level() == CONN_LOW 
			&& _clients[i]->get_type() != smgp_def::LOGIN_MODE::MO)
		{
			if(_low_count == 0)
			{
				_low_first = i;	
			}
			_low_count++;
		}
	}

	//	���ó��η��͵������±�
	if (_low_first >= 0)
	{
		_low_index = _low_first;
	}
	else
	{
		_low_index = -1;
	}
}

//�ύ����
unsigned long CSmgp::Submit(SUBMIT_SM &submit, bool prio)
{
	unsigned long sys_seq = 0;
	unsigned long con_id = 0;
	char tmp[255];

/*
	if (!prio || (prio && (_high_index < 0)))
	{
		//	��������Ϣ
		if (_low_index >= 0)
		{
			//	�е�����
			sys_seq = _clients[_low_index]->submit(submit);
			con_id = _low_index;

			//_low_index��ֵ
			_low_index++;
			if ((unsigned long)_low_index == (_low_first + _low_count))
			{
				_low_index = _low_first;
			}
		}// end of _low_index >= 0
	}// end of p_submit->_prio == MSG_LOW
	else
	{
		//	��������Ϣ
		if (_high_index >= 0)
		{	
			//	�и���������
			sys_seq = _clients[_high_index]->submit(submit);
			con_id = _high_index;
		}
	}//end of p_submit->_prio == MSG_LOW else
*/

		_client_idx = 0;


	sys_seq = _clients[_client_idx]->submit(submit);
	sprintf(tmp, "con_id = %d, seq_id = %d", con_id, sys_seq);
	OutputDebugString(tmp);
	return sys_seq;
}

//  ȡ�õ�ǰseq
unsigned long CSmgp::GetSeqID(void) const
{
	if ( _seq != NULL )
	{
		return _seq->get_seq();
	}
	else
		return 0;
}

//	����
int CSmgp::Cleanup(void)
{
	//	�ͷű����������ж���
	for(unsigned long i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i] != NULL)
		{
			delete _clients[i];
			_clients[i] = NULL;
		}
	}
	_clients.clear();

	//	ɾ�����кŲ�����
	if (_seq != NULL)
	{
		delete _seq;
		_seq = NULL;
	}

	return 0;
}


CClient::CClient(CSmgp *p_smgp,
				 smgp_seq *p_seq,
				 CONNECT_INFO *conn_info,
				 CONNECTION *conn)
	: _smgp(p_smgp)
	, _sender(NULL)
	, _buffer(NULL)
	, _smgp_seq(p_seq)
	, _is_connected(false)
	, _conn_info(*conn_info)
	, _conn(*conn)
	, _last_time(0x7FFFFFFF)
	, _uc_coding(NULL)
{
	//	������ip
	memset(_svr_ip, 0, sizeof(_svr_ip));
	strncpy(_svr_ip, _conn_info._smg_ip, MAX_IP_LEN);

		//  �������
	_uc_coding = codeFactory::createEncoding("unicode");

	//	�������˿�
	if (_conn.type == 0)
	{	
		//	���л��ߺ�����
		_port = _conn_info._smg_port_mt;
	}
	else
	{
		//	����
		_port = _conn_info._smg_port_mo;
	}

	//	���ݻ���
	_buffer = new buffer_block();

	//	���Ʒ�����
	_sender = new CSender(this, false, _conn_info._slip_size, _conn_info._interval);

	//	��������
	Connect(_svr_ip, _port);

	BeginThread();

}
CClient::~CClient(void)
{
	EndThread();
	
	if (_sender != NULL)
	{
		delete _sender;
		_sender = NULL;
	}
	
	if (_buffer != NULL)
	{
		delete _buffer;
		_buffer = NULL;
	}

	if (_uc_coding != NULL)
	{
		delete _uc_coding;
		_uc_coding = NULL;
	}
}

//	��������
void CClient::connect_smg(void)
{
	smgp_login *login_pkg = new smgp_login;

	//	����smgp_login
	login_pkg->set_login_mode(_conn.type);
	login_pkg->set_client_id(_conn_info._login_name);
	login_pkg->set_shared_secret(_conn_info._login_pswd);
	login_pkg->set_tm_stmp(0);
	login_pkg->set_version(SMGP_VER);

	//	�������к�
	_smgp_seq->set_seq(login_pkg);

	//	����
	login_pkg->encode();

	//	����
	_sender->SendPacket(login_pkg);
}

/*
//	�ύ����
unsigned long CClient::submit(SUBMIT_SM &submit_pkg)
{
	//�ȼ����������״��,������Ӳ�������,�ύʧ��
	unsigned long sys_seq = 0;

	if (_is_connected)
	{
		//	��������
		smgp_submit *submit = new smgp_submit;

		//	���ø��ֶ�ֵ

		//	��Ϣ����,�ӿ��ݲ�֧��
		submit->set_msg_type(smgp_def::MSG_TYPE::ORDER_PROG);         
		
		submit->set_dest_term_id(submit_pkg._mobile);
		submit->set_dest_id_cnt(1);
		submit->set_src_term_id(submit_pkg._sp_num);
		submit->set_charge_term_id(submit_pkg._mobile);

		//	���·�����fix_fee
		if (atoi(submit_pkg._fee_type) == 2)
		{
			submit->set_fee_code("00");
			submit->set_fixed_fee(submit_pkg._fee_value);
		}
		else
		{
			submit->set_fee_code(submit_pkg._fee_value);
		}

		submit->set_fee_type(submit_pkg._fee_type);
		submit->set_msg_cont(submit_pkg._cont, 
							(unsigned char)strlen(submit_pkg._cont), 
			                 smgp_def::MSG_FMT::GB2312);
		submit->set_pri(submit_pkg._prio);
		submit->set_nd_rpt(submit_pkg._report);
		submit->set_svr_id(submit_pkg._srv_type);
		submit->set_at_time(submit_pkg._sched_time);
		submit->set_vld_time("");
		
		//	�ⶥ���,�ӿ��ݲ�֧��
		submit->set_fixed_fee("00");

		//	�������к�
		_smgp_seq->set_seq(submit);

		//	����
		submit->encode();

		//	��¼sys_seq
		sys_seq = submit->get_seq_id();

		//	����
		_sender->AddPacket(submit);
	}
	return sys_seq;
}
*/


//	�ύ����
unsigned long CClient::submit(SUBMIT_SM &submit_pkg)
{
	//�ȼ����������״��,������Ӳ�������,�ύʧ��
	unsigned long sys_seq = 0;
	char enc_buf[MAX_LONG_SM_LEN * 2 + 1] = { 0 };
	int enc_len = MAX_LONG_SM_LEN * 2;
	char err[1024];

	_uc_coding->encode(submit_pkg._cont, (int)strlen(submit_pkg._cont), enc_buf, &enc_len);

	//	����Ҫ���
	sys_seq = submit_norm(submit_pkg);

	if (!_is_connected)
	{
		sprintf(err, "link disconnect id = %d", _conn.id);
		OutputDebugString(err);
		Sleep(10);
		connect_smg();
	}
#ifdef _DEBUG
	if (sys_seq == 0)
	{
		sprintf(err, "mo:%s, sys_seq:%d\r\n", submit_pkg._mobile, sys_seq);
		OutputDebugStr(err);
		printf("%s", err);
	}
#endif
	return sys_seq;
}

// ��������
unsigned long CClient::submit_norm(SUBMIT_SM & submit_pkg)
{
	//�ȼ����������״��,������Ӳ�������,�ύʧ��
	unsigned long sys_seq = 0;

	//	��������
	smgp_submit *submit = new smgp_submit;

	//	���ø��ֶ�ֵ
	submit->set_dest_term_id(submit_pkg._mobile);
	submit->set_dest_id_cnt(1);
	submit->set_src_term_id(submit_pkg._sp_num);
	submit->set_charge_term_id(submit_pkg._mobile);


	//	���·�����fix_fee
	if (atoi(submit_pkg._fee_type) == 2)
	{
		submit->set_fee_code("00");
		submit->set_fixed_fee(submit_pkg._fee_value);
	}
	else
	{
		submit->set_fee_code(submit_pkg._fee_value);
	}

#ifdef _SMGP3_
	submit->set_msg_type(smgp_def::MSG_TYPE::QUERY_REQ);
	submit->set_link_id(submit_pkg._link_id);
	if (strlen(submit_pkg._cont) > 5)
	{
		if ((strncmp(submit_pkg._cont, "QX ", 3) == 0) 
			||(strncmp(submit_pkg._cont, "DG ", 3) == 0) 
			||(strncmp(submit_pkg._cont, "00000", 5) == 0))
		{
			submit->set_submit_msg_type(15);
		}
	}
#else
	//	��Ϣ����,�ӿ��ݲ�֧��
	if (atoi(submit_pkg._fee_type) == 2 && atoi(submit_pkg._fee_value) > 0)
	{
		/*������*/
		submit->set_msg_type(smgp_def::MSG_TYPE::SMC_ORDER);
	}
	else if (atoi(submit_pkg._fee_type) == 3)
	{
		/*ȡ����*/
		submit->set_msg_type(smgp_def::MSG_TYPE::SMC_CANCEL);
	}
	else
	{
		submit->set_msg_type(smgp_def::MSG_TYPE::ORDER_PROG);         
	}
#endif


	submit->set_fee_type(submit_pkg._fee_type);
	submit->set_msg_cont(submit_pkg._cont, 
						(unsigned char)strlen(submit_pkg._cont), 
			                smgp_def::MSG_FMT::GB2312);
	submit->set_pri(submit_pkg._prio);
	submit->set_nd_rpt(submit_pkg._report);
	submit->set_svr_id(submit_pkg._srv_type);
	submit->set_at_time(submit_pkg._sched_time);
	submit->set_vld_time("");
	
	//	�ⶥ���,�ӿ��ݲ�֧��
	submit->set_fixed_fee("00");

	//	�������к�
	_smgp_seq->set_seq(submit);

	//	���
	//	����
	submit->encode();

	//	��¼sys_seq
	sys_seq = submit->get_seq_id();

	//	����
	_sender->AddPacket(submit, false);

	return sys_seq;
}

//	�ָ����
unsigned long CClient::submit_cut_sm(SUBMIT_SM & submit_pkg)
{
	unsigned long sys_seq;
	char enc_buf[MAX_LONG_SM_LEN * 2 + 1] = { 0 };
	char fee_type[MAX_FEE_TYPE_LEN + 1] = { 0 };
	char fee_code[MAX_FEE_VALUE_LEN + 1] = {0};
	int pkg_count = 0;
	int cont_len = 0;
	int i = 0;
	int enc_len = MAX_LONG_SM_LEN * 2;

    _uc_coding->encode(submit_pkg._cont, (int)strlen(submit_pkg._cont), enc_buf, &enc_len);
	pkg_count = (int)(enc_len / MAX_CONT_ENC_LEN) + 1;

	for(i = 1; i <= pkg_count; i++)
	{
		try
		{
			//	��������
			smgp_submit *submit = new smgp_submit;

			//	���ø��ֶ�ֵ
			try
			{
				//	���𳤶�
				if (i == pkg_count)
				{
					cont_len = enc_len % MAX_CONT_ENC_LEN;
				}
				else
				{
					cont_len = MAX_CONT_ENC_LEN;
				}

				//	����Ʒ�
				if (i == 1)
				{
					sprintf(fee_type, "%02d", atoi(submit_pkg._fee_type));
					strncpy(fee_code, submit_pkg._fee_value, MAX_FEE_VALUE_LEN);
				}
				else
				{
					strcpy(fee_type, FEE_TYPE_FREE);
					strcpy(fee_code, "00");
				}

				//	���ø��ֶ�ֵ
				submit->set_fee_type(fee_type);
				submit->set_msg_cont(enc_buf + (i - 1) * MAX_CONT_ENC_LEN, cont_len, smgp_def::MSG_FMT::UCS2);

				//	���·�����fix_fee
				if (atoi(submit_pkg._fee_type) == 2)
				{
					submit->set_fee_code("00");
					submit->set_fixed_fee(fee_code);
				}
				else
				{
					submit->set_fee_code(fee_code);
				}

				//	��Ϣ����,�ӿ��ݲ�֧��
				if (atoi(submit_pkg._fee_type) == 2 && atoi(fee_code) > 0)
				{
					submit->set_msg_type(smgp_def::MSG_TYPE::SMC_ORDER);
				}
				else
				{
					submit->set_msg_type(smgp_def::MSG_TYPE::ORDER_PROG);         
				}

				submit->set_dest_term_id(submit_pkg._mobile);
				submit->set_dest_id_cnt(1);
				submit->set_src_term_id(submit_pkg._sp_num);
				submit->set_charge_term_id(submit_pkg._mobile);

				submit->set_pri(submit_pkg._prio);
				submit->set_nd_rpt(submit_pkg._report);
				submit->set_svr_id(submit_pkg._srv_type);
				submit->set_at_time(submit_pkg._sched_time);
				submit->set_vld_time("");
				
				//	�ⶥ���,�ӿ��ݲ�֧��
				submit->set_fixed_fee("00");

				//	�������к�
				_smgp_seq->set_seq(submit);

			}
			catch(...)
			{
				OutputDebugString("[CClient::submit]encode except!");
			}
	#ifdef _DEBUG
		
			printf("Submit:%s,%s,%s\n",submit_pkg._sp_num, submit_pkg._mobile, submit_pkg._cont);
	#endif
			//	�������к�
			_smgp_seq->set_seq(submit);

			//	����
			submit->encode();

			if (i == 1)
			{
				//	��¼msgid ֻ��¼��һ����
				sys_seq = submit->get_seq_id();
				//	����
				_sender->AddPacket(submit);
			}
			else
			{
				_sender->AddPacket(submit);
			}
		}
		catch(...)
		{
			OutputDebugString("[CClient::submit]except !");
		}
	}
   

	return sys_seq;
}


unsigned int CClient::ThreadEntry(void)
{
	smgp_active_test *p_active_test = NULL;
	time_t now_time;

	while (!terminated_)
	{
		Sleep(1000);
		now_time = time(0);

		if (_is_connected && ((now_time -_last_time) >= IDLE_TIME))
		{
			//	���ͼ���
			_last_time = now_time;
			p_active_test = new smgp_active_test;
			_smgp_seq->set_seq(p_active_test);
			p_active_test->encode();
			_sender->SendPacket(p_active_test);
		}
		else
		{
			if (!_is_connected)
			{
				#ifdef _DEBUG
					printf("��ʱ������:%d�����ӶϿ�\r\n",_conn.id);
				#endif
				connect_smg();
			}
		}
	}//while
	return 0;
}

void CClient::OnRecv(char *buf, int len)
{

	char tmp_a[255] = {0};
	buffer rcv_buf, do_buf;
	smgp_head *p_smgp_head = (smgp_head *)&_packets.head;

	sprintf(tmp_a, "OnRecv..Data Len = %d", len); 
	OutputDebugString(tmp_a);
	//	�Ȱ����ݲ��뻺��
	rcv_buf.buf = buf;
	rcv_buf.len = len;
	_buffer->append(rcv_buf);
	while (_buffer->len() >= smgp_def::HEAD_LEN)
	{
		//	ȡ�û������׵�ַ
		_buffer->get_buffer(do_buf);
		
		//	����Ϣͷ
		p_smgp_head->decode(do_buf.buf);
		
		//	��鳤���Ƿ��㹻һ����
		if (p_smgp_head->get_pkt_len() > (unsigned int)_buffer->len())
		{
			//����һ��������,�˳�����
			sprintf(tmp_a, "Half Packet bytes = %d", _buffer->len());
			OutputDebugString(tmp_a);
			break;
		}

		//	����Ϣ���ͽ��
		switch (p_smgp_head->get_req_id()) 
		{
		case smgp_def::REQ_ID::SUBMIT_RESP:
			{
				_packets.submit_resp.decode(do_buf.buf);
				do_submit_resp(&_packets.submit_resp);
			}
			break;
		case smgp_def::REQ_ID::DELIVER:
			{
				_packets.deliver.decode(do_buf.buf, p_smgp_head->get_pkt_len());
				do_deliver(&_packets.deliver);
			}
			break;
		case smgp_def::REQ_ID::ACTIVE_TEST:
			{
				_packets.active_test.decode(do_buf.buf);
				do_active_test(&_packets.active_test);
			}
			break;
		case smgp_def::REQ_ID::ACTIVE_TEST_RESP:
			{
				_packets.active_test_resp.decode(do_buf.buf);
				do_active_test_resp(&_packets.active_test_resp);
			}
			break;
		case smgp_def::REQ_ID::LOGIN_RESP:
			{
				_packets.login_resp.decode(do_buf.buf);
				do_login_resp(&_packets.login_resp);
			}
			break;
		case smgp_def::REQ_ID::QUERY_RESP:
			{
				_packets.query_resp.decode(do_buf.buf);
				do_query_resp(&_packets.query_resp);
			}
			break;
		case smgp_def::REQ_ID::EXIT:
			{
				_packets.exit.decode(do_buf.buf);
				do_exit(&_packets.exit);
			}
			break;
		case smgp_def::REQ_ID::EXIT_RESP:
			{
				_packets.exit_resp.decode(do_buf.buf);
				do_exit_resp(&_packets.exit_resp);
			}
			break;
		default:
			OutputDebugString("Unknow packet type !");
			_buffer->clear();
			break;
		}

		//��¼����յ���Ϣ��ʱ��
		_last_time = time(0);

		sprintf(tmp_a, "Packet size = %d", p_smgp_head->get_pkt_len());
		OutputDebugString(tmp_a);
		//	ɾ�������������
		_buffer->erase(p_smgp_head->get_pkt_len());
	}		
}

void CClient::do_submit_resp(smgp_submit_resp *p_submit_resp)
{
	char tmp[255];
	sprintf(tmp,"resp: conn_id:%d, sys_seq_id:%d, result:%d \r\n",
		_conn.id, p_submit_resp->get_seq_id(),p_submit_resp->get_status());

	OutputDebugString(tmp);
	#ifdef _DEBUG
		printf("\r\n");
	#endif	

	_submit_resp._msg_id = p_submit_resp->get_msg_id_seq();
	_submit_resp._seq_id = p_submit_resp->get_seq_id();
	_submit_resp._resp_code = p_submit_resp->get_status();

	//	�ύ���û�
	try
	{
		_smgp->OnSubmitResp(_submit_resp);
	}
	catch(...)
	{
		OutputDebugString("OnSubmitResp exception !");
	#ifdef _DEBUG
		printf("OnSubmitResp exception !\r\n");
	#endif
	}

	if (p_submit_resp->get_status() == smgp_def::SUBMIT_OK)
	{
		_sender->Response(p_submit_resp->get_seq_id());
	}
	else
	{
			sprintf(tmp, "sumbmit error conn_id:%d, sys_seq_id:%d, result:%d \r\n",
				_conn.id, p_submit_resp->get_seq_id(),p_submit_resp->get_status());
			OutputDebugString(tmp);
		#ifdef _DEBUG
			printf(tmp);
		#endif
	}
}

void CClient::do_deliver(smgp_deliver *p_deliver)
{
	//	��һ��deliver_resp��Ϣ
	BYTE msg_id[smgp_def::MSG_ID_LEN+1]	= {0};

	smgp_deliver_resp *p_deliver_resp = new smgp_deliver_resp;
	//	���к�
	p_deliver_resp->set_seq_id(p_deliver->get_seq_id());
	
	//	msg_id
	p_deliver->get_msg_id(msg_id);
	p_deliver_resp->set_msg_id(msg_id);

	//	��ȷ
	p_deliver_resp->set_status(smgp_def::DELVIER_OK);

	p_deliver_resp->encode();

	_sender->AddPacket(p_deliver_resp,false);

	//	����������Ϣ
	if (p_deliver->get_is_rpt() == smgp_def::IS_REPORT)
	{
		#ifdef _DEBUG
			printf("smgp_report \r\n");
		#endif	
		_smgp_report.decode(p_deliver->get_msg_cont(), p_deliver->get_msg_len());

        //	����report
		_report._msg_id = _smgp_report.get_msg_id_seq();

		strcpy(_report._mobile, p_deliver->get_src_term_id());
		strcpy(_report._err_code, _smgp_report.get_err_code());
		_report._stat = (unsigned char)_smgp_report.get_stat();
		
		//	�ύ���û�
		try
		{
			_smgp->OnReport(_report);
		}
		catch(...)
		{
			#ifdef _DEBUG
				printf("OnReport exception !\r\n");
			#endif
		}		
	}
	else
	{
		#ifdef _DEBUG
			printf("smgp_deliver \r\n");
            
		#endif	
		//	���ж���
		memset(&_deliver, 0, sizeof(_deliver));
		_deliver._coding = (unsigned char)p_deliver->get_msg_fmt();
		memcpy(_deliver._cont, p_deliver->get_msg_cont(), p_deliver->get_msg_len());

		strncpy(_deliver._mobile, p_deliver->get_src_term_id(), MAX_ADDR_LEN);
		strncpy(_deliver._sp_num, p_deliver->get_dest_term_id(), MAX_ADDR_LEN);
#ifdef _SMGP3_
		strncpy(_deliver._link_id, p_deliver->get_link_id(), MAX_LINK_ID_LEN);
#endif

		try
		{
#ifdef _SMGP3_
			printf("%s,%s,%s,%s\n", _deliver._mobile, _deliver._sp_num, _deliver._cont, _deliver._link_id);
#endif
			_smgp->OnDeliver(_deliver);
		}
		catch (...)
		{
			#ifdef _DEBUG
				printf("OnDeliver exception !\r\n");
			#endif
		}
	}
}

void CClient::do_active_test(smgp_active_test *p_active_test)
{
    //	�ظ�active_test_resp
	smgp_active_test_resp *p_act_resp = new smgp_active_test_resp;
	p_act_resp->set_seq_id(p_active_test->get_seq_id());

	p_act_resp->encode();

	_sender->SendPacket(p_act_resp);
}

void CClient::do_login_resp(smgp_login_resp *p_login_resp)
{
	//	��¼������״̬
	if (p_login_resp->get_status() == 0)
	{
		_is_connected = true;
		_sender->set_bind(_is_connected);
		#ifdef _DEBUG
			printf("���ӳɹ� id=%d, ״̬:%d\r\n", _conn.id, p_login_resp->get_status());
		#endif

	}
	else
	{
		#ifdef _DEBUG
			printf("����ʧ�� id=%d, ״̬:%d\r\n", _conn.id, p_login_resp->get_status());
		#endif
	}
}

void CClient::do_query_resp(smgp_query_resp *p_query_resp)
{
	#ifdef _DEBUG
		printf("query_resp \r\n");
	#endif
}

void CClient::do_exit(smgp_exit *p_exit)
{
	#ifdef _DEBUG
		printf("exit_resp \r\n");
	#endif
}

void CClient::do_active_test_resp(smgp_active_test_resp *p_active_test_resp)
{
	#ifdef _DEBUG
		printf("conn_id:%d, active_test_resp \r\n",_conn.id);
	#endif
}

void CClient::do_exit_resp(smgp_exit_resp *p_exit_resp)
{
	#ifdef _DEBUG
		printf("exit_resp \r\n");
	#endif
}


void CClient::OnConnect(tcpip::CTcpClient *tcp_client)
{
	//	����
	#ifdef _DEBUG
		printf("connect_smg()...\n");
	#endif
	connect_smg();
}

void CClient::OnDisconnect(tcpip::CTcpClient *tcp_client)
{
	//	�Ͽ�������
	_is_connected = false;
	_sender->set_bind(_is_connected);
	Connect(_svr_ip, _port);
}

void CClient::OnReconnect(tcpip::CTcpClient *tcp_client)
{
	//	��ʱ����
}