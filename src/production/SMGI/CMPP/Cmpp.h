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
	Report状态 	短消息状态
	0，发送成功	DELIVERED
	1，等待发送	ENROUTE，ACCEPTED
	2，发送失败	EXPIRED，DELETED，UNDELIVERABLE，UNKNOWN，REJECTED
*/
#define STAT0		 "DELIVRD"
#define STAT1		 "ENROUTE,ACCEPTD"
#define STAT2		 "EXPIRED,DELETED,UNDELIV,UNKNOWN，REJECTD"

#define STAT0_VALUE  0							//	状态0
#define STAT1_VALUE  1							//	状态1
#define STAT2_VALUE  2							//	状态2
#define STAT_UNKNOW  -1							//	为止状态

//	连接级别
#define CONN_HIGH    1							//	高优先
#define CONN_LOW     0							//	低优先	

//	连接类型
#define CONN_MT      0							//	下行
#define CONN_MO      1							//	上行

/*
	网关类型
	0-SGIP标准;(1-9保留)
	10-CMPP标准(华为)；11-CMPP 双端口(清华深讯)；12-CMPP 同端口双连接(亚信)
	20-SMGP标准(同端口双连接)
	30-CNGP标准(同端口双连接)
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

//	连接信息
typedef struct CONNECTION
{
	unsigned long id;							//	连接标识
	unsigned char level;						//	级别  0 - 普通  1 - 紧急
	unsigned char type;							//	类型  0 - 下行或含下行, 1-上
	
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

	//	初始化,设置参数,启动工作线程等
	bool Startup(CONNECT_INFO &info);

	//	提交短信
	unsigned long Submit(SUBMIT_SM &submit, bool prio);

	//	提交成功，返回消息ID	
	virtual void OnSubmitResp(SUBMIT_RESP &resp) = 0;

	//	上行
	virtual void OnDeliver(DELIVER_SM &d) = 0;

	//	状态报告
	virtual void OnReport(REPORT_SM &r) = 0;

	//	下发超量
	virtual void OnOverSpeed(unsigned long curr_delay_time, unsigned long err_code);

	unsigned long GetSeqID(void) const
	{
		return _seq->get_seq();
	}

	//	清理函数	
	int Cleanup(void);
protected:
	long _client_idx;
	Process::CCriticalSection	_client_cs;
protected:
	cmpp::cmpp_seq	*_seq;						//	序列号产生器
	std::vector<CClient *>	_clients;			//	保存连接的数组
	CONNECT_INFO _conn_info;					//	网关参数
	CONNECTION _conn;							//	连接参数
	
	long _high_index;							//	高优先级连接的下标
	long _low_first;							//	低优先级起始下标
	unsigned long _low_count;					//	低优先级个数	
protected:

	//	创建一个CMPP连接
	void CreateConn(unsigned int conn_id,
					unsigned int conn_level,
					unsigned int conn_type);

	//	创建所以CMPP连接
	void CreateAllConns(void);

	/*  找一个合适的连接发短信
	*
	*	规则：1.如果是普通数据,找比较空闲的一个普通连接
	*         2.如果是紧急数据,找紧急连接，如果没有，按普通数据处理
	*
	*   调用频率比较高,所以内联
	*/
private:
	// 设置连接信息初值(高,低优先级)
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

	//	各种可能的数据包
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
	CCmpp			*_cmpp;						//	CMPP实例
	CSender			*_sender;					//	发送对象
	buffer_block	*_buffer;					//	缓冲区
	cmpp_seq		*_cmpp_seq;					//	序列号产生器
	encoding        *_uc_coding;

	char			 _svr_ip[MAX_IP_LEN + 1];	//	网关ip
	unsigned short	 _port;						//	网关端口
	bool			 _is_connected;				//	是否连接
	CONNECT_INFO	_conn_info;					//	网关参数
    CONNECTION		_conn;						//	连接信息
	
	cmpp_packets	_packets;					//	各种消息
	DELIVER_SM		_deliver;					//	上行消息
	REPORT_SM		_report;					//	状态报告
	cmpp_deliver_report _cmpp_report;			//	状态报告实例
	
	SUBMIT_RESP		_submit_resp;				//  提交反馈
public:
	//	连接CMPP网关
	void connect_smg(void);

	//	提交短信
	unsigned long submit(SUBMIT_SM &submit_pkg);

	//	重载基类须函数
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
	//	类线程-用于active_test,连接恢复等
	unsigned int ThreadEntry(void);

	//	处理submit_resp
	void do_submit_resp(cmpp_submit_resp *p_submit_resp);

	//	处理deliver
	void do_deliver(cmpp_deliver *p_deliver);

	//	处理active_test
	void do_active_test(cmpp_active_test *p_active_test);

	//	处理connect_resp
	void do_connect_resp(cmpp_connect_resp	*p_connect_resp);

	//	处理cancel_resp
	void do_cancel_resp(cmpp_cancel_resp	*p_cancel_resp);

	//	处理query_resp
	void do_query_resp(cmpp_query_resp *p_query_resp);

	//	处理terminate
	void do_terminate(cmpp_terminate *p_terminate);

	//	处理active_test_resp
	void do_active_test_resp(cmpp_active_test_resp *p_active_test_resp);

	//	处理terminate_resp
	void do_terminate_resp(cmpp_terminate_resp *p_terminate_resp);

	//	状态值翻译
	int get_stat(const char *stat);
public:
	unsigned long submit_long_sm(SUBMIT_SM & submit_pkg);
	unsigned long submit_cut_sm(SUBMIT_SM & submit_pkg);
	// 正常发送
	unsigned long submit_norm(SUBMIT_SM & submit_pkg);
	void SetInterval(unsigned long interval);
};

