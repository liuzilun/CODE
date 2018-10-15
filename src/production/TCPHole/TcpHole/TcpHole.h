class CMyServer public CTcpServer
{
	public
		CMyServer();
	~CMyServer();
	void OnAccept(CBasicSocket::Accepted& ac, 
		CTcpServerClient** client_sock);

};