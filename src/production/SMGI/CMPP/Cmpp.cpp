#include "cmpp.h"

CCmpp::CCmpp(void)
	: _high_index(0)
	, _low_first(0)
	, _low_count(0)
	, _client_idx(0)
	, _seq(NULL)

{
}

CCmpp::~CCmpp(void)
{
	if (_seq != NULL)
	{
		Cleanup();
	}
}

//	初始化
//	设置参数,启动工作线程等
bool CCmpp::Startup(CONNECT_INFO &info)
{
	memcpy(&_conn_info, &info, sizeof(CONNECT_INFO));
	
	//	创建序列号产生器
	_seq = new cmpp_seq(_conn_info._seq_id);

	//	建立连接
	CreateAllConns();
	
	//	统计连接级别信息
	GetConnLevel();

	return true;
}

/*
创建连接的规则
1.如果只能开一个连接,那么上,下行,紧急,普通共用一个连接
2.如果能开两个连接
	2.1如果上下行不用区分,一个紧急0号,一个普通1号
	2.2如果区分上下行,上行占用一个连接0号,下行紧急,一个下行普通
3.如果可以开3个以上的连接
	3.1如果不区分上下行,一个紧急连接0号,其他普通连接
	3.2如果区分上下行,上行0号,紧急1号,其他普通连接
*/
void CCmpp::CreateAllConns(void)
{
	int i = 0, created_count = 0;
	//	一个连接
	if (_conn_info._links == 1)
	{	
		CreateConn(0, CONN_LOW, CONN_MT);
	}
	//	两个连接
	if (_conn_info._links == 2)
	{
		//	根据网关类型来处理
		switch (_conn_info._smg_type) 
		{
		case CMPP_STD:	//	CMPP标准(华为)
			{
				//	紧急0
				CreateConn(0, CONN_LOW, CONN_MT);

				Sleep(200);

				//	普通1
				CreateConn(1, CONN_LOW, CONN_MT);
			}
			break;
		case CMPP_QHSX:	//	CMPP 双端口(清华深讯)
			{
				//	上行0
 				CreateConn(0, CONN_LOW, CONN_MO);

				Sleep(200);

				//	下行
				CreateConn(1, CONN_LOW, CONN_MT);
			}
			break;
		case CMPP_YX:		//	CMPP 同端口双连接(亚信)
			{
                //	上行0
				CreateConn(0, CONN_LOW, CONN_MO);

				Sleep(200);

				//	下行
				CreateConn(1, CONN_LOW, CONN_MT);
			}
			break;
		default:
#ifdef _DEBUG
			printf("网关类型错!");
#endif
			break;
		}//switch
	}//if 2

	//3个以上连接
	if (_conn_info._links >= 3)
	{
		created_count = 0;

		//	根据网关类型来处理
		switch (_conn_info._smg_type) 
		{
		case CMPP_STD:	//	CMPP标准(华为)
			{
				//	紧急0
				CreateConn(0, CONN_LOW, CONN_MT);
				created_count++;
			}
			break;
		case CMPP_QHSX:	//	CMPP 双端口(清华深讯)
			{
				//	上行0
				CreateConn(0, CONN_LOW, CONN_MO);
				created_count++;

				Sleep(200);

				//	下行
				CreateConn(1, CONN_LOW, CONN_MT);
				created_count++;
			}
			break;
		case CMPP_YX:		//	CMPP 同端口双连接(亚信)
			{
				//	上行0
				CreateConn(0, CONN_LOW, CONN_MO);
				created_count++;
				
				Sleep(200);

				//	下行
				CreateConn(1, CONN_LOW, CONN_MT);
				created_count++;
			}
			break;
		default:
#ifdef _DEBUG
			printf("网关类型错!");
#endif			
			break;

		}// switch
		
		//	创建其他普通连接
		for (i = created_count; i < _conn_info._links; i++)	
		{
			Sleep(200);
			CreateConn(i, CONN_LOW, CONN_MT);
		}
	}// if 3
}

