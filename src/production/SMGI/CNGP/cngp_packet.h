#pragma once

#define _TLV
/*
** Copyright (c) 2005
** Shen Zhen RongDi Information Technology Co., Ltd.
** All rights reserved.
**
** cngp_packet.h
** class cngp_xxx :Implement the encoding & decoding code of the whole packet 
**				that sp->smg & smg->sp communication using.
**
**
**
**
** Version:	1.0
** Author:	qsf
** Time:	2005/04/08
*/

#include <memory.h>					//  memcpy,memset
#include <stdio.h>					//  sprintf
#include <winsock.h>				//  htonl, ntohl ,htons, ntohs
#include <math.h>					// 	pow
#include "..\SMGI\SMGDefine.h"		//	SMS common define
#include "..\common\sync.h"			// 	CCriticalSection

using namespace sms::smgi;

#pragma warning(disable : 4103)
#pragma pack(push, 1)

/*
Report状态 	短消息状态
0，发送成功	DELIVERED
1，等待发送	ENROUTE，ACCEPTED
2，发送失败	EXPIRED，DELETED，UNDELIVERABLE，UNKNOWN，REJECTED
*/
#define STAT0		 "DELIVRD"
#define STAT1		 "ENROUTE,ACCEPTD"
#define STAT2		 "EXPIRED,DELETED,UNDELIV,UNKNOWN，REJECTD"

#define STAT0_VALUE   0  //  状态0
#define STAT1_VALUE   1  //  状态1
#define STAT2_VALUE   2	 //  状态2
#define STAT_UNKNOW   9  //  未知状态

//  数据类型
#define UINT8 unsigned __int64
#define UINT4 unsigned long
#define UINT2 unsigned short
#define UINT1 unsigned char

//	消息字段长度的常量定义

//	login
static const int MAX_CLIENT_ID_LEN		= 10;
static const int MAX_AUTH_CLIENT_LEN	= 16;
static const int MAX_TIME_STAMP_LEN		= 10;
static const int MAX_SHARED_SECRET_LEN	= 15;
static const int MAX_MD5_SRC_LEN		= 50;

//  login_resp
static const int MAX_AUTH_SERVER_LEN	= 16;

//	submit
static const int MAX_SPID_LEN			= 10;
static const int MAX_SERVICE_ID_LEN		= 10;
static const int MAX_FEE_CODE_LEN		= 6;
static const int MAX_FEE_TYPE_LENGTH	= 2;
static const int MAX_FIXED_FEE_LEN		= 6;
static const int MAX_VALID_TIME_LEN		= 17;
static const int MAX_AT_TIME_LEN		= 17;
static const int MAX_SRC_TERM_ID_LEN	= 21;
static const int MAX_CHARGE_TERM_ID_LEN = 21;
static const int MAX_DEST_TERM_ID_LEN	= 21;
static const int MAX_MSG_CONT_LEN		= 254;
static const int MAX_RESERVE_LEN		= 8;

//	MSG_ID
static const int MAX_MSG_ID_LEN			= 10;
static const int MAX_SMG_ID_LEN			= 3;
static const int MAX_MSG_ID_SEQ			= 7;

//	deliver
static const int MAX_RECV_TIME_LEN		= 14;

//	report label
static const int LB_ID_LEN				= 3;
static const int LB_SUB_LEN				= 4;
static const int LB_DLVRD_LEN			= 6;
static const int LB_SUBMIT_DATE_LEN		= 12;
static const int LB_DONE_DATE_LEN		= 10;
static const int LB_STAT_LEN			= 5;
static const int LB_ERR_LEN				= 4;
static const int LB_TXT_LEN				= 4;
	
//	report field
static const int MAX_ID_LEN				= 10;
static const int MAX_SUB_LEN			= 3;
static const int MAX_DLVRD_LEN			= 3;
static const int MAX_SUBMIT_DATE_LEN	= 10;
static const int MAX_DONE_DATE_LEN		= 10;
static const int MAX_STAT_LEN			= 7;
static const int MAX_ERR_LEN			= 3;
static const int MAX_TXT_LEN			= 20;

