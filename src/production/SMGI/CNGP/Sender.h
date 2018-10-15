#pragma once

#include <hash_map>
#include <time.h>
#include "..\tcpip\BasicSock.h"
#include "..\common\workerthread.h"
#include "cngp_packet.h"

using namespace sms;

class CSender 
	: public Process::CWorkerThread
{
	struct cngp_packets
	{
		cngp::cngp_packet	*packet;
		time_t				t;			//time
		int					r;			//resend times;
		bool				need_confirm;
	};
	typedef unsigned long seq_type;
	typedef std::hash_map<seq_type, cngp_packets*> packets_map;// htonl¹ýµÄseqÎªkey
public:
	CSender(tcpip::CAbsTcpClient *tc, 
			bool is_server = false,
			long slip_size = 32,
			unsigned long interval = 50);
	~CSender();

	void AddPacket(cngp::cngp_packet *packet, 
					bool need_confirm = true);
	void Response(seq_type resp_seq);
	void RefreshPackets();		// called while client disconnect
	void InitPackets();			// called while client connect
	bool SendPacket(cngp::cngp_packet *packet);
protected:
	unsigned int ThreadEntry(void);

	bool AddRef(void);			// for slip_window, return true, if ok to send.
	void Release(void);			// for slip_window
	void DeletePacket(seq_type resp_seq);
	void DeletePacket(cngp_packets *packet);
	void SendPacket(cngp_packets &pack);
protected:
	bool _is_server;
	unsigned long	_interval;	//	submit interval time, seconds.
private:
	Process::CCriticalSection	_sender_cs;
	packets_map					_packets;
	long						_slip_window;
	long						_slip_size;
	tcpip::CAbsTcpClient*		_client;
};