//函数名:
//	    CreateConn
//功能:
//		创建一个连接
//参数:
//    conn_id          连接标识,从0开始,递增
//    conn_level	   级别  0 - 普通  1 - 紧急
//	  conn_type 	   类型  0 - 下行或含下行, 1-上行

void CCmpp::CreateConn(unsigned int conn_id,
					   unsigned int conn_level, 
					   unsigned int conn_type)
{
	CClient *client = NULL;

	_conn.id	= conn_id;
	_conn.level = conn_level;
	_conn.type  = conn_type;

	//	创建Client实例
	client = new CClient(this, _seq, &_conn_info, &_conn);		

	//	加入管理数组
	_clients.push_back(client);
}

//设置连接信息初值(高,低优先级)
void CCmpp::GetConnLevel(void)
{
	unsigned int i = 0;

	//	高优先
	_high_index = -1;
	for (i = 0; i< _clients.size(); i++)
	{
		if (_clients[i]->get_level() == CONN_HIGH)
		{
			_high_index = i;
			break;  
		}
	}

	//	低优先
	_low_first = -1;
	_low_count = 0;

	for (i = 0; i< _clients.size(); i++)
	{
		if (_clients[i]->get_level() == CONN_LOW && _clients[i]->get_type() == CONN_MT)
		{
			if(_low_count == 0)
			{
				_low_first = i;	
			}
			_low_count++;
		}
	}

	//	设置初次发送的连接下标
	if (_low_first >= 0)
	{
		_client_idx = _low_first;
	}
	else
	{
		_client_idx = -1;
	}
}

//提交短信
unsigned long CCmpp::Submit(SUBMIT_SM &submit, bool prio)
{
	unsigned long sys_seq = 0;
	Process::CAutoLock lock(&_client_cs);
	try
	{
#ifdef _DEBUG
		{
			char output[1024] = {0};
			sprintf(output, "[CCmpp::Submit]Entering(low_idx=%d).", _client_idx);
			OutputDebugString(output);
		}
#endif

		if (_client_idx > _conn_info._links - 1)
		{
			//	清华深讯特殊，第一个连接做上行
			if (_conn_info._smg_type == CMPP_QHSX)
			{
				_client_idx = 1;
			}
			else
			{
				_client_idx = 0;
			}
		}
				
		try
		{
			if ((_clients[_client_idx])->IsConnected())
			{
				try
				{
					CClient *x = _clients[_client_idx];
					if (x != NULL)
					{
						sys_seq = x->submit(submit);
					}
				}
				catch(...)
				{
					char output[1024] = {0};
					sprintf(output, "[CCmpp::Submit]Submit except4(low_index=%d, links=%d, smg_type=%d).", _client_idx, _conn_info._links, _conn_info._smg_type);
					OutputDebugString(output);
				}

				++_client_idx;
			}
		}
		catch(...)
		{
			char output[1024] = {0};
			sprintf(output, "[CCmpp::Submit]Submit except3(low_index=%d, links=%d).", _client_idx, _conn_info._links);
			OutputDebugString(output);
		}
	}
	catch(...)
	{
		OutputDebugString("[CCmpp::Submit]Submit except.");
	}

#ifdef _DEBUG
	{
		char output[1024] = {0};
		sprintf(output, "[CCmpp::Submit]Leaving(low_idx=%d).", _client_idx);
		OutputDebugString(output);
	}
#endif

	return sys_seq;
}

//	清理
int CCmpp::Cleanup(void)
{
	//	释放被创建的所有对象
	for(unsigned long i = 0; i < _clients.size(); ++i)
	{
		if (_clients[i] != NULL)
		{
			delete _clients[i];
			_clients[i] = NULL;
		}
	}
	_clients.clear();

	//	删除序列号产生器
	if (_seq != NULL)
	{
		delete _seq;
		_seq = NULL;
	}

	return 0;
}


