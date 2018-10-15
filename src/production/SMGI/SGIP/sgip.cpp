#include "sgip.h"
#include <time.h>

#include "..\lib\codeFactory.h"

#ifdef _DEBUG
#include <iostream>
#endif

static const char	*SUBMIT_GIVEN_VALUE			= "0";
static const char	*SUBMIT_CHARGE_NUM			= "000000000000000000000";
static const unsigned char SUBMIT_USER_COUNT	= 1;
static const unsigned char SUBMIT_AGENT_FLAG	= 0;	//代收费标志 0：应收，1实收
static const unsigned char SUBMIT_TP_PID		= 0;
static const unsigned char SUBMIT_TP_UDHI		= 0;
static const unsigned char SUBMIT_MSG_CODING	= 15;	//8: UCS2, 0: ASCII, 15:GBK
static const unsigned char SUBMIT_MSG_TYPE		= 0;
static const unsigned int  SUBMIT_VALID_MINUTES	= 24*60;

sms::sgip::sgip_protocol g_sgip;

char* submit_time(char* ret, bool is_schedule)
{
	if(!ret) return NULL;

	time_t timer = time(0);
	struct tm *tblock;
	tblock = localtime(&timer);

	memset(ret, 0, 16);
	if(is_schedule)
		sprintf(ret, "%02d%02d%02d%02d%02d%02d032+", 
			tblock->tm_year%100, tblock->tm_mon + 1, tblock->tm_mday, 
			tblock->tm_hour, tblock->tm_min, tblock->tm_sec);
	else	//ExpireTime
	{
		sprintf(ret, "%02d%02d%02d%02d%02d%02d032+", 
			tblock->tm_year%100, tblock->tm_mon + 1, tblock->tm_mday + 1, 
			tblock->tm_hour, tblock->tm_min, tblock->tm_sec);
	}
	return ret;
}

//=================================================================================================
//
//CSgip::CSgipServerClient::CSgipServerClient
//
//=================================================================================================
CSgip::CServerClient::CServerClient(const CBasicSocket::Accepted &ac, 
									CSgip *sgip)
	:CTcpServerClient(ac), _sgip(sgip)
{
	_sender = new CSgip::CSender(this, true, _sgip->_info._slip_size,
		_sgip->_info._resp_wait_time, _sgip->_info._resp_resends, 
		_sgip->_info._interval);
}

CSgip::CServerClient::~CServerClient(void)
{
	if (_sender != NULL)
	{
		delete _sender;
		_sender = NULL;
	}
}

void CSgip::CServerClient::OnRecv(char *buf, int len)
{
	try
	{
		DecodeServer(buf, len);
	}
	catch(...)
	{
	}
}

void CSgip::CServerClient::OnDisconnect(void)
{
}

