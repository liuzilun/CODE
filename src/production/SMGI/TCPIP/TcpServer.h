#pragma once

#include <list>
#include "basicsock.h"
#include "..\common\workthread.h"
#include "..\common\buf.h"

namespace tcpip
{
	class CTcpServerClient;

	class CTcpServer :
		public Process::CWorkThread,
		private CUsesWinsock 
	{
		typedef std::list<CTcpServerClient*> accepted_arr;
	public:
		CTcpServer(void);
		virtual ~CTcpServer(void);

		bool Listen(unsigned short port);
		virtual void OnAccept(CBasicSocket::Accepted& ac, 
			CTcpServerClient** client_sock) = 0;
	protected:
		UINT Execute(void);
		void CleanUpAccepts(void);
	private:
		CBasicSocket		_listenner;
		accepted_arr		_accepted;
		unsigned short		_port;
	};

	class CTcpServerClient 
		: public tcpip::CBasicSocket
		, private Process::CWorkThread
		, public CAbsTcpClient
	{
	public:
		CTcpServerClient(const CBasicSocket::Accepted &ac);
		virtual ~CTcpServerClient(void);

		bool Send(char* buf, int len)
		{
			bool ret = true;
			try
			{
				Write(buf, len);
			}
			catch(...)
			{
				ret = false;
			}
			return ret;
		}

		bool IsConnected(void)
		{
			return (GetState() == CBasicSocket::ACCEPTED 
				|| GetState() == CBasicSocket::CONNECTED);
		}

		void Disconnect(void)
		{
			try
			{
				Close();
			}
			catch(...)
			{
			}
			stopped_ = true;
		}

		void Run(void)
		{
			Start();
		}

		virtual void OnRecv(char* buf, int len) = 0;
		virtual void OnDisconnect(void) = 0;
	protected:
		UINT Execute(void);
	};
}