#include "tcpserver.h"
#include <iostream>

#pragma warning(disable: 4101)

using namespace tcpip;

CTcpServer::CTcpServer(void)
{
}

CTcpServer::~CTcpServer(void)
{
	Stop();
	_listenner.Close();

	accepted_arr::iterator it = _accepted.begin();
	while (it != _accepted.end())
	{
		CTcpServerClient *cli = *it;
		if (cli != NULL) 
		{
			try
			{
				cli->Close();
			}
			catch(...)
			{
			}

			delete cli;
			cli = NULL;
		}
		++it;
	}
	_accepted.clear();
}

bool CTcpServer::Listen(unsigned short port)
{
	bool ret = false;
	_port = port;
	try
	{
		_listenner.Listen(_port);
		ret = true;
		Start();
	}
	catch(socket_logic_error &le)
	{
#ifdef _DEBUG
		printf("[CTcpServer::Listen]LogicError: %s\n", le.what());
#endif
		char err[1024] = {0};
		sprintf(err, "[CTcpServer::FATAL]Listen logic error(%s)", le.what());
		OutputDebugStr(err);
	}
	catch(socket_runtime_error &re)
	{
#ifdef _DEBUG
		printf("[CTcpServer::Listen]RuntimeError: %s\n", re.what());
#endif
		char err[1024] = {0};
		sprintf(err, "[CTcpServer::FATAL]Listen runtime error(%s)", re.what());
		OutputDebugStr(err);
	}
	catch(char *err_msg)
	{
#ifdef _DEBUG
		printf("[CTcpServer::Listen]Listen error, err_num = %s\n", err_msg);
#endif
		char err[1024] = {0};
		sprintf(err, "[CTcpServer::FATAL]Listen error(%s)", err_msg);
		OutputDebugStr(err);
	}
	return ret;
}

void CTcpServer::CleanUpAccepts(void)
{
	accepted_arr::iterator first = _accepted.begin();
	while (first != _accepted.end())
	{
		accepted_arr::iterator next = first;
		++next;

		if ((*first)->GetState() == CBasicSocket::CLOSED)
		{
			delete *first;
			*first = NULL;
			_accepted.erase(first);
		}
		first = next;
	}
}

UINT CTcpServer::Execute(void)
{
	while (!stopped_)
	{
		try
		{
			CTcpServerClient* client_sock = 0;
			OnAccept(_listenner.Accept(), &client_sock);
			CleanUpAccepts();
			if(client_sock)
			{
				_accepted.push_back(client_sock);
			}
		}
		catch(socket_runtime_error &re)
		{
#ifdef _DEBUG
		printf("[CTcpServer::Execute]Accepted RuntimeError: %s\n", re.what());
#endif
			char err[1024] = {0};
			sprintf(err, "[CTcpServer::Execute]Accepted runtime error(%s)", re.what());
			OutputDebugStr(err);
		}
		catch(socket_logic_error &le)
		{
#ifdef _DEBUG
		printf("[CTcpServer::Execute]Accepted LogicError: %s\n", le.what());
#endif
			char err[1024] = {0};
			sprintf(err, "[CTcpServer::Execute]Accepted logic error(%s)", le.what());
			OutputDebugStr(err);
		}
		catch (char *err_msg)
		{
#ifdef _DEBUG
		printf("[CTcpServer::Execute]Accepted error, err_num = %s\n", err_msg);
#endif
			char err[1024] = {0};
			sprintf(err, "[CTcpServer::Execute]Accepted error(%s)", err_msg);
			OutputDebugStr(err);
		}
	}
	return 0;
}

//=================================================================================
//
//
//
//=================================================================================
CTcpServerClient::CTcpServerClient(const CBasicSocket::Accepted &ac)
	: CBasicSocket(ac)
{
}

CTcpServerClient::~CTcpServerClient(void)
{
	Disconnect();
	Stop();
}

UINT CTcpServerClient::Execute(void)
{
	char buff[4096] = {0};
	while (!stopped_ && GetState() != CLOSED)
	{
		try
		{
			int recv_len = Read(buff, 4096);
			if (recv_len > 0) 
			{
				OnRecv(buff, recv_len);
				memset(buff, 0, 4096);
			}
			else
			{
				//Client Disconnected
				Sleep(500);
			}
		}
		catch(socket_logic_error &le)
		{
#ifdef _DEBUG
		printf("[CTcpServerClient::Execute]Recv LogicError: %s\n", le.what());
#endif
			Disconnect();
		}
		catch(socket_runtime_error &re)
		{
#ifdef _DEBUG
		printf("[CTcpServerClient::Execute]Recv RuntimeError: %s\n", re.what());
#endif
			Disconnect();
		}
		catch(...)
		{
			int errnum = ::WSAGetLastError();
#ifdef _DEBUG
		printf("[CTcpServerClient::Execute]Recv error, err_num = %d\n", errnum);
#endif
			Disconnect();
		}

		if (!stopped_)
		{
			OnDisconnect();
		}
	}	

	return 0;
}