void CSgip::CServerClient::DecodeServer(char *buf, int len)
{
	try
	{
		bool decode_over = false;

		_server_buff.append(buffer(buf, len));
		unsigned long packet_len, erase_len = 0;

		buffer decode_buf;
		_server_buff.get_buffer(decode_buf);

		while (!decode_over)
		{
			sgip_head head;
			switch (g_sgip.decode(decode_buf, head))
			{
			case sms::sgip::INVALID_PACKET:
				{
					_server_buff.clear();
					decode_over = true;
				}
				break;
			case sms::sgip::HALF_PACKET:
				{
					decode_over = true;			// do nothing.
				}
				break;			
			case sms::sgip::WHOLE_PACKET:
				{					
					packet_len = head.get_pkt_len();
					switch (head.get_cmd_id())
					{
					case SGIP_CMD_BIND:
						{
							sgip_bind bind;
							bind.decode(decode_buf.buf);

							sgip_bind_resp *resp = new sgip_bind_resp(bind.get_seq());
							resp->encode();
							_sender->AddPacket(resp, false);
						}
						break;
					case SGIP_CMD_REPORT:
						{
							sgip_report report;
							report.decode(decode_buf.buf);

							sgip_report_resp *resp = new sgip_report_resp(report.get_seq());
							resp->encode();
							_sender->AddPacket(resp, false);
						
							REPORT_SM sm;
							strncpy(sm._mobile, report.get_user_num(), MAX_ADDR_LEN);
							sprintf(sm._err_code, "%d", report.get_err_code());
							sm._stat = report.get_state();
							sm._msg_id = report.get_report_seq()._seq_id;
							_sgip->OnReport(sm);
						}
						break;
					case SGIP_CMD_DELIVER:
						{
							sgip_deliver deliver;
							deliver.decode(decode_buf.buf);

							sgip_deliver_resp *resp = new sgip_deliver_resp(deliver.get_seq());
							resp->encode();
							_sender->AddPacket(resp, false);

							DELIVER_SM sm;
							strncpy(sm._sp_num, deliver.get_sp_num(), MAX_ADDR_LEN);
							strncpy(sm._mobile, deliver.get_user_num(), MAX_ADDR_LEN);
							sm._coding	= deliver.get_msg_coding();
							strncpy(sm._link_id, deliver.get_link_id(), MAX_LINK_ID_LEN);

							if (sm._coding == SGIP_MSG_CODING_UCS2) // UCS2编码
							{
								char tp_ud_decode[255] = {0};
								const char *ptr = deliver.get_content();

								for (unsigned int i = 0; i < deliver.get_msg_len(); i++)
								{
									if (i % 2 == 0)
										tp_ud_decode[i] = ptr[i + 1];
									else
										tp_ud_decode[i] = ptr[i - 1];
								}

								char tp_ud[256] = {0};
								int tp_ud_length = 255;
								static encoding* x = codeFactory::createEncoding("unicode");
								x->decode(tp_ud_decode, deliver.get_msg_len(), tp_ud, &tp_ud_length);
								strncpy(sm._cont, tp_ud, 
									tp_ud_length > MAX_CONT_LEN ? MAX_CONT_LEN : tp_ud_length);
							}
							else
							{
								strncpy(sm._cont, deliver.get_content(), MAX_CONT_LEN);
							}

							_sgip->OnDeliver(sm);				
						}
						break;
					case SGIP_CMD_UNBIND:
						{
							#ifdef _DEBUG							
								printf("Client(SMG)->Server(SP) unbind.\n");
							#endif
							sgip_unbind unbind;
							unbind.decode(decode_buf.buf);

							sgip_unbind_resp *resp = new sgip_unbind_resp(unbind.get_seq());
							resp->encode();
							_sender->AddPacket(resp, false);

							Disconnect();
						}
						break;
					default:
						break;
					}
					erase_len += packet_len;
					decode_buf.go_ahead(packet_len);
				}
				break;
			}
		}//while

		if (erase_len > 0)
		{
			_server_buff.erase(erase_len);
		}
	}
	catch(...)
	{
		OutputDebugStr("[CSgip::CServerClient]DecodeServer except!\n");
	}
}

//=================================================================================================
CSgip::CServer::CServer(CSgip *sgip) 
	: _sgip(sgip)
{
}

CSgip::CServer::~CServer(void)
{
}

void CSgip::CServer::OnAccept(tcpip::CBasicSocket::Accepted &ac,
							  tcpip::CTcpServerClient **client_sock)
{
	const char *ip_addr = ac.get_ip_addr();
	if (ip_addr != NULL)
	{
#ifdef _DEBUG
		printf("%s:%d connect online.\n", ip_addr, ac.get_port());
#endif
		*client_sock = new CSgip::CServerClient(ac, _sgip);
		if (strcmp(ip_addr, _sgip->_info._smg_ip) == 0)
		{			
			(*client_sock)->Run();
		}
		else
		{
			(*client_sock)->Disconnect();
#ifdef _DEBUG
			printf("Disconnect from %s:%d.\n", ip_addr, ac.get_port());
#endif
		}
	}	
}

//=================================================================================================
//
//	CSgip::CSender
//
//=================================================================================================
CSgip::CSender::CSender(tcpip::CAbsTcpClient *tc, 
						bool is_server,
						long slip_size,
						unsigned short wait_time,
						unsigned short resend_time,
						unsigned long interval) 
	: _client(tc)
	, _slip_window(0)
	, _slip_size(slip_size)
	, _wait_time(wait_time)
	, _resend_time(resend_time)
	, _is_server(is_server)
	, _interval(interval)
	, _is_bind(false)
{
	BeginThread();
}

