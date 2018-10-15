#pragma once

#include <hash_map>
#include <time.h>
#include "..\tcpip\BasicSock.h"
#include "..\common\workerthread.h"
#include "..\common\sync.h"
#include "cmpp_pkg.h"
#include "..\SMGI\SMGDefine.h"

using namespace sms;
using namespace sms::smgi;

class CSender 
	: public Process::CWorkerThread
{
	struct cmpp_packets
	{
		cmpp::cmpp_pkg	*packet;
		time_t			t;			//time
		int				r;			//resend times;
		bool			need_confirm;
	};
	typedef unsigned long seq_type;
	typedef std::hash_map<seq_type, cmpp_packets*> packets_map;// htonl¹ýµÄseqÎªkey
public:
	CSender(tcpip::CAbsTcpClient *tc, 
			bool is_server = false,
			long slip_size = 32,
			unsigned long interval = 50);
	~CSender();

	void AddPacket(cmpp::cmpp_pkg *packet, 
					bool need_confirm = true);
	void Response(seq_type resp_seq);
	void RefreshPackets();		// called while client disconnect
	void InitPackets();			// called while client connect
	bool SendPacket(cmpp::cmpp_pkg *packet);

	void set_bind(bool is_bind)
	{
		_is_bind = is_bind;
	}

protected:
	unsigned int ThreadEntry(void);

	bool AddRef(void);			// for slip_window, return true, if ok to send.
	void Release(void);			// for slip_window
	void DeletePacket(seq_type resp_seq);
	void DeletePacket(cmpp_packets *packet);
	void SendPacket(cmpp_packets &pack);
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
public:
	void SetInterval(unsigned long interval);
	unsigned long get_interval(void);
};