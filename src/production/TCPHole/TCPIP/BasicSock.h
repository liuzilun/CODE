#pragma once

#include <winsock2.h>
#include <stdexcept>

#pragma warning(disable:4290)
#pragma comment(lib, "ws2_32.lib")

namespace tcpip
{
	class socket_runtime_error : public std::runtime_error 
	{
	public:
		explicit socket_runtime_error(const std::string& what);
		~socket_runtime_error() throw() {}

		const char* what(void) const throw();
		int errornumber(void) const throw()
		{
			return _errnum;
		}

	private:
		mutable std::string _msg;
		int _errnum;
	};

	class socket_logic_error 
		: public std::logic_error 
	{
	public:
		explicit socket_logic_error(const std::string& what) 
			: std::logic_error(what) 
		{
		}
	};

	class CUsesWinsock
	{
	public:
		CUsesWinsock(void);
		~CUsesWinsock(void);

	private:
		WSADATA _data;
	};

	class CBasicSocket
	{
		typedef SOCKET	socket_type;

	public:
		class Accepted
		{
		private:
			Accepted(socket_type s, sockaddr_in a);

			// disable copy
			Accepted(const Accepted&);
			Accepted& operator= (const Accepted&);
			
			socket_type	sock;
			sockaddr_in	addr;

			friend class CBasicSocket;
		public:
			const char *get_ip_addr(void)
			{
				return inet_ntoa(addr.sin_addr);
			}

			unsigned short get_port(void)
			{
				return addr.sin_port;
			}
		};

	public:
		typedef	enum {CLOSED, LISTENING, ACCEPTED, CONNECTED} socket_state;

		CBasicSocket(void);
		CBasicSocket::CBasicSocket(const CBasicSocket::Accepted& ac);
		virtual ~CBasicSocket(void);

		void Connect(const char *ip, unsigned short port) throw(socket_logic_error, socket_runtime_error);
		void Close(void) throw(socket_runtime_error);

		int Read(void* buf, int len) throw(socket_logic_error, socket_runtime_error);
		void Write(const void* buf, int len) throw(socket_logic_error, socket_runtime_error);

		void Listen(unsigned short port, int backlog = 100) throw(socket_logic_error, socket_runtime_error);
		Accepted Accept(void) throw(socket_logic_error, socket_runtime_error);
		
		const char* GetAddress(void) throw(socket_logic_error);
		unsigned short GetPort(void) throw(socket_logic_error);
		socket_state GetState(void) const { return _state; }

	private:
		socket_state	_state;
		socket_type		_socket;
		sockaddr_in		_address;
	};

	class CAbsTcpClient
	{
	public:
		virtual bool Send(char *buf, int len) = 0;
		virtual bool IsConnected() = 0;
	};
}