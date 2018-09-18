// XTcp.h: interface for the XTcp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTCP_H__F39B5585_3A61_46CA_A674_E03D62C60A71__INCLUDED_)
#define AFX_XTCP_H__F39B5585_3A61_46CA_A674_E03D62C60A71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>

#ifdef WIN32
#ifdef XSOCKET_EXPORTS
#define XSOCKET_API __declspec(dllexport)
#else
#define XSOCKET_API __declspec(dllimport)
#endif
#else
#define XSOCKET_API
#endif

class XSOCKET_API XTcp
{
public:
	int CreateSocket();
	bool Bind(unsigned short port);
	XTcp Accept();
	void Close();

	int Recv(char* buf,int bufsize);
	int Send(const char* buf,int sendsize);

	bool Connect(const char* ip, unsigned short port,int timeout=1000);

	bool SetBlock(bool isblock);

	XTcp();
	virtual ~XTcp();
public:
	int sock =0;
	unsigned short port;
	char ip[16];

};

#endif // !defined(AFX_XTCP_H__F39B5585_3A61_46CA_A674_E03D62C60A71__INCLUDED_)