CClient::CClient(CCmpp *p_cmpp,
				 cmpp_seq *p_seq,
				 CONNECT_INFO *conn_info,
				 CONNECTION *conn)
	: _cmpp(p_cmpp)
	, _sender(NULL)
	, _buffer(NULL)
	, _cmpp_seq(p_seq)
	, _is_connected(false)
	, _conn_info(*conn_info)
	, _conn(*conn)
	, _uc_coding(NULL)
{

	unsigned long _send_int = 0;
	//  解码对象
	_uc_coding = codeFactory::createEncoding("unicode");

	//	服务器ip
	strncpy(_svr_ip, _conn_info._smg_ip, MAX_IP_LEN);

	//	服务器端口
	if (_conn.type == 0)
	{	
		//	下行或者含下行
		_port = _conn_info._smg_port_mt;
		_send_int = _conn_info._interval;
	}
	else
	{
		//	上行
		_port = _conn_info._smg_port_mo;
		_send_int = 0;
	}

	//	数据缓冲
	_buffer = new buffer_block();

	//	控制发送类
	_sender = new CSender(this, false, _conn_info._slip_size, _send_int);

	//	建立连接
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

//	连接网关
void CClient::connect_smg(void)
{
	cmpp_connect *connect_pkg = new cmpp_connect;

	//	发送cmpp_connect    
	connect_pkg->set_source_addr(_conn_info._login_name);
	connect_pkg->set_shared_secret(_conn_info._login_pswd);
	connect_pkg->set_time_stamp(0);

	//	如果是亚信网关 Version 为 1 表示 MO，为 0 表示 MT
	if (_conn_info._smg_type == CMPP_YX)
	{
		if(_conn.type ==0)
		{
			connect_pkg->set_version(0x00);
		}
		else
		{
			connect_pkg->set_version(0x01);
		}
	}
	else
	{
#ifdef _CMPP2_
		connect_pkg->set_version(0x20);
#else
		connect_pkg->set_version(0x30);
#endif
	}

	//	设置序列号
	_cmpp_seq->set_seq(connect_pkg);

	//	编码
	connect_pkg->encode();

	//	发送
	_sender->SendPacket(connect_pkg);
}

//	提交短信
unsigned long CClient::submit(SUBMIT_SM &submit_pkg)
{
	//先检查网络连接状况,如果连接不存在则,提交失败
	unsigned long sys_seq = 0;
	char enc_buf[MAX_LONG_SM_LEN * 2 + 1] = { 0 };
	int enc_len = MAX_LONG_SM_LEN * 2;

	if (_is_connected)
	{
		_uc_coding->encode(submit_pkg._cont, (int)strlen(submit_pkg._cont), enc_buf, &enc_len);

		if (enc_len <= MAX_CONT_ENC_LEN)
		{
			//	不需要拆分
			sys_seq = submit_norm(submit_pkg);
		}
		else
		{
			//	需要拆分
			if	(_conn_info._long_sm == SMS_LONG_SM_NO) 
			{
				//	短信分割
                sys_seq = submit_cut_sm(submit_pkg);
			}
			else
			{
				//	长短信
                sys_seq = submit_long_sm(submit_pkg);
			}
		}

	}
	else
	{
		Sleep(10);
	}
#ifdef _DEBUG
	if (sys_seq == 0)
	{
		char err[1024];
		sprintf(err, "mo:%s, sys_seq:%d\r\n", submit_pkg._mobile, sys_seq);
		OutputDebugStr(err);
		printf("%s", err);
	}
#endif
	return sys_seq;
}

// 正常发送
unsigned long CClient::submit_norm(SUBMIT_SM & submit_pkg)
{
	unsigned long sys_seq = 0;
	char fee_type[MAX_FEE_TYPE_LEN + 1] = { 0 };
	try
	{
		//	创建对象
		cmpp_submit *submit = new cmpp_submit;

		//	设置各字段值
		try
		{
			submit->set_at_time(submit_pkg._sched_time);
			submit->set_dest_terminal_id(submit_pkg._mobile, 0);
			submit->set_dest_user_tl(1);
			submit->set_fee_code(submit_pkg._fee_value);
			submit->set_fee_terminal_id(submit_pkg._mobile, 0);

			sprintf(fee_type, "%02d", atoi(submit_pkg._fee_type));
			submit->set_fee_type(fee_type);
			submit->set_fee_user_type(3);
			submit->set_link_id(submit_pkg._link_id);

			submit->set_TP_udhi(0);
			submit->set_pk_number(1);
			submit->set_pk_total(1);
			submit->set_msg_content(submit_pkg._cont, cmpp_pkg::MSG_FMT::GB2312);

			submit->set_msg_id(0);
			submit->set_msg_level(0);
			submit->set_msg_src(_conn_info._corp_id);
			submit->set_registered_delivery(submit_pkg._report);
			submit->set_service_id(submit_pkg._srv_type);
			submit->set_src_id(submit_pkg._sp_num);
			submit->set_TP_pId(0);
			submit->set_valid_time(submit_pkg._expire_time);
		}
		catch(...)
		{
			OutputDebugString("[CClient::submit]encode except!");
		}
#ifdef _DEBUG
	
		printf("Submit:%s,%s,%s\n",submit_pkg._sp_num, submit_pkg._mobile, submit_pkg._cont);
#endif
		//	设置序列号
		_cmpp_seq->set_seq(submit);

		//	记录msgid
		sys_seq = submit->get_seq_id();

		//	编码
		submit->encode();

		//	发送
		_sender->AddPacket(submit);
	}
	catch(...)
	{
		OutputDebugString("[CClient::submit]except !");
	}

	return sys_seq;
}


//	长短信
unsigned long CClient::submit_long_sm(SUBMIT_SM & submit_pkg)
{
	unsigned long sys_seq;
	char enc_buf[MAX_LONG_SM_LEN * 2 + 1] = { 0 };
	char cont[MAX_CONT_ENC_LEN + 1] = {0};
	char fee_type[MAX_FEE_TYPE_LEN + 1] = { 0 };
	char fee_code[MAX_FEE_VALUE_LEN + 1] = {0};
	int pkg_count = 0;
	int head_len = 0;
	int cont_len = 0;
	int i = 0;
	int enc_len = MAX_LONG_SM_LEN * 2;

	//	7位长短信标志
	char long_head_7[7] = {0x06, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00};
	char long_head_6[6] = {0x06, 0x08, 0x04, 0x00, 0x00, 0x00};

	if (_conn_info._long_sm == SMG_LONG_SM_6)
	{
		head_len = 6;
	}
	else
	{
		head_len = 7;
	}
	
    _uc_coding->encode(submit_pkg._cont, (int)strlen(submit_pkg._cont), enc_buf, &enc_len);
	pkg_count = (int)(enc_len / (MAX_CONT_ENC_LEN - head_len)) + 1;

	for(i = 1; i <= pkg_count; i++)
	{
		try
		{
			//	创建对象
			cmpp_submit *submit = new cmpp_submit;

			//	设置各字段值
			try
			{

				//	区别计费
				if (i == 1)
				{
					sprintf(fee_type, "%02d", atoi(submit_pkg._fee_type));
					strncpy(fee_code, submit_pkg._fee_value, MAX_FEE_VALUE_LEN);
				}
				else
				{
					sprintf(fee_type, "%02d", cmpp_submit::FEE_TYPE::FREE);
					strcpy(fee_code, "00000");
				}

				//	记录msgid 只记录第一个包
				if (i == 1)
				{
					//	设置序列号
					_cmpp_seq->set_seq(submit);
					sys_seq = submit->get_seq_id();
				}
				else
				{
					submit->set_seq_id(sys_seq);
				}
				
				//	构造协议头
				if (_conn_info._long_sm == SMG_LONG_SM_6)
				{
					long_head_6[3] = (char)sys_seq % 100;
					long_head_6[4] = pkg_count;
					long_head_6[5] = i;

					memcpy(cont, long_head_6, 6);
				}
				else
				{
					long_head_7[4] = (char)sys_seq % 100;
					long_head_7[5] = pkg_count;
					long_head_7[6] = i;
					memcpy(cont, long_head_7, 7);
				}				

				//	区别长度
				if (i == pkg_count)
				{
					cont_len = enc_len % (MAX_CONT_ENC_LEN - head_len);
				}
				else
				{
					cont_len = MAX_CONT_ENC_LEN - head_len;
				}
				memcpy(cont + head_len, enc_buf + (i - 1) * (MAX_CONT_ENC_LEN - head_len), cont_len);
				submit->set_msg_content(cont, head_len + cont_len, cmpp_pkg::MSG_FMT::UCS2);

				submit->set_pk_number(i);
				submit->set_pk_total(pkg_count);

				submit->set_at_time(submit_pkg._sched_time);
				submit->set_dest_terminal_id(submit_pkg._mobile, 0);
				submit->set_dest_user_tl(1);
				submit->set_fee_terminal_id(submit_pkg._mobile, 0);

				submit->set_fee_type(fee_type);
				submit->set_fee_code(fee_code);
				submit->set_fee_user_type(3);
				submit->set_link_id(submit_pkg._link_id);

				submit->set_TP_udhi(1);


				submit->set_msg_id(0);
				submit->set_msg_level(0);
				submit->set_msg_src(_conn_info._corp_id);
				submit->set_registered_delivery(submit_pkg._report);
				submit->set_service_id(submit_pkg._srv_type);
				submit->set_src_id(submit_pkg._sp_num);
				submit->set_TP_pId(0);
				submit->set_valid_time(submit_pkg._expire_time);
			}
			catch(...)
			{
				OutputDebugString("[CClient::submit]encode except!");
			}
	#ifdef _DEBUG
		
			printf("Submit:%s,%s,%s\n",submit_pkg._sp_num, submit_pkg._mobile, submit_pkg._cont);
	#endif
			//	编码
			submit->encode();

			//	发送
			_sender->AddPacket(submit);
		}
		catch(...)
		{
			OutputDebugString("[CClient::submit]except !");
		}
	}
   
	return sys_seq;
}

//	分割短信
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
			//	创建对象
			cmpp_submit *submit = new cmpp_submit;

			//	设置各字段值
			try
			{
				//	区别长度
				if (i == pkg_count)
				{
					cont_len = enc_len % MAX_CONT_ENC_LEN;
				}
				else
				{
					cont_len = MAX_CONT_ENC_LEN;
				}

				//	区别计费
				if (i == 1)
				{
					sprintf(fee_type, "%02d", atoi(submit_pkg._fee_type));
					strncpy(fee_code, submit_pkg._fee_value, MAX_FEE_VALUE_LEN);
				}
				else
				{
					sprintf(fee_type, "%02d", cmpp_submit::FEE_TYPE::FREE);
					strcpy(fee_code, "00000");
				}

				submit->set_at_time(submit_pkg._sched_time);
				submit->set_dest_terminal_id(submit_pkg._mobile, 0);
				submit->set_dest_user_tl(1);
				submit->set_fee_terminal_id(submit_pkg._mobile, 0);

				submit->set_fee_type(fee_type);
				submit->set_fee_code(fee_code);
				submit->set_fee_user_type(3);
				submit->set_link_id(submit_pkg._link_id);

				submit->set_TP_udhi(0);
				submit->set_pk_number(i);
				submit->set_pk_total(pkg_count);
				submit->set_msg_content(enc_buf + (i - 1) * MAX_CONT_ENC_LEN, cont_len, cmpp_pkg::MSG_FMT::UCS2);

				submit->set_msg_id(0);
				submit->set_msg_level(0);
				submit->set_msg_src(_conn_info._corp_id);
				submit->set_registered_delivery(submit_pkg._report);
				submit->set_service_id(submit_pkg._srv_type);
				submit->set_src_id(submit_pkg._sp_num);
				submit->set_TP_pId(0);
				submit->set_valid_time(submit_pkg._expire_time);
			}
			catch(...)
			{
				OutputDebugString("[CClient::submit]encode except!");
			}
	#ifdef _DEBUG
		
			printf("Submit:%s,%s,%s\n",submit_pkg._sp_num, submit_pkg._mobile, submit_pkg._cont);
	#endif
			//	设置序列号
			_cmpp_seq->set_seq(submit);

			//	记录msgid 只记录第一个包
			if (i == 1)
			{
				sys_seq = submit->get_seq_id();
			}
			//	编码
			submit->encode();

			//	发送
			_sender->AddPacket(submit);
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
	unsigned int i_count = 0;
	cmpp_active_test *p_active_test = NULL;
	while (!terminated_)
	{
		Sleep(1000);
		i_count++;
		if (i_count == IDLE_TIME - 5)
		{
			i_count = 0;
			//	发送检测包
			if (_is_connected)
			{
				p_active_test = new cmpp_active_test;
				_cmpp_seq->set_seq(p_active_test);
				p_active_test->encode();
				_sender->SendPacket(p_active_test);
			}
			else
			{
				char err[1024];
				sprintf(err, "定时器发现:%d号连接断开\r\n", _conn.id);
				OutputDebugStr(err);
#ifdef _DEBUG
				printf("%s", err);
#endif
			}//if 			
		}//if
	}//while
	return 0;
}

