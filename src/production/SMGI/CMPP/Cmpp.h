#pragma once

#include <vector>
#include <hash_map>
#include <hash_set>

#include "..\tcpip\tcpclient.h"
#include "cmpp_pkg.h"
#include "Sender.h"
#include "..\common\buf.h"
#include "..\SMGI\SMGDefine.h"
#include "..\lib\codeFactory.h"			//	bcd decode

using namespace sms::smgi;
/*
	Report״̬ 	����Ϣ״̬
	0�����ͳɹ�	DELIVERED
	1���ȴ�����	ENROUTE��ACCEPTED
	2������ʧ��	EXPIRED��DELETED��UNDELIVERABLE��UNKNOWN��REJECTED
*/
#define STAT0		 "DELIVRD"
#define STAT1		 "ENROUTE,ACCEPTD"
#define STAT2		 "EXPIRED,DELETED,UNDELIV,UNKNOWN��REJECTD"

#define STAT0_VALUE  0							//	״̬0
#define STAT1_VALUE  1							//	״̬1
#define STAT2_VALUE  2							//	״̬2
#define STAT_UNKNOW  -1							//	Ϊֹ״̬

//	���Ӽ���
#define CONN_HIGH    1							//	������
#define CONN_LOW     0							//	������	

//	��������
#define CONN_MT      0							//	����
#define CONN_MO      1							//	����

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

using namespace sms;
using namespace sms::cmpp;

//	������Ϣ
typedef struct CONNECTION
{
	unsigned long id;							//	���ӱ�ʶ
	unsigned char level;						//	����  0 - ��ͨ  1 - ����
	unsigned char type;							//	����  0 - ���л�����, 1-��
	
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

class CCmpp
{
public:
	CCmpp(void);
	~CCmpp(void);

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

	//	�·�����
	virtual void OnOverSpeed(unsigned long curr_delay_time, unsigned long err_code);

	unsigned long GetSeqID(void) const
	{
		return _seq->get_seq();
	}

	//	������	
	int Cleanup(void);
protected:
	long _client_idx;
	Process::CCriticalSection	_client_cs;
protected:
	cmpp::cmpp_seq	*_seq;						//	���кŲ�����
	std::vector<CClient *>	_clients;			//	�������ӵ�����
	CONNECT_INFO _conn_info;					//	���ز���
	CONNECTION _conn;							//	���Ӳ���
	
	long _high_index;							//	�����ȼ����ӵ��±�
	long _low_first;							//	�����ȼ���ʼ�±�
	unsigned long _low_count;					//	�����ȼ�����	
protected:

	//	����һ��CMPP����
	void CreateConn(unsigned int conn_id,
					unsigned int conn_level,
					unsigned int conn_type);

	//	��������CMPP����
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
	void SetInterval(unsigned long interval);
};

class CClient 
	: public tcpip::CTcpClient
	, public Process::CWorkerThread
{
public:
	CClient(CCmpp *p_cmpp,
		    cmpp_seq *p_seq,
	     	CONNECT_INFO *conn_info,
	    	CONNECTION *conn);

	~CClient(void);

	//	���ֿ��ܵ����ݰ�
	typedef struct cmpp_packets
	{
		cmpp_head				  cmpp_head;
		cmpp_submit_resp		  submit_resp;
		cmpp_deliver			  deliver;
		cmpp_active_test		  active_test;
		cmpp_active_test_resp	  active_test_resp;
		cmpp_connect_resp		  connect_resp;
		cmpp_cancel_resp		  cancel_resp;
		cmpp_query_resp			  query_resp;
		cmpp_terminate			  terminate;
		cmpp_terminate_resp       terminate_resp;
	}cmpp_packets;

protected:
	CCmpp			*_cmpp;						//	CMPPʵ��
	CSender			*_sender;					//	���Ͷ���
	buffer_block	*_buffer;					//	������
	cmpp_seq		*_cmpp_seq;					//	���кŲ�����
	encoding        *_uc_coding;

	char			 _svr_ip[MAX_IP_LEN + 1];	//	����ip
	unsigned short	 _port;						//	���ض˿�
	bool			 _is_connected;				//	�Ƿ�����
	CONNECT_INFO	_conn_info;					//	���ز���
    CONNECTION		_conn;						//	������Ϣ
	
	cmpp_packets	_packets;					//	������Ϣ
	DELIVER_SM		_deliver;					//	������Ϣ
	REPORT_SM		_report;					//	״̬����
	cmpp_deliver_report _cmpp_report;			//	״̬����ʵ��
	
	SUBMIT_RESP		_submit_resp;				//  �ύ����
public:
	//	����CMPP����
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
	//	���߳�-����active_test,���ӻָ���
	unsigned int ThreadEntry(void);

	//	����submit_resp
	void do_submit_resp(cmpp_submit_resp *p_submit_resp);

	//	����deliver
	void do_deliver(cmpp_deliver *p_deliver);

	//	����active_test
	void do_active_test(cmpp_active_test *p_active_test);

	//	����connect_resp
	void do_connect_resp(cmpp_connect_resp	*p_connect_resp);

	//	����cancel_resp
	void do_cancel_resp(cmpp_cancel_resp	*p_cancel_resp);

	//	����query_resp
	void do_query_resp(cmpp_query_resp *p_query_resp);

	//	����terminate
	void do_terminate(cmpp_terminate *p_terminate);

	//	����active_test_resp
	void do_active_test_resp(cmpp_active_test_resp *p_active_test_resp);

	//	����terminate_resp
	void do_terminate_resp(cmpp_terminate_resp *p_terminate_resp);

	//	״ֵ̬����
	int get_stat(const char *stat);
public:
	unsigned long submit_long_sm(SUBMIT_SM & submit_pkg);
	unsigned long submit_cut_sm(SUBMIT_SM & submit_pkg);
	// ��������
	unsigned long submit_norm(SUBMIT_SM & submit_pkg);
	void SetInterval(unsigned long interval);
};

