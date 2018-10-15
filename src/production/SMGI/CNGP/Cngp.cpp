#include "cngp.h"

CCngp::CCngp(void)
	: _high_index(0)
	, _low_first(0)
	, _low_count(0)
	, _low_index(0)
	, _seq(NULL)

{
}

CCngp::~CCngp(void)
{
	Cleanup();
}

//	��ʼ��
//	���ò���,���������̵߳�
bool CCngp::Startup(CONNECT_INFO &info)
{
	memcpy(&_conn_info, &info, sizeof(CONNECT_INFO));
	
	//	�������кŲ�����
	_seq = new cngp_seq(_conn_info._seq_id);

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
void CCngp::CreateAllConns(void)
{
	int i = 0, created_count = 0;
	//	һ������
	if (_conn_info._links == 1)
	{
		CreateConn(0, CONN_LOW, cngp_def::LOGIN_MODE::MO_MT);
	}
	//	��������
	if (_conn_info._links == 2)
	{
		//	������������������
		switch (_conn_info._smg_type) 
		{
		//	cngp��׼
		case SMG_TYPE_CNGP_STD:	
			{
				//	����0
				CreateConn(0, CONN_HIGH, cngp_def::LOGIN_MODE::MO);

				Sleep(200);

				//	��ͨ1
				CreateConn(1, CONN_LOW, cngp_def::LOGIN_MODE::MT);
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
		case SMG_TYPE_CNGP_STD:
			{
				//	����0
				CreateConn(0, CONN_LOW, cngp_def::LOGIN_MODE::MO);
				created_count++;

				Sleep(200);

				//	����
				CreateConn(1, CONN_HIGH, cngp_def::LOGIN_MODE::MT);
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
			CreateConn(i, CONN_LOW, cngp_def::LOGIN_MODE::MT);
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

void CCngp::CreateConn(unsigned int conn_id,
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
void CCngp::GetConnLevel(void)
{
	unsigned int i = 0;

	//	������
	_high_index = -1;
	for (i = 0; i< _clients.size(); i++)
	{
		if (_clients[i]->get_level() == CONN_HIGH 
			&& _clients[i]->get_type() != cngp_def::LOGIN_MODE::MO)
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
			&& _clients[i]->get_type() != cngp_def::LOGIN_MODE::MO)
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
unsigned long CCngp::Submit(SUBMIT_SM &submit, bool prio)
{
	unsigned long sys_seq = 0;
	
	if (!prio || (prio && (_high_index < 0)))
	{
		//	��������Ϣ
		if (_low_index >= 0)
		{
			//	�е�����
			sys_seq = _clients[_low_index]->submit(submit);

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
		}
	}//end of p_submit->_prio == MSG_LOW else

	return sys_seq;
}

//  ȡ�õ�ǰseq
unsigned long CCngp::GetSeqID(void) const
{
	if ( _seq != NULL )
	{
		return _seq->get_seq();
	}
	else
		return 0;
}

//	����
int CCngp::Cleanup(void)
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


CClient::CClient(CCngp *p_cngp,
				 cngp_seq *p_seq,
				 CONNECT_INFO *conn_info,
				 CONNECTION *conn)
	: _cngp(p_cngp)
	, _sender(NULL)
	, _buffer(NULL)
	, _cngp_seq(p_seq)
	, _is_connected(false)
	, _conn_info(*conn_info)
	, _conn(*conn)
	, _last_time(0x7FFFFFFF)
{
	//	������ip
	strncpy(_svr_ip, _conn_info._smg_ip, MAX_IP_LEN);

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
}

//	��������
void CClient::connect_smg(void)
{
	cngp_login *login_pkg = new cngp_login;

	//	����cngp_login
	login_pkg->set_login_mode(_conn.type);
	login_pkg->set_client_id(_conn_info._login_name);
	login_pkg->set_shared_secret(_conn_info._login_pswd);
	login_pkg->set_tm_stmp(0);
	login_pkg->set_version(CNGP_VER);

	//	�������к�
	_cngp_seq->set_seq(login_pkg);

	//	����
	login_pkg->encode();

	//	����
	_sender->AddPacket(login_pkg);
}

//	�ύ����
unsigned long CClient::submit(SUBMIT_SM &submit_pkg)
{
	//�ȼ����������״��,������Ӳ�������,�ύʧ��
	unsigned long sys_seq = 0;

	if (_is_connected)
	{
		//	��������
		cngp_submit *submit = new cngp_submit;

		//	���ø��ֶ�ֵ

		//	spid
		submit->set_spid(_conn_info._corp_id);

		//	��Ϣ����,�ӿ��ݲ�֧��
		submit->set_msg_type(cngp_def::MSG_TYPE::ORDER_MT);         

		submit->set_dest_term_id(submit_pkg._mobile);
		submit->set_dest_id_cnt(1);
		submit->set_src_term_id(submit_pkg._sp_num);
		submit->set_charge_term_id(submit_pkg._mobile);
		submit->set_fee_code(submit_pkg._fee_value);		
		submit->set_fee_type(submit_pkg._fee_type);
		submit->set_msg_cont(submit_pkg._cont, 
							(unsigned char)strlen(submit_pkg._cont), 
			                 cngp_def::MSG_FMT::GB2312);
		submit->set_pri(submit_pkg._prio);
		submit->set_nd_rpt(submit_pkg._report);
		submit->set_svr_id(submit_pkg._srv_type);
		submit->set_at_time(submit_pkg._sched_time);
		submit->set_vld_time("");
		
		//	�ⶥ���,�ӿ��ݲ�֧��
		submit->set_fee_user_type(cngp_def::FEE_USER_TYPE::FEE_DEST_TERM_ID);

		//	�������к�
		_cngp_seq->set_seq(submit);

		//	����
		submit->encode();

		//	��¼sys_seq
		sys_seq = submit->get_seq_id();

		//	����
		_sender->AddPacket(submit);
	}
	return sys_seq;
}

unsigned int CClient::ThreadEntry(void)
{
	cngp_active_test *p_active_test = NULL;
	time_t now_time;

	while (!terminated_)
	{
		Sleep(1000);
		now_time = time(0);

		if (_is_connected && ((now_time -_last_time) >= IDLE_TIME))
		{
			//	���ͼ���
			_last_time = now_time;
			p_active_test = new cngp_active_test;
			_cngp_seq->set_seq(p_active_test);
			p_active_test->encode();
			_sender->AddPacket(p_active_test);
		}
		else
		{
			if (!_is_connected)
			{
				#ifdef _DEBUG
					printf("��ʱ������:%d�����ӶϿ�\r\n",_conn.id);
				#endif
			}
		}
	}//while
	return 0;
}

void CClient::OnRecv(char *buf, int len)
{
	buffer rcv_buf, do_buf;
	cngp_head *p_cngp_head = (cngp_head *)&_packets.head;

	//	�Ȱ����ݲ��뻺��
	rcv_buf.buf = buf;
	rcv_buf.len = len;
	_buffer->append(rcv_buf);
	while (_buffer->len() >= cngp_def::HEAD_LEN)
	{
		//	ȡ�û������׵�ַ
		_buffer->get_buffer(do_buf);
		
		//	����Ϣͷ
		p_cngp_head->decode(do_buf.buf);
		
		//	��鳤���Ƿ��㹻һ����
		if (p_cngp_head->get_pkt_len() > (unsigned int)_buffer->len())
		{
			//����һ��������,�˳�����
			break;
		}

		//	����Ϣ���ͽ��
		switch (p_cngp_head->get_req_id()) 
		{
		case cngp_def::REQ_ID::SUBMIT_RESP:
			{
				_packets.submit_resp.decode(do_buf.buf);
				do_submit_resp(&_packets.submit_resp);
			}
			break;
		case cngp_def::REQ_ID::DELIVER:
			{
				_packets.deliver.decode(do_buf.buf, p_cngp_head->get_pkt_len());
				do_deliver(&_packets.deliver);
			}
			break;
		case cngp_def::REQ_ID::ACTIVE_TEST:
			{
				_packets.active_test.decode(do_buf.buf);
				do_active_test(&_packets.active_test);
			}
			break;
		case cngp_def::REQ_ID::ACTIVE_TEST_RESP:
			{
				_packets.active_test_resp.decode(do_buf.buf);
				do_active_test_resp(&_packets.active_test_resp);
			}
			break;
		case cngp_def::REQ_ID::LOGIN_RESP:
			{
				_packets.login_resp.decode(do_buf.buf);
				do_login_resp(&_packets.login_resp);
			}
			break;
		case cngp_def::REQ_ID::EXIT:
			{
				_packets.exit.decode(do_buf.buf);
				do_exit(&_packets.exit);
			}
			break;
		case cngp_def::REQ_ID::EXIT_RESP:
			{
				_packets.exit_resp.decode(do_buf.buf);
				do_exit_resp(&_packets.exit_resp);
			}
			break;
		default:
			break;
		}

		//��¼����յ���Ϣ��ʱ��
		_last_time = time(0);

		//	ɾ�������������
		_buffer->erase(p_cngp_head->get_pkt_len());
	}		
}

void CClient::do_submit_resp(cngp_submit_resp *p_submit_resp)
{
	#ifdef _DEBUG
		printf("cngp_submit_resp \r\n");
		printf("conn_id:%d, sys_seq_id:%d, result:%d \r\n",
			_conn.id, p_submit_resp->get_seq_id(),p_submit_resp->get_status());
		printf("\r\n");
	#endif	
	if (p_submit_resp->get_status() == cngp_def::SUBMIT_OK)
	{
		_sender->Response(p_submit_resp->get_seq_id());

		//	�ύ���û�
		_submit_resp._msg_id = p_submit_resp->get_msg_id_seq();
		_submit_resp._seq_id = p_submit_resp->get_seq_id();

		try
		{
			_cngp->OnSubmitResp(_submit_resp);
		}
		catch(...)
		{
			#ifdef _DEBUG
				printf("OnSubmitResp exception !\r\n");
			#endif
		}
	
	}
	else
	{
		#ifdef _DEBUG
			printf("conn_id:%d, sys_seq_id:%d, result:%d \r\n",
				_conn.id, p_submit_resp->get_seq_id(),p_submit_resp->get_status());
		#endif
	}

}

void CClient::do_deliver(cngp_deliver *p_deliver)
{
	//	��һ��deliver_resp��Ϣ
	BYTE msg_id[cngp_def::MSG_ID_LEN+1]	= {0};

	cngp_deliver_resp *p_deliver_resp = new cngp_deliver_resp;
	//	���к�
	p_deliver_resp->set_seq_id(p_deliver->get_seq_id());
	
	//	msg_id
	p_deliver->get_msg_id(msg_id);
	p_deliver_resp->set_msg_id(msg_id);

	//	��ȷ
	p_deliver_resp->set_status(cngp_def::DELVIER_OK);

	p_deliver_resp->encode();

	_sender->AddPacket(p_deliver_resp,false);

	//	����������Ϣ
	if (p_deliver->get_is_rpt() == cngp_def::IS_REPORT)
	{
		#ifdef _DEBUG
			printf("cngp_report \r\n");
		#endif	
		_cngp_report.decode(p_deliver->get_msg_cont(), p_deliver->get_msg_len());

        //	����report
		_report._msg_id = _cngp_report.get_msg_id_seq();

		strcpy(_report._mobile, p_deliver->get_src_term_id());
		strcpy(_report._err_code, _cngp_report.get_err_code());
		_report._stat = (unsigned char)_cngp_report.get_stat();
		
		//	�ύ���û�
		try
		{
			_cngp->OnReport(_report);
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
			printf("cngp_deliver \r\n");
		#endif	
		//	���ж���
		memset(&_deliver, 0, sizeof(_deliver));
		_deliver._coding = (unsigned char)p_deliver->get_msg_fmt();
		memcpy(_deliver._cont, p_deliver->get_msg_cont(), p_deliver->get_msg_len());

		strncpy(_deliver._mobile, p_deliver->get_src_term_id(), MAX_ADDR_LEN);
		strncpy(_deliver._sp_num, p_deliver->get_dest_term_id(), MAX_ADDR_LEN);

		try
		{
			_cngp->OnDeliver(_deliver);
		}
		catch (...)
		{
			#ifdef _DEBUG
				printf("OnDeliver exception !\r\n");
			#endif
		}
	}
}

void CClient::do_active_test(cngp_active_test *p_active_test)
{
    //	�ظ�active_test_resp
	cngp_active_test_resp *p_act_resp = new cngp_active_test_resp;
	p_act_resp->set_seq_id(p_active_test->get_seq_id());

	p_act_resp->encode();

	_sender->AddPacket(p_act_resp, false);

	#ifdef _DEBUG
		printf("����id = %d, active_test\n", _conn.id);
	#endif

}

void CClient::do_login_resp(cngp_login_resp *p_login_resp)
{
	//	��¼������״̬
	if (p_login_resp->get_status() == 0)
	{
		_is_connected = true;
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

	//	ɾ���ȴ�Ӧ��İ�
    _sender->Response(p_login_resp->get_seq_id());
}


void CClient::do_exit(cngp_exit *p_exit)
{
#ifdef _DEBUG
	printf("exit_resp \r\n");
#endif
}

void CClient::do_active_test_resp(cngp_active_test_resp *p_active_test_resp)
{
	//	ɾ���ȴ���active_test��
	_sender->Response(p_active_test_resp->get_seq_id());

	#ifdef _DEBUG
		printf("conn_id:%d, active_test_resp \r\n",_conn.id);
	#endif
}

void CClient::do_exit_resp(cngp_exit_resp *p_exit_resp)
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
	Connect(_svr_ip, _port);
}

void CClient::OnReconnect(tcpip::CTcpClient *tcp_client)
{
	//	��ʱ����
}