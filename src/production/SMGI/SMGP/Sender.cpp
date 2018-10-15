//=================================================================================================
//
//	CSMGP::CSender
//
//=================================================================================================
#include "Sender.h"

using namespace sms;
using namespace sms::smgp;
using namespace sms::smgi;

CSender::CSender(tcpip::CAbsTcpClient *tc, 
				 bool is_server,
				 long slip_size,
				 unsigned long interval) 
	: _client(tc)
	, _slip_window(0)
	, _slip_size(slip_size)
	, _is_server(is_server)
	, _interval(interval)
	, _is_bind(false)
{
	BeginThread();
}

CSender::~CSender()
{
	EndThread();
}

void CSender::AddPacket(smgp_packet *pack, bool need_confirm)
{
	smgp_packets *p = new smgp_packets;
	p->packet = pack;
	p->t = 0;
	p->r = 0;
	p->need_confirm = need_confirm;

	_sender_cs.Lock();
	_packets[pack->get_seq_id()] = p;
	_sender_cs.Unlock();
}

void CSender::DeletePacket(smgp_packets *p)
{
	// release packet data
	if ((p != NULL) && (p->packet != NULL))
	{
		p->packet->release();

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

void CSender::DeletePacket(seq_type resp_seq)
{
	packets_map::iterator it = _packets.find(resp_seq);
	if (it != _packets.end())
	{
		DeletePacket(it->second);
		// delete node
		_packets.erase(it);
		Release();
	}
}

void CSender::Response(seq_type resp_seq)
{
	_sender_cs.Lock();
	DeletePacket(resp_seq);
	_sender_cs.Unlock();
}

bool CSender::SendPacket(smgp_packet *packet)
{
	if (packet == NULL)
	{
		return false;
	}
	else
	{
		return _client->Send((char *)packet->buf(), packet->len());
	}
}

void CSender::SendPacket(smgp_packets &pack)
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
			/*if (pack.r > RESEND_TIMES)
			{
				char err[MAX_PATH] = {0};
				sprintf(err, "[SMGP::FATAL]send_packet lost seq_id:%lu(ThreadID:%lu).\n",
					pack.packet->get_seq_id(), GetThreadID());
				OutputDebugStr(err);
			}*/
			DeletePacket(pack.packet->get_seq_id()); 
		}
	}
}

// called while client connect
void CSender::InitPackets()		
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

void CSender::RefreshPackets(void)
{
	_sender_cs.Lock();
	packets_map::iterator p;
	for(p = _packets.begin(); p != _packets.end(); ++p)
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

bool CSender::AddRef(void)
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

void CSender::Release(void)
{
	if (_slip_window > 0)
	{
		::InterlockedDecrement(&_slip_window); 
	}
}

unsigned int CSender::ThreadEntry(void)
{
	bool can_send = true;
	char tmp[255] = { 0 };
	DWORD t1 = 0, t2 = 0;
	packets_map::iterator p, q; 
	while (!terminated_)
	{
		if (_is_bind)
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
								//t1 = GetTickCount();
								SendPacket(*(p->second));
								//t2 = GetTickCount();
								//sprintf(tmp, "Send run %u ms", t2 -t1);
								//OutputDebugString(tmp);
								//	SendPacket(*(p->second));
								/*sprintf(tmp, "interval = %d, _is_bind = %d, slip_window=%d, list.size = %d",
									 _interval, _is_bind, _slip_window, _packets.size());
								OutputDebugString(tmp);*/

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
							OutputDebugString("ReSend Packet !");
						}
						p = q;			
					}
				}
			}
			catch(...)
			{
				int err_num = ::GetLastError(); 
				OutputDebugString("Sender Thread exception !");
			}
			_sender_cs.Unlock();
		}
		Sleep(1);
	}
	return 0;
}