#include "smpp.h"
#include <time.h>
#include "..\lib\codeFactory.h"

using namespace sms;
using namespace sms::smpp;
using namespace sms::smgi;
using namespace Process;

static const char	*SUBMIT_GIVEN_VALUE			= "0";
static const char	*SUBMIT_CHARGE_NUM			= "000000000000000000000";
static const unsigned char SUBMIT_AGENT_FLAG	= 0;	//代收费标志 0：应收，1实收
static const unsigned char SUBMIT_TP_PID		= 0;
static const unsigned char SUBMIT_TP_UDHI		= 0;
static const unsigned char SUBMIT_MSG_CODING	= 8;	//8: UCS2, 0: ASCII, 15:GBK
static const unsigned char SUBMIT_MSG_TYPE		= 0;
static const unsigned int  SUBMIT_VALID_MINUTES	= 24*60;
static const unsigned char SUBMIT_ESM_CLASS		= 0;

unsigned __int64 hton64(unsigned __int64 host64)
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

unsigned __int64 ntoh64(unsigned __int64 net64)
{
	return hton64(net64);
}

sms::smpp::smpp_protocol g_smpp;

//=================================================================================================
//
//	CSgip::CSender
//
//=================================================================================================
CSmpp::CSender::CSender(tcpip::CAbsTcpClient *tc,
						long slip_size,
						unsigned long interval) 
	: _client(tc)
	, _slip_window(0)
	, _slip_size(slip_size)
	, _interval(interval)
{
	BeginThread();
}

CSmpp::CSender::~CSender()
{
	EndThread();
}

void CSmpp::CSender::AddPacket(sms::smpp::smpp_packet	*pack, 
							   bool			need_confirm)
{
	smpp_packets *p = new smpp_packets;
	p->packet = pack;
	p->t = 0;
	p->r = 0;
	p->need_confirm = need_confirm;
	
	_sender_cs.Lock();
	_packets[pack->get_seq_id()] = p;
	_sender_cs.Unlock();
}

void CSmpp::CSender::DeletePacket(smpp_packets *p)
{
	// release packet data
	if ((p != NULL) && (p->packet != NULL))
	{
		//p->packet->release();
		
		// release packet class
		if (p->packet != NULL)
		{
			delete p->packet; 
			p->packet = NULL;
		}

		// release packets
		delete p;
		p = NULL;
	}
}

void CSmpp::CSender::DeletePacket(seq_type resp_seq)
{
	packets_map::iterator it = _packets.find(resp_seq);
	if (it != _packets.end())
	{
		DeletePacket(it->second);
		// delete node
		_packets.erase(it);
	}

	Release();
}

void CSmpp::CSender::Response(seq_type resp_seq)
{
	_sender_cs.Lock();
	DeletePacket(resp_seq);
	_sender_cs.Unlock();
}

bool CSmpp::CSender::SendPacket(sms::smpp::smpp_packet *packet)
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

void CSmpp::CSender::SendPacket(smpp_packets &pack)
{
	if (SendPacket(pack.packet))
	{
		++(pack.r);
		pack.t = time(0);
		if(!pack.need_confirm || pack.r > RESEND_TIMES)
		{
			if (pack.r > RESEND_TIMES)
			{
				char err[MAX_PATH] = {0};
				sprintf(err, "[SMPP::FATAL]send_packet lost seq_id:%lu(ThreadID:%lu).\n",
					pack.packet->get_seq_id(), GetThreadID());
				OutputDebugStr(err);
			}
			DeletePacket(pack.packet->get_seq_id()); 
		}
	}
}

// called while client connect
void CSmpp::CSender::InitPackets()		
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

void CSmpp::CSender::RefreshPackets(void)
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

bool CSmpp::CSender::AddRef(void)
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

void CSmpp::CSender::Release(void)
{
	if (_slip_window > 0)
	{
		::InterlockedDecrement(&_slip_window); 
	}
}

