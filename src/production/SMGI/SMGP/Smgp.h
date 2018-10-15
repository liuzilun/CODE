#pragma once

#include <vector>								// stl	vector
#include "..\tcpip\tcpclient.h"					// class	CTcpClient
#include "smgp_packet.h"						// classes	smgp packet encode & decode
#include "Sender.h"								// class CSender
#include "..\common\buf.h"						// class  buffer_block
#include "..\smgi\smgdefine.h"
#include "..\lib\codeFactory.h"					//	bcd decode

using namespace sms::smgi;

//	连接级别
#define CONN_HIGH    1							//	高优先
#define CONN_LOW     0							//	低优先	

#ifdef _SMGP3_ 
	#define SMGP_VER     0x30
#else
	#define SMGP_VER     0x13                   //  版本 1.3
#endif
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

//using namespace sms;
using namespace smgp;

//	连接信息
typedef struct CONNECTION
{
	unsigned long id;							//	连接标识
	unsigned char level;						//	级别  0 - 普通  1 - 紧急
	unsigned char type;							//	类型  0 - 下行, 1-上行 ,2 -上下行
	
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

	//	清理函数	
	int Cleanup(void);
protected:
	smgp_seq		*_seq;						//	序列号产生器
	std::vector<CClient *>	_clients;			//	保存连接的数组
	CONNECT_INFO _conn_info;					//	网关参数
	CONNECTION _conn;							//	连接参数
	
	long _high_index;							//	高优先级连接的下标
	long _low_first;							//	低优先级起始下标
	unsigned long _low_count;					//	低优先级个数
	long _low_index;							//	上次发送的连接下表
	long _client_idx;
protected:

	//	创建一个连接
	void CreateConn(unsigned int conn_id,
					unsigned int conn_level,
					unsigned int conn_type);

	//	创建所有连接
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

	//	各种可能的数据包
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
	CSmgp			*_smgp;						//	SMGP实例
	CSender			*_sender;					//	发送对象
	buffer_block	*_buffer;					//	缓冲区
	smgp_seq		*_smgp_seq;					//	序列号产生器
	encoding		*_uc_coding;				//	编解码器

	char			_svr_ip[MAX_IP_LEN + 1];	//	网关ip
	unsigned short	_port;						//	网关端口
	bool			_is_connected;				//	是否连接
	CONNECT_INFO	_conn_info;					//	网关参数
    CONNECTION		_conn;						//	连接信息
	
	smgp_packets	_packets;					//	各种消息
	DELIVER_SM		_deliver;					//	上行消息
	REPORT_SM		_report;					//	状态报告
	smgp_report		_smgp_report;		    	//	状态报告实例
	
	SUBMIT_RESP		_submit_resp;				//  提交反馈
	time_t			_last_time;					//  最后一次消息的时间
public:
	//	连接网关
	void connect_smg(void);

	//	提交短信
	unsigned long submit(SUBMIT_SM &submit_pkg);

	//	重载基类须函数
	void OnConnect(tcpip::CTcpClient *tcp_client);
	void OnDisconnect(tcpip::CTcpClient *tcp_client);
	void OnRecv(char *buf, int len);
	void OnReconnect(tcpip::CTcpClient *tcp_client);

	//	分割短信
	unsigned long submit_cut_sm(SUBMIT_SM & submit_pkg);
	// 正常发送
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
	//	线程-用于active_test,连接恢复等
	unsigned int ThreadEntry(void);

	//	处理submit_resp
	void do_submit_resp(smgp_submit_resp *p_submit_resp);

	//	处理deliver
	void do_deliver(smgp_deliver *p_deliver);

	//	处理active_test
	void do_active_test(smgp_active_test *p_active_test);

	//	处理active_test_resp
	void do_active_test_resp(smgp_active_test_resp *p_active_test_resp);

	//	处理connect_resp
	void do_login_resp(smgp_login_resp	*p_login_resp);

	//	处理query_resp
	void do_query_resp(smgp_query_resp *p_query_resp);

	//	处理exit
	void do_exit(smgp_exit *p_exit);

	//	处理exit_resp
	void do_exit_resp(smgp_exit_resp *p_exit_resp);
};

