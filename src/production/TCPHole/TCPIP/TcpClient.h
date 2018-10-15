#pragma once

#include <string>
#include "basicsock.h"
#include "..\common\workthread.h"
#include "..\common\buf.h"

#pragma warning (disable : 4103)
#pragma pack (push, 1)

namespace tcpip
{
	class CTcpClient 
		: public Process::CWorkThread
		, private tcpip::CUsesWinsock
		, public CAbsTcpClient
	{
	public:
		CTcpClient(void);
		virtual ~CTcpClient(void);

		bool Connect(const char* ip, unsigned short port);
		void Disconnect();
		bool Send(char* buf, int len);
		bool IsConnected(void) { return _connected; }

		virtual void OnRecv(char* buf, int len) = 0;
		virtual void OnConnect(CTcpClient*) = 0;
		virtual void OnDisconnect(CTcpClient*) = 0;
		virtual void OnReconnect(CTcpClient*) = 0;

	protected:
		UINT Execute(void);
		bool DoConnect();
		void CheckConnection(void);

	private:
		CBasicSocket	_socket;
		bool			_connected;
		bool			_connecting;
		std::string		_ip;
		unsigned short	_port;		
	};
}
#pragma pack (pop, 1)