CSgip::CSender::~CSender(void)
{
	EndThread();
}

void CSgip::CSender::AddPacket(sgip_packet	*pack, 
							   bool			need_confirm)
{
	sgip_packets *p = new sgip_packets;
	p->packet = pack;
	p->t = 0;
	p->r = 0;
	p->need_confirm = need_confirm;
	
	_sender_cs.Lock();
	_packets[pack->get_seq_id()] = p;
	_sender_cs.Unlock();
}

void CSgip::CSender::DeletePacket(sgip_packets *p)
{
	// release packet data
	if ((p != NULL) && (p->packet != NULL))
	{
		//p->packet->free_buf();
		
		try
		{
			// release packet class
			if (p->packet != NULL)
			{
				delete p->packet; 
				p->packet = NULL;
			}
		}
		catch(...)
		{
			;
		}

		// release packets
		delete p;
		p = NULL;
	}
}

void CSgip::CSender::DeletePacket(seq_type resp_seq)
{
	packets_map::iterator it = _packets.find(resp_seq);
	if (it != _packets.end())
	{
		DeletePacket(it->second);
		// delete node
		_packets.erase(it);
	}

	//	dec slip windows
	Release();
}

void CSgip::CSender::Response(seq_type resp_seq)
{
	_sender_cs.Lock();
	DeletePacket(resp_seq);
	_sender_cs.Unlock();
}

bool CSgip::CSender::SendPacket(sgip_packet *packet)
{
	if (packet == NULL)
	{
		return false;
	}
	else
	{
		return _client->Send((char*)packet->get_buf(), packet->get_len());
	}
}

void CSgip::CSender::SendPacket(sgip_packets &pack)
{
	++(pack.r);
	pack.t = time(0);
	if (pack.r > 1)
	{
		DeletePacket(pack.packet->get_seq_id()); 
	}
	else if (SendPacket(pack.packet))
	{
		if (!pack.need_confirm)
		{
			/*if (pack.r > _resend_time)
			{
				char err[MAX_PATH] = {0};
				sprintf(err, "[SGIP::FATAL]send_packet lost seq_id:%lu(ThreadID:%lu).\n",
					pack.packet->get_seq_id(), GetThreadID());
				OutputDebugStr(err);
			}*/
			DeletePacket(pack.packet->get_seq_id()); 
		}
	}
}

// called while client connect
void CSgip::CSender::InitPackets(void)		
{
	_sender_cs.Lock();
	packets_map::iterator p;
	for(p = _packets.begin(); p != _packets.end(); ++p)
	{
		p->second->t = 0;
	}
	_slip_window = 0;
	_sender_cs.Unlock();
}

void CSgip::CSender::RefreshPackets(void)
{
	_sender_cs.Lock();
	packets_map::iterator p;
	for (p = _packets.begin(); p != _packets.end(); ++p)
	{
		p->second->t = 0;
		if (_is_server)
		{
			DeletePacket(p->second);
		}
	}
	if (_is_server)
	{
		_packets.clear();
	}
	_slip_window = 0;
	_sender_cs.Unlock();
}

bool CSgip::CSender::AddRef(void)
{	
	if (_slip_size > _slip_window)
	{
		::InterlockedIncrement(&_slip_window);
		return true;
	}
	else
	{
		return false;
	}
}

void CSgip::CSender::Release(void)
{
	if (_slip_window > 0)
	{
		::InterlockedDecrement(&_slip_window); 
	}
}

unsigned int CSgip::CSender::ThreadEntry(void)
{
	bool can_send = true;
	packets_map::iterator p, q; 
	while (!terminated_)
	{
		if (_is_server || _is_bind)
		{
			_sender_cs.Lock();
			try
			{
				if(!_packets.empty())
				{
					p = _packets.begin();
					while(p != _packets.end())
					{
						q = p; 
						++q;
						if((p->second)->t == 0)
						{
							can_send = true;
							if(p->second->need_confirm)
							{
								can_send = AddRef();
							}
							if (can_send)
							{
								SendPacket(*(p->second));
								Sleep(_interval);
							}
							else
							{
								Sleep(1);
							}
						}
						// resend
						else if (time(0) - (p->second)->t >= _wait_time)
						{
							SendPacket(*(p->second));
							Sleep(_interval);
						}
						p = q;
					}
				}
			}
			catch(...)
			{
				int err_num = ::GetLastError();
			}
			_sender_cs.Unlock();
		}
		Sleep(1);
	}
	return 0;
}

