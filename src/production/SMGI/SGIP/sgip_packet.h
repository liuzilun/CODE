//
//
//	Sgip Class
//
//	auth: Chris
//
//	date: 2005-4-13 ver: 4rd
//	
//	date: 2003-12-15 ver: 3rd
//		Implement the encoding & decoding code of the whole packet that sp->smg & smg->sp
//		communication using.
//	
//	date: 2003-08-07 ver: 2nd
//		Implement the SGIP protocol to filter the SGIP packet.
//	
//

#ifndef SGIP_PACKET_H
#define SGIP_PACKET_H

#ifdef _DEBUG
#include <string>
#endif

#include <string.h>		//strncpy
#ifndef _WINSOCKAPI_
#include <winsock2.h>	//decode:ntohs, ntohl; encode:htons, htonl.
#else
#define _WINSOCK2API_
#endif
#pragma comment(lib, "ws2_32.lib") 
#include <stdio.h>		//sprintf 
#include <time.h>
#include "..\common\buf.h"

namespace sms
{
	namespace sgip
	{
		enum PACKET_STATE 
		{
			INVALID_PACKET, 
			HALF_PACKET, 
			WHOLE_PACKET 
		};

		//	SGIP Response错误代码表
		enum SGIP_RESP_ERR_CODE
		{
			SGIP_RESP_ERR_CODE_0		= 0,		//	无错误，命令正确接收
			SGIP_RESP_ERR_CODE_1		= 1,		//	非法登录，如登录名、口令出错、登录名与口令不符等。
			SGIP_RESP_ERR_CODE_2		= 2,		//	重复登录，如在同一TCP/IP连接中连续两次以上请求登录。
			SGIP_RESP_ERR_CODE_3		= 3,		//	连接过多，指单个节点要求同时建立的连接数过多。
			SGIP_RESP_ERR_CODE_4		= 4,		//	登录类型错，指bind命令中的logintype字段出错。
			SGIP_RESP_ERR_CODE_5		= 5,		//	参数格式错，指命令中参数值与参数类型不符或与协议规定的范围不符。
			SGIP_RESP_ERR_CODE_6		= 6,		//	非法手机号码，协议中所有手机号码字段出现非86130号码或手机号码前未加“86”时都应报错。
			SGIP_RESP_ERR_CODE_7		= 7,		//	消息ID错
			SGIP_RESP_ERR_CODE_8		= 8,		//	信息长度错
			SGIP_RESP_ERR_CODE_9		= 9,		//	非法序列号，包括序列号重复、序列号格式错误等
			SGIP_RESP_ERR_CODE_10		= 10,		//	非法操作GNS
			SGIP_RESP_ERR_CODE_11		= 11,		//	节点忙，指本节点存储队列满或其他原因，暂时不能提供服务的情况
			SGIP_RESP_ERR_CODE_21		= 21,		//	目的地址不可达，指路由表存在路由且消息路由正确但被路由的节点暂时不能提供服务的情况
			SGIP_RESP_ERR_CODE_22		= 22,		//	路由错，指路由表存在路由但消息路由出错的情况，如转错SMG等
			SGIP_RESP_ERR_CODE_23		= 23,		//	路由不存在，指消息路由的节点在路由表中不存在
			SGIP_RESP_ERR_CODE_24		= 24,		//	计费号码无效，鉴权不成功时反馈的错误信息
			SGIP_RESP_ERR_CODE_25		= 25,		//	用户不能通信（如不在服务区、未开机等情况）
			SGIP_RESP_ERR_CODE_26		= 26,		//	手机内存不足
			SGIP_RESP_ERR_CODE_27		= 27,		//	手机不支持短消息
			SGIP_RESP_ERR_CODE_28		= 28,		//	手机接收短消息出现错误
			SGIP_RESP_ERR_CODE_29		= 29,		//	不知道的用户
			SGIP_RESP_ERR_CODE_30		= 30,		//	不提供此功能
			SGIP_RESP_ERR_CODE_31		= 31,		//	非法设备
			SGIP_RESP_ERR_CODE_32		= 32,		//	系统失败
			SGIP_RESP_ERR_CODE_33		= 33,		//	短信中心队列满
			SGIP_RESP_ERR_CODE_90		= 90,		//	SGIP包长度错误
			SGIP_RESP_ERR_CODE_97		= 97		//	限制SP接入号给外地用户下发消息
		};

