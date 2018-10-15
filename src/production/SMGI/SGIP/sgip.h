#pragma once

#include <string>
#include <vector>
#include <hash_map>
#include "sgip_packet.h"
#include "..\tcpip\tcpclient.h"
#include "..\tcpip\tcpserver.h"
#include "..\common\buf.h"
#include "..\common\sync.h"
#include "..\common\thread.h"
#include "..\common\workerthread.h"
#include "..\SMGI\SMGDefine.h"
#include "..\lib\codeFactory.h"			//	bcd decode

using namespace sms;
using namespace sms::sgip;
using namespace sms::smgi;
using namespace Process;

#pragma pack (push, 1)
class CSgip
{
	//	send packet to SMG(Server/Client)
	class CSender 
		: public Process::CWorkerThread
	{
		struct sgip_packets
		{
			sms::sgip::sgip_packet	*packet;
			time_t					t;			//time
			int						r;			//resend times;
			bool					need_confirm;
		};
		typedef unsigned long seq_type;
		typedef std::hash_map<seq_type, sgip_packets*> packets_map;// htonl过的seq为key
	public:
		CSender(tcpip::CAbsTcpClient *tc, 
				bool is_server,
				long slip_size,
				unsigned short wait_time,
				unsigned short resend_time,
				unsigned long interval);
		~CSender();

		void AddPacket(sms::sgip::sgip_packet *packet, 
					   bool need_confirm = true);
		void Response(seq_type resp_seq);
		void RefreshPackets();		// called while client disconnect
		void InitPackets();			// called while client connect
		bool SendPacket(sms::sgip::sgip_packet *packet);

		void set_bind(bool is_bind)
		{
			_is_bind = is_bind;
		}
	protected:
		unsigned int ThreadEntry(void);

		bool AddRef(void);			// for slip_window, return true, if ok to send.
		void Release(void);			// for slip_window
		void DeletePacket(seq_type resp_seq);
		void DeletePacket(sgip_packets *packet);
		void SendPacket(sgip_packets &pack);
	protected:
		bool _is_server;
		unsigned long	_interval;	//	submit interval time, seconds.
	
		bool _is_bind;
	private:
		Process::CCriticalSection	_sender_cs;
		packets_map					_packets;
		long						_slip_window;
		long						_slip_size;
		unsigned short				_wait_time;
		unsigned short				_resend_time;
		tcpip::CAbsTcpClient*		_client;
	};

	// SMG->SP for deliver
	class CServerClient 
		: public tcpip::CTcpServerClient
	{
	public:
		CServerClient(const CBasicSocket::Accepted &ac, 
					  CSgip *sgip);
		~CServerClient(void);
	protected:
		void DecodeServer(char *buf, int len);
		void OnRecv(char *buf, int len);
		void OnDisconnect(void);
	private:
		CSgip			*_sgip;
		CSender			*_sender;
		buffer_block	_server_buff;
	};

	class CServer 
		: public tcpip::CTcpServer
	{
	public:
		CServer(CSgip *sgip);
		~CServer();
	protected:
		void OnAccept(tcpip::CBasicSocket::Accepted &ac, 
					  tcpip::CTcpServerClient **client_sock);
	private:
		CSgip *_sgip;
	};

	class CClient;
	class CKeepAlive
		: public Process::CWorkerThread
	{
	public:
		CKeepAlive(CClient *client);
		~CKeepAlive();
	protected:
		unsigned int ThreadEntry(void);
	private:
		CClient	*_client;;
	};

	//	as tcp client, connecting smg ip/port(SMG)
	class CClient
		: public tcpip::CTcpClient
	{
	public:
		CClient(CSgip *sgip);
		virtual ~CClient();

		unsigned long Submit(SUBMIT_SM &submit);
		unsigned long GetSenderThreadID(void)
		{
			return _sender->GetThreadID();
		}
	protected:
		void DecodeClient(char *buf, int len);			// 解SP->SMG连接的包, Submit

		void OnRecv(char *buf, int len);
		void OnConnect(tcpip::CTcpClient *client);
		void OnDisconnect(tcpip::CTcpClient *client);
		void OnReconnect(tcpip::CTcpClient *client)
		{
			OnConnect(client);
		}

		void Bind(void);
		void Unbind(void);
		void KeepAlive(void);
	private:
		CSgip			*_sgip;
		CSender			*_sender;
		CKeepAlive		*_keep_alive;
		friend class	CKeepAlive;
		buffer_block	_buff;						//	缓冲区
		encoding        *_uc_coding;

	public:
		//	长短信
		unsigned long submit_long_sm(SUBMIT_SM & submit_pkg);
		//	分割短信
		unsigned long submit_cut_sm(SUBMIT_SM & submit_pkg);
		// 正常发送
		unsigned long submit_norm(SUBMIT_SM & submit_pkg);
	};
public:
	CSgip();
	CSgip(CONNECT_INFO &info);
	~CSgip();

	bool Startup(CONNECT_INFO &info);
	unsigned long Submit(SUBMIT_SM &submit, bool prio);

	unsigned long GetSeqID(void) const 
	{
		return _sgip_seq_id->get_seq_id();
	}

	CONNECT_INFO			_info;
protected:
	virtual void OnSubmitResp(SUBMIT_RESP &resp) = 0;
	virtual void OnDeliver(DELIVER_SM &deliver) = 0;
	virtual void OnReport(REPORT_SM &report) = 0;
private:
	sms::sgip::sgip_seq_id	*_sgip_seq_id;
	
	CServer			*_server;					//	SP作为Server,SMG作为Client
	friend class	CServer;
	
	//CClient					*_high_client;
	std::vector<CClient*>	_client;			//	SP作为Client,SMG作为Server
	friend class			CClient;					
	Process::CCriticalSection	_client_cs;
	long						_client_idx;
};
#pragma pack (pop, 1)