//=================================================================================================
//
//	CSgip::CKeepAlive
//
//=================================================================================================
CSgip::CKeepAlive::CKeepAlive(CClient *client)
	: _client(client)
{
	BeginThread();
}

CSgip::CKeepAlive::~CKeepAlive(void)
{
	EndThread();
}

unsigned int CSgip::CKeepAlive::ThreadEntry(void)
{
	int count = 0;
	while (!terminated_)
	{
		if (count > IDLE_TIME - 5)
		{
			count = 0;
			_client->KeepAlive();
		}
		else
		{
			++count;
			Sleep(1000);
		}
	}
	return 0;
}

//=================================================================================================
//
//	CClient
//
//=================================================================================================
CSgip::CClient::CClient(CSgip *sgip)
	: _sgip(sgip)
	, _keep_alive(NULL)
	, _sender(NULL)
	, _uc_coding(NULL)
{
	_keep_alive = new CSgip::CKeepAlive(this);
	_sender = new CSgip::CSender(this, false, _sgip->_info._slip_size, 
		_sgip->_info._resp_wait_time, _sgip->_info._resp_resends, 
		_sgip->_info._interval);

		//  解码对象
	_uc_coding = codeFactory::createEncoding("unicode");
}

CSgip::CClient::~CClient(void)
{
	if (_keep_alive != NULL)
	{
		delete _keep_alive;
		_keep_alive = NULL;
	}

	if (_sender != NULL)
	{
		delete _sender;
		_sender = NULL;
	}	

	if (_uc_coding != NULL)
	{
		delete _uc_coding;
		_uc_coding = NULL;
	}
}

//	SgipClient
void CSgip::CClient::OnRecv(char *buf, int len)
{
	try
	{
		DecodeClient(buf, len); 
	}
	catch(...)
	{
	}
}

void CSgip::CClient::OnConnect(tcpip::CTcpClient *client)
{
#ifdef _DEBUG
	printf("on_connect\n\n");
#endif

	char cont[1024] = {0};
	sprintf(cont, "[SGIP]OnConnect thread_id=%lu.\n", GetThreadID());
	OutputDebugStr(cont);

	Bind();
}

void CSgip::CClient::OnDisconnect(tcpip::CTcpClient *client)
{
#ifdef _DEBUG
	printf("on_disconnect\n\n");
#endif

	_sender->set_bind(false);
	Unbind();
}

void CSgip::CClient::Bind(void)
{
	_sender->RefreshPackets();
	sgip_bind *x = new sgip_bind(_sgip->_sgip_seq_id->get_seq());
	x->set_login_type(SGIP_LOGIN_TYPE_SP2SMG);
	x->set_login_name(_sgip->_info._login_name);
	x->set_login_pswd(_sgip->_info._login_pswd);
	x->encode();
	_sender->SendPacket(x);
}

void CSgip::CClient::Unbind(void)
{
	sgip_unbind *x = new sgip_unbind(_sgip->_sgip_seq_id->get_seq());
	x->encode();
	_sender->SendPacket(x);
}

void CSgip::CClient::KeepAlive(void)
{
	if (_sgip->_info._smg_type == SMG_TYPE_SGIP_OPENET)
	{
		sgip_keepalive *x = new sgip_keepalive(_sgip->_sgip_seq_id->get_seq());
		x->encode();
		_sender->SendPacket(x);
	}
}