		//	Commands
		enum SGIP_CMD
		{
			SGIP_CMD_BIND				= 0x00000001,
			SGIP_CMD_BIND_RESP			= 0x80000001,
			SGIP_CMD_UNBIND				= 0x00000002,
			SGIP_CMD_UNBIND_RESP		= 0x80000002,
			SGIP_CMD_SUBMIT				= 0x00000003,
			SGIP_CMD_SUBMIT_RESP		= 0x80000003,
			SGIP_CMD_DELIVER			= 0X00000004,
			SGIP_CMD_DELIVER_RESP		= 0X80000004,
			SGIP_CMD_REPORT				= 0X00000005,
			SGIP_CMD_REPORT_RESP		= 0X80000005,
			SGIP_CMD_ADDSP				= 0X00000006,
			SGIP_CMD_ADDSP_RESP			= 0X80000006,
			SGIP_CMD_MODIFYSP			= 0X00000007,
			SGIP_CMD_MODIFYSP_RESP		= 0X80000007,
			SGIP_CMD_DELETESP			= 0X00000008,
			SGIP_CMD_DELETESP_RESP		= 0X80000008,
			SGIP_CMD_QUERYROUTE			= 0X00000009,
			SGIP_CMD_QUERYROUTE_RESP	= 0X80000009,
			SGIP_CMD_ADDTELESEG			= 0X0000000A,
			SGIP_CMD_ADDTELESEG_RESP	= 0X8000000A,
			SGIP_CMD_MODIFYTELESEG		= 0X0000000B,
			SGIP_CMD_MODIFYTELESEG_RESP	= 0X8000000B,
			SGIP_CMD_DELETETELESEG		= 0X0000000C,
			SGIP_CMD_DELETETELESEG_RESP	= 0X8000000C,
			SGIP_CMD_ADDSMG				= 0X0000000D,
			SGIP_CMD_ADDSMG_RESP		= 0X8000000D,
			SGIP_CMD_MODIFYSMG			= 0X0000000E,
			SGIP_CMD_MODIFYSMG_RESP		= 0X8000000E,
			SGIP_CMD_DELETESMG			= 0X0000000F,
			SGIP_CMD_DELETESMG_RESP		= 0X8000000F,
			SGIP_CMD_CHECKUSER			= 0X00000010,
			SGIP_CMD_CHECKUSER_RESP		= 0X80000010,
			SGIP_CMD_USERRPT			= 0X00000011,
			SGIP_CMD_USERRPT_RESP		= 0X80000011,
			SGIP_CMD_TRACE				= 0X00001000,
			SGIP_CMD_TRACE_RESP			= 0X80001000,

			//	傲天网关
			SGIP_CMD_KEEPALIVE			= 0X10000001,
			SGIP_CMD_KEEPALIVE_RESP		= 0X10000010,			
		};



		enum SGIP_LEN
		{
			SGIP_LEN_SM_CONT			= 160,
			SGIP_LEN_HEAD				= 20,
			SGIP_LEN_RESP				= 29,
			SGIP_LEN_RESERVE			= 8,
			SGIP_LEN_LINK_ID			= 8,
			SGIP_LEN_ADDR				= 21,
			
			//	SGIP_BIND
			SGIP_LEN_BIND				= 61,
			SGIP_LEN_LOGIN_NAME			= 16,
			SGIP_LEN_LOGIN_PSWD			= 16,

			//	SGIP_SUBMIT
			SGIP_LEN_SUBMIT				= 324,
			SGIP_LEN_CORP				= 5,
			SGIP_LEN_SRV_TYPE			= 10,
			SGIP_LEN_FEE_VALUE			= 6,
			SGIP_LEN_TIME				= 16,

			SGIP_LEN_REPORT				= 74,
		};

		enum SGIP_LOGIN_TYPE
		{
			SGIP_LOGIN_TYPE_SP2SMG	= 1, 
			SGIP_LOGIN_TYPE_SMG2SP	= 2, 
			SGIP_LOGIN_TYPE_SMG2SMG	= 3, 
			SGIP_LOGIN_TYPE_SMG2GNS = 4, 
			SGIP_LOGIN_TYPE_GNS2SMG	= 5, 
			SGIP_LOGIN_TYPE_GNS2GNS	= 6, 
			SGIP_LOGIN_TYPE_TEST	= 11
		};

		//	Msg_coding
		enum SGIP_MSG_CODING   
		{
			SGIP_MSG_CODING_ASCII	  = 0,		//	ASCII串；
			SGIP_MSG_CODING_WRT_CARD3 = 3,		//	短信写卡操作；
			SGIP_MSG_CODING_BIN		  = 4,		//	二进制信息；
			SGIP_MSG_CODING_UCS2	  = 8,		//	UCS2编码；
			SGIP_MSG_CODING_GB2312	  = 15,		//	含GB汉字
		};

		struct SGIP_SEQ
		{
			unsigned long	_src_id;	//	源节点号
			unsigned long	_time;		//	命令产生的时间
			unsigned long	_seq_id;	//	消息流水号,顺序累加,步长为1,循环使用
			