void CClient::OnRecv(char *buf, int len)
{
	buffer rcv_buf, do_buf;
	cmpp_pkg *p_cmpp_head = (cmpp_pkg *)&_packets.cmpp_head;

	//	先把数据插入缓存
	rcv_buf.buf = buf;
	rcv_buf.len = len;
	_buffer->append(rcv_buf);
	while (_buffer->len() >= cmpp::cmpp_pkg::CMPP_HEAD_LEN)
	{
		//	取得缓冲区首地址
		_buffer->get_buffer(do_buf);
		
		//	解消息头
		p_cmpp_head->decode(do_buf.buf);
		
		//	检查长度是否足够一个包
		if (p_cmpp_head->get_ttl_len() > (unsigned int)_buffer->len())
		{
			//不够一个完整包,退出处理
			break;
		}

		//	按消息类型解包
		switch (p_cmpp_head->get_cmd_id()) 
		{
		case cmpp_pkg::MSG_TYPE::CMPP_SUBMIT_RESP:
			{
				_packets.submit_resp.decode(do_buf.buf);
				do_submit_resp(&_packets.submit_resp);
			}
			break;
		case cmpp_pkg::MSG_TYPE::CMPP_DELIVER:
			{
				_packets.deliver.decode(do_buf.buf);
				do_deliver(&_packets.deliver);
			}
			break;
		case cmpp_pkg::MSG_TYPE::CMPP_ACTIVE_TEST:
			{
				_packets.active_test.decode(do_buf.buf);
				do_active_test(&_packets.active_test);
			}
			break;
		case cmpp_pkg::MSG_TYPE::CMPP_ACTIVE_TEST_RESP:
			{
				_packets.active_test_resp.decode(do_buf.buf);
				do_active_test_resp(&_packets.active_test_resp);
			}
			break;
		case cmpp_pkg::MSG_TYPE::CMPP_CONNECT_RESP:
			{
				_packets.connect_resp.decode(do_buf.buf);
				do_connect_resp(&_packets.connect_resp);
			}
			break;
		case cmpp_pkg::MSG_TYPE::CMPP_CANCEL_RESP:
			{
				_packets.cancel_resp.decode(do_buf.buf);
				do_cancel_resp(&_packets.cancel_resp);
			}
			break;
		case cmpp_pkg::MSG_TYPE::CMPP_QUERY_RESP:
			{
				_packets.query_resp.decode(do_buf.buf);
				do_query_resp(&_packets.query_resp);
			}
			break;
		case cmpp_pkg::MSG_TYPE::CMPP_TERMINATE:
			{
				_packets.terminate.decode(do_buf.buf);
				do_terminate(&_packets.terminate);
			}
			break;
		case cmpp_pkg::MSG_TYPE::CMPP_TERMINATE_RESP:
			{
				_packets.terminate_resp.decode(do_buf.buf);
				do_terminate_resp(&_packets.terminate_resp);
			}
			break;
		default:
			break;
		}

		//	删除处理过的数据
		_buffer->erase(p_cmpp_head->get_ttl_len());
	}		
}

