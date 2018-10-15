#pragma once

#include <string>
#include <vector>
#include <hash_map>
#include "smpp_packet.h"
#include "..\tcpip\tcpclient.h"
#include "..\tcpip\tcpserver.h"
#include "..\common\buf.h"
#include "..\common\sync.h"
#include "..\common\thread.h"
#include "..\common\workerthread.h"
#include "..\SMGI\SMGDefine.h"

using namespace sms::smgi;

/*
	Report状态 	短消息状态
	0，发送成功	DELIVERED
	1，等待发送	ENROUTE，ACCEPTED
	2，发送失败	EXPIRED，DELETED，UNDELIVERABLE，UNKNOWN，REJECTED
*/
#define STAT0		 "DELIVRD"
#define STAT1		 "ENROUTE,ACCEPTD"
#define STAT2		 "EXPIRED,DELETED,UNDELIV,UNKNOWN，REJECTD"

#define STAT0_VALUE  0							//	状态0
#define STAT1_VALUE  1							//	状态1
#define STAT2_VALUE  2							//	状态2
#define STAT_UNKNOW  255						//	为止状态

class CSmpp
{
	//	连接类型MO,MT,MO/MT
	enum BIND_TYPE
	{
		BIND_TYPE_MO	= 0,
		BIND_TYPE_MT	= 1,
		BIND_TYPE_MOMT	= 2
	};

	//	send packet to SMG(Server/Client)
	class CSender 
		: public Process::CWorkerThread
	{
		struct smpp_packets
		{
			sms::smpp::smpp_packet	*packet;
			time_t					t;			//time
			int						r;			//resend times;
			bool					need_confirm;
		};
		typedef unsigned long seq_type;
		typedef std::hash_map<seq_type, smpp_packets*> packets_map;// htonl过的seq为key
	public:
		CSender(tcpip::CAbsTcpClient *tc,
				long slip_size = 32,
				unsigned long interval = 50);
		~CSender();

		void AddPacket(sms::smpp::smpp_packet *packet, 
					   bool need_confirm = true);
		void Response(seq_type resp_seq);
		void RefreshPackets();		// called while client disconnect
		void InitPackets();			// called while client connect
		bool SendPacket(sms::smpp::smpp_packet *packet);
	protected:
		unsigned int ThreadEntry(void);

		bool AddRef(void);			// for slip_window, return true, if ok to send.
		void Release(void);			// for slip_window
		void DeletePacket(seq_type resp_seq);
		void DeletePacket(smpp_packets *packet);
		void SendPacket(smpp_packets &pack);
	protected:
		unsigned long	_interval;	//	submit interval time, seconds.
	private:
		Process::CCriticalSection	_sender_cs;
		packets_map					_packets;
		long						_slip_window;
		long						_slip_size;
		tcpip::CAbsTcpClient*		_client;
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
		CClient	*_client;
	};

	//	as tcp client, connecting smg ip/port(SMG)
	class CClient
		: public tcpip::CTcpClient
	{
	public:
		CClient(CSmpp *smpp, unsigned char bind_type = BIND_TYPE_MO);
		virtual ~CClient();

		unsigned long Submit(SUBMIT_SM &submit);
		unsigned long GetSenderThreadID(void)
		{
			return _sender->GetThreadID();
		}
	protected:
		void Decode(char *buf, int len);			// 解SP->SMG连接的包, Submit

		void OnRecv(char *buf, int len);
		void OnConnect(tcpip::CTcpClient *client);
		void OnDisconnect(tcpip::CTcpClient *client);
		void OnReconnect(tcpip::CTcpClient *client)
		{
			OnConnect(client);
		}

		void BindReceiver(void);
		void BindTransmitter(void);
		void BindTransceiver(void);
		void Unbind(void);
		void KeepAlive(void);

		unsigned char get_stat(const char *stat)
		{
			unsigned char ret = STAT_UNKNOW;
			if (strstr(STAT0, stat) != NULL)
			{
				ret = STAT0_VALUE;
			}
			else
			{
				if (strstr(STAT1, stat) != NULL)
				{
					ret = STAT1_VALUE;
				}
				else
				{
					if (strstr(STAT2, stat) != NULL)
					{
						ret = STAT2_VALUE;
					}
					else
					{
						ret = STAT_UNKNOW;
					}
				}
			}

			return ret;
		}
	protected:
		unsigned char	_bind_type;					//	网关类型
	private:
		CSmpp			*_smpp;
		CSender			*_sender;
		CKeepAlive		*_keep_alive;
		friend class	CKeepAlive;
		buffer_block	_buff;						//	缓冲区
	};
public:
	CSmpp(void);
	CSmpp(CONNECT_INFO &info);
	~CSmpp(void);

	bool Startup(CONNECT_INFO &info);
	unsigned long Submit(SUBMIT_SM &submit, bool prio);
	unsigned long GetSeqID(void) const 
	{
		return _smpp_seq_id->get_seq_id();
	}
protected:
	virtual void OnSubmitResp(SUBMIT_RESP &resp) = 0;
	virtual void OnDeliver(DELIVER_SM &deliver) = 0;
	virtual void OnReport(REPORT_SM &report) = 0;
private:
	CONNECT_INFO				_info;
	sms::smpp::smpp_seq_id		*_smpp_seq_id;
	
	CClient						*_high_mo;
	CClient						*_high_mt;
	std::vector<CClient*>		_low_client;		//	SP作为Client,SMG作为Server
	friend class	CClient;					
	Process::CCriticalSection	_client_cs;
	long						_client_idx;	
};