unsigned long CSgip::CClient::submit_norm(SUBMIT_SM &submit_pkg)
{
	sgip_submit	*x = new sgip_submit(_sgip->_sgip_seq_id->get_seq());
	if (0 == strlen(submit_pkg._sp_num))
	{
		x->set_sp_num(_sgip->_info._sp_num);
	}
	else
	{
		x->set_sp_num(submit_pkg._sp_num);
	}

    if (0 == strlen(submit_pkg._charge_num))
	{
		x->set_charge_num(submit_pkg._mobile);
	}
	else
	{
		x->set_charge_num(submit_pkg._charge_num);
	}

	x->set_user_count(SUBMIT_USER_COUNT);
	x->set_user_num(submit_pkg._mobile);
	x->set_corp_id(_sgip->_info._corp_id);
	x->set_srv_type(submit_pkg._srv_type);
	x->set_fee_type((unsigned char)atol(submit_pkg._fee_type));	
	x->set_fee_value(submit_pkg._fee_value);
	x->set_given_value(SUBMIT_GIVEN_VALUE);
	x->set_agent_flag(submit_pkg._agent_flag);
	x->set_morelateto_mtflag(submit_pkg._mt_flag);
	x->set_priority(submit_pkg._prio);

	//	add by 2005-10-14
	x->set_expire_time(submit_pkg._expire_time);
	x->set_schedule_time(submit_pkg._sched_time);
	x->set_report_flag(submit_pkg._report);
	x->set_msg_coding(submit_pkg._coding);
	x->set_msg_len((unsigned long)strlen(submit_pkg._cont));
	x->set_content(submit_pkg._cont);
	x->set_link_id(submit_pkg._link_id);
	x->encode();
	_sender->AddPacket(x);
	return x->get_seq_id();
}

//	长短信
unsigned long  CSgip::CClient::submit_long_sm(SUBMIT_SM & submit_pkg)
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

	
	if (_sgip->_info._long_sm == SMG_LONG_SM_6)
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
			sgip_submit	*x = new sgip_submit(_sgip->_sgip_seq_id->get_seq());

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
					sprintf(fee_type, "%02d", 1);
					strcpy(fee_code, "00000");
				}

				//	构造协议头
				if (_sgip->_info._long_sm == SMG_LONG_SM_6)
				{
					long_head_6[3] = (char)(x->get_seq_id() % 100);
					long_head_6[4] = pkg_count;
					long_head_6[5] = i;

					memcpy(cont, long_head_6, 6);
				}
				else
				{
					long_head_7[4] = (char)(x->get_seq_id() % 100);
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

				x->set_content(cont, head_len + cont_len);
				x->set_msg_len(head_len + cont_len);
				x->set_tp_udhi(1);

				x->set_fee_type((unsigned char)atol(fee_type));	
				x->set_fee_value(fee_code);


				if (0 == strlen(submit_pkg._sp_num))
				{
					x->set_sp_num(_sgip->_info._sp_num);
				}
				else
				{
					x->set_sp_num(submit_pkg._sp_num);
				}

				if (0 == strlen(submit_pkg._charge_num))
				{
					x->set_charge_num(submit_pkg._mobile);
				}
				else
				{
					x->set_charge_num(submit_pkg._charge_num);
				}

				x->set_user_count(SUBMIT_USER_COUNT);
				x->set_user_num(submit_pkg._mobile);
				x->set_corp_id(_sgip->_info._corp_id);
				x->set_srv_type(submit_pkg._srv_type);
				x->set_given_value(SUBMIT_GIVEN_VALUE);
				x->set_agent_flag(submit_pkg._agent_flag);
				x->set_morelateto_mtflag(submit_pkg._mt_flag);
				x->set_priority(submit_pkg._prio);

				//	add by 2005-10-14
				x->set_expire_time(submit_pkg._expire_time);
				x->set_schedule_time(submit_pkg._sched_time);
				x->set_report_flag(submit_pkg._report);
				x->set_msg_coding(sms::sgip::SGIP_MSG_CODING_UCS2);
				x->set_link_id(submit_pkg._link_id);
			}
			catch(...)
			{
				OutputDebugString("[CClient::submit]encode except!");
			}
	#ifdef _DEBUG
		
			printf("Submit:%s,%s,%s\n",submit_pkg._sp_num, submit_pkg._mobile, submit_pkg._cont);
	#endif
			x->encode();

			//	只有第一个包需要回复
			if (i == 1) 
			{
				_sender->AddPacket(x);
				sys_seq = x->get_seq_id();
			}
			else
			{
				_sender->AddPacket(x, false);
			}

		}
		catch(...)
		{
			OutputDebugString("[CClient::submit]except !");
		}
	}
   
	return sys_seq;
}