			SGIP_SEQ(void)
			{
				memset(this, 0, sizeof(SGIP_SEQ));
			}

			~SGIP_SEQ(void)
			{
			}

			SGIP_SEQ(const SGIP_SEQ &seq)
			{
				*this = seq;
			}

			const SGIP_SEQ& operator=(const SGIP_SEQ &seq)
			{
				_src_id	= seq._src_id;
				_time	= seq._time;
				_seq_id = seq._seq_id;
				return *this;
			}
		};

		// Packets
		class sgip_packet
		{
		public:
			~sgip_packet(void)
			{
				free_buf();
			}

			void set_cmd_id(unsigned long cmd_id)
			{
				_cmd_id = cmd_id;
			}

			unsigned long get_cmd_id(void) const
			{
				return _cmd_id;
			}

			void set_pkt_len(unsigned long pkt_len)
			{
				_pkt_len = pkt_len;
			}

			unsigned long get_pkt_len(void) const
			{
				return _pkt_len;
			}

			void set_src_id(unsigned long src_id)
			{
				_seq._src_id = src_id;
			}

			unsigned long get_src_id(void) const
			{
				return _seq._src_id;
			}

			void set_pkt_time(unsigned long pkt_time)
			{
				_seq._time = pkt_time;
			}

			unsigned long get_pkt_time(void) const
			{
				return _seq._time;
			}

			void set_seq_id(unsigned long seq_id)
			{
				_seq._seq_id = seq_id;
			}

			unsigned long get_seq_id(void) const
			{
				return _seq._seq_id;
			}

			void set_seq(SGIP_SEQ &seq)
			{
				_seq = seq;
			}

			SGIP_SEQ& get_seq(void)
			{
				return _seq;
			}
		public:
			const char* get_buf(void)
			{
				return _buf;
			}

			unsigned long get_len(void) const
			{
				return _len;
			}

			void free_buf(void)
			{
				try
				{
					if (_buf != NULL)
					{
						delete []_buf;
						_buf = NULL;
					}
				}
				catch(...)
				{
					;
				}
			}
		protected:
            sgip_packet(void)
				: _pkt_len(0)
				, _cmd_id(0)
				, _buf(NULL)
			{
			}

		protected:
			virtual unsigned long encode(void) = 0;
			virtual unsigned long decode(const char *buf) = 0;

			unsigned long encode_head(char *buf)
			{
				unsigned long len = 0;
				if (buf != NULL)
				{
					unsigned long pkt_len	= htonl(_pkt_len);
					unsigned long cmd_id	= htonl(_cmd_id);
					unsigned long src_id	= htonl(_seq._src_id);
					unsigned long pkt_time	= htonl(_seq._time);
					unsigned long seq_id	= htonl(_seq._seq_id);

					memcpy(buf + len, &pkt_len, sizeof(pkt_len));	
					len += sizeof(pkt_len);

					memcpy(buf + len, &cmd_id, sizeof(cmd_id));
					len += sizeof(cmd_id);

					memcpy(buf + len, &src_id, sizeof(src_id));
					len += sizeof(src_id);

					memcpy(buf + len, &pkt_time, sizeof(pkt_time));
					len += sizeof(pkt_time);

					memcpy(buf + len, &seq_id, sizeof(seq_id));
					len += sizeof(seq_id);
				}
				return len;
			}

			unsigned long decode_head(const char *buf)
			{
				unsigned long len = 0;
				if (buf != NULL)
				{
                    memcpy(&_pkt_len, buf + len, sizeof(_pkt_len));
					len += sizeof(_pkt_len);
					_pkt_len = ntohl(_pkt_len);

					memcpy(&_cmd_id, buf + len, sizeof(_cmd_id));
					len += sizeof(_cmd_id);
					_cmd_id = ntohl(_cmd_id);

					memcpy(&_seq._src_id, buf + len, sizeof(_seq._src_id));
					len += sizeof(_seq._src_id);
					_seq._src_id = ntohl(_seq._src_id);

					memcpy(&_seq._time, buf + len, sizeof(_seq._time));
					len += sizeof(_seq._time);
					_seq._time = ntohl(_seq._time);

					memcpy(&_seq._seq_id, buf + len, sizeof(_seq._seq_id));
					len += sizeof(_seq._seq_id);
					_seq._seq_id = ntohl(_seq._seq_id);
				}

				return len;
			}
		protected:
			unsigned long 	_pkt_len;		//	消息总长度(含消息头及消息体)
			unsigned long 	_cmd_id;		//	命令或响应类型
			SGIP_SEQ		_seq;			//	命令序列号

			char			*_buf;			//	存储编码数据的缓冲区
			unsigned long	_len;			//	存储编码数据的长度
		};

