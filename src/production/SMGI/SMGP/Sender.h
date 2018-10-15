#pragma once

#include <hash_map>
#include <time.h>
#include "..\tcpip\BasicSock.h"
#include "..\common\workerthread.h"
#include "smgp_packet.h"

using namespace sms;

class CSender 
	: public Process::CWorkerThread
{
	struct smgp_packets
	{
		smgp::smgp_packet	*packet;
		time_t				t;			//time
		int					r;			//resend times;
		bool				need_confirm;
	};
	typedef unsigned long seq_type;
	typedef std::hash_map<seq_type, smgp_packets*> packets_map;// htonl¹ýµÄseqÎªkey
public:
	CSender(tcpip::CAbsTcpClient *tc, 
			bool is_server = false,
			long slip_size = 32,
			unsigned long interval = 50);
	~CSender();

	void AddPacket(smgp::smgp_packet *packet, 
					bool need_confirm = true);
	void Response(seq_type resp_seq);
	void RefreshPackets();		// called while client disconnect
	void InitPackets();			// called while client connect
	bool SendPacket(smgp::smgp_packet *packet);

	void set_bind(bool is_bind)
	{
		_is_bind = is_bind;
	}
protected:
	unsigned int ThreadEntry(void);

	bool AddRef(void);			// for slip_window, return true, if ok to send.
	void Release(void);			// for slip_window
	void DeletePacket(seq_type resp_seq);
	void DeletePacket(smgp_packets *packet);
	void SendPacket(smgp_packets &pack);
protected:
	bool _is_server;
	unsigned long	_interval;	//	submit interval time, seconds.
private:
	Process::CCriticalSection	_sender_cs;
	packets_map					_packets;
	long						_slip_window;
	long						_slip_size;
	tcpip::CAbsTcpClient*		_client;
	bool						_is_bind;
};