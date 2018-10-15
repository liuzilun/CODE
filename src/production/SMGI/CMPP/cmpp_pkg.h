/*
** Copyright (c) 2005
** Shen Zhen RongDi Information Technology Co., Ltd.
** All rights reserved.
**
** cmpp_pkg.h
** class CCMPP:Implement the encoding & decoding code of the whole packet 
**				that sp->smg & smg->sp communication using.
**
** Version:	2.0
** Author:	Chris
** Time:	2005/03/30
**
** Version:	1.0
** Author:	qsf
** Time:	2005/03/29
*/

#pragma once

#include <string.h>		//strncpy
#ifndef _WINSOCKAPI_
#include <winsock2.h>	//decode:ntohs, ntohl; encode:htons, htonl.
#else
#define _WINSOCK2API_
#endif
#pragma comment(lib, "ws2_32.lib") 
#include <stdio.h>			//sprintf 
#include "..\common\sync.h"    //CCriticalSection

#define UINT1 unsigned char
#define UINT4 unsigned long
#define UINT8 unsigned __int64

#pragma pack(push, 1)
namespace sms
{
	namespace cmpp
	{
		//cmpp消息包基类
		class cmpp_pkg
		{
		public:
			enum MSG_TYPE
			{
				CMPP_CONNECT			= 0x00000001,	//请求连接
				CMPP_CONNECT_RESP		= 0x80000001,	//请求连接应答
				CMPP_TERMINATE			= 0x00000002,	//终止连接
				CMPP_TERMINATE_RESP		= 0x80000002,	//终止连接应答
				CMPP_SUBMIT				= 0x00000004,	//提交短信
				CMPP_SUBMIT_RESP		= 0x80000004,	//提交短信应答
				CMPP_DELIVER			= 0x00000005,	//短信下发
				CMPP_DELIVER_RESP		= 0x80000005,	//下发短信应答
				CMPP_QUERY				= 0x00000006,	//发送短信状态查询
				CMPP_QUERY_RESP			= 0x80000006,	//发送短信状态查询应答
				CMPP_CANCEL				= 0x00000007,	//删除短信
				CMPP_CANCEL_RESP		= 0x80000007,	//删除短信应答
				CMPP_ACTIVE_TEST		= 0x00000008,	//激活测试
				CMPP_ACTIVE_TEST_RESP	= 0x80000008,	//激活测试应答		    		
			};

			//信息格式
			enum MSG_FMT   
			{
				ASCII	  = 0,							//ASCII串；
				WRT_CARD3 = 3,							//短信写卡操作；
				BIN		  = 4,							//二进制信息；
				UCS2	  = 8,							//UCS2编码；
				GB2312	  = 15,							//含GB汉字
			};

			enum 
			{
				CMPP_HEAD_LEN				= 12,       //消息头长度
				CMPP_CONNECT_LEN			= 39,
				CMPP_SUBMIT_LEN				= 350,
				CMPP_DELIVER_RESP_LEN		= 24,
				CMPP_ACTIVE_TEST_LEN		= 12,
				CMPP_ACTIVE_TEST_RESP_LEN   = 13,
				CMPP_QUERY_LEN				= 39,
				CMPP_TERMINATE_LEN			= 12,
				CMPP_TERMINATE_RESP_LEN		= 12,
				CMPP_CANCEL_LEN 			= 20,
				MAX_CONTENT_LEN				= 160,      //内容最大长度
			};			

		public:	
			~cmpp_pkg(void)
			{
			}

			void set_ttl_len(UINT4 ttl_len)
			{
				_ttl_len = ttl_len;
			}

			const UINT4 get_ttl_len(void) const
			{
				return _ttl_len;
			}

			void set_seq_id(UINT4 seq_id)
			{
				_seq_id = seq_id;
			}

			const UINT4 get_seq_id(void) const
			{
				return _seq_id;
			}
			
			const UINT4 get_cmd_id(void) const
			{
				return _cmd_id;
			}

			void release(void)
			{
				if (_buf != NULL)
				{
					delete []_buf;
					_buf = NULL;
				}
			}

			char* buf(void)
			{
				return _buf;
			}

			const UINT4 len(void) const
			{
                return _enc_len;
			}

			virtual UINT4 encode(void) = 0;
			virtual UINT4 decode(const char *buf) = 0;

		protected:
			cmpp_pkg(void)
				: _ttl_len(0)
				, _cmd_id(0)
				, _seq_id(0)
				, _enc_len(0)
				, _buf(NULL)
			{
			}

			UINT4 encode_cmpp_head(char *buf);
			UINT4 decode_cmpp_head(const char *buf);
			
