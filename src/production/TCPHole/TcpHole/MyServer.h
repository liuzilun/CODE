#pragma once
#include "..\tcpip\TcpServer.h"
#include "MyServer.h"
#include "..\common\sync.h"
#include <map>

using namespace Process;
using namespace std;
using namespace tcpip;

typedef struct SClientInfo
{
	char mMainIP[20];    //��Ϣ������IP
	int  mMainPort;      //��Ϣ�������˿�
	int  mSlavPort;      //�ļ����Ͷ˿�
	int  mClientId;      //�ͻ��˱��
}SCLIENT,*pSCLIENT;



typedef std::map<unsigned int,SCLIENT*> CLIENT_LIST;

typedef pair <unsigned int, SCLIENT*> CLIENT_Pair;

class CMyServerClient :public tcpip::CTcpServerClient
{
public:
	CMyServerClient(const tcpip::CBasicSocket::Accepted &ac);
	~CMyServerClient(){};
	
	 void setSrvClientId(unsigned int clientId) {mClientId = clientId;}
	 void OnRecv(char* buf, int len);
	 void OnDisconnect(void);

private:
	unsigned int mClientId;

};

class CMyServer :public CTcpServer
{
public:
	CMyServer(void);
	~CMyServer(void);


	void OnAccept(CBasicSocket::Accepted& ac, 
		CTcpServerClient** client_sock);
	void push_client(SCLIENT client);



private:
	CLIENT_LIST client_list;
	Process::CCriticalSection    _cs;
};