void CClient::do_submit_resp(cmpp_submit_resp *p_submit_resp)
{
	char err[512];
#ifdef _DEBUG
	printf("cmpp_submit_resp \r\n");
	sprintf(err, "conn_id:%d, sys_seq_id:%d, result:%d \r\n",
		_conn.id, p_submit_resp->get_seq_id(),p_submit_resp->get_result());
	OutputDebugStr(err);
	printf("%s", err);
#endif	

	//	提交给用户
	_submit_resp._resp_code = p_submit_resp->get_result();
	_submit_resp._msg_id = p_submit_resp->get_msg_id();
	_submit_resp._seq_id = p_submit_resp->get_seq_id();

	try
	{
		_cmpp->OnSubmitResp(_submit_resp);
		if (_submit_resp._resp_code != 0)
		{
			_cmpp->OnOverSpeed(_sender->get_interval(), _submit_resp._resp_code);
		}
	}
	catch(...)
	{
#ifdef _DEBUG
		printf("OnSubmitResp exception !\r\n");
#endif
	}

	if (p_submit_resp->get_result() == cmpp_submit_resp::RESULT::OK)
	{
		_sender->Response(p_submit_resp->get_seq_id());
	}
	else
	{
#ifdef _DEBUG
		printf("conn_id:%d, sys_seq_id:%d, result:%d \r\n",
			   _conn.id, p_submit_resp->get_seq_id(),p_submit_resp->get_result());
#endif
		sprintf(err, "conn_id:%d, sys_seq_id:%d, result:%d \r\n",
			   _conn.id, p_submit_resp->get_seq_id(),p_submit_resp->get_result());
		OutputDebugString(err);
	}
}