			UINT8 hton64(UINT8 host64);
			UINT8 ntoh64(UINT8 net64);
		protected:
			UINT4	_ttl_len;			//	消息总长度(含消息头及消息体)
			UINT4	_cmd_id;			//	命令或响应类型
			UINT4	_seq_id;			//	消息流水号,顺序累加,步长为1,循环使用
			UINT4	_enc_len;           //	编码后的包长度
			char	*_buf;				//	存储编码数据的缓冲区 
		};

		//cmpp_connect消息类
		class cmpp_connect
			: public cmpp_pkg
		{
		public:
			cmpp_connect(void);
			~cmpp_connect(void);

			void set_source_addr(const char *source_addr);
			void set_version(UINT1 version);
			void set_time_stamp(UINT4 time_stamp);
			void set_shared_secret(const char *shared_secret);
			
			UINT4 encode(void);
			UINT4 decode(const char *buf)
			{
				return 0;
			}
		protected:
			char	_source_addr[6];
			char	_auth_source[16];
			char	_shared_secret[7];
			UINT1	_version;
			UINT4	_time_stamp;
		};

		//cmpp_connect_resp消息类
		class cmpp_connect_resp
			: public cmpp_pkg
		{
		public:
			//状态值
			enum
			{
    			ST_OK = 0,					 //正确
				ST_MSG_STRUCT_ERR	  = 1,	 //消息结构错
				ST_SRC_ADDR_ERR		  = 2,   //非法源地址
				ST_AUTHENTICATION_ERR = 3,   //认证错		    
				ST_VERSION_HIGH		  = 4,   //版本太高
				ST_OTHER_ERR          = 5    //其他错误
			};
		public:
			cmpp_connect_resp(void);
			~cmpp_connect_resp(void);
			
			UINT4 get_status(void);
			UINT1 get_version(void);
			UINT4 encode(void)
			{
				return 0;
			}
			UINT4 decode(const char *buf);
		protected:
#ifdef _CMPP2_
			UINT1 _status;
#else
			UINT4 _status;
#endif
			char _authenticator_ISMG[16];
			UINT1 _version;
		};

		//cmpp_terminate消息类
		class cmpp_terminate
			: public cmpp_pkg
		{
		public:
			cmpp_terminate(void);
			UINT4 encode(void);
			UINT4 decode(const char *buf);
		};

		//cmpp_terminate消息类
		class cmpp_terminate_resp
			: public cmpp_pkg
		{
		public:
			cmpp_terminate_resp(void);
			UINT4 encode(void);
			UINT4 decode(const char *buf);
		};

		//cmpp_submit消息类
		class cmpp_submit
			: public cmpp_pkg
		{
		public:
			//	计费用户类型字段
			enum FEE_USER_TYPE 
			{
				SRC    = 0,						//	对目的终端MSISDN计费；
				DEST   = 1,						//	对源终端MSISDN计费；
				SP	   = 2,						//	对SP计费；
				NO_USE = 3,						//	表示本字段无效，对谁计费参见Fee_terminal_Id字段。
			};

			enum FEE_TYPE
			{
				FREE     = 1,						//	01：对“计费用户号码”免费；
				FEE	     = 2,						//	02：对“计费用户号码”按条计信息费；
				FEE_SMC  = 3						//	03：对“计费用户号码”按包月收取信息
			};

			//	是否需要报告
			enum NEED_REPORT    
			{
#ifdef _CMPP2_
				SMC = 2,						//  SMC话单			
#endif
				YES = 1,						//	需要
				NO  = 0,						//	不需要
			};

			//	号码类型
			enum TERMINAL_TYPE   
			{
				TRUE_NUMBER  = 0,				//	真实号码
				FALSE_NUMBER = 1				//	伪号码
			};	
		public:
			cmpp_submit(void);
			~cmpp_submit(void);

			void set_msg_id(UINT8 i_msg_id);
			void set_pk_total(UINT1 i_pk_total);
			void set_pk_number(UINT4 i_pk_number);
			void set_registered_delivery(UINT1 i_registered_delivery);
			void set_msg_level(UINT1 i_msg_level);
			void set_service_id(const char *service_id);
			void set_fee_user_type(UINT1 i_fee_user_type);
			void set_fee_terminal_id(const char *fee_terminal_id, UINT1 i_fee_terminal_type);
			void set_TP_pId(UINT1 i_TP_pId);
			void set_TP_udhi(UINT1 i_TP_udhi);
			void set_msg_src(const char *msg_src);
			void set_fee_type(const char *fee_type);
			void set_fee_code(const char *fee_code);
			void set_valid_time(const char *valid_time);
			void set_at_time(const char *at_time);
			void set_src_id(const char *src_id);
			void set_dest_user_tl(UINT1 i_dest_user_tl);
			void set_dest_terminal_id(const char *dest_terminal_id, UINT1 i_dest_terminal_type);
			void set_msg_length(UINT1 i_msg_length);
			void set_msg_content(const char *msg_content,UINT1 i_msg_fmt);
			void set_msg_content(const char *buf, int i_len, UINT1 i_msg_fmt);
			void set_link_id(const char *link_id);
			
			UINT4 encode(void);
			UINT4 decode(const char *buf)
			{
				return 0;
			}
		protected:
   			UINT8 _msg_id;
			UINT1 _pk_total;
			UINT1 _pk_number;
			UINT1 _registered_delivery;
			UINT1 _msg_level;
			char   _service_id[10];
			UINT1 _fee_user_type;
#ifdef _CMPP2_
			char   _fee_terminal_id[21];
#else
			char   _fee_terminal_id[32];
#endif
			UINT1 _fee_terminal_type;
			UINT1 _TP_pId;
			UINT1 _TP_udhi;
			UINT1 _msg_fmt;
			char   _msg_src[6];
			char   _fee_type[2];
			char   _fee_code[6];
			char   _valid_time[17];
			char   _at_time[17];
			char   _src_id[21];
			UINT1 _dest_user_tl;
#ifdef _CMPP2_
			char   _dest_terminal_id[21];
#else
			char   _dest_terminal_id[32];	
#endif
			UINT1 _dest_terminal_type;
			UINT1 _msg_length;
			char   _msg_content[160]; 
#ifdef _CMPP2_					
			char   _link_id[8];
#else
			char   _link_id[20];
#endif
			
		};