		class sgip_head
			: public sgip_packet
		{
		public:
			sgip_head(void)
			{
			}

			sgip_head(SGIP_SEQ &seq)
			{
				set_seq(seq);
			}

			~sgip_head(void)
			{
			}
		public:
			unsigned long encode(void)
			{
				return 0;
			}

			unsigned long decode(const char *buf)
			{
				return decode_head(buf);
			}
		};

		class sgip_resp
			: public sgip_packet
		{
		public:
			sgip_resp(void)
			{
				_result = 0;
				memset(_reserve, 0, sizeof(_reserve));
			}

			sgip_resp(SGIP_SEQ &seq)
			{
				set_seq(seq);

				_result = 0;
				memset(_reserve, 0, sizeof(_reserve));
			}

			~sgip_resp(void)
			{
			}

			void set_result(unsigned char result)
			{
				_result = result;
			}

			unsigned char get_result(void) const
			{
				return _result;
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SGIP_LEN_RESP + 1];
				memset(_buf, 0, SGIP_LEN_RESP + 1);
				_len = SGIP_LEN_HEAD;

				memcpy(_buf + _len, &_result, sizeof(_result));
				_len += sizeof(_result);
				
				memcpy(_buf + _len, _reserve, SGIP_LEN_RESERVE);
				_len += SGIP_LEN_RESERVE;	
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				unsigned long len = 0;
				if (buf != NULL)
				{
					decode_head(buf);
					len = SGIP_LEN_HEAD;

					memcpy(&_result, buf + len, sizeof(_result));
					len += sizeof(_result);

					memcpy(_reserve, buf + len, SGIP_LEN_RESERVE);
					len += SGIP_LEN_RESERVE;
				}
				return len;
			}
		private:
			unsigned char 	_result;
			char 			_reserve[SGIP_LEN_RESERVE + 1];
		};