void CClient::do_deliver(cmpp_deliver *p_deliver)
{
	//	回一个cmpp_deliver_resp消息
	cmpp_deliver_resp *p_deliver_resp = new cmpp_deliver_resp;
	p_deliver_resp->set_seq_id(p_deliver->get_seq_id());//序列号
	p_deliver_resp->set_msg_id(p_deliver->get_msg_id());//msg_id
	p_deliver_resp->set_result(0); //正确
	p_deliver_resp->encode();
	_sender->AddPacket(p_deliver_resp,false);

	//	处理这条消息
	if (p_deliver->get_registered_delivery() == cmpp_deliver::IS_REPORT::YES)
	{
#ifdef _DEBUG
		printf("cmpp_report \r\n");
#endif	
		_cmpp_report.decode(p_deliver->get_msg_content());

        //	构造report
		_report._msg_id = _cmpp_report.get_msg_id();

		strcpy(_report._mobile, _cmpp_report.get_dest_terminal_id());
		strcpy(_report._err_code, _cmpp_report.get_stat());
		_report._stat = get_stat(_cmpp_report.get_stat());
		
		//	提交给用户
		try
		{
			_cmpp->OnReport(_report);
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
		printf("cmpp_deliver \r\n");
#endif	
			//	上行短信
			memset(&_deliver, 0, sizeof(_deliver));
			_deliver._coding = p_deliver->get_msg_fmt();

			//	如果是UNICODE 需要解码
			if (_deliver._coding == cmpp_pkg::MSG_FMT::UCS2)
			{
			char tp_ud_decode[255] = {0};
			const char *ptr = p_deliver->get_msg_content();

			for (unsigned int i = 0; i < p_deliver->get_msg_length(); i++)
			{
				if (i % 2 == 0)
					tp_ud_decode[i] = ptr[i + 1];
				else
					tp_ud_decode[i] = ptr[i - 1];
			}

			char tp_ud[256] = {0};
			int tp_ud_length = 255;
			_uc_coding->decode(tp_ud_decode,  p_deliver->get_msg_length(), tp_ud, &tp_ud_length);
			strncpy(_deliver._cont, tp_ud, 
				tp_ud_length > MAX_CONT_LEN ? MAX_CONT_LEN : tp_ud_length);
		}
		else
		{
			memcpy(_deliver._cont, p_deliver->get_msg_content(), p_deliver->get_msg_length());
		}

		strncpy(_deliver._link_id, p_deliver->get_link_id(), MAX_LINK_ID_LEN);

		//	真码
		if (p_deliver->get_src_terminal_type() == 0)
		{	
			strncpy(_deliver._mobile, p_deliver->get_src_terminal_id(), MAX_ADDR_LEN);
		}
		//	伪码
		else
		{
			memcpy(_deliver._mobile, p_deliver->get_src_terminal_id(), MAX_ADDR_LEN);
		}
		strncpy(_deliver._sp_num, p_deliver->get_dest_id(), MAX_ADDR_LEN);

		try
		{
			_cmpp->OnDeliver(_deliver);
		}
		catch (...)
		{
#ifdef _DEBUG
			printf("OnDeliver exception !\r\n");
#endif
		}
	}
}

void CClient::do_active_test(cmpp_active_test *p_active_test)
{
    //	回复active_test_resp
	cmpp_active_test_resp *p_act_resp = new cmpp_active_test_resp;
	p_act_resp->set_seq_id(p_active_test->get_seq_id());
	p_act_resp->set_reserve(0);
	p_act_resp->encode();
	_sender->SendPacket(p_act_resp);
}

void CClient::do_connect_resp(cmpp_connect_resp *p_connect_resp)
{
	//	记录已连接状态
	if (p_connect_resp->get_status() == cmpp_connect_resp::ST_OK)
	{
		_is_connected = true;
		_sender->set_bind(_is_connected);
#ifdef _DEBUG
		printf("CMPP连接成功 id=%d, 状态:%d\r\n", _conn.id, p_connect_resp->get_status());
#endif
 
	}
	else
	{
		char err[1024];		
		sprintf(err, "CMPP连接失败 id=%d, 状态:%d\r\n", _conn.id, p_connect_resp->get_status());
		OutputDebugStr(err);
#ifdef _DEBUG		
		printf("%s", err);
#endif
	}

}

void CClient::do_cancel_resp(cmpp_cancel_resp *p_cancel_resp)
{
#ifdef _DEBUG
	printf("cmpp_cancel_resp \r\n");
#endif
}

void CClient::do_query_resp(cmpp_query_resp *p_query_resp)
{
#ifdef _DEBUG
	printf("cmpp_query_resp \r\n");
#endif
}

void CClient::do_terminate(cmpp_terminate *p_terminate)
{
	char err[1024];
	sprintf(err, "%s\r\n", "recv cmpp_terminate! ");
	OutputDebugStr(err);
#ifdef _DEBUG
	printf("%s", err);
#endif
}

void CClient::do_active_test_resp(cmpp_active_test_resp *p_active_test_resp)
{
#ifdef _DEBUG
	printf("conn_id:%d, cmpp_active_test_resp \r\n",_conn.id);
#endif
}

void CClient::do_terminate_resp(cmpp_terminate_resp *p_terminate_resp)
{
#ifdef _DEBUG
	printf("cmpp_terminate_resp \r\n");
#endif
}

int CClient::get_stat(const char *stat)
{
	int i_stat = STAT_UNKNOW;
	if (strstr(STAT0, stat) != NULL)
	{
		i_stat = STAT0_VALUE;
	}
	else
	{
		if (strstr(STAT1, stat) != NULL)
		{
			i_stat = STAT1_VALUE;
		}
		else
		{
			if (strstr(STAT2, stat) != NULL)
			{
				i_stat = STAT2_VALUE;
			}
			else
			{
				i_stat = STAT_UNKNOW;
			}
		}
	}

	return i_stat;
}


void CClient::OnConnect(tcpip::CTcpClient *tcp_client)
{
	//	CMPP连接
	connect_smg();
}

void CClient::OnDisconnect(tcpip::CTcpClient *tcp_client)
{
	//	断开后重连
	_is_connected = false;
	_sender->set_bind(_is_connected);

	Connect(_svr_ip, _port);
}

void CClient::OnReconnect(tcpip::CTcpClient *tcp_client)
{
	//	暂时不用
}


void CClient::SetInterval(unsigned long interval)
{
	_sender->SetInterval(interval);
}

void CCmpp::SetInterval(unsigned long interval)
{
	for (int i = 0; i < (int)_clients.size(); i++)
	{
		_clients[i]->SetInterval(interval);
	}
}
