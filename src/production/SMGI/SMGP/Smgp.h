#pragma once

#include <vector>								// stl	vector
#include "..\tcpip\tcpclient.h"					// class	CTcpClient
#include "smgp_packet.h"						// classes	smgp packet encode & decode
#include "Sender.h"								// class CSender
#include "..\common\buf.h"						// class  buffer_block
#include "..\smgi\smgdefine.h"
#include "..\lib\codeFactory.h"					//	bcd decode

using namespace sms::smgi;

//	���Ӽ���
#define CONN_HIGH    1							//	������
#define CONN_LOW     0							//	������	

#ifdef _SMGP3_ 
	#define SMGP_VER     0x30
#else
	#define SMGP_VER     0x13                   //  �汾 1.3
#endif
/*
	��������
	0-SGIP��׼;(1-9����)
	10-CMPP��׼(��Ϊ)��11-CMPP ˫�˿�(�廪��Ѷ)��12-CMPP ͬ�˿�˫����(����)
	20-SMGP��׼(ͬ�˿�˫����)
	30-CNGP��׼(ͬ�˿�˫����)
*/
enum SMG_TYPE
{
	SGIP_STD	= 0,
	CMPP_STD	= 10,
	CMPP_QHSX	= 11,
	CMPP_YX		= 12,
	SMGP_STD	= 20,
	CNGP_STD	= 30
};

//using namespace sms;
using namespace smgp;

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

class CSmgp
{
public:
	CSmgp(void);
	~CSmgp(void);

	//	��ʼ��,���ò���,���������̵߳�
	bool Startup(CONNECT_INFO &info);

	//	�ύ����
	unsigned long Submit(SUBMIT_SM &submit, bool prio);

	//	�ύ�ɹ���������ϢID	
	virtual void OnSubmitResp(SUBMIT_RESP &resp) = 0;

	//	����
	virtual void OnDeliver(DELIVER_SM &d) = 0;

	//	״̬����
	virtual void OnReport(REPORT_SM &r) = 0;

	//	������	
	int Cleanup(void);
protected:
	smgp_seq		*_seq;						//	���кŲ�����
	std::vector<CClient *>	_clients;			//	�������ӵ�����
	CONNECT_INFO _conn_info;					//	���ز���
	CONNECTION _conn;							//	���Ӳ���
	
	long _high_index;							//	�����ȼ����ӵ��±�
	long _low_first;							//	�����ȼ���ʼ�±�
	unsigned long _low_count;					//	�����ȼ�����
	long _low_index;							//	�ϴη��͵������±�
	long _client_idx;
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
	CClient(CSmgp *p_smgp,
		    smgp_seq *p_seq,
	     	CONNECT_INFO *conn_info,
	    	CONNECTION *conn);

	~CClient(void);

	//	���ֿ��ܵ����ݰ�
	typedef struct smgp_packets
	{
		smgp_head				  head;
		smgp_submit_resp		  submit_resp;
		smgp_deliver			  deliver;
		smgp_active_test		  active_test;
		smgp_active_test_resp	  active_test_resp;
		smgp_login_resp			  login_resp;
		smgp_query_resp			  query_resp;
		smgp_exit				  exit;
		smgp_exit_resp			  exit_resp;
	}smgp_packets;

protected:
	CSmgp			*_smgp;						//	SMGPʵ��
	CSender			*_sender;					//	���Ͷ���
	buffer_block	*_buffer;					//	������
	smgp_seq		*_smgp_seq;					//	���кŲ�����
	encoding		*_uc_coding;				//	�������

	char			_svr_ip[MAX_IP_LEN + 1];	//	����ip
	unsigned short	_port;						//	���ض˿�
	bool			_is_connected;				//	�Ƿ�����
	CONNECT_INFO	_conn_info;					//	���ز���
    CONNECTION		_conn;						//	������Ϣ
	
	smgp_packets	_packets;					//	������Ϣ
	DELIVER_SM		_deliver;					//	������Ϣ
	REPORT_SM		_report;					//	״̬����
	smgp_report		_smgp_report;		    	//	״̬����ʵ��
	
	SUBMIT_RESP		_submit_resp;				//  �ύ����
	time_t			_last_time;					//  ���һ����Ϣ��ʱ��
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

	//	�ָ����
	unsigned long submit_cut_sm(SUBMIT_SM & submit_pkg);
	// ��������
	unsigned long submit_norm(SUBMIT_SM & submit_pkg);
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
	void do_submit_resp(smgp_submit_resp *p_submit_resp);

	//	����deliver
	void do_deliver(smgp_deliver *p_deliver);

	//	����active_test
	void do_active_test(smgp_active_test *p_active_test);

	//	����active_test_resp
	void do_active_test_resp(smgp_active_test_resp *p_active_test_resp);

	//	����connect_resp
	void do_login_resp(smgp_login_resp	*p_login_resp);

	//	����query_resp
	void do_query_resp(smgp_query_resp *p_query_resp);

	//	����exit
	void do_exit(smgp_exit *p_exit);

	//	����exit_resp
	void do_exit_resp(smgp_exit_resp *p_exit_resp);
};

