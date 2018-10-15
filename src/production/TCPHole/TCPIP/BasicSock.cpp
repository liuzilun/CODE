#include "basicsock.h"
#include <sstream>
#include <string>

using namespace tcpip;

socket_runtime_error::socket_runtime_error(const std::string &what)
	: runtime_error(what) 
{
	_errnum = ::WSAGetLastError();
};

const char* socket_runtime_error::what(void) const throw()
{
	std::ostringstream ss;
	ss << runtime_error::what();
	ss << " error number: " << _errnum;
	_msg = ss.str();
	return _msg.c_str();
}

CUsesWinsock::CUsesWinsock(void)
{
   WORD wVersionRequested = 0x202;

   if (0 != ::WSAStartup(wVersionRequested, &_data))
   {
      throw socket_runtime_error("CUsesWinsock::CUsesWinsock() WSAStartup fail");
   }
}

CUsesWinsock::~CUsesWinsock(void)
{
   WSACleanup();
}

CBasicSocket::Accepted::Accepted(socket_type s, 
								 sockaddr_in a)
	: sock(s)
	, addr(a)
{
}

CBasicSocket::CBasicSocket(void) 
	: _state(CLOSED)
{
	//	qsf 2008-9-11
	_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == INVALID_SOCKET)
	{
		throw socket_runtime_error("CBasicSocket::CBasicSocket() create socket failed");
	}
}
CBasicSocket::CBasicSocket(const CBasicSocket::Accepted &ac)
	: _socket(ac.sock)
	, _address(ac.addr)
	, _state(ACCEPTED)
{
}

CBasicSocket::~CBasicSocket(void)
{
	if (_state != CLOSED) 
	{
		closesocket(_socket);
	}
}
void CBasicSocket::Listen(unsigned short port, int backlog)
{
	if (_state != CLOSED)
	{
		throw socket_logic_error("CBasicSocket::Listen() socket not in CLOSED state");
	}

	_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == INVALID_SOCKET)
	{
		throw socket_runtime_error("CBasicSocket::Listen() create socket failed");
	}

	sockaddr_in	local;
	memset(&local, 0, sizeof local);
	local.sin_family	= AF_INET;
	local.sin_port		= htons(port);
	local.sin_addr.s_addr=htonl(INADDR_ANY);

	if (::bind(_socket, (sockaddr* )&local, sizeof(local)) == SOCKET_ERROR)
	{
		throw socket_runtime_error("CBasicSocket::Listen() bind failed");
	}

	if (::listen(_socket, backlog) == SOCKET_ERROR)
	{
		throw socket_runtime_error("CBasicSocket::Listen() listen failed");
	}

	memset(&_address, 0, sizeof _address);
	_state = LISTENING;
}

CBasicSocket::Accepted CBasicSocket::Accept(void)
{
	if (_state != LISTENING)
	{
		throw socket_logic_error("CBasicSocket::Accept() socket not listening");
	}

	sockaddr_in	from;
	int len = sizeof(from);
	memset(&from, 0, len);

	socket_type newsocket = ::accept(_socket, (sockaddr*)&from, &len);
	if (newsocket == INVALID_SOCKET)
	{
		throw socket_runtime_error("CBasicSocket::Accept() accept failed");
	}

	return Accepted(newsocket, from);
}

void CBasicSocket::Connect(const char* address, unsigned short port)
{
#ifdef _DEBUG
	printf("CBasicSocket::Connect()...\n");
	printf("\t address = %s\n", address);
	printf("\t port = %d\n", port);
#endif

	if (_state != CLOSED)
	{
		throw socket_logic_error("CBasicSocket::Connect() socket not in CLOSED state");
	}

//	qsf
//	_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == INVALID_SOCKET)
	{
		throw socket_runtime_error("CBasicSocket::Connect() socket failed");
	}

	//hostent* hp;
	//unsigned long addr = inet_addr(address);
	//if(addr != INADDR_NONE)
	//	hp = gethostbyaddr((const char*)&addr, 4, AF_INET);
	//else
	//	hp = gethostbyname(address);

	//if(hp == NULL)
	//	throw socket_runtime_error("CBasicSocket::Connect() cannot resolve address");
	//if(hp->h_addrtype != AF_INET)
	//	throw socket_runtime_error("CBasicSocket::Connect() address resolved with TCP incompatible type");

	memset(&_address, 0, sizeof _address);
	//memcpy(&(_address.sin_addr), hp->h_addr_list[0], hp->h_length);
	_address.sin_addr.s_addr	= inet_addr(address);
	_address.sin_family			= AF_INET;
	_address.sin_port			= htons((u_short)port);

	if (::connect(_socket, (sockaddr* )&_address, sizeof _address) == SOCKET_ERROR)
	{
		//	qsf socket失效时才重新创建（10038）
		if (WSAGetLastError() == WSAENOTSOCK)
		{
			_socket = ::socket(AF_INET, SOCK_STREAM, 0);
		}
		throw socket_runtime_error("CBasicSocket::Connect() connect failed");
	}

	_state = CONNECTED;
}

const char* CBasicSocket::GetAddress(void)
{
	if (_state != CONNECTED && _state != ACCEPTED)
	{
		throw socket_logic_error("CBasicSocket::GetAddress() socket not connected");
	}

	return inet_ntoa(_address.sin_addr);
}

unsigned short CBasicSocket::GetPort(void)
{
	if (_state != CONNECTED && _state != ACCEPTED)
	{
		throw socket_logic_error("CBasicSocket::GetPort() socket not connected");
	}

	return ntohs(_address.sin_port);
}

void CBasicSocket::Write(const void *buf, int len)
{
	if (_state != CONNECTED && _state != ACCEPTED)
	{
		throw socket_logic_error("CBasicSocket::Write() socket not connected");
	}

	int written = 0;
	while (len > 0) 
	{
		if ((written = ::send(_socket, (const char* )buf, len, 0)) == SOCKET_ERROR)
		{
			throw socket_runtime_error("CBasicSocket::Write() write failed");
		}

		len -= written;
		buf = (const char* )buf + written;
	}
}

int CBasicSocket::Read(void *buf, int len)
{
	if (_state != CONNECTED && _state != ACCEPTED)
	{
		throw socket_logic_error("CBasicSocket::Read() socket not connected");
	}

	int readn = ::recv(_socket, (char* )buf, len, 0);
    if ((readn == 0)
		|| (readn == SOCKET_ERROR))
	{
		throw socket_runtime_error("CBasicSocket::Read() read failed");
	}

	return readn;
}

void CBasicSocket::Close(void)
{
	if (_state != CLOSED) 
	{
		_state = CLOSED;
		if (::closesocket(_socket) == SOCKET_ERROR)
		{
			throw socket_runtime_error("CBasicSocket::Close() close failed");
		}
	}
}