//	收费类型
#define FEE_TYPE_FREE		"00"					//	免费
#define FEE_TYPE_ITEM		"01"					//	按条收费
#define FEE_TYPE_MONTH		"02"					//	包月
#define FEE_TYPE_FIXED		"03"					//	封顶
#define FEE_TYPE_MONTH_REQ	"04"					//	包月扣费请求
#define FEE_TYPE_CR			"05"					//	CR话单

//	tlv param_tag定义
static const UINT2 PROT_ID		= 0x0100;			//	协议ID
static const UINT2 CONG_STAT	= 0x0428;			//	拥赛状态	

static UINT8 bcd2int(const BYTE src_str[], UINT4 src_len);

namespace sms
{
	namespace cngp 
	{
		class cngp_def
		{
		public:
			//  请求标识
			enum REQ_ID														
			{
				LOGIN				= 0X00000001,   //  登录
				LOGIN_RESP			= 0X80000001,	//  登录回应
				SUBMIT				= 0X00000002,	//  提交
				SUBMIT_RESP			= 0X80000002,	//  提交回应
				DELIVER				= 0X00000003,	//  上行
				DELIVER_RESP		= 0X80000003,	//  上行回应
				ACTIVE_TEST			= 0X00000004,	//  线路检测
				ACTIVE_TEST_RESP	= 0X80000004,	//  线路检测回应
				EXIT                = 0X00000006,	//  退出
				EXIT_RESP           = 0X80000006,	//  退出回应
			};
			
			//  登录模式
			enum LOGIN_MODE
			{
				MT		= 0,						//  MO
				MO		= 1,						//  MT
				MO_MT   = 2							//  MO & MT 
			};
			//  信息格式
			enum MSG_FMT   
			{
				ASCII	  = 0,						//  ASCII串；
				WRT_CARD  = 3,						//  短信写卡操作；
				BIN		  = 4,						//  二进制信息；
				UCS2	  = 8,						//  UCS2编码；
				GB2312	  = 15,						//  含GB汉字
			};
			
			//	SUBMIT MSG_TYPE
			enum MSG_TYPE
			{
				CANCEL_ORDER	= 0,				//取消订阅
				ORDER_REQ		= 1,				//订阅或点播请求
				ORDER_MT		= 2,				//点播下发
				ORDER_SER_MT	= 3,				//订阅下发
			};
			//其他定义
			enum 
			{
				HEAD_LEN		= 16,				//	头部长度
				MSG_ID_LEN		= 10,				//  MSG_ID长度
				IS_REPORT		= 1,				//	需要(是)报告
				SUBMIT_OK		= 0,				//	提交成功
				DELVIER_OK		= 0					//  递送成功
			};

			enum FEE_USER_TYPE
			{
                FEE_DEST_TERM_ID	= 0,			//	对目的终端计费
				FEE_SRC_TERM_ID		= 1,			//	对源终端计费
				FEE_SP				= 2,			//	对SP计费
				FEE_CHARGE_TERM_ID	= 3				//	对计费用户号码计费
			};
			//	MSG_ID的格式
			typedef struct MSG_ID
			{
				BYTE smg_id[3];
				BYTE seq_id[7];
			}MSG_ID;

			//  定义各种包结构
			
			//	TLV
			typedef struct cngp_tlv
			{
				UINT2 param_tag;
				UINT2 len;
				UINT1 value;				
				cngp_tlv(void)
				{	
					memset(this, 0, sizeof(cngp_tlv));
				}
			}cngp_tlv;

			//  包头
			typedef struct cngp_head 
			{
				UINT4 pkt_len;
				UINT4 req_id;
				UINT4 req_stat;
				UINT4 seq_id;
				cngp_head(void)
				{		
					memset(this, 0, sizeof(cngp_head));
				}
			}cngp_head;

			//  登录包
			typedef struct cngp_login
			{
				cngp_head head;
				char client_id[MAX_CLIENT_ID_LEN];
				char authenticator_client[MAX_AUTH_CLIENT_LEN];
				UINT1 login_mode;
				UINT4 time_stamp;
				UINT1 version;
				cngp_login(void)
				{
					memset(this, 0, sizeof(cngp_login));
				}
			}cngp_login;

			//  登录回应包
			typedef struct cngp_login_resp
			{	
				cngp_head head;
				char  authenticator_server[MAX_AUTH_SERVER_LEN];
				UINT1 version;
				cngp_login_resp(void)
				{
					memset(this, 0, sizeof(cngp_login_resp));
				}
			}cngp_login_resp;

