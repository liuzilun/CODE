// TcpHole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\tcpip\TcpServer.h"
#include "MyServer.h"
using namespace tcpip;

int _tmain(int argc, _TCHAR* argv[])
{
	
	CMyServer mysrv;
	mysrv.Listen(9999);

    getchar();

	return 0;
}

