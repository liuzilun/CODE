//=================================================================================================
//
//	CCmpp::CSender
//
//=================================================================================================
#include "Sender.h"

using namespace cmpp;

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

void CSender::AddPacket(cmpp_pkg *pack, bool need_confirm)
{
	cmpp_packets *p = new cmpp_packets;
	p->packet = pack;
	p->t = 0;
	p->r = 0;
	p->need_confirm = need_confirm;

	_sender_cs.Lock();
	_packets[pack->get_seq_id()] = p;
	_sender_cs.Unlock();
}

void CSender::DeletePacket(cmpp_packets *p)
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
	}

	Release();
}

void CSender::Response(seq_type resp_seq)
{
	_sender_cs.Lock();
	DeletePacket(resp_seq);
	_sender_cs.Unlock();
}

bool CSender::SendPacket(cmpp_pkg *packet)
{
	if (packet == NULL)
	{
		return false;
	}
	else
	{
		return _client->Send(packet->buf(), packet->len());
	}
}

void CSender::SendPacket(cmpp_packets &pack)
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
				sprintf(err, "[CMPP::FATAL]send_packet lost seq_id:%lu(ThreadID:%lu).\n",
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
								SendPacket(*(p->second));
								if (_interval > 0)
								{
									Sleep(_interval);
								}
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
		}
		Sleep(1);
	}
	return 0;
}
void CSender::SetInterval(unsigned long interval)
{
	_interval = interval;
}

unsigned long CSender::get_interval(void)
{
	return _interval;
}
