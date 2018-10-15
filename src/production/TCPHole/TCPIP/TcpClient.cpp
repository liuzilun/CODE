#include "tcpclient.h"

#pragma warning(disable: 4101)

using namespace tcpip;

CTcpClient::CTcpClient(void) 
	: _connected(false)
	, _connecting(false)
{
}

CTcpClient::~CTcpClient(void)
{
	Stop();
}

bool CTcpClient::DoConnect(void)
{
	try
	{
		_socket.Connect(_ip.c_str(), _port);
		_connected = true;
		OnConnect(this);
	}
	catch(socket_logic_error &le)
	{
#ifdef _DEBUG
		printf("[CTcpClient::DoConnect]LogicError: %s\n", le.what());
#endif
		_connected = false;

		char err[MAX_PATH] = {0};
		sprintf(err, "[CTcpClient::FATAL]DoConnect LogicError: %s.\n", le.what());
		OutputDebugStr(err);
	}
	catch(socket_runtime_error &re)
	{
#ifdef _DEBUG
		printf("[CTcpClient::DoConnect]RuntimeError: %s\n", re.what());
#endif
		_connected = false;

		char err[MAX_PATH] = {0};
		sprintf(err, "[CTcpClient::FATAL]DoConnect RuntimeError: %s.\n", re.what());
		OutputDebugStr(err);
	}
	catch(...)
	{
		int errnum = ::WSAGetLastError();
		_connected = false;
#ifdef _DEBUG
		printf("[CTcpClient::DoConnect]Connect error, err_num = %d\n", 
			errnum);
#endif
		char err[MAX_PATH] = {0};
		sprintf(err, "[CTcpClient::FATAL]DoConnect error, err_num = %d.\n", errnum);
		OutputDebugStr(err);
	}

	return _connected;
}

bool CTcpClient::Connect(const char		*ip, 
						 unsigned short port)
{
	if (ip == NULL) 
	{
		return false;
	}
	
	_ip		= ip; 
	_port	= port;

	if (!_connecting)
	{
		if (!DoConnect())
		{
			Sleep(2000);
		}
	}

	Start();

	return true;
}

void CTcpClient::Disconnect(void)
{
	try
	{
		_socket.Close();
	}
	catch(...)
	{
	}
	_connected = false;
}

bool CTcpClient::Send(char *buf, int len)
{
	if (buf == NULL || len <= 0) 
	{
		return false;
	}

	bool ret = false;
	try
	{
		if (IsConnected()) 
		{
			_socket.Write(buf, len);
			ret = true;
		}
	}
	catch(socket_logic_error &le)
	{
#ifdef _DEBUG
		printf("[CTcpClient::Send]LogicError: %s\n", le.what());
#endif
		char err[MAX_PATH] = {0};
		sprintf(err, "[CTcpClient::FATAL]Send LogicError: %s.\n", le.what());
		OutputDebugStr(err);

		Disconnect();
	}
	catch(socket_runtime_error &re)
	{
#ifdef _DEBUG
		printf("[CTcpClient::Send]RuntimeError: %s\n", re.what());
#endif
		char err[MAX_PATH] = {0};
		sprintf(err, "[CTcpClient::FATAL]Send RuntimeError: %s.\n", re.what());
		OutputDebugStr(err);

		Disconnect();
	}
	catch(...)
	{
		int errnum = ::WSAGetLastError();
#ifdef _DEBUG
		printf("[CTcpClient::Send]Send error, err_num = %d\n", 
			errnum);
#endif
		char err[MAX_PATH] = {0};
		sprintf(err, "[CTcpClient::FATAL]Send error, err_num = %d.\n", errnum);
		OutputDebugStr(err);

		Disconnect();
	}
	return ret;
}

void CTcpClient::CheckConnection(void)
{
	_connecting = true;
	_connected = (_socket.GetState() != CBasicSocket::CLOSED);
	while (!_connected)
	{
		DoConnect();
		Sleep(2000);
	}
	_connecting = false;
}

UINT CTcpClient::Execute(void)
{
	char buff[4096] = {0};
	
	while (!stopped_)
	{
		try
		{
			int recv_len = _socket.Read(buff, 4096);
			if (recv_len > 0) 
			{
				OnRecv(buff, recv_len);
				memset(buff, 0, 4096);
			}
			else
			{
				Sleep(500);
			}
		}
		catch(socket_logic_error &le)
		{
#ifdef _DEBUG
		printf("[CTcpClient::Execute]LogicError: %s\n", le.what());
#endif
			if (_connected) 
			{
				memset(buff, 0, 4096);
				OnDisconnect(this);
				Disconnect();
			}
		}
		catch(socket_runtime_error &re)
		{
#ifdef _DEBUG
		printf("[CTcpClient::Execute]RuntimeError: %s\n", re.what());
#endif
			if (this->_connected) 
			{
				memset(buff, 0, 4096);
				OnDisconnect(this);
				Disconnect();
			}
		}
		catch(...)
		{
			int errnum = ::WSAGetLastError();
#ifdef _DEBUG
		printf("[CTcpClient::Execute]Recv error, err_num = %d\n", errnum);
#endif
			if (this->_connected) 
			{
				memset(buff, 0, 4096);
				OnDisconnect(this);
				Disconnect();
			}
		}
		
		if (!stopped_)
		{
			if (!_connecting)
			{
				CheckConnection();
			}
		}
	}

	Disconnect();
	
	return 0;
}