			//  提交包
			typedef struct cngp_submit
			{
				cngp_head head;
				char  spid[MAX_SPID_LEN];
				UINT1 msg_type;
				UINT1 need_report;
				UINT1 priority;
				char  service_id[MAX_SERVICE_ID_LEN];
				char  fee_type[MAX_FEE_TYPE_LENGTH];
				UINT1 fee_user_type;
				char  fee_code[MAX_FEE_CODE_LEN];
				UINT1 msg_fmt;
				char  valid_time[MAX_VALID_TIME_LEN];
				char  at_time[MAX_AT_TIME_LEN];
				char  src_term_id[MAX_SRC_TERM_ID_LEN];
				char  charge_term_id[MAX_CHARGE_TERM_ID_LEN];
				UINT1 dest_term_id_count;
				char  dest_term_id[MAX_DEST_TERM_ID_LEN];
				UINT1 msg_len;
				char  msg_content[MAX_MSG_CONT_LEN];     //  最多58个汉字
				#ifdef _TLV
					cngp_tlv prot_id;
				#endif				
				cngp_submit(void)
				{
					memset(this, 0, sizeof(cngp_submit));
				}
			}cngp_submit;

			//  提交回应包
			typedef struct cngp_submit_resp
			{
				cngp_head head;
				union
				{
					MSG_ID		  msg_id;
					unsigned char sz_msg_id[MAX_MSG_ID_LEN];
				};
				#ifdef _TLV
					cngp_tlv cong_stat;
				#endif
				cngp_submit_resp(void)
				{
					memset(this, 0, sizeof(cngp_submit_resp));
				}
			}cngp_submit_resp;

			//  上行包
			typedef struct cngp_deliver
			{
				cngp_head head;
				union
				{
					MSG_ID			 msg_id;
					unsigned char	 sz_msg_id[MAX_MSG_ID_LEN];
				};
				UINT1 is_report;
				UINT1 msg_fmt;
				char rcv_time[MAX_RECV_TIME_LEN];
				char src_term_id[MAX_SRC_TERM_ID_LEN];
				char dest_term_id[MAX_DEST_TERM_ID_LEN];
				UINT1 msg_length;
				char msg_content[MAX_MSG_CONT_LEN];
				#ifdef _TLV
					cngp_tlv prot_id;
				#endif
				cngp_deliver(void)
				{
					memset(this, 0, sizeof(cngp_deliver));
				}
			}cngp_deliver;

			//  上行回应包
			typedef struct cngp_deliver_resp
			{
				cngp_head head;
				union
				{
					MSG_ID			msg_id;
					unsigned char	sz_msg_id[MAX_MSG_ID_LEN];
				};
				#ifdef _TLV
					cngp_tlv cong_stat;
				#endif
				cngp_deliver_resp(void)
				{
					memset(this, 0, sizeof(cngp_deliver_resp));
				}
			}cngp_deliver_resp;

			//  状态报告
			typedef struct cngp_report
			{
				char id_label[LB_ID_LEN];
				union
				{
					MSG_ID			msg_id;
					unsigned char	sz_msg_id[MAX_MSG_ID_LEN+1];
				};
				char sub_label[LB_SUB_LEN];
				char sub[MAX_SUB_LEN + 1];
				char dlvrd_lable[LB_DLVRD_LEN];
				char dlvrd[MAX_DLVRD_LEN + 1];
				char submit_date_label[LB_SUBMIT_DATE_LEN];
				char submit_date[MAX_SUBMIT_DATE_LEN + 1];
				char done_date_label[LB_DONE_DATE_LEN];
				char done_time[MAX_DONE_DATE_LEN + 1];
				char stat_label[LB_STAT_LEN];
				char stat[MAX_STAT_LEN + 1];
				char err_label[LB_ERR_LEN];
				char err[MAX_ERR_LEN + 1];
				char txt_label[LB_TXT_LEN];
				char txt[MAX_TXT_LEN];
				cngp_report(void)
				{
					memset(this, 0, sizeof(cngp_report));
				}
			}cngp_report;

			//  线路检测包
			typedef struct cngp_active_test 
			{
				cngp_head head;
			}cngp_active_test;

