// XTcp.h: interface for the XTcp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTCP_H__F39B5585_3A61_46CA_A674_E03D62C60A71__INCLUDED_)
#define AFX_XTCP_H__F39B5585_3A61_46CA_A674_E03D62C60A71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
class XTcp  
{
public:
	int CreateSocket();
	bool Bind(unsigned short port);
	XTcp Accept();
	void Close();

	int Recv(char* buf,int bufsize);
	int Send(const char* buf,int sendsize);

	XTcp();
	virtual ~XTcp();
public:
	int sock =0;
	unsigned short port;
	std::string ip;

};

#endif // !defined(AFX_XTCP_H__F39B5585_3A61_46CA_A674_E03D62C60A71__INCLUDED_)