		//cmpp_submit_resp消息类
		class cmpp_submit_resp
			: public cmpp_pkg
		{
		public:
			//	返回结果
			enum RESULT  
			{
				OK = 0,							//	正确；
				MSG_STRUCT_ERR = 1,				//	消息结构错；
				MSG_COMMAND_ERR = 2,			//	命令字错；
				MSG_SQ_REPEAT = 3,				//	消息序号重复；
				MSG_LEN_ERR = 4,				//	消息长度错；
				FEE_CODE_ERR = 5,				//	资费代码错；
				MSG_TOO_LEN = 6,				//	超过最大信息长；
				SERVICE_ID_ERR = 7,				//	业务代码错；
				FLOW_CONTROL_ERR = 8,			//	流量控制错；
				FEE_USER_ERR = 9,				//	本网关不负责服务此计费号码；
				SRC_ID_ERR = 10,				//	Src_Id错误；
				MSG_SRC_ERR = 11,				//	Msg_src错误；
				FEE_TERMINAL_ID_ERR = 12,		//	Fee_terminal_Id错误；
				DEST_TERMINAL_ID_ERR = 13		//	Dest_terminal_Id错误；
			};
		public:
			cmpp_submit_resp(void)
			{
			}

			~cmpp_submit_resp(void)
			{
			}

			UINT8 get_msg_id(void);
			UINT4 get_result(void);

			UINT4 encode(void)
			{
				return 0;
			}
			UINT4 decode(const char *buf);
		protected:
			UINT8 _msg_id;
#ifdef _CMPP2_
			UINT1 _result;
#else
			UINT4 _result;
#endif
		};

		//cmpp_active_test消息类
		class cmpp_active_test
			: public cmpp_pkg
		{
		public:
			cmpp_active_test(void);
			UINT4 encode(void);
			UINT4 decode(const char *buf);
		};

		//cmpp_active_test_resp消息类
		class cmpp_active_test_resp
			: public cmpp_pkg
		{
		public:
			UINT4 encode(void);
			UINT4 decode(const char *buf);

			UINT1 get_reserve(void);
			cmpp_active_test_resp(void);
			void set_reserve(UINT1 i_reserve);
		protected: 
			UINT1 _reserve;
		};

		//cmpp_deliver消息类
		class cmpp_deliver
			: public cmpp_pkg
		{
		public:
			//	短信内容是否是状态报告
			enum IS_REPORT  
			{
				YES = 1,						//	是
				NO  = 0,						//	不是
			};	
		public:
			UINT8 get_msg_id(void);
			char * get_dest_id(void);
			char * get_service_id(void);
			UINT1 get_tp_pid(void);
			UINT1 get_tp_udhi(void);
			UINT1 get_msg_fmt(void);
			UINT1 get_registered_delivery(void);
			char * get_src_terminal_id(void);
			UINT1 get_src_terminal_type(void);
			UINT1 get_msg_length(void);
			char * get_msg_content(void);
			char * get_link_id(void);

			UINT4 encode(void)
			{
				return 0;
			}
			UINT4 decode(const char *buf);

		protected:
			UINT8 _msg_id;
			char _dest_id[21+1];
			char _service_id[10+1];
			UINT1 _tp_pid;
			UINT1 _tp_udhi;
			UINT1 _msg_fmt;
#ifdef _CMPP2_
			char _src_terminal_id[21+1];
#else
			char _src_terminal_id[32+1];
#endif
			UINT1 _src_terminal_type;
			UINT1 _registered_delivery;
			UINT1 _msg_length;
			char _msg_content[MAX_CONTENT_LEN+1];
#ifdef _CMPP2_
			char _link_id[8+1];
#else
			char _link_id[20+1];
#endif
		};