			//  线路检测回应包
			typedef struct cngp_active_test_resp
			{
				cngp_head head;
			}cngp_active_test_resp ;

			//  退出
			typedef struct cngp_exit
			{
				cngp_head head;
			}cngp_exit;

			//  退出回应包
			typedef struct cngp_exit_resp {
				cngp_head head;
			} cngp_exit_resp;

		};
		class cngp_packet
		{
		public:
			cngp_packet(void);
			~cngp_packet(void);
		
		public:
			//	释放缓冲区
			virtual void release(void) = 0;

			//	返回序列号
			virtual UINT4 get_seq_id(void) const = 0;

			//	对消息编码
			static void encode_head(cngp_def::cngp_head &head);

			//	对消息头解码
			static void decode_head(cngp_def::cngp_head & head);

		public:
			//	返回缓冲区地址
			virtual const char* buf(void)
			{
				return NULL;
			}

			//	返回消息长度
			virtual UINT4 len(void) const
			{
				return 0;
			}

			//	设置消息序列号
			virtual void set_seq_id(UINT4)
			{ 
				return;
			}

			//	解码
			virtual void decode(const char *buf)
			{
				return;
			}

			//	编码
			virtual void encode(void)
			{
				return;
			}

			//	tlv编码
			static void encode_tlv(cngp_def::cngp_tlv & tlv);
		};
		
		//  登录
		class cngp_login :
			public cngp_packet
		{
		public:
			cngp_login(void);
			~cngp_login(void);

		public:
			void encode(void);
			void release(void);

		public:
			const char* buf(void)
			{
				return (char *)_data;
			}

			UINT4 len(void) const
			{
				return sizeof(cngp_def::cngp_login);
			}

			void set_client_id(const char *client_id)
			{
				strncpy(_data->client_id, client_id,sizeof(_data->client_id));
			}

			void set_login_mode(UINT1 login_mode)
			{
				_data->login_mode = login_mode;
			}

			void set_tm_stmp(UINT4 tm_stmp)
			{
				_data->time_stamp = tm_stmp;
			}

			void set_version(UINT1 version)
			{
				_data->version = version;
			}

			void set_shared_secret(const char *shared_secret)
			{
				strncpy(_shared_secret, shared_secret, sizeof(_shared_secret));
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}
			
			UINT4 get_seq_id(void) const
			{
				return ntohl(_data->head.seq_id);
			}

		private:		
			//  构造加密串
			UINT4 set_authenticator_client(void);

		private:
			cngp_def::cngp_login *_data;

			char _shared_secret[MAX_SHARED_SECRET_LEN];

		};

		//  登录回应
		class cngp_login_resp :
			public cngp_packet
		{
		public:
			cngp_login_resp(void);
			~cngp_login_resp(void);

		public:
			void release(void);

			void decode(const char *buf);
		
		public:
			UINT4 get_seq_id(void) const
			{
				return _data->head.seq_id;
			}

			UINT4 get_status(void) const
			{
				return _data->head.req_stat;
			}

			UINT4 get_version(void) const
			{
				return _data->version;
			}

		private:
			cngp_def::cngp_login_resp *_data;
			
		};

