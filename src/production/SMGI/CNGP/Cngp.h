#pragma once

#include <vector>								// stl	vector
#include "..\tcpip\tcpclient.h"					// class	CTcpClient
#include "cngp_packet.h"						// classes	cngp packet encode & decode
#include "Sender.h"								// class CSender
#include "..\common\buf.h"						// class  buffer_block

//	���Ӽ���
#define CONN_HIGH    1							//	������
#define CONN_LOW     0							//	������	

#define CNGP_VER     0x10                       //  �汾 2.0

using namespace sms;
using namespace sms::cngp;
using namespace sms::smgi;

//	������Ϣ
typedef struct CONNECTION
{
	unsigned long id;							//	���ӱ�ʶ
	unsigned char level;						//	����  0 - ��ͨ  1 - ����
	unsigned char type;							//	����  0 - ����, 1-���� ,2 -������
	
	CONNECTION(void)
	{
		memset(this, 0, sizeof(CONNECTION));
	}

	~CONNECTION(void)
	{
	}

	CONNECTION(const CONNECTION &conn)
	{
		*this = conn;
	}

	const CONNECTION& operator=(const CONNECTION &conn)
	{
		memcpy(this, &conn, sizeof(CONNECTION));
		return *this;
	}
}CONNECTION;

class CClient;

class CCngp
{
public:
	CCngp(void);
	~CCngp(void);

	//	��ʼ��,���ò���,���������̵߳�
	bool Startup(CONNECT_INFO &info);

	//	�ύ����
	unsigned long Submit(SUBMIT_SM &submit, bool prio);

	//	����
	virtual void OnDeliver(DELIVER_SM &d) = 0;

	//	�ύ�ɹ���������ϢID	
	virtual void OnSubmitResp(SUBMIT_RESP &resp) = 0;

	//	״̬����
	virtual void OnReport(REPORT_SM &r) = 0;

	//	������	
	int Cleanup(void);
protected:
	cngp_seq		*_seq;						//	���кŲ�����
	std::vector<CClient *>	_clients;			//	�������ӵ�����
	CONNECT_INFO _conn_info;					//	���ز���
	CONNECTION _conn;							//	���Ӳ���
	
	long _high_index;							//	�����ȼ����ӵ��±�
	long _low_first;							//	�����ȼ���ʼ�±�
	unsigned long _low_count;					//	�����ȼ�����
	long _low_index;							//	�ϴη��͵������±�
protected:

	//	����һ������
	void CreateConn(unsigned int conn_id,
					unsigned int conn_level,
					unsigned int conn_type);

	//	������������
	void CreateAllConns(void);

	/*  ��һ�����ʵ����ӷ�����
	*
	*	����1.�������ͨ����,�ұȽϿ��е�һ����ͨ����
	*         2.����ǽ�������,�ҽ������ӣ����û�У�����ͨ���ݴ���
	*
	*   ����Ƶ�ʱȽϸ�,��������
	*/
private:
	// ����������Ϣ��ֵ(��,�����ȼ�)
	void GetConnLevel(void);
public:

	unsigned long GetSeqID(void) const;

};

class CClient 
	: public tcpip::CTcpClient
	, public Process::CWorkerThread
{
public:
	CClient(CCngp *p_cngp,
		    cngp_seq *p_seq,
	     	CONNECT_INFO *conn_info,
	    	CONNECTION *conn);

	~CClient(void);

	//	���ֿ��ܵ����ݰ�
	typedef struct cngp_packets
	{
		cngp_head				  head;
		cngp_submit_resp		  submit_resp;
		cngp_deliver			  deliver;
		cngp_active_test		  active_test;
		cngp_active_test_resp	  active_test_resp;
		cngp_login_resp			  login_resp;
		cngp_exit				  exit;
		cngp_exit_resp			  exit_resp;
	}cngp_packets;

protected:
	CCngp			*_cngp;						//	cngpʵ��
	CSender			*_sender;					//	���Ͷ���
	buffer_block	*_buffer;					//	������
	cngp_seq		*_cngp_seq;					//	���кŲ�����

	char			_svr_ip[MAX_IP_LEN + 1];	//	����ip
	unsigned short	_port;						//	���ض˿�
	bool			_is_connected;				//	�Ƿ�����
	CONNECT_INFO	_conn_info;					//	���ز���
    CONNECTION		_conn;						//	������Ϣ
	
	cngp_packets	_packets;					//	������Ϣ
	DELIVER_SM		_deliver;					//	������Ϣ
	REPORT_SM		_report;					//	״̬����
	cngp_report		_cngp_report;		    	//	״̬����ʵ��
	
	SUBMIT_RESP		_submit_resp;				//  �ύ����
	time_t			_last_time;					//	���һ����Ϣ��ʱ��
public:
	//	��������
	void connect_smg(void);

	//	�ύ����
	unsigned long submit(SUBMIT_SM &submit_pkg);

	//	���ػ����뺯��
	void OnConnect(tcpip::CTcpClient *tcp_client);
	void OnDisconnect(tcpip::CTcpClient *tcp_client);
	void OnRecv(char *buf, int len);
	void OnReconnect(tcpip::CTcpClient *tcp_client);

public:
	unsigned long get_id(void)
	{
		return _conn.id;
	}
	
	unsigned char get_level(void)
	{
		return _conn.level;
	}

	unsigned char get_type(void)
	{
		return _conn.type;
	}
protected:
	//	�߳�-����active_test,���ӻָ���
	unsigned int ThreadEntry(void);

	//	����submit_resp
	void do_submit_resp(cngp_submit_resp *p_submit_resp);

	//	����deliver
	void do_deliver(cngp_deliver *p_deliver);

	//	����active_test
	void do_active_test(cngp_active_test *p_active_test);

	//	����active_test_resp
	void do_active_test_resp(cngp_active_test_resp *p_active_test_resp);

	//	����connect_resp
	void do_login_resp(cngp_login_resp	*p_login_resp);

	//	����exit
	void do_exit(cngp_exit *p_exit);

	//	����exit_resp
	void do_exit_resp(cngp_exit_resp *p_exit_resp);
};