//	分割短信
unsigned long  CSgip::CClient::submit_cut_sm(SUBMIT_SM & submit_pkg)
{
	unsigned long sys_seq;
	char enc_buf[MAX_LONG_SM_LEN * 2 + 1] = { 0 };
	char cont[MAX_CONT_ENC_LEN + 1] = {0};
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
			sgip_submit	*x = new sgip_submit(_sgip->_sgip_seq_id->get_seq());

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
					sprintf(fee_type, "%02d", 1);
					strcpy(fee_code, "00000");
				}
	

				//	区别长度
				if (i == pkg_count)
				{
					cont_len = enc_len % (MAX_CONT_ENC_LEN);
				}
				else
				{
					cont_len = MAX_CONT_ENC_LEN;
				}
				memcpy(cont, enc_buf + (i - 1) * MAX_CONT_ENC_LEN, cont_len);

				x->set_content(cont, cont_len);
				x->set_msg_len(cont_len);
				x->set_tp_udhi(1);

				x->set_fee_type((unsigned char)atol(fee_type));	
				x->set_fee_value(fee_code);


				if (0 == strlen(submit_pkg._sp_num))
				{
					x->set_sp_num(_sgip->_info._sp_num);
				}
				else
				{
					x->set_sp_num(submit_pkg._sp_num);
				}

				if (0 == strlen(submit_pkg._charge_num))
				{
					x->set_charge_num(submit_pkg._mobile);
				}
				else
				{
					x->set_charge_num(submit_pkg._charge_num);
				}

				x->set_user_count(SUBMIT_USER_COUNT);
				x->set_user_num(submit_pkg._mobile);
				x->set_corp_id(_sgip->_info._corp_id);
				x->set_srv_type(submit_pkg._srv_type);
				x->set_given_value(SUBMIT_GIVEN_VALUE);
				x->set_agent_flag(submit_pkg._agent_flag);
				x->set_morelateto_mtflag(submit_pkg._mt_flag);
				x->set_priority(submit_pkg._prio);

				//	add by 2005-10-14
				x->set_expire_time(submit_pkg._expire_time);
				x->set_schedule_time(submit_pkg._sched_time);
				x->set_report_flag(submit_pkg._report);
				x->set_msg_coding(sms::sgip::SGIP_MSG_CODING_UCS2);
				x->set_link_id(submit_pkg._link_id);
			}
			catch(...)
			{
				OutputDebugString("[CClient::submit]encode except!");
			}
	#ifdef _DEBUG
		
			printf("Submit:%s,%s,%s\n",submit_pkg._sp_num, submit_pkg._mobile, submit_pkg._cont);
	#endif
			x->encode();

			//	只有第一个包需要回复
			if (i == 1) 
			{
				sys_seq = x->get_seq_id();
				_sender->AddPacket(x);
			}
			else
			{
				_sender->AddPacket(x, false);
			}

		}
		catch(...)
		{
			OutputDebugString("[CClient::submit]except !");
		}
	}
   
	return sys_seq;
}