		//  提交
		class cngp_submit :
			public cngp_packet
		{
		public:
			cngp_submit(void);
			~cngp_submit(void);

		public:
			void release(void);
			void encode(void);

		public:
			const char* buf(void)
			{
				return (char *)_data;
			}
			
			UINT4 len(void) const
			{
				return _enc_len;
			}

			UINT4 get_seq_id(void) const
			{
				return ntohl(_data->head.seq_id);
			}

			void set_spid(const char *spid)
			{
				strncpy(_data->spid, spid, sizeof(_data->spid));
			}

			void set_msg_type(UINT1 msg_type)
			{
				_data->msg_type = msg_type;
			}

			void set_nd_rpt(UINT1 nd_rpt)
			{
				_data->need_report = nd_rpt;
			}
			
			void set_pri(UINT1 pri)
			{
				_data->priority = pri;
			}

			void set_svr_id(const char *svr_id)
			{
				strncpy(_data->service_id, svr_id, sizeof(_data->service_id));
			}

			void set_fee_type(const char *fee_type)
			{
				strncpy(_data->fee_type, fee_type, sizeof(_data->fee_type));
			}

			void set_fee_user_type(UINT1 fee_user_type)
			{
				_data->fee_user_type = fee_user_type;
			}

			void set_fee_code(const char *fee_code)
			{
				strncpy(_data->fee_code, fee_code, sizeof(_data->fee_code));
			}

			void set_vld_time(const char *vld_time)
			{
				strncpy(_data->valid_time, vld_time, sizeof(_data->valid_time));
			}

			void set_at_time(const char *at_time)
			{
				strncpy(_data->at_time, at_time, sizeof(_data->at_time));
			}

			void set_src_term_id(const char *src_term_id)
			{
				strncpy(_data->src_term_id, src_term_id, sizeof(_data->src_term_id));
			}

			void set_charge_term_id(const char *charge_term_id)
			{
				strncpy(_data->charge_term_id, 
					    charge_term_id, 
						sizeof(_data->charge_term_id));
			}

			void set_dest_id_cnt(UINT1 dest_id_cnt)
			{
				_data->dest_term_id_count = dest_id_cnt;
			}

			void set_dest_term_id(const char *dest_term_id)
			{
				strncpy(_data->dest_term_id, dest_term_id, sizeof(_data->dest_term_id));
			}

			void set_msg_cont(const char *msg, UINT1 msg_len, UINT1 msg_fmt)
			{
				_data->msg_fmt = msg_fmt;
				_data->msg_len = msg_len;
				memcpy(_data->msg_content, msg, min(msg_len,sizeof(_data->msg_content)));
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

		private:
			cngp_def::cngp_submit * _data;
			//	编码后长度
			UINT4					_enc_len;
		};

		//  提交回应
		class cngp_submit_resp :
			public cngp_packet
		{
		public:
			cngp_submit_resp(void);
			~cngp_submit_resp(void);
		private:
			cngp_def::cngp_submit_resp *_data;
			//  msg_id的序列号部分
			UINT8	_msg_id;   
		public:
			void release(void);
			void decode(const char *buf);

		public:
			UINT4 get_seq_id(void) const
			{
				return _data->head.seq_id;
			}

			//  取MSG_ID的序列号部分
			UINT8 get_msg_id_seq(void) const
			{
				return _msg_id;
			}

			UINT4 get_status(void) const
			{
				return _data->head.req_stat;
			}
		};

		//  上行
		class cngp_deliver :
			public cngp_packet
		{
		public:
			cngp_deliver(void);
			~cngp_deliver(void);

		public:
			void release(void);
			void decode(const char *buf, UINT4 len);

		public:
			UINT4 get_seq_id(void) const
			{
				return _data->head.seq_id;
			}

			UINT8 get_msg_id_seq(void) const
			{
				return _msg_id;
			}

			UINT4 get_msg_id(LPBYTE msg_id) const
			{
				memcpy(msg_id, &_data->sz_msg_id, sizeof(_data->sz_msg_id));
				return sizeof(_data->sz_msg_id);
			}

			UINT4 get_is_rpt(void) const
			{
				return _data->is_report;
			}

			UINT4 get_msg_fmt(void) const
			{
				return _data->msg_fmt;
			}

			const char* get_src_term_id(void)
			{
				return _data->src_term_id;
			}

			const char* get_dest_term_id(void)
			{
				return _data->dest_term_id;
			}

			const char* get_msg_cont(void)
			{
				return _data->msg_content;
			}
			
			UINT4 get_msg_len(void) const
			{
				return _data->msg_length;
			}

		private:
			cngp_def::cngp_deliver *_data;

			//	msg_id的序列号部分
			UINT8 _msg_id;

		};

		//  上行回应
		class cngp_deliver_resp :
			public cngp_packet
		{
		public:
			cngp_deliver_resp(void);
			~cngp_deliver_resp(void);

		public:
			void encode(void);
			void release(void);

		public:
			void set_status(UINT4 status)
			{
				_data->head.req_stat = status;
			}

			void set_msg_id(LPBYTE msg_id)
			{
				memcpy(&_data->sz_msg_id, msg_id, sizeof(_data->sz_msg_id));
			}

			const char* buf(void)
			{
				return (char *)_data;
			}

			UINT4 len(void) const
			{
				return sizeof(cngp_def::cngp_deliver_resp);
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

			UINT4 get_seq_id(void) const
			{
				return ntohl(_data->head.seq_id);
			}

		private:
			cngp_def::cngp_deliver_resp *_data;

		};

		//  报告
		class cngp_report 
		{
		public:
			cngp_report(void);
			~cngp_report(void);

		private:
			UINT4	 get_stat_code(const char *stat);
		
		public:
			void decode(const char * buf, UINT4 len);
			void release(void);

		public:
			UINT4 get_stat(void) const
			{
				return _stat;
			}

			const char* get_err_code(void)
			{
				return (char*)_err_code;
			}

			UINT8 get_msg_id_seq(void)
			{
				return _msg_id;
			}
		private:
			cngp_def::cngp_report *_data;

			UINT4	_stat;

			char	_err_code[MAX_ERR_CODE_LEN + 1];

			UINT8   _msg_id;

		};

		//  线路检测
		class cngp_active_test :
			public cngp_packet
		{
		public:
			cngp_active_test(void);
			~cngp_active_test(void);

		public:
			void decode(const char *buf);
			void release(void);
			void encode(void);

		public:
			const char* buf(void)
			{
				return (char *)_data;
			}

			UINT4 len(void) const
			{
				return sizeof(cngp_def::cngp_active_test);
			}

			UINT4 get_seq_id(void) const
			{
				return ntohl(_data->head.seq_id);
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

		private:
			cngp_def::cngp_active_test *_data;

		};

		//  线路检测回复
		class cngp_active_test_resp :
			public cngp_packet
		{
		public:
			cngp_active_test_resp(void);
			~cngp_active_test_resp(void);
		
		public:
			void encode(void);
			void decode(const char *buf);
			void release(void);
	
		public:
			const char* buf(void)
			{
				return (char *)_data;
			}

			UINT4 get_seq_id(void) const
			{
				return ntohl(_data->head.seq_id);
			}

			UINT4 len(void) const
			{
				return sizeof(cngp_def::cngp_active_test_resp);
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

		private:
			cngp_def::cngp_active_test_resp *_data;

		};

		//  退出
		class cngp_exit :
			public cngp_packet
		{
		public:
			cngp_exit(void);
			~cngp_exit(void);

		public:
			void encode(void);
			void decode(const char *buf);
			void release(void);

		public:
			const char* buf(void)
			{
				return (char *)_data;
			}

			UINT4 get_seq_id(void) const
			{
				return ntohl(_data->head.seq_id);       
			}

			UINT4 len(void) const
			{
				return sizeof(cngp_def::cngp_exit);
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

		private:
			cngp_def::cngp_exit *_data;
		};

		//  退出回应
		class cngp_exit_resp :
			public cngp_packet
		{
		public:
			cngp_exit_resp(void);
			~cngp_exit_resp(void);
        
		public:
			void encode(void);
			void decode(const char *buf);
			void release(void);

		public:
			const char* buf(void)
			{
				return (char *)_data;
			}

			UINT4 get_seq_id(void) const
			{
				return ntohl(_data->head.seq_id);
			}

			UINT4 len(void) const
			{
				return sizeof(cngp_def::cngp_exit_resp);
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

		private:
			cngp_def::cngp_exit_resp *_data;

		};

		//  消息头
		class cngp_head :
			public cngp_packet
		{
		private:
			cngp_def::cngp_head *_data;
		public:
			cngp_head(void);
			~cngp_head(void);

		public:
			void decode(const char * buf);
			void release(void);

		public:
			UINT4 get_seq_id(void) const
			{
				return _data->req_id;
			}
			UINT4 get_req_id(void) const
			{
				return _data->req_id;
			}

			UINT4 get_pkt_len(void)
			{
				return _data->pkt_len;
			}

		};

		//  seq类,用于设置消息序列号
		class cngp_seq
		{
		public:
			cngp_seq(UINT4 i_seed);
			~cngp_seq(void);

		public:
			void set_seq(cngp_packet &cngp_pkt);
			void set_seq(cngp_packet *cngp_pkt);
			UINT4 get_seq(void) const
			{
				return _seed;
			}
		private:
			Process::CCriticalSection m_lock;
			UINT4 _seed;

		};
	}
}
#pragma pack(pop)