		//cmpp_deliver_resp消息类
		class cmpp_deliver_resp
			: public cmpp_pkg
		{
		public:
			void set_msg_id(UINT8 i_msg_id);
			void set_result(UINT4 i_result);
			
			UINT4 encode(void);
			UINT4 decode(const char *buf)
			{
				return 0;
			}
			cmpp_deliver_resp(void);
		protected:
			UINT8 _msg_id;
#ifdef _CMPP2_
			UINT1 _result;
#else
			UINT4 _result;
#endif
		};

		//cmpp_deliver_report消息类
		class cmpp_deliver_report
			: public cmpp_pkg
		{
		public:
			cmpp_deliver_report(void)
			{
			}
			~cmpp_deliver_report(void)
			{
			}

			UINT4 encode(void)
			{
				return 0;
			}
			UINT4 decode(const char *buf);

			UINT8 get_msg_id(void);
			UINT4 get_smsc_sequence(void);
			char * get_dest_terminal_id(void);
			char * get_submit_time(void);
			char * get_stat(void);
			char * get_done_time(void);
		protected:
			UINT8 _msg_id;
			char _stat[7+1];
			char _submit_time[10+1];
			char _done_time[10+1];
#ifdef _CMPP2_
			char _dest_terminal_id[21+1];
#else
			char _dest_terminal_id[32+1];
#endif
			UINT4 _smsc_sequence;
		};

		//cmpp_query消息类
		class cmpp_query
			: public cmpp_pkg
		{
		public:
			cmpp_query(void);
			void set_query_type(UINT1 i_query_type);
			void set_query_code(const char *query_code);
			void set_time(const char *time);
			void set_reserve(const char * reserve);

			UINT4 encode(void);
			UINT4 decode(const char *buf)
			{
				return 0;
			}
		protected:
			char   _time[8];
			UINT1 _query_type;
			char   _query_code[10];
			char   _reserve[8];
		};

		//cmpp_query_resp消息类
		class cmpp_query_resp
			: public cmpp_pkg
		{
		public:
			char* get_time(void);
			char* get_query_code(void);
			UINT1 get_query_type(void);
			UINT4 get_fl(void);
			UINT4 get_mo_fl(void);
			UINT4 get_mo_scs(void);
			UINT4 get_mo_wt(void);
			UINT4 get_mt_tl_msg(void);
			UINT4 get_mt_tl_user(void);
			UINT4 get_scs(void);
			UINT4 get_wt(void);

			UINT4 encode(void)
			{
				return 0;
			}
			UINT4 decode(const char *buf);
		protected:
			char _time[8+1];
			UINT1 _query_type;
			char _query_code[10+1];
			UINT4 _mt_tl_msg;
			UINT4 _mt_tl_user;
			UINT4 _scs;
			UINT4 _wt;
			UINT4 _fl;
			UINT4 _mo_scs;
			UINT4 _mo_wt;
			UINT4 _mo_fl;
		};

		//cmpp_cancel消息类
		class cmpp_cancel
			: public cmpp_pkg
		{
		public:
			cmpp_cancel(void);

			void set_msg_id(UINT8 i_msg_id);

			UINT4 encode(void);
			UINT4 decode(const char *buf)
			{
				return 0;
			}
		protected:
			UINT8 _msg_id;
		};

		//cmpp_cancel_resp消息类
		class cmpp_cancel_resp
			: public cmpp_pkg
		{
		public:
			UINT4 get_success_id(void);
			
			UINT4 encode(void)
			{
				return 0;
			}
			UINT4 decode(const char *buf);
		protected:
#ifdef _CMPP2_
			UINT1 _success_id;
#else
			UINT4 _success_id;
#endif
		};
		
		//cmpp头类
		class cmpp_head: public cmpp_pkg
		{
		public:
			UINT4 encode(void)
			{
				return 0;
			}
			UINT4 decode(const char *buf)
			{
				return decode_cmpp_head(buf);
			}
		};

		//cmpp_seq类,用于设置cmpp消息序列号
		class cmpp_seq
		{
		public:
			cmpp_seq(UINT4 seq_id);
			~cmpp_seq(void);

			void set_seq(cmpp_pkg &cmpp_package);
			void set_seq(cmpp_pkg *cmpp_package);

			UINT4 get_seq(void)
			{
				return _seq_id;
			}
		private:
			UINT4 _seq_id;
		};

	};
}
#pragma pack(pop, 1)