#include "StdAfx.h"
#include "MyServer.h"

CMyServer::CMyServer(void)
{
	mClientId = 0;
}

CMyServer::~CMyServer(void)
{
}
void CMyServer::OnAccept(CBasicSocket::Accepted& ac, 
						 CTcpServerClient** client_sock)
{
	CMyServerClient *SrvClient = new CMyServerClient(ac);
	*client_sock = SrvClient;
    SCLIENT newcl;
	
	newcl.mClientId = ++mClientId;
	strncpy(newcl.mMainIP,ac.get_ip_addr(),sizeof(newcl.mMainIP));
	newcl.mMainPort = ac.get_port();

	push_client(&newcl);
	
	return;
}

void CMyServer::push_client(SCLIENT client)
{
	_cs.Lock();
	SCLIENT* cl = new SCLIENT();
	strncpy(cl->mMainIP,client.mMainIP,sizeof(client.mMainIP));
	cl->mMainPort = client.mMainPort;
	cl->mSlavPort = client.mSlavPort;
    
	client_list.insert(CLIENT_Pair(client.mClientId,cl));
	_cs.Unlock();
}

void CMyServerClient::OnRecv(char* buf, int len)
{
    
	int i = 0;
}
void CMyServerClient::OnDisconnect()
{
    int i = 0;
}

CMyServerClient::CMyServerClient(const CBasicSocket::Accepted &ac)
:CTcpServerClient(ac)
{
    
}