//	提交短信
unsigned long CSgip::CClient::Submit(SUBMIT_SM &submit_pkg)
{
	//先检查网络连接状况,如果连接不存在则,提交失败
	unsigned long sys_seq = 0;
	char enc_buf[MAX_LONG_SM_LEN * 2 + 1] = { 0 };
	int enc_len = MAX_LONG_SM_LEN * 2;

	_uc_coding->encode(submit_pkg._cont, (int)strlen(submit_pkg._cont), enc_buf, &enc_len);

	if (enc_len <= MAX_CONT_ENC_LEN)
	{
		//	不需要拆分
		sys_seq = submit_norm(submit_pkg);
	}
	else
	{
		//	需要拆分
		if	(_sgip->_info._long_sm == SMS_LONG_SM_NO) 
		{
			//	短信分割
            sys_seq = submit_norm(submit_pkg);
		}
		else
		{
			//	长短信
            sys_seq = submit_long_sm(submit_pkg);
		}
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

// 解SP->SMG连接的包, Submit
void CSgip::CClient::DecodeClient(char *buf, int len)	
{
	try
	{
		bool decode_over = false;

		_buff.append(buffer(buf, len));
		unsigned long packet_len = 0;
		unsigned long erase_len = 0;

		buffer decode_buf;
		_buff.get_buffer(decode_buf);

		while (!decode_over)
		{
			sgip_head head;
			switch (g_sgip.decode(decode_buf, head))
			{
			case INVALID_PACKET:
				{
					_buff.clear();
					decode_over = true;
				}
				break;
			case HALF_PACKET:
				{
					decode_over = true;			// do nothing.
				}
				break;			
			case WHOLE_PACKET:
				{
					packet_len = head.get_pkt_len();
					switch (head.get_cmd_id())
					{
					case SGIP_CMD_BIND_RESP:
						{
							sgip_bind_resp resp;
							resp.decode(decode_buf.buf);
							#ifdef _DEBUG							
								printf("bind_resp.result = %d.\n", resp.get_result());
							#endif
							if (resp.get_result() != 0)
							{
								char err[MAX_PATH] = {0};
								sprintf(err, "[SGIP::FATAL]bind_resp.result = %d.\n", resp.get_result());
								OutputDebugStr(err);
							}
							else
							{
								_sender->set_bind(true);
							}
						}
						break;
					case SGIP_CMD_SUBMIT_RESP:
						{
							sgip_submit_resp resp;
							resp.decode(decode_buf.buf);
							#ifdef _DEBUG
								printf("submit_resp.result = %d.\n", resp.get_result());
							#endif

							//	
							SUBMIT_RESP sm;
							sm._seq_id	= resp.get_seq_id();
							sm._resp_code = resp.get_result();
							sm._msg_id	= sm._seq_id;
							
							_sgip->OnSubmitResp(sm);

							if (resp.get_result() == 0)
							{
								_sender->Response(head.get_seq_id());
							}
							else
							{
								char err[MAX_PATH] = {0};
								sprintf(err, "[SGIP::FATAL]seq_id=%lu,submit_resp.result=%d.\n",
									head.get_seq_id(), resp.get_result());
								OutputDebugStr(err);

								switch (_sgip->_info._smg_type)
								{
								case SMG_TYPE_SGIP_STD:
									{
										switch (resp.get_result())
										{
											case SGIP_RESP_ERR_CODE_5:
											case SGIP_RESP_ERR_CODE_6:
											case SGIP_RESP_ERR_CODE_7:
											case SGIP_RESP_ERR_CODE_8:
											case SGIP_RESP_ERR_CODE_9:
											case SGIP_RESP_ERR_CODE_21:
											case SGIP_RESP_ERR_CODE_22:
											case SGIP_RESP_ERR_CODE_23:
											case SGIP_RESP_ERR_CODE_27:
												{
													_sender->Response(head.get_seq_id());
												}
												break;
											default:
												break;
										}
									}
									break;
								case SMG_TYPE_SGIP_OPENET:
									{
										//	对这些错误不需要重发
										switch (resp.get_result())
										{
											case SGIP_RESP_ERR_CODE_5:
											case SGIP_RESP_ERR_CODE_6:
											case SGIP_RESP_ERR_CODE_7:
											case SGIP_RESP_ERR_CODE_8:
											case SGIP_RESP_ERR_CODE_23:
											case SGIP_RESP_ERR_CODE_90:
											case SGIP_RESP_ERR_CODE_97:
												{
													_sender->Response(head.get_seq_id());
												}
												break;
											default:
												break;
										}
									}
									break;
								case SMG_TYPE_SGIP_INTRINT:
									{
										//	对这些错误不需要重发
										switch (resp.get_result())
										{
										case SGIP_RESP_ERR_CODE_5:
										case SGIP_RESP_ERR_CODE_6:
										case SGIP_RESP_ERR_CODE_8:
										case SGIP_RESP_ERR_CODE_9:
											{
												_sender->Response(head.get_seq_id());
											}
											break;
										default:
											break;
										}
									}
									break;
								default:
									break;
								}
							}
						}
						break;
					case SGIP_CMD_UNBIND:
						{
							#ifdef _DEBUG							
								printf("Server(SMG)->Client(SP) unbind.\n");
							#endif

							if (_sgip->_info._smg_type != SMG_TYPE_SGIP_INTRINT)
							{
								sgip_unbind unbind;
								unbind.decode(decode_buf.buf);

								sgip_unbind_resp *resp = new sgip_unbind_resp(unbind.get_seq());
								resp->encode();
								_sender->AddPacket(resp, false);

								_sender->set_bind(false);
							}
							Disconnect();
						}
						break;
					case SGIP_CMD_UNBIND_RESP:
						{
							sgip_unbind_resp resp;
							resp.decode(decode_buf.buf);

							#ifdef _DEBUG
								printf("unbind_resp.result = 0.\n");
							#endif
						}
						break;
					case SGIP_CMD_KEEPALIVE_RESP:
						{
							sgip_keepalive_resp resp;
							resp.decode(decode_buf.buf);

							#ifdef _DEBUG
								printf("keepalive_resp.result = 0.\n");
							#endif
						}
						break;
					default:
						{
						}
						break;
					}
					erase_len += packet_len;
					decode_buf.go_ahead(packet_len);
				}
				break;
			}
		}//while

		if (erase_len > 0)
		{
			_buff.erase(erase_len);
		}
	}
	catch(...)
	{
		OutputDebugStr("[CSgip::CClient]DecodeClient except!\n");
	}
}

//=================================================================================================
//
//	CSgip
//
//=================================================================================================
CSgip::CSgip() 
	: _server(NULL)
	//, _high_client(NULL)
	, _client_idx(0)
	, _sgip_seq_id(NULL)
{
}

CSgip::CSgip(CONNECT_INFO &info)
	: _server(NULL)
	//, _high_client(NULL)
	, _client_idx(0)
	, _info(info)
	, _sgip_seq_id(NULL)
{
	Startup(info);
}

CSgip::~CSgip()
{
	/*if (_high_client != NULL)
	{
		delete _high_client;
		_high_client = NULL;
	}*/

	for (unsigned long i = 0; i < _client.size(); ++i)
	{
		if (_client[i] != NULL)
		{
			delete _client[i];
			_client[i] = NULL;
		}
	}
	
	if (_server != NULL)
	{
		delete _server;
		_server = NULL;
	}
}

bool CSgip::Startup(CONNECT_INFO &info)
{
	bool ret = true;
	try
	{
		_info = info;
		_sgip_seq_id = new sgip_seq_id(info._src_id, info._seq_id, 
			info._min_seq_id, info._max_seq_id);

		_server = new CServer(this);
		if (_server->Listen(info._sp_port))
		{
            /*_high_client = new CClient(this);
			_high_client->Connect(info._smg_ip, info._smg_port_mo);

			char high_cont[1024] = {0};
			sprintf(high_cont, "[SGIP]High client thread_id=%lu, sender thread_id=%lu.\n", 
				_high_client->GetThreadID(), _high_client->GetSenderThreadID());
			OutputDebugStr(high_cont);*/
			
			for (short i = 0; i < info._links; ++i)
			{
				CClient *x = new CClient(this);
				x->Connect(info._smg_ip, info._smg_port_mo);

				char cont[1024] = {0};
				sprintf(cont, "[SGIP]client thread_id=%lu, sender thread_id=%lu.\n",
					x->GetThreadID(), x->GetSenderThreadID());
				OutputDebugStr(cont);
				
				_client.push_back(x);
			}
		}
		else
		{
			ret = false;
		}
	}
	catch(...)
	{
		ret = false;
	}
	return ret;
}

unsigned long CSgip::Submit(SUBMIT_SM &submit, bool prio)
{
	unsigned long sm_id = 0;
	Process::CAutoLock lock(&_client_cs);
	if (_client_idx > _info._links - 1)
	{
		_client_idx = 0;
	}
	
	if ((_client[_client_idx])->IsConnected())
	{
		sm_id = (_client[_client_idx])->Submit(submit);
		++_client_idx;
	}
	
	return sm_id;
}