unsigned int CSmpp::CSender::ThreadEntry(void)
{
	bool can_send = true;
	packets_map::iterator p, q; 
	while (!terminated_)
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
					else if (time(0) - (p->second)->t >= WAIT_TIME)
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
		Sleep(1);
	}
	return 0;
}

//=================================================================================================
//
//	CSgip::CKeepAlive
//
//=================================================================================================
CSmpp::CKeepAlive::CKeepAlive(CClient *client)
	: _client(client)
{
	BeginThread();
}

CSmpp::CKeepAlive::~CKeepAlive(void)
{
	EndThread();
}

unsigned int CSmpp::CKeepAlive::ThreadEntry(void)
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
CSmpp::CClient::CClient(CSmpp *smpp, unsigned char bind_type)
	: _smpp(smpp)
	, _sender(NULL)
	, _keep_alive(NULL)
	, _bind_type(bind_type)
{
	_sender = new CSmpp::CSender(this, 
		_smpp->_info._slip_size, _smpp->_info._interval);
	_keep_alive = new CSmpp::CKeepAlive(this);
}

CSmpp::CClient::~CClient(void)
{
	if (_keep_alive != NULL)
	{
		delete _keep_alive;
		_keep_alive = NULL;
	}

	Unbind();
	Stop();

	if (_sender != NULL)
	{
		delete _sender;
		_sender = NULL;
	}
}

void CSmpp::CClient::OnRecv(char *buf, int len)
{
	try
	{
		Decode(buf, len); 
	}
	catch(...)
	{
#ifdef _DEBUG
		int i = 0;
#endif
	}
}

void CSmpp::CClient::OnConnect(tcpip::CTcpClient *client)
{
#ifdef _DEBUG
	printf("on_connect\n\n");
#endif

	switch (_bind_type)
	{
	case BIND_TYPE_MO:
		{
			BindReceiver();
		}
		break;
	case BIND_TYPE_MT:
		{
			BindTransmitter();
		}
		break;
	case BIND_TYPE_MOMT:
		{
			BindTransceiver();
		}
		break;
	default:
		{
			BindReceiver();
		}
		break;
	}
}

void CSmpp::CClient::OnDisconnect(tcpip::CTcpClient *client)
{
#ifdef _DEBUG
	printf("on_disconnect\n\n");
#endif
	Unbind();
}

void CSmpp::CClient::BindReceiver(void)
{
	_sender->RefreshPackets();
	smpp_bind_receiver *x = new smpp_bind_receiver(_smpp->_smpp_seq_id->inc_seq_id());
	x->set_sys_id(_smpp->_info._login_name);
	x->set_pswd(_smpp->_info._login_pswd);
	x->encode();
	_sender->SendPacket(x);
}

void CSmpp::CClient::BindTransmitter(void)
{
	_sender->RefreshPackets();
	smpp_bind_transmitter *x = new smpp_bind_transmitter(_smpp->_smpp_seq_id->inc_seq_id());
	x->set_sys_id(_smpp->_info._login_name);
	x->set_pswd(_smpp->_info._login_pswd);
	x->encode();
	_sender->SendPacket(x);
}

void CSmpp::CClient::BindTransceiver(void)
{
	_sender->RefreshPackets();
	smpp_bind_transceiver *x = new smpp_bind_transceiver(_smpp->_smpp_seq_id->inc_seq_id());
	x->set_sys_id(_smpp->_info._login_name);
	x->set_pswd(_smpp->_info._login_pswd);
	x->encode();
	_sender->SendPacket(x);
}

void CSmpp::CClient::Unbind(void)
{
	_sender->RefreshPackets();
	smpp_unbind *x = new smpp_unbind(_smpp->_smpp_seq_id->inc_seq_id());
	x->encode();
	_sender->SendPacket(x);
}