		class sgip_bind
			: public sgip_packet
		{
		public:
			sgip_bind(void)
			{
				_login_type = SGIP_LOGIN_TYPE_SP2SMG;
				memset(_login_name, 0, sizeof(_login_name));
				memset(_login_pswd, 0, sizeof(_login_pswd));
				memset(_reserve, 0, sizeof(_reserve));
			}

			sgip_bind(SGIP_SEQ &seq)
			{
				set_seq(seq);
				set_cmd_id(SGIP_CMD_BIND);

				_login_type = SGIP_LOGIN_TYPE_SP2SMG;
				memset(_login_name, 0, sizeof(_login_name));
				memset(_login_pswd, 0, sizeof(_login_pswd));
				memset(_reserve, 0, sizeof(_reserve));
			}

			~sgip_bind(void)
			{
			}

			//body
			void set_login_type(unsigned char login_type)
			{
				_login_type = login_type;
			}

			unsigned char get_login_type(void) const
			{
				return _login_type;
			}

			void set_login_name(const char *login_name)
			{
				strncpy(_login_name, login_name, SGIP_LEN_LOGIN_NAME);
			}

			const char* get_login_name(void) const
			{
				return _login_name;
			}

			void set_login_pswd(const char *login_pswd)
			{
				strncpy(_login_pswd, login_pswd, SGIP_LEN_LOGIN_PSWD);
			}

			const char* get_login_pswd(void) const
			{
				return _login_pswd;
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SGIP_LEN_BIND + 1];
				memset(_buf, 0, SGIP_LEN_BIND + 1);
				_len = SGIP_LEN_HEAD;

				memcpy(_buf + _len, &_login_type, sizeof(_login_type));
				_len += sizeof(_login_type);

				memcpy(_buf + _len, _login_name, SGIP_LEN_LOGIN_NAME);
				_len += SGIP_LEN_LOGIN_NAME;

				memcpy(_buf + _len, _login_pswd, SGIP_LEN_LOGIN_NAME);
				_len += SGIP_LEN_LOGIN_NAME;
				
				memcpy(_buf + _len, _reserve, SGIP_LEN_RESERVE);
				_len += SGIP_LEN_RESERVE;	
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				unsigned long len = 0;
				if (buf != NULL)
				{
					decode_head(buf);
					len = SGIP_LEN_HEAD;

					memcpy(&_login_type, buf + len, sizeof(_login_type));
					len += sizeof(_login_type);

					memcpy(_login_name, buf + len, SGIP_LEN_LOGIN_NAME);
					len += SGIP_LEN_LOGIN_NAME;

					memcpy(_login_pswd, buf + len, SGIP_LEN_LOGIN_PSWD);
					len += SGIP_LEN_LOGIN_PSWD;

					memcpy(_reserve, buf + len, SGIP_LEN_RESERVE);
					len += SGIP_LEN_RESERVE;
				}
				return len;
			}
		private:
			unsigned char 	_login_type;
			char 			_login_name[SGIP_LEN_LOGIN_NAME + 1];
			char 			_login_pswd[SGIP_LEN_LOGIN_PSWD + 1];
			char 			_reserve[SGIP_LEN_RESERVE + 1];			
		};

		class sgip_bind_resp
			: public sgip_resp
		{
		public:
			sgip_bind_resp(void)
			{
			}

			sgip_bind_resp(SGIP_SEQ &seq)
				: sgip_resp(seq)
			{
				set_cmd_id(SGIP_CMD_BIND_RESP);
			}

			~sgip_bind_resp(void)
			{
			}
		};

		class sgip_unbind
			: public sgip_packet
		{
		public:
			sgip_unbind(void)
			{
			}

			sgip_unbind(SGIP_SEQ &seq)
			{
				set_seq(seq);
				set_cmd_id(SGIP_CMD_UNBIND);
			}

			~sgip_unbind(void)
			{
			}

		public:
			unsigned long encode(void)
			{
				_buf = new char[SGIP_LEN_HEAD + 1];
				memset(_buf, 0, SGIP_LEN_HEAD + 1);
				_len = SGIP_LEN_HEAD;

				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return decode_head(buf);
			}
		};

		class sgip_unbind_resp 
			: public sgip_head
		{
		public:
			sgip_unbind_resp(void)
			{
			}

			sgip_unbind_resp(SGIP_SEQ &seq)
				: sgip_head(seq)
			{
				set_cmd_id(SGIP_CMD_UNBIND_RESP);
			}

			~sgip_unbind_resp(void)
			{
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SGIP_LEN_HEAD + 1];
				memset(_buf, 0, SGIP_LEN_HEAD + 1);
				_len = SGIP_LEN_HEAD;

				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}
		};

		class sgip_keepalive 
			: public sgip_packet
		{
		public:
			sgip_keepalive(SGIP_SEQ &seq)
			{
				set_seq(seq);
				set_cmd_id(SGIP_CMD_KEEPALIVE);
			}

			~sgip_keepalive(void)
			{
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SGIP_LEN_HEAD + 1];
				memset(_buf, 0, SGIP_LEN_HEAD + 1);
				_len = SGIP_LEN_HEAD;

				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		};

		class sgip_keepalive_resp
			: public sgip_head
		{
		public:
			sgip_keepalive_resp(void)
			{
			}

			~sgip_keepalive_resp(void)
			{
			}
		};

		class sgip_report 
			: public sgip_packet
		{
		public:
			sgip_report(void)
			{
				_report_type	= 0;
				_state			= 0;
				_err_code		= 0;
				memset(_user_num, 0, sizeof(_user_num));
				memset(_reserve, 0, sizeof(_reserve));
			}

			~sgip_report(void)
			{
			}

			unsigned char get_report_type(void) const
			{
				return _report_type;
			}

			const char* get_user_num(void) const
			{
				return _user_num;
			}

			unsigned char get_state(void) const
			{
				return _state;
			}

			unsigned char get_err_code(void) const
			{
				return _err_code;
			}

			SGIP_SEQ& get_report_seq(void)
			{
				return _report_seq;
			}
		public:
			unsigned long encode(void)
			{
				return 0;
			}

			unsigned long decode(const char *buf)
			{
				unsigned long len = 0;
				if (buf != NULL)
				{
					decode_head(buf);
					len = SGIP_LEN_HEAD;

					memcpy(&_report_seq, buf + len, sizeof(SGIP_SEQ));
					len += sizeof(SGIP_SEQ);
					_report_seq._seq_id = ntohl(_report_seq._seq_id);
					_report_seq._src_id	= ntohl(_report_seq._src_id);
					_report_seq._time	= ntohl(_report_seq._time);

					memcpy(&_report_type, buf + len, sizeof(_report_type));
					len += sizeof(_report_type);

					memcpy(_user_num, buf + len, SGIP_LEN_ADDR);
					len += SGIP_LEN_ADDR;

					memcpy(&_state, buf + len, sizeof(_state));
					len += sizeof(_state);

					memcpy(&_err_code, buf + len, sizeof(_err_code));
					len += sizeof(_err_code);

					memcpy(_reserve, buf + len, SGIP_LEN_RESERVE);
					len += SGIP_LEN_RESERVE;
				}
				return len;
			}
		private:
			SGIP_SEQ		_report_seq;
			unsigned char	_report_type;
			char 			_user_num[SGIP_LEN_ADDR + 1];
			unsigned char	_state;
			unsigned char	_err_code;
			char 			_reserve[SGIP_LEN_RESERVE + 1];
		};

		class sgip_report_resp
			: public sgip_resp
		{
		public:
			sgip_report_resp(SGIP_SEQ &seq)
				: sgip_resp(seq)
			{
				set_cmd_id(SGIP_CMD_REPORT_RESP);
			}

			~sgip_report_resp(void)
			{
			}
		};

		class sgip_submit 
			: public sgip_packet
		{
		public:
			sgip_submit(SGIP_SEQ &seq)
			{
				set_seq(seq);
				set_cmd_id(SGIP_CMD_SUBMIT);

				memset(_sp_num, 0, sizeof(_sp_num));
				memset(_charge_num, 0, sizeof(_charge_num));
				_user_count = 0;
				memset(_user_num, 0, sizeof(_user_num));
				memset(_corp_id, 0, sizeof(_corp_id));
				memset(_srv_type, 0, sizeof(_srv_type));
				_fee_type = 0;
				memset(_fee_value, 0, sizeof(_fee_value));
				memset(_given_value, 0, sizeof(_given_value));
				_morelateto_mtflag = 0;
				_priority = 0;
				memset(_expire_time, 0, sizeof(_expire_time));
				memset(_schedule_time, 0, sizeof(_schedule_time));
				_report_flag = 0;
				_tp_pid = 0;
				_tp_udhi = 0;
				_msg_coding = 0;
				_msg_type = 0;
				_msg_len = 0;
				memset(_content, 0, sizeof(_content));
				memset(_link_id, 0, sizeof(_link_id));
			}

			~sgip_submit(void)
			{
			}

			//body
			void set_sp_num(const char *sp_num)
			{
				strncpy(_sp_num, sp_num, SGIP_LEN_ADDR);
			}

			void set_charge_num(const char *charge_num)
			{
				strncpy(_charge_num, charge_num, SGIP_LEN_ADDR);
			}

			void set_user_count(unsigned char user_count)
			{
				_user_count = user_count;
			}

			void set_user_num(const char *user_num)
			{
				strncpy(_user_num, user_num, SGIP_LEN_ADDR);
			}

			void set_corp_id(const char *corp_id)
			{
				strncpy(_corp_id, corp_id, SGIP_LEN_CORP);
			}

			void set_srv_type(const char *srv_type)
			{
				strncpy(_srv_type, srv_type, SGIP_LEN_SRV_TYPE);
			}

			void set_fee_type(unsigned char fee_type)
			{
				_fee_type = fee_type;
			}

			void set_fee_value(const char *fee_value)
			{
				strncpy(_fee_value, fee_value, SGIP_LEN_FEE_VALUE);
			}

			void set_given_value(const char *given_value)
			{
				strncpy(_given_value, given_value, SGIP_LEN_FEE_VALUE);
			}

			void set_agent_flag(unsigned char agent_flag)
			{
				_agent_flag = agent_flag;
			}

			void set_morelateto_mtflag(unsigned char morelateto_mtflag)
			{
				_morelateto_mtflag = morelateto_mtflag;
			}

			void set_priority(unsigned char priority)
			{
				_priority = priority;
			}

			void set_expire_time(const char *expire_time)
			{
				strncpy(_expire_time, expire_time, SGIP_LEN_TIME);
			}

			void set_schedule_time(const char *schedule_time)
			{
				strncpy(_schedule_time, schedule_time, SGIP_LEN_TIME);
			}

			void set_report_flag(unsigned char report_flag)
			{
				_report_flag = report_flag;
			}

			void set_tp_pid(unsigned char tp_pid)
			{
				_tp_pid = tp_pid;
			}

			void set_tp_udhi(unsigned char tp_udhi)
			{
				_tp_udhi = tp_udhi;
			}

			void set_msg_coding(unsigned char msg_coding)
			{
				_msg_coding = msg_coding;
			}
			
			void set_msg_type(unsigned char msg_type)
			{
				_msg_type = msg_type;
			}

			void set_msg_len(unsigned long msg_len)
			{
				_msg_len = msg_len > SGIP_LEN_SM_CONT ? SGIP_LEN_SM_CONT : msg_len;
			}

			void set_content(const char *cont)
			{
				strncpy(_content, cont, 
					_msg_len > SGIP_LEN_SM_CONT ? SGIP_LEN_SM_CONT : _msg_len);
			}

			void set_content(const char * cont, unsigned long len)
			{
				memcpy(_content, cont, len);
			}

			void set_link_id(const char *link_id)
			{
				strncpy(_link_id, link_id, SGIP_LEN_LINK_ID);
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SGIP_LEN_SUBMIT + 1];
				memset(_buf, 0, SGIP_LEN_SUBMIT + 1);
				_len = SGIP_LEN_HEAD;

				memcpy(_buf + _len, _sp_num, SGIP_LEN_ADDR);
				_len += SGIP_LEN_ADDR;

				memcpy(_buf + _len, _charge_num, SGIP_LEN_ADDR);
				_len += SGIP_LEN_ADDR;

				memcpy(_buf + _len, &_user_count, sizeof(_user_count));
				_len += sizeof(_user_count);
				
				memcpy(_buf + _len, _user_num, SGIP_LEN_ADDR);
				_len += SGIP_LEN_ADDR;

				memcpy(_buf + _len, _corp_id, SGIP_LEN_CORP);
				_len += SGIP_LEN_CORP;

				memcpy(_buf + _len, _srv_type, SGIP_LEN_SRV_TYPE);
				_len += SGIP_LEN_SRV_TYPE;

				memcpy(_buf + _len, &_fee_type, sizeof(_fee_type));
				_len += sizeof(_fee_type);

				memcpy(_buf + _len, _fee_value, SGIP_LEN_FEE_VALUE);
				_len += SGIP_LEN_FEE_VALUE;

				memcpy(_buf + _len, _given_value, SGIP_LEN_FEE_VALUE);
				_len += SGIP_LEN_FEE_VALUE;

				memcpy(_buf + _len, &_agent_flag, sizeof(_agent_flag));
				_len += sizeof(_agent_flag);

				memcpy(_buf + _len, &_morelateto_mtflag, sizeof(_morelateto_mtflag));
				_len += sizeof(_morelateto_mtflag);

				memcpy(_buf + _len, &_priority, sizeof(_priority));
				_len += sizeof(_priority);

				memcpy(_buf + _len, _expire_time, SGIP_LEN_TIME);
				_len += SGIP_LEN_TIME;

				memcpy(_buf + _len, _schedule_time, SGIP_LEN_TIME);
				_len += SGIP_LEN_TIME;

				memcpy(_buf + _len, &_report_flag, sizeof(_report_flag));
				_len += sizeof(_report_flag);

				memcpy(_buf + _len, &_tp_pid, sizeof(_tp_pid));
				_len += sizeof(_tp_pid);

				memcpy(_buf + _len, &_tp_udhi, sizeof(_tp_udhi));
				_len += sizeof(_tp_udhi);

				memcpy(_buf + _len, &_msg_coding, sizeof(_msg_coding));
				_len += sizeof(_msg_coding);

				memcpy(_buf + _len, &_msg_type, sizeof(_msg_type));
				_len += sizeof(_msg_type);

				unsigned long msg_len = htonl(_msg_len);
				memcpy(_buf + _len, &msg_len, sizeof(msg_len));
				_len += sizeof(msg_len);

				memcpy(_buf + _len, _content, _msg_len);
				_len += _msg_len;

				memcpy(_buf + _len, _link_id, SGIP_LEN_LINK_ID);
				_len += SGIP_LEN_LINK_ID;

				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		private:
			char 			_sp_num[SGIP_LEN_ADDR + 1];
			char 			_charge_num[SGIP_LEN_ADDR + 1];
			unsigned char	_user_count;
			char 			_user_num[SGIP_LEN_ADDR + 1];
			char 			_corp_id[SGIP_LEN_CORP + 1];
			char 			_srv_type[SGIP_LEN_SRV_TYPE + 1];
			unsigned char	_fee_type;
			char 			_fee_value[SGIP_LEN_FEE_VALUE + 1];
			char 			_given_value[SGIP_LEN_FEE_VALUE + 1];
			unsigned char	_agent_flag;
			unsigned char	_morelateto_mtflag;
			unsigned char	_priority;
			char 			_expire_time[SGIP_LEN_TIME + 1];
			char 			_schedule_time[SGIP_LEN_TIME + 1];
			unsigned char	_report_flag;
			unsigned char	_tp_pid;
			unsigned char	_tp_udhi;
			unsigned char	_msg_coding;
			unsigned char	_msg_type;
			unsigned long	_msg_len;
			char 			_content[SGIP_LEN_SM_CONT + 1];
			char 			_link_id[SGIP_LEN_LINK_ID + 1];			


};

		class sgip_submit_resp
			: public sgip_resp
		{
		public:
			sgip_submit_resp(void)
			{
			}

			~sgip_submit_resp(void)
			{
			}
		};

		class sgip_deliver 
			: public sgip_packet
		{
		public:
			sgip_deliver(void)
			{
				_tp_pid		= 0;
				_tp_udhi	= 0;
				_msg_coding	= 0;
				_msg_len	= 0;
				memset(_user_num, 0, sizeof(_user_num));
				memset(_sp_num, 0, sizeof(_sp_num));
				memset(_content, 0, sizeof(_content));
				memset(_link_id, 0, sizeof(_link_id));
			}

			~sgip_deliver(void)
			{
			}

			//body
			const char* get_user_num(void) const
			{
				return _user_num;
			}

			const char* get_sp_num(void) const
			{
				return _sp_num;
			}

			unsigned char get_tp_pid(void) const
			{
				return _tp_pid;
			}

			unsigned char get_tp_udhi(void) const
			{
				return _tp_udhi;
			}

			unsigned char get_msg_coding(void) const
			{
				return _msg_coding;
			}

			unsigned long get_msg_len(void) const
			{
				return _msg_len;
			}

			const char* get_content(void) const
			{
				return _content;
			}

			const char* get_link_id(void) const
			{
				return _link_id;
			}
		public:
			unsigned long encode(void)
			{
				return 0;
			}

			unsigned long decode(const char *buf)
			{
				unsigned long len = 0;
				if (buf != NULL)
				{
					decode_head(buf);
					len = SGIP_LEN_HEAD;

					memcpy(_user_num, buf + len, SGIP_LEN_ADDR);
					len += SGIP_LEN_ADDR;

					memcpy(_sp_num, buf + len, SGIP_LEN_ADDR);
					len += SGIP_LEN_ADDR;

					memcpy(&_tp_pid, buf + len, sizeof(_tp_pid));
					len += sizeof(_tp_pid);

					memcpy(&_tp_udhi, buf + len, sizeof(_tp_udhi));
					len += sizeof(_tp_udhi);

					memcpy(&_msg_coding, buf + len, sizeof(_msg_coding));
					len += sizeof(_msg_coding);

					memcpy(&_msg_len, buf + len, sizeof(_msg_len));
					len += sizeof(_msg_len);
					_msg_len = ntohl(_msg_len);

					memcpy(_content, buf + len, _msg_len);
					len += _msg_len;

					memcpy(_link_id, buf + len, SGIP_LEN_LINK_ID);
					len += SGIP_LEN_LINK_ID;
				}
				return len;
			}
		private:
			char 			_user_num[SGIP_LEN_ADDR + 1];
			char 			_sp_num[SGIP_LEN_ADDR + 1];
			unsigned char	_tp_pid;
			unsigned char	_tp_udhi;
			unsigned char	_msg_coding;
			unsigned long	_msg_len;
			char 			_content[SGIP_LEN_SM_CONT + 1];
			char 			_link_id[SGIP_LEN_LINK_ID + 1];

		};

		class sgip_deliver_resp
			: public sgip_resp
		{
		public:
			sgip_deliver_resp(SGIP_SEQ &seq)
				: sgip_resp(seq)
			{
				set_cmd_id(SGIP_CMD_DELIVER_RESP);
			}

			~sgip_deliver_resp(void)
			{
			}
		};

		// Protocol
		class sgip_protocol
		{
		public:
			sgip_protocol(void)
			{
			}

			~sgip_protocol(void)
			{
			}

			PACKET_STATE decode(buffer &buf, sgip_head &head);
		protected:
			bool find_command(unsigned long);
		};

		class sgip_seq_id
        {
        public:
			sgip_seq_id(unsigned long src_id, 
						unsigned long seq_id,
						unsigned long min_seq_id,
                        unsigned long max_seq_id)
                : _min_seq_id(min_seq_id)
				, _max_seq_id(max_seq_id)
			{
				_seq._src_id	= src_id;
				_seq._seq_id	= seq_id;	
			}

			~sgip_seq_id(void)
			{
			}

			unsigned long get_seq_id(void) const
			{
				return _seq._seq_id;
			}

			void set_seq(sgip_packet *pack)
			{
				pack->set_seq(get_seq());
			}

			SGIP_SEQ& get_seq(void)
			{
				_seq._seq_id	= inc_seq_id();
				_seq._time		= get_time();
				return _seq;
			}
		private:
			unsigned long inc_seq_id(void)
			{
				if (_seq._seq_id < _min_seq_id)
				{
					_seq._seq_id = _min_seq_id;
				}

                if ((_max_seq_id != 0) && (_seq._seq_id >= _max_seq_id))
                {
                    _seq._seq_id = _min_seq_id;
                }
				return (unsigned long)::InterlockedIncrement((LPLONG)&_seq._seq_id);
			}

			unsigned long get_time(void)
			{
				time_t timer = time(NULL);
				struct tm *tblock = localtime(&timer);
				return (tblock->tm_mon + 1) * 100000000
						+ tblock->tm_mday * 1000000
						+ tblock->tm_hour * 10000
						+ tblock->tm_min * 100 
						+ tblock->tm_sec;
			}
		private:
			SGIP_SEQ        _seq;
			unsigned long	_min_seq_id;
            unsigned long   _max_seq_id;
        };
	};
};
#endif	// SGIP_PACKET_H