void CSmpp::CClient::KeepAlive(void)
{
	smpp_enquire_link *x = new smpp_enquire_link(_smpp->_smpp_seq_id->inc_seq_id());
	x->encode();
	_sender->SendPacket(x);
}

unsigned long CSmpp::CClient::Submit(SUBMIT_SM &submit)
{
	smpp_submit *x = new smpp_submit(_smpp->_smpp_seq_id->inc_seq_id());
	x->set_srv_type(submit._srv_type);
	x->set_src_addr_npi(1);
	x->set_src_addr_ton(1);
	if (0 == strlen(submit._sp_num))
	{
		x->set_src_addr(_smpp->_info._sp_num);
	}
	else
	{
		x->set_src_addr(submit._sp_num);
	}
	x->set_dst_addr_npi(1);
	x->set_dst_addr_ton(1);
	x->set_dst_addr(submit._mobile);
	x->set_esm_class(SUBMIT_ESM_CLASS);
	x->set_pid(SUBMIT_TP_PID);
	x->set_prio(submit._prio);
	x->set_sched_dt(submit._sched_time);
	x->set_report(submit._report);
	x->set_coding(submit._coding);

	if (submit._coding == SMPP_MSG_CODING_UCS2)
	{
		char tp_ud[255] = {0};
		int tp_ud_length = 255;
		static encoding *en = codeFactory::createEncoding("unicode");
		en->encode(submit._cont, (int)strlen(submit._cont), tp_ud, &tp_ud_length);

		char cont[255] = {0};
		for (int i = 0; i < tp_ud_length; i++)
		{
			if (i % 2 == 0)
				cont[i] = tp_ud[i + 1];
			else
				cont[i] = tp_ud[i - 1];
		}

		x->set_sm_len(tp_ud_length);
		x->set_sm_cont(cont);
	}
	else
	{
		x->set_sm_len((unsigned char)strlen(submit._cont));
		x->set_sm_cont(submit._cont);
	}

	x->encode();
	_sender->AddPacket(x); 
	return x->get_seq_id();
}

// 解SP->SMG连接的包, Submit
void CSmpp::CClient::Decode(char *buf, int len)	
{
	try
	{
		bool decode_over = false;

		_buff.append(buffer(buf, len));
		unsigned long packet_len, erase_len = 0;

		smpp_head head;
		buffer decode_buf;

		_buff.get_buffer(decode_buf);

		while (!decode_over)
		{
			switch (g_smpp.decode(decode_buf, head))
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
					case SMPP_CMD_BIND_RECEIVER_RESP:
						{
							smpp_bind_receiver_resp resp;
							resp.decode(decode_buf.buf);

							if (resp.get_cmd_stat() != 0)
							{
								char err[MAX_PATH] = {0};
								sprintf(err, "[SMPP::FATAL]bind_receiver_resp.result = %d.\n", resp.get_cmd_stat());
								OutputDebugStr(err);
								Sleep(1000);
							}
							#ifdef _DEBUG							
								printf("bind_receiver_resp.result = %lu.\n", resp.get_cmd_stat());
							#endif						
						}
						break;
					case SMPP_CMD_BIND_TRANSMITTER_RESP:
						{
							smpp_bind_transmitter_resp resp;
							resp.decode(decode_buf.buf);

							if (resp.get_cmd_stat() != 0)
							{
								char err[MAX_PATH] = {0};
								sprintf(err, "[SMPP::FATAL]bind_transmitter_resp.result = %d.\n", resp.get_cmd_stat());
								OutputDebugStr(err);
								Sleep(1000);
							}
							#ifdef _DEBUG							
								printf("bind_transmitter_resp.result = %lu.\n", resp.get_cmd_stat());
							#endif	
						}
						break;
					case SMPP_CMD_BIND_TRANSCEIVER_RESP:
						{
							smpp_bind_transceiver_resp resp;
							resp.decode(decode_buf.buf);

							if (resp.get_cmd_stat() != 0)
							{
								char err[MAX_PATH] = {0};
								sprintf(err, "[SMPP::FATAL]bind_transceiver_resp.result = %d.\n", resp.get_cmd_stat());
								OutputDebugStr(err);
								Sleep(1000);
							}
							#ifdef _DEBUG							
								printf("bind_transceiver_resp.result = %lu.\n", resp.get_cmd_stat());
							#endif	
						}
						break;
					case SMPP_CMD_SUBMIT_RESP:
						{
							smpp_submit_resp resp;
							resp.decode(decode_buf.buf);

							#ifdef _DEBUG
								printf("submit_resp.result = %d.\n", resp.get_cmd_stat());
							#endif

							if (resp.get_cmd_stat() == 0)
							{
								_sender->Response(head.get_seq_id());
								
								SUBMIT_RESP x;
								x._seq_id = head.get_seq_id();
								memcpy(&x._msg_id, resp.get_msg_id(), sizeof(unsigned __int64));
								_smpp->OnSubmitResp(x);
							}
							else
							{
								char err[MAX_PATH] = {0};
								sprintf(err, "[SMPP::FATAL]seq_id=%lu,submit_resp.result=%d.\n",
									head.get_seq_id(), resp.get_cmd_stat());
								OutputDebugStr(err);
							}
						}
						break;
					case SMPP_CMD_DELIVER:
						{
							smpp_deliver deliver;
							deliver.decode(decode_buf.buf);

							//smpp_deliver_resp *resp = new smpp_deliver_resp(_smpp->_smpp_seq_id->inc_seq_id());
							smpp_deliver_resp *resp = new smpp_deliver_resp(deliver.get_seq_id());
							resp->encode();
                            _sender->AddPacket(resp, false);
							
							if (deliver.get_report() == SMPP_REPORT_YES)
							{
								smpp_report report;
								report.decode(deliver.get_sm_cont());

								REPORT_SM sm;
								strncpy(sm._mobile, deliver.get_src_addr(), MAX_ADDR_LEN);
								strncpy(sm._err_code, report.get_err(), MAX_ERR_CODE_LEN);
								sm._stat = get_stat(report.get_stat());
								//sm._msg_id = _atoi64(report.get_msg_id());
								memcpy(&sm._msg_id, report.get_msg_id(), sizeof(unsigned __int64));
								_smpp->OnReport(sm);
							}
							else
							{
								DELIVER_SM sm;
								strncpy(sm._mobile, deliver.get_src_addr(), MAX_ADDR_LEN);
								strncpy(sm._sp_num, deliver.get_dst_addr(), MAX_ADDR_LEN);
								sm._coding = deliver.get_coding();

								if (sm._coding == SMPP_MSG_CODING_UCS2) // UCS2编码
								{
									char tp_ud_decode[255] = {0};
									const char *ptr = deliver.get_sm_cont();

									for (unsigned int i = 0; i < deliver.get_sm_len(); i++)
									{
										if (i % 2 == 0)
											tp_ud_decode[i] = ptr[i + 1];
										else
											tp_ud_decode[i] = ptr[i - 1];
									}

									char tp_ud[255] = {0};
									int tp_ud_length = 255;
									static encoding* x = codeFactory::createEncoding("unicode");
									x->decode(tp_ud_decode, deliver.get_sm_len(), tp_ud, &tp_ud_length);
									memcpy(sm._cont, tp_ud, tp_ud_length > MAX_CONT_LEN ? MAX_CONT_LEN : tp_ud_length);
								}
								else
								{
									memcpy(sm._cont, deliver.get_sm_cont(), deliver.get_sm_len() > MAX_CONT_LEN ? MAX_CONT_LEN : deliver.get_sm_len());
								}
								_smpp->OnDeliver(sm);
							}
						}
						break;
					case SMPP_CMD_UNBIND_RESP:
						{
							#ifdef _DEBUG
								smpp_unbind_resp resp;
								resp.decode(decode_buf.buf); 
								printf("unbind_resp.result = %d.\n", resp.get_cmd_stat());
							#endif
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
		OutputDebugStr("[SMPP::CClient]DecodeClient except!\n");
	}
}

//=================================================================================================
//
//	CSgip
//
//=================================================================================================
CSmpp::CSmpp() 
	: _high_mo(NULL)
	, _high_mt(NULL)
	, _client_idx(0)
	, _smpp_seq_id(NULL)
{
}

CSmpp::CSmpp(CONNECT_INFO &info)
	: _high_mo(NULL)
	, _high_mt(NULL)
	, _client_idx(0)
	, _smpp_seq_id(NULL)
{
	Startup(info);
}

CSmpp::~CSmpp()
{
	if (_high_mo != NULL)
	{
		delete _high_mo;
		_high_mo = NULL;
	}

	if (_high_mt != NULL)
	{
		delete _high_mt;
		_high_mt = NULL;
	}

	for (unsigned long i = 0; i < _low_client.size(); ++i)
	{
		if (_low_client[i] != NULL)
		{
			delete _low_client[i];
			_low_client[i] = NULL;
		}
	}

	if (_smpp_seq_id != NULL)
	{
		delete _smpp_seq_id;
		_smpp_seq_id = NULL;
	}
}

bool CSmpp::Startup(CONNECT_INFO &info)
{
	_info = info;
	_smpp_seq_id = new smpp_seq_id(info._seq_id);
	
	switch (info._smg_type)
	{
	case SMG_TYPE_SMPP_SL_V33:
		{
			_high_mo = new CClient(this, BIND_TYPE_MO);
			_high_mo->Connect(info._smg_ip, info._smg_port_mo);
		}
		break;
	case SMG_TYPE_SMPP_DL_V33:
		{
			_high_mo = new CClient(this, BIND_TYPE_MO);
			_high_mo->Connect(info._smg_ip, info._smg_port_mo);

			_high_mt = new CClient(this, BIND_TYPE_MT);
			_high_mt->Connect(info._smg_ip, info._smg_port_mo);
		}
		break;
	case SMG_TYPE_SMPP_SL_V34:
		{
			_high_mo = new CClient(this, BIND_TYPE_MOMT);
			_high_mo->Connect(info._smg_ip, info._smg_port_mo);
		}
		break;
	case SMG_TYPE_SMPP_DL_V34:
	default:
		{
			_high_mo = new CClient(this, BIND_TYPE_MO);
			_high_mo->Connect(info._smg_ip, info._smg_port_mo);

			_high_mt = new CClient(this, BIND_TYPE_MT);
			_high_mt->Connect(info._smg_ip, info._smg_port_mo);
		}
		break;
	}
	
	for (unsigned char i = 0; i < info._links - 1; ++i)
	{
		CClient *x = new CClient(this, BIND_TYPE_MT);
		x->Connect(info._smg_ip, info._smg_port_mo);
		
		_low_client.push_back(x);
	}

	return true;
}

//	对于高优先级的短信，直接放入队列中
//	对于低优先级的短信，检查连接是否成功，成功才发送
unsigned long CSmpp::Submit(SUBMIT_SM &submit, bool prio)
{
	unsigned long sm_id = 0;
	if ((prio) || _info._links == 1)
	{
		if (_high_mt != NULL)
		{
			sm_id = _high_mt->Submit(submit);
		}
		else
		{
			sm_id = _high_mo->Submit(submit);
		}
	}
	else
	{
		Process::CAutoLock lock(&_client_cs);
		if (_client_idx > _info._links - 2)
		{
			_client_idx = 0;
		}
		
		if ((_low_client[_client_idx])->IsConnected())
		{
			sm_id = (_low_client[_client_idx])->Submit(submit);
			++_client_idx;
		}
	}

	return sm_id;
}
