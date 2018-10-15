//	Smpp_packet Class
//
//	auth: Chris
//	date: 2004-12-01
//	ver: 
//	desc:
//		Just implemented encoding SMPP_DELIVER_RESP, SMPP_SUBMIT_RESP, 
//		and decoding SUBMIT, RESP items, and all packet lengths to pass.
//

#ifndef SMPP_PACKET_H
#define SMPP_PACKET_H

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
#include "..\common\buf.h"

namespace sms
{
	namespace smpp
	{
		enum PACKET_STATE 
		{
			INVALID_PACKET, 
			HALF_PACKET, 
			WHOLE_PACKET 
		};

		// Commands
		enum SMPP_CMD
		{
			SMPP_CMD_GENERIC_NACK				= 0X80000000,
			SMPP_CMD_BIND_RECEIVER				= 0X00000001,
			SMPP_CMD_BIND_RECEIVER_RESP			= 0X80000001,
			SMPP_CMD_BIND_TRANSMITTER			= 0X00000002,
			SMPP_CMD_BIND_TRANSMITTER_RESP		= 0X80000002,
			SMPP_CMD_UNBIND						= 0X00000006,
			SMPP_CMD_UNBIND_RESP				= 0X80000006,
			SMPP_CMD_QUERY_SM					= 0X00000003,
			SMPP_CMD_QUERY_SM_RESP				= 0X80000003,
			SMPP_CMD_SUBMIT						= 0X00000004,
			SMPP_CMD_SUBMIT_RESP				= 0X80000004,
			SMPP_CMD_DELIVER					= 0X00000005,
			SMPP_CMD_DELIVER_RESP				= 0X80000005,
			SMPP_CMD_REPLACE_SM					= 0X00000007,
			SMPP_CMD_REPLACE_SM_RESP			= 0X80000007,
			SMPP_CMD_CANCEL_SM					= 0X00000008,
			SMPP_CMD_CANCEL_SM_RESP				= 0X80000008,
			SMPP_CMD_BIND_TRANSCEIVER			= 0X00000009,
			SMPP_CMD_BIND_TRANSCEIVER_RESP		= 0X80000009,
			SMPP_CMD_OUTBIND					= 0X0000000B,
			SMPP_CMD_ENQUIRE_LINK				= 0X00000015,
			SMPP_CMD_ENQUIRE_LINK_RESP			= 0X80000015,
			SMPP_CMD_SUBMIT_MILTI				= 0X00000021,
			SMPP_CMD_SUBMIT_MILTI_RESP			= 0X80000021,
			SMPP_CMD_QUERY_LAST_MSGS			= 0X00000023,
			SMPP_CMD_QUERY_LAST_MSGS_RESP		= 0X80000023,
			SMPP_CMD_QUERY_MSG_DETAILS			= 0X00000024,
			SMPP_CMD_QUERY_MSG_DETAILS_RESP		= 0X80000024,
			SMPP_CMD_ALERT_NOTIFICATION			= 0X00000102,
			SMPP_CMD_DATA_SM					= 0X00000103,
			SMPP_CMD_DATA_SM_RESP				= 0X80000103
		};

		enum SMPP_LEN
		{
			SMPP_LEN_HEAD						= 16,
			SMPP_LEN_CONT						= 160,
			SMPP_LEN_MSG_ID						= 9,
			
			//	SMPP_BIND_RECEIVER
			SMPP_LEN_BIND_RECEIVER				= 98,
			SMPP_LEN_SYS_ID						= 16,
			SMPP_LEN_PSWD						= 9,
			SMPP_LEN_SYS_TYPE					= 13,
			SMPP_LEN_ADDR_RANGE					= 41,

			//	SMPP_BIND_TRANSMITTER
			SMPP_LEN_BIND_TRANSMITTER			= 98,

			//	SMPP_BIND_TRANSCEIVER
			SMPP_LEN_BIND_TRANSCEIVER			= 98,

			//	SMPP_UNBIND
			SMPP_LEN_UNBIND						= 16,

			//	SMPP_DELIVER_RESP
			SMPP_LEN_DELIVER_RESP				= 25,

			//	SMPP_SUBMIT
			SMPP_LEN_SUBMIT						= 271,
			SMPP_LEN_SRV_TYPE					= 6,
			SMPP_LEN_TIME						= 17,
			SMPP_LEN_ADDR						= 21,

			//	SMPP_REPORT
			SMPP_LEN_REPORT_MSG_ID				= 10,
			SMPP_LEN_SUB						= 3,
			SMPP_LEN_DATE						= 10,
			SMPP_LEN_STAT						= 7,
			SMPP_LEN_ERR						= 3,
			SMPP_LEN_TEXT						= 20,

			//	SMPP_ENQUIRE_LINK
			SMPP_LEN_ENQUIRE_LINK				= 16,

			//	SMPP_SUBMIT_MULTI
			SMPP_LEN_SUBMIT_MULTI				= 5032,
			SMPP_LEN_DST_NUMBERS				= 200,

			//	SMPP_QUERY
			SMPP_LEN_QUERY						= 48,

			//	SMPP_CANCEL
			SMPP_LEN_CANCEL						= 61,

			//	SMPP_REPLACE
			SMPP_LEN_REPLACE					= 229,
		};

		//	是否需要报告
		enum SMPP_REPORT
		{
			SMPP_REPORT_YES	 = 1,		//	需要
			SMPP_REPORT_NO	 = 0		//	不需要
		};

		enum SMPP_MSG_CODING   
		{
			SMPP_MSG_CODING_ASCII	  = 0,		//	ASCII串；
			SMPP_MSG_CODING_BIN		  = 4,		//	二进制信息；
			SMPP_MSG_CODING_UCS2	  = 8,		//	UCS2编码；
		};

		struct SME_DST_ADDRESS
		{
			unsigned char	_dst_flag;
			unsigned char	_dst_addr_ton;		
			unsigned char	_dst_addr_npi;		
			char			_dst_addr[SMPP_LEN_ADDR + 1];
		};

		struct UNSUCC_SME_ADDRESS
		{
			unsigned char	_dst_addr_ton;		
			unsigned char	_dst_addr_npi;		
			char			_dst_addr[SMPP_LEN_ADDR + 1];
			unsigned long	_err_code;
		};

		// Packets
		class smpp_packet
		{
		public:
			~smpp_packet(void)
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

			void set_cmd_stat(unsigned long cmd_stat)
			{
				_cmd_stat = cmd_stat;
			}

			unsigned long get_cmd_stat(void) const
			{
				return _cmd_stat;
			}

			void set_seq_id(unsigned long seq_id)
			{
				_seq_id = seq_id;
			}

			unsigned long get_seq_id(void) const
			{
				return _seq_id;
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
				if (_buf != NULL)
				{
					delete []_buf;
					_buf = NULL;
				}
			}
		protected:
            smpp_packet(void)
				: _pkt_len(0)
				, _cmd_id(0)
				, _cmd_stat(0)
				, _seq_id(0)
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
					unsigned long cmd_stat	= htonl(_cmd_stat);
					unsigned long seq_id	= htonl(_seq_id);

					memcpy(buf + len, &pkt_len, sizeof(pkt_len));	
					len += sizeof(pkt_len);

					memcpy(buf + len, &cmd_id, sizeof(cmd_id));
					len += sizeof(cmd_id);

					memcpy(buf + len, &cmd_stat, sizeof(cmd_stat));
					len += sizeof(cmd_stat);

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

					memcpy(&_cmd_stat, buf + len, sizeof(_cmd_stat));
					len += sizeof(_cmd_stat);
					_cmd_stat = ntohl(_cmd_stat);

					memcpy(&_seq_id, buf + len, sizeof(_seq_id));
					len += sizeof(_seq_id);
					_seq_id = ntohl(_seq_id);
				}

				return len;
			}
		protected:
			unsigned long 	_pkt_len;		//	消息总长度(含消息头及消息体)
			unsigned long 	_cmd_id;		//	命令或响应类型
			unsigned long	_cmd_stat;		//	命令状态
			unsigned long 	_seq_id;		//	消息流水号,顺序累加,步长为1,循环使用

			char			*_buf;			//	存储编码数据的缓冲区
			unsigned long	_len;			//	存储编码数据的长度
		};

		class smpp_head
			: public smpp_packet
		{
		public:
			smpp_head(void)
			{
			}

			~smpp_head(void)
			{
			}

			unsigned long encode(void)
			{
				return 0;
			}

			unsigned long decode(const char *buf)
			{
				return decode_head(buf);
			}
		};

		class smpp_bind_receiver
			: public smpp_packet
		{
		public:
			smpp_bind_receiver(unsigned long seq_id)
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_BIND_RECEIVER);

				memset(_sys_id, 0, sizeof(_sys_id));
				memset(_pswd, 0, sizeof(_pswd));
				memset(_sys_type, 0, sizeof(_sys_type));
				memset(_addr_range, 0, sizeof(_addr_range));
				_version	= 0;
				_addr_ton	= 0;
				_addr_npi	= 0;
			}

			~smpp_bind_receiver(void)
			{
			}

			void set_sys_id(const char *sys_id)
			{
				memcpy(_sys_id, sys_id, SMPP_LEN_SYS_ID);
			}

			void set_pswd(const char *pswd)
			{
				memcpy(_pswd, pswd, SMPP_LEN_PSWD);
			}

			void set_sys_type(const char *sys_type)
			{
				memcpy(_sys_type, sys_type, SMPP_LEN_SYS_TYPE);
			}

			void set_version(unsigned char version)
			{
				_version = version;
			}

			void set_addr_ton(unsigned char addr_ton)
			{
				_addr_ton = addr_ton;
			}

			void set_addr_npi(unsigned char addr_npi)
			{
				_addr_npi = addr_npi;
			}

			void set_addr_range(const char *addr_range)
			{
				memcpy(_addr_range, addr_range, SMPP_LEN_ADDR_RANGE);
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_BIND_RECEIVER];
				memset(_buf, 0, SMPP_LEN_BIND_RECEIVER);
				_len = SMPP_LEN_HEAD;

				memcpy(_buf + _len, _sys_id, strlen(_sys_id));
				_len += (unsigned long)strlen(_sys_id) + 1;

				memcpy(_buf + _len, _pswd, strlen(_pswd));
				_len += (unsigned long)strlen(_pswd) + 1;

				memcpy(_buf + _len, _sys_type, strlen(_sys_type));
				_len += (unsigned long)strlen(_sys_type) + 1;
				
				memcpy(_buf + _len, &_version, sizeof(_version));
				_len += sizeof(_version);

				memcpy(_buf + _len, &_addr_ton, sizeof(_addr_ton));
				_len += sizeof(_addr_ton);

				memcpy(_buf + _len, &_addr_npi, sizeof(_addr_npi));
				_len += sizeof(_addr_npi);

				memcpy(_buf + _len, _addr_range, strlen(_addr_range));
				_len += (unsigned long)strlen(_addr_range) + 1;				
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		private:
			char			_sys_id[SMPP_LEN_SYS_ID + 1];
			char			_pswd[SMPP_LEN_PSWD + 1];
			char			_sys_type[SMPP_LEN_SYS_TYPE + 1];
			unsigned char	_version;
			unsigned char	_addr_ton;
			unsigned char	_addr_npi;
			char			_addr_range[SMPP_LEN_ADDR_RANGE + 1];
		};

		class smpp_bind_receiver_resp
			: public smpp_packet
		{
		public:
			smpp_bind_receiver_resp(void)
			{
				memset(_sys_id, 0, sizeof(_sys_id));
			}

			~smpp_bind_receiver_resp(void)
			{
			}

			const char* get_sys_id(void) const
			{
				return _sys_id;
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
					len = SMPP_LEN_HEAD;

					memcpy(_sys_id, buf + len, SMPP_LEN_SYS_ID);
					len += SMPP_LEN_SYS_ID;
				}
				return len;
			}
		private:
			char			_sys_id[SMPP_LEN_SYS_ID + 1];
		};

		class smpp_bind_transmitter
			: public smpp_packet
		{
		public:
			smpp_bind_transmitter(unsigned long seq_id) 
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_BIND_TRANSMITTER);

				memset(_sys_id, 0, sizeof(_sys_id));
				memset(_pswd, 0, sizeof(_pswd));
				memset(_sys_type, 0, sizeof(_sys_type));
				memset(_addr_range, 0, sizeof(_addr_range));
				_version	= 0;
				_addr_ton	= 0;
				_addr_npi	= 0;
			}

			~smpp_bind_transmitter(void)
			{
			}

			void set_sys_id(const char *sys_id)
			{
				strncpy(_sys_id, sys_id, SMPP_LEN_SYS_ID);
			}

			void set_pswd(const char *pswd)
			{
				strncpy(_pswd, pswd, SMPP_LEN_PSWD);
			}

			void set_sys_type(const char *sys_type)
			{
				strncpy(_sys_type, sys_type, SMPP_LEN_SYS_TYPE);
			}

			void set_version(unsigned char version)
			{
				_version = version;
			}

			void set_addr_ton(unsigned char addr_ton)
			{
				_addr_ton = addr_ton;
			}

			void set_addr_npi(unsigned char addr_npi)
			{
				_addr_npi = addr_npi;
			}

			void set_addr_range(const char *addr_range)
			{
				strncpy(_addr_range, addr_range, SMPP_LEN_ADDR_RANGE);
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_BIND_TRANSMITTER];
				memset(_buf, 0, SMPP_LEN_BIND_TRANSMITTER);
				_len = SMPP_LEN_HEAD;

				memcpy(_buf + _len, _sys_id, strlen(_sys_id));
				_len += (unsigned long)strlen(_sys_id) + 1;

				memcpy(_buf + _len, _pswd, strlen(_pswd));
				_len += (unsigned long)strlen(_pswd) + 1;

				memcpy(_buf + _len, _sys_type, strlen(_sys_type));
				_len += (unsigned long)strlen(_sys_type) + 1;
				
				memcpy(_buf + _len, &_version, sizeof(_version));
				_len += sizeof(_version);

				memcpy(_buf + _len, &_addr_ton, sizeof(_addr_ton));
				_len += sizeof(_addr_ton);

				memcpy(_buf + _len, &_addr_npi, sizeof(_addr_npi));
				_len += sizeof(_addr_npi);

				memcpy(_buf + _len, _addr_range, strlen(_addr_range));
				_len += (unsigned long)strlen(_addr_range) + 1;				
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		private:
			char			_sys_id[SMPP_LEN_SYS_ID + 1];
			char			_pswd[SMPP_LEN_PSWD + 1];
			char			_sys_type[SMPP_LEN_SYS_TYPE + 1];
			unsigned char	_version;
			unsigned char	_addr_ton;
			unsigned char	_addr_npi;
			char			_addr_range[SMPP_LEN_ADDR_RANGE + 1];
		};

		class smpp_bind_transmitter_resp
			: public smpp_packet
		{
		public:
			smpp_bind_transmitter_resp(void)
			{
			}

			~smpp_bind_transmitter_resp(void)
			{
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
					len = SMPP_LEN_HEAD;
				}
				return len;
			}
		};

		class smpp_bind_transceiver
			: public smpp_packet
		{
		public:
			smpp_bind_transceiver(unsigned long seq_id)
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_BIND_TRANSCEIVER);

				memset(_sys_id, 0, sizeof(_sys_id));
				memset(_pswd, 0, sizeof(_pswd));
				memset(_sys_type, 0, sizeof(_sys_type));
				memset(_addr_range, 0, sizeof(_addr_range));
				_version	= 0;
				_addr_ton	= 0;
				_addr_npi	= 0;
			}

			~smpp_bind_transceiver(void)
			{
			}

			void set_sys_id(const char *sys_id)
			{
				memcpy(_sys_id, sys_id, SMPP_LEN_SYS_ID);
			}

			void set_pswd(const char *pswd)
			{
				memcpy(_pswd, pswd, SMPP_LEN_PSWD);
			}

			void set_sys_type(const char *sys_type)
			{
				memcpy(_sys_type, sys_type, SMPP_LEN_SYS_TYPE);
			}

			void set_version(unsigned char version)
			{
				_version = version;
			}

			void set_addr_ton(unsigned char addr_ton)
			{
				_addr_ton = addr_ton;
			}

			void set_addr_npi(unsigned char addr_npi)
			{
				_addr_npi = addr_npi;
			}

			void set_addr_range(const char *addr_range)
			{
				memcpy(_addr_range, addr_range, SMPP_LEN_ADDR_RANGE);
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_BIND_TRANSCEIVER];
				memset(_buf, 0, SMPP_LEN_BIND_TRANSCEIVER);
				_len = SMPP_LEN_HEAD;

				memcpy(_buf + _len, _sys_id, strlen(_sys_id));
				_len += (unsigned long)strlen(_sys_id) + 1;

				memcpy(_buf + _len, _pswd, strlen(_pswd));
				_len += (unsigned long)strlen(_pswd) + 1;

				memcpy(_buf + _len, _sys_type, strlen(_sys_type));
				_len += (unsigned long)strlen(_sys_type) + 1;
				
				memcpy(_buf + _len, &_version, sizeof(_version));
				_len += sizeof(_version);

				memcpy(_buf + _len, &_addr_ton, sizeof(_addr_ton));
				_len += sizeof(_addr_ton);

				memcpy(_buf + _len, &_addr_npi, sizeof(_addr_npi));
				_len += sizeof(_addr_npi);

				memcpy(_buf + _len, _addr_range, strlen(_addr_range));
				_len += (unsigned long)strlen(_addr_range) + 1;				
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		private:
			char			_sys_id[SMPP_LEN_SYS_ID + 1];
			char			_pswd[SMPP_LEN_PSWD + 1];
			char			_sys_type[SMPP_LEN_SYS_TYPE + 1];
			unsigned char	_version;
			unsigned char	_addr_ton;
			unsigned char	_addr_npi;
			char			_addr_range[SMPP_LEN_ADDR_RANGE + 1];
		};

		class smpp_bind_transceiver_resp
			: public smpp_packet
		{
		public:
			smpp_bind_transceiver_resp(void)
			{
				memset(_sys_id, 0, sizeof(_sys_id));
			}

			~smpp_bind_transceiver_resp(void)
			{
			}

			const char* get_sys_id(void) const
			{
				return _sys_id;
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
					len = SMPP_LEN_HEAD;

					memcpy(_sys_id, buf + len, SMPP_LEN_SYS_ID);
					len += SMPP_LEN_SYS_ID;
				}
				return len;
			}
		private:
			char			_sys_id[SMPP_LEN_SYS_ID + 1];
		};

		class smpp_unbind
			: public smpp_packet
		{
		public:
			smpp_unbind(unsigned long seq_id)
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_UNBIND);
			}

			~smpp_unbind(void)
			{
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_UNBIND];
				memset(_buf, 0, SMPP_LEN_UNBIND);
				_len = SMPP_LEN_UNBIND;				
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		};

		class smpp_unbind_resp
			: public smpp_packet
		{
		public:
			smpp_unbind_resp(void)
			{
			}

			~smpp_unbind_resp(void)
			{
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
					len = SMPP_LEN_HEAD;
				}
				return len;
			}		
		};

		class smpp_submit
			: public smpp_packet
		{
		public:
			smpp_submit(unsigned long seq_id)
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_SUBMIT);

				memset(_srv_type, 0, sizeof(_srv_type));

				_src_addr_ton = 0;
				_src_addr_npi = 0;
				memset(_src_addr, 0, sizeof(_src_addr));

				_dst_addr_ton = 0;
				_dst_addr_npi = 0;
				memset(_dst_addr, 0, sizeof(_dst_addr));

				_esm_class	= 0;
				_pid		= 0;
				_prio		= 0;
				memset(_sched_dt, 0, sizeof(_sched_dt));
				memset(_valid_dt, 0, sizeof(_valid_dt));
				_report		= 0;
                _replace	= 0;
				_coding		= 0;
				_msg_id		= 0;
				_sm_len		= 0;
				memset(_sm_cont, 0, sizeof(_sm_cont));
			}

			~smpp_submit(void)
			{
			}

			void set_srv_type(const char *srv_type)
			{
				memcpy(_srv_type, srv_type, SMPP_LEN_SRV_TYPE);
			}

			void set_src_addr_ton(unsigned char src_addr_ton)
			{
				_src_addr_ton = src_addr_ton;
			}

			void set_src_addr_npi(unsigned char src_addr_npi)
			{
				_src_addr_npi = src_addr_npi;
			}

			void set_src_addr(const char *sp_num)
			{ 
				memcpy(_src_addr, sp_num, SMPP_LEN_ADDR);
			}

			void set_dst_addr_ton(unsigned char dst_addr_ton)
			{
				_dst_addr_ton = dst_addr_ton;
			}

			void set_dst_addr_npi(unsigned char dst_addr_npi)
			{
				_dst_addr_npi = dst_addr_npi;
			}

			void set_dst_addr(const char *user_num)
			{ 
				memcpy(_dst_addr, user_num, SMPP_LEN_ADDR);
			}

			void set_esm_class(unsigned char esm_class)
			{
				_esm_class = esm_class;
			}

			void set_pid(unsigned char pid)
			{
				_pid = pid;
			}

			void set_prio(unsigned char prio)
			{
				_prio = prio;
			}

			void set_sched_dt(const char *sched_dt)
			{
				memcpy(_sched_dt, sched_dt, SMPP_LEN_TIME);
			}

			void set_valid_dt(const char *valid_dt)
			{
				memcpy(_valid_dt, valid_dt, SMPP_LEN_TIME);
			}

			void set_report(unsigned char report)
			{
				_report = report;
			}

			void set_replace(unsigned char replace)
			{
				_replace = replace;
			}

			void set_coding(unsigned char coding)
			{
				_coding = coding;
			}

			void set_msg_id(unsigned char msg_id)
			{
				_msg_id = msg_id;
			}

			void set_sm_len(unsigned char sm_len)
			{
				_sm_len = sm_len;
			}

			void set_sm_cont(const char *sm_cont)
			{
				memcpy(_sm_cont, sm_cont, _sm_len);
				/*memcpy(_sm_cont, sm_cont, 
					strlen(sm_cont) > SMPP_LEN_CONT ? SMPP_LEN_CONT : strlen(sm_cont));*/
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_SUBMIT];
				memset(_buf, 0, SMPP_LEN_SUBMIT);
				_len = SMPP_LEN_HEAD;

				memcpy(_buf + _len, _srv_type, strlen(_srv_type));
				_len += (unsigned long)strlen(_srv_type) + 1;

				//	src_addr
				memcpy(_buf + _len, &_src_addr_ton, sizeof(_src_addr_ton));
				_len += sizeof(_src_addr_ton);

				memcpy(_buf + _len, &_src_addr_npi, sizeof(_src_addr_npi));
				_len += sizeof(_src_addr_npi);

				memcpy(_buf + _len, _src_addr, strlen(_src_addr));
				_len += (unsigned long)strlen(_src_addr) + 1;

				// dst_addr
				memcpy(_buf + _len, &_dst_addr_ton, sizeof(_dst_addr_ton));
				_len += sizeof(_dst_addr_ton);

				memcpy(_buf + _len, &_dst_addr_npi, sizeof(_dst_addr_npi));
				_len += sizeof(_dst_addr_npi);

				memcpy(_buf + _len, _dst_addr, strlen(_dst_addr));
				_len += (unsigned long)strlen(_dst_addr) + 1;
				
				//	esm_class
				memcpy(_buf + _len, &_esm_class, sizeof(_esm_class));
				_len += sizeof(_esm_class);

				memcpy(_buf + _len, &_pid, sizeof(_pid));
				_len += sizeof(_pid);

				memcpy(_buf + _len, &_prio, sizeof(_prio));
				_len += sizeof(_prio);

				//	sched_dt
				unsigned long dt_len = (unsigned long)strlen(_sched_dt);
				memcpy(_buf + _len, _sched_dt, dt_len);
				if (dt_len == 0)
				{
					++_len;
				}
				else
				{
					_len += dt_len + 1;
				}

				dt_len = (unsigned long)strlen(_valid_dt);
				memcpy(_buf + _len, _valid_dt, dt_len);
				if (dt_len == 0)
				{
					++_len;
				}
				else
				{
					_len += dt_len + 1;
				}

				//	
				memcpy(_buf + _len, &_report, sizeof(_report));
				_len += sizeof(_report);

				memcpy(_buf + _len, &_replace, sizeof(_replace));
				_len += sizeof(_replace);

				memcpy(_buf + _len, &_coding, sizeof(_coding));
				_len += sizeof(_coding);

				memcpy(_buf + _len, &_msg_id, sizeof(_msg_id));
				_len += sizeof(_msg_id);

				memcpy(_buf + _len, &_sm_len, sizeof(_sm_len));
				_len += sizeof(_sm_len);

				//	sm_cont
				memcpy(_buf + _len, _sm_cont, _sm_len);
				_len += _sm_len;
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		private:
			char			_srv_type[SMPP_LEN_SRV_TYPE + 1];	
			unsigned char	_src_addr_ton;		
			unsigned char	_src_addr_npi;		
			char			_src_addr[SMPP_LEN_ADDR + 1];	
			unsigned char	_dst_addr_ton;		
			unsigned char	_dst_addr_npi;		
			char			_dst_addr[SMPP_LEN_ADDR + 1];			
			unsigned char	_esm_class;			
			unsigned char	_pid;			
			unsigned char	_prio;		
			char			_sched_dt[SMPP_LEN_TIME + 1];	// or unsigned char for NULL
			char			_valid_dt[SMPP_LEN_TIME + 1];	// or unsigned char for NULL
			unsigned char	_report;		
			unsigned char	_replace;		
			unsigned char	_coding;			
			unsigned char	_msg_id;			
			unsigned char	_sm_len;				
			char			_sm_cont[SMPP_LEN_CONT + 1];
		};

		class smpp_submit_resp
			: public smpp_packet
		{
		public:
			smpp_submit_resp(void)
			{
				memset(_msg_id, 0, sizeof(_msg_id));
			}

			~smpp_submit_resp(void)
			{
			}
	
			const char* get_msg_id(void) const
			{
				return _msg_id;
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
					len = SMPP_LEN_HEAD;

					memcpy(_msg_id, buf + len, SMPP_LEN_MSG_ID);
					len += (unsigned long)strlen(_msg_id);
				}
				return len;
			}		
		private:
			char	_msg_id[SMPP_LEN_MSG_ID + 1];
		};

		class smpp_deliver
			: public smpp_packet
		{
		public:
			smpp_deliver(void)
			{
				memset(_srv_type, 0, sizeof(_srv_type));

				_src_addr_ton = 0;
				_src_addr_npi = 0;
				memset(_src_addr, 0, sizeof(_src_addr));

				_dst_addr_ton = 0;
				_dst_addr_npi = 0;
				memset(_dst_addr, 0, sizeof(_dst_addr));

				_esm_class	= 0;
				_pid		= 0;
				_prio		= 0;
				memset(_sched_dt, 0, sizeof(_sched_dt));
				memset(_valid_dt, 0, sizeof(_valid_dt));
				_report		= 0;
                _replace	= 0;
				_coding		= 0;
				_msg_id		= 0;
				_sm_len		= 0;
				memset(_sm_cont, 0, sizeof(_sm_cont));
			}

			~smpp_deliver(void)
			{
			}

			//body
			const char* get_srv_type(void) const
			{ 
				return _srv_type;
			}

			unsigned char get_src_addr_ton(void) const
			{
				return _src_addr_ton;
			}

			unsigned char get_src_addr_npi(void) const
			{
				return _src_addr_npi;
			}

			const char* get_src_addr(void) const
			{ 
				return _src_addr;
			}

			unsigned char get_dst_addr_ton(void) const
			{
				return _dst_addr_ton;
			}

			unsigned char get_dst_addr_npi(void) const
			{
				return _dst_addr_npi;
			}

			const char* get_dst_addr(void) const
			{ 
				return _dst_addr;
			}

			unsigned char get_esm_class(void) const
			{
				return _esm_class;
			}

			unsigned char get_pid(void) const
			{
				return _pid;
			}

			unsigned char get_prio(void) const
			{
				return _prio;
			}

			unsigned char get_report(void) const
			{
				return _report;
			}

			unsigned char get_coding(void) const
			{
				return _coding;
			}

			unsigned char get_sm_len(void) const
			{
				return _sm_len;
			}

			const char* get_sm_cont(void) const
			{
				return _sm_cont;
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
					len = SMPP_LEN_HEAD;

					memcpy(_srv_type, buf + len, SMPP_LEN_SRV_TYPE);
					len += (unsigned long)strlen(_srv_type) + 1;

					//	src_addr
					memcpy(&_src_addr_ton, buf + len, sizeof(_src_addr_ton));
					len += sizeof(_src_addr_ton);

					memcpy(&_src_addr_npi, buf + len, sizeof(_src_addr_npi));
					len += sizeof(_src_addr_npi);

					memcpy(_src_addr, buf + len, SMPP_LEN_ADDR);
					len += (unsigned long)strlen(_src_addr) + 1;

					//	dst_addr
					memcpy(&_dst_addr_ton, buf + len, sizeof(_dst_addr_ton));
					len += sizeof(_dst_addr_ton);

					memcpy(&_dst_addr_npi, buf + len, sizeof(_dst_addr_npi));
					len += sizeof(_dst_addr_npi);

					memcpy(_dst_addr, buf + len, SMPP_LEN_ADDR);
					len += (unsigned long)strlen(_dst_addr) + 1;

					//	esm_class
					memcpy(&_esm_class, buf + len, sizeof(_esm_class));
					len += sizeof(_esm_class);

					memcpy(&_pid, buf + len, sizeof(_pid));
					len += sizeof(_pid);

					memcpy(&_prio, buf + len, sizeof(_prio));
					len += sizeof(_prio);

					memcpy(_sched_dt, buf + len, SMPP_LEN_TIME);
					len += (unsigned long)strlen(_sched_dt) + 1;

					memcpy(_valid_dt, buf + len, SMPP_LEN_TIME);
					len += (unsigned long)strlen(_valid_dt) + 1;

					memcpy(&_report, buf + len, sizeof(_report));
					len += sizeof(_report);

					memcpy(&_replace, buf + len, sizeof(_replace));
					len += sizeof(_replace);

					memcpy(&_coding, buf + len, sizeof(_coding));
					len += sizeof(_coding);

					memcpy(&_msg_id, buf + len, sizeof(_msg_id));
					len += sizeof(_msg_id);

					memcpy(&_sm_len, buf + len, sizeof(_sm_len));
					len += sizeof(_sm_len);

					memcpy(_sm_cont, buf + len, SMPP_LEN_CONT);
					len += _sm_len;
				}
				return len;
			}		
		private:
			char			_srv_type[SMPP_LEN_SRV_TYPE + 1];	
			unsigned char	_src_addr_ton;		
			unsigned char	_src_addr_npi;		
			char			_src_addr[SMPP_LEN_ADDR + 1];	
			unsigned char	_dst_addr_ton;		
			unsigned char	_dst_addr_npi;		
			char			_dst_addr[SMPP_LEN_ADDR + 1];			
			unsigned char	_esm_class;			
			unsigned char	_pid;			
			unsigned char	_prio;		
			char			_sched_dt[SMPP_LEN_TIME + 1];
			char			_valid_dt[SMPP_LEN_TIME + 1];
			unsigned char	_report;		
			unsigned char	_replace;		
			unsigned char	_coding;			
			unsigned char	_msg_id;			
			unsigned char	_sm_len;				
			char			_sm_cont[SMPP_LEN_CONT + 1];			
		};

		class smpp_deliver_resp
			: public smpp_packet
		{
		public:
			smpp_deliver_resp(unsigned long seq_id)
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_DELIVER_RESP);

				memset(_msg_id, 0, SMPP_LEN_MSG_ID);
			}

			~smpp_deliver_resp(void)
			{
			}

			void set_msg_id(const char *msg_id)
			{
				memcpy(_msg_id, msg_id, SMPP_LEN_MSG_ID);
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_DELIVER_RESP];
				memset(_buf, 0, SMPP_LEN_DELIVER_RESP);
				_len = SMPP_LEN_HEAD;

				memcpy(_buf + _len, _msg_id, strlen(_msg_id));
				_len += (unsigned long)strlen(_msg_id) + 1;
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		private:
			char	_msg_id[SMPP_LEN_MSG_ID + 1];
		};

		class smpp_report
			: public smpp_packet
		{
		public:
			smpp_report(void)
			{
				memset(&_report, 0, sizeof(SMPP_REPORT));
			}

			~smpp_report(void)
			{
			}

			const char* get_msg_id(void) const
			{
				return _report._msg_id;
			}

            const char* get_sub(void) const
			{
				return _report._sub;
			}

			const char* get_dlvrd(void) const
			{
				return _report._dlvrd;
			}

			const char* get_submit_date(void) const
			{
				return _report._submit_date;
			}

			const char* get_done_date(void) const
			{
				return _report._done_date;
			}

			const char* get_stat(void) const
			{
				return _report._stat;
			}

			const char* get_err(void) const
			{
				return _report._err;
			}

			const char* get_text(void) const
			{
				return _report._text;
			}
		public:
			unsigned long encode(void)
			{
				return 0;
			}

			//	id:1435612162 sub:001 dlvrd:001 submit date:0504102217 done date:0504102217 stat:DELIVRD err:000 text:
			unsigned long decode(const char *buf)
			{
				unsigned long len = 0;
				if (buf != NULL)
				{
					memset(&_report, 0, sizeof(SMPP_REPORT));
					
					len += 3;

					memcpy(_report._msg_id, buf + len, SMPP_LEN_REPORT_MSG_ID);
					len += SMPP_LEN_REPORT_MSG_ID + 5;

					memcpy(_report._sub, buf + len, SMPP_LEN_SUB);
					len += SMPP_LEN_SUB + 7;

					memcpy(_report._dlvrd, buf + len, SMPP_LEN_SUB);
					len += SMPP_LEN_SUB + 13;

					memcpy(_report._submit_date, buf + len, SMPP_LEN_DATE);
					len += SMPP_LEN_DATE + 11;

					memcpy(_report._done_date, buf + len, SMPP_LEN_DATE);
					len += SMPP_LEN_DATE + 6;

					memcpy(_report._stat, buf + len, SMPP_LEN_STAT);
					len += SMPP_LEN_STAT + 5;

					memcpy(_report._err, buf + len, SMPP_LEN_ERR);
					len += SMPP_LEN_ERR + 6;

					memcpy(_report._text, buf + len, SMPP_LEN_TEXT);
					len += (unsigned long)strlen(_report._text);				
				}
				return len;
			}
		private:
			struct SMPP_REPORT
			{
				char	_msg_id[SMPP_LEN_REPORT_MSG_ID + 1];
				char	_sub[SMPP_LEN_SUB + 1];
				char	_dlvrd[SMPP_LEN_SUB + 1];
				char	_submit_date[SMPP_LEN_DATE + 1];
				char	_done_date[SMPP_LEN_DATE + 1];
				char	_stat[SMPP_LEN_STAT + 1];
				char	_err[SMPP_LEN_ERR + 1];
				char	_text[SMPP_LEN_TEXT + 1];
			} _report;
		};

		class smpp_enquire_link
			: public smpp_packet
		{
		public:
			smpp_enquire_link(unsigned long seq_id)
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_ENQUIRE_LINK);
			}

			~smpp_enquire_link(void)
			{
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_ENQUIRE_LINK];
				_len = SMPP_LEN_ENQUIRE_LINK;
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		};

		class smpp_enquire_link_resp
			: public smpp_packet
		{
		public:
			smpp_enquire_link_resp(void)
			{
			}

			~smpp_enquire_link_resp(void)
			{
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
					len = SMPP_LEN_HEAD;
				}
				return len;
			}		
		};

		class smpp_submit_multi
			: public smpp_packet
		{
		public:
			smpp_submit_multi(unsigned long seq_id)
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_SUBMIT_MILTI);

				memset(_srv_type, 0, sizeof(_srv_type));

				_src_addr_ton = 0;
				_src_addr_npi = 0;
				memset(_src_addr, 0, sizeof(_src_addr));

				_dst_numbers = 0;
				memset(_dst_addr, 0, sizeof(_dst_addr));

				_esm_class	= 0;
				_pid		= 0;
				_prio		= 0;
				memset(_sched_dt, 0, sizeof(_sched_dt));
				memset(_valid_dt, 0, sizeof(_valid_dt));
				_report		= 0;
                _replace	= 0;
				_coding		= 0;
				_msg_id		= 0;
				_sm_len		= 0;
				memset(_sm_cont, 0, sizeof(_sm_cont));
			}

			~smpp_submit_multi(void)
			{
			}

			void set_srv_type(const char *srv_type)
			{
				memcpy(_srv_type, srv_type, SMPP_LEN_SRV_TYPE);
			}

			void set_src_addr_ton(unsigned char src_addr_ton)
			{
				_src_addr_ton = src_addr_ton;
			}

			void set_src_addr_npi(unsigned char src_addr_npi)
			{
				_src_addr_npi = src_addr_npi;
			}

			void set_src_addr(const char *sp_num)
			{ 
				memcpy(_src_addr, sp_num, SMPP_LEN_ADDR);
			}

			void set_dst_numbers(unsigned char dst_numbers)
			{
				_dst_numbers = dst_numbers;
			}

			void set_dst_addr(SME_DST_ADDRESS *dst_addr)
			{
				memcpy(_dst_addr, dst_addr, _dst_numbers * sizeof(SME_DST_ADDRESS));
			}

			void set_esm_class(unsigned char esm_class)
			{
				_esm_class = esm_class;
			}

			void set_pid(unsigned char pid)
			{
				_pid = pid;
			}

			void set_prio(unsigned char prio)
			{
				_prio = prio;
			}

			void set_sched_dt(const char *sched_dt)
			{
				memcpy(_sched_dt, sched_dt, SMPP_LEN_TIME);
			}

			void set_valid_dt(const char *valid_dt)
			{
				memcpy(_valid_dt, valid_dt, SMPP_LEN_TIME);
			}

			void set_report(unsigned char report)
			{
				_report = report;
			}

			void set_replace(unsigned char replace)
			{
				_replace = replace;
			}

			void set_coding(unsigned char coding)
			{
				_coding = coding;
			}

			void set_msg_id(unsigned char msg_id)
			{
				_msg_id = msg_id;
			}

			void set_sm_len(unsigned char sm_len)
			{
				_sm_len = sm_len;
			}

			void set_sm_cont(const char *sm_cont)
			{
				memcpy(_sm_cont, sm_cont, _sm_len);
					//strlen(sm_cont) > SMPP_LEN_CONT ? SMPP_LEN_CONT : strlen(sm_cont));
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_SUBMIT_MULTI];
				memset(_buf, 0, SMPP_LEN_SUBMIT_MULTI);
				_len = SMPP_LEN_HEAD;

				memcpy(_buf + _len, _srv_type, strlen(_srv_type));
				_len += (unsigned long)strlen(_srv_type) + 1;

				//	src_addr
				memcpy(_buf + _len, &_src_addr_ton, sizeof(_src_addr_ton));
				_len += sizeof(_src_addr_ton);

				memcpy(_buf + _len, &_src_addr_npi, sizeof(_src_addr_npi));
				_len += sizeof(_src_addr_npi);

				memcpy(_buf + _len, _src_addr, strlen(_src_addr));
				_len += (unsigned long)strlen(_src_addr) + 1;

				// dst_addr
				memcpy(_buf + _len, &_dst_numbers, sizeof(_dst_numbers));
				_len += sizeof(_dst_numbers);

				memcpy(_buf + _len, &_dst_addr, _dst_numbers * sizeof(SME_DST_ADDRESS));
				_len += _dst_numbers * sizeof(SME_DST_ADDRESS);
				
				//	esm_class
				memcpy(_buf + _len, &_esm_class, sizeof(_esm_class));
				_len += sizeof(_esm_class);

				memcpy(_buf + _len, &_pid, sizeof(_pid));
				_len += sizeof(_pid);

				memcpy(_buf + _len, &_prio, sizeof(_prio));
				_len += sizeof(_prio);

				//	sched_dt
				unsigned long dt_len = (unsigned long)strlen(_sched_dt);
				memcpy(_buf + _len, _sched_dt, dt_len);
				if (dt_len == 0)
				{
					++_len;
				}
				else
				{
					_len += dt_len + 1;
				}

				dt_len = (unsigned long)strlen(_valid_dt);
				memcpy(_buf + _len, _valid_dt, dt_len);
				if (dt_len == 0)
				{
					++_len;
				}
				else
				{
					_len += dt_len + 1;
				}

				//	
				memcpy(_buf + _len, &_report, sizeof(_report));
				_len += sizeof(_report);

				memcpy(_buf + _len, &_replace, sizeof(_replace));
				_len += sizeof(_replace);

				memcpy(_buf + _len, &_coding, sizeof(_coding));
				_len += sizeof(_coding);

				memcpy(_buf + _len, &_msg_id, sizeof(_msg_id));
				_len += sizeof(_msg_id);

				memcpy(_buf + _len, &_sm_len, sizeof(_sm_len));
				_len += sizeof(_sm_len);

				//	sm_cont
				memcpy(_buf + _len, _sm_cont, _sm_len);
				_len += _sm_len;
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		private:
			char			_srv_type[SMPP_LEN_SRV_TYPE + 1];	
			unsigned char	_src_addr_ton;		
			unsigned char	_src_addr_npi;		
			char			_src_addr[SMPP_LEN_ADDR + 1];
			
			unsigned char	_dst_numbers;
			SME_DST_ADDRESS	_dst_addr[SMPP_LEN_DST_NUMBERS];

			unsigned char	_esm_class;			
			unsigned char	_pid;			
			unsigned char	_prio;		
			char			_sched_dt[SMPP_LEN_TIME + 1];	// or unsigned char for NULL
			char			_valid_dt[SMPP_LEN_TIME + 1];	// or unsigned char for NULL
			unsigned char	_report;		
			unsigned char	_replace;		
			unsigned char	_coding;			
			unsigned char	_msg_id;			
			unsigned char	_sm_len;				
			char			_sm_cont[SMPP_LEN_CONT + 1];

		};

		class smpp_submit_multi_resp
			: public smpp_packet
		{
			smpp_submit_multi_resp(void)
			{
				_unsucc_num = 0;
				memset(_msg_id, 0, sizeof(_msg_id));
				memset(_unsucc_addr, 0, SMPP_LEN_DST_NUMBERS * sizeof(UNSUCC_SME_ADDRESS));
			}

			~smpp_submit_multi_resp(void)
			{
			}
	
			const char* get_msg_id(void) const
			{
				return _msg_id;
			}

			UNSUCC_SME_ADDRESS* get_unsucc_addr(unsigned char &unsucc_num)
			{
				unsucc_num = _unsucc_num;
				return _unsucc_addr;
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
					len = SMPP_LEN_HEAD;

					memcpy(_msg_id, buf + len, SMPP_LEN_MSG_ID);
					len += (unsigned long)strlen(_msg_id) + 1;

					memcpy(&_unsucc_num, buf + len, sizeof(_unsucc_num));
					len += sizeof(_unsucc_num);

					memcpy(&_unsucc_addr, buf + len, _unsucc_num * sizeof(UNSUCC_SME_ADDRESS));
					len += _unsucc_num * sizeof(UNSUCC_SME_ADDRESS);
				}
				return len;
			}		
		private:
			char				_msg_id[SMPP_LEN_MSG_ID + 1];
			unsigned char		_unsucc_num;
			UNSUCC_SME_ADDRESS	_unsucc_addr[SMPP_LEN_DST_NUMBERS];
		};

		class smpp_query
			: public smpp_packet
		{
		public:
			smpp_query(unsigned long seq_id)
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_QUERY_SM);

				_src_addr_ton = 0;
				_src_addr_npi = 0;
				memset(_msg_id, 0, sizeof(_msg_id));
				memset(_src_addr, 0, sizeof(_src_addr));
			}

			~smpp_query(void)
			{
			}

			void set_msg_id(const char *msg_id)
			{
				strncpy(_msg_id, msg_id, SMPP_LEN_MSG_ID);
			}

			void set_src_addr_ton(unsigned char src_addr_ton)
			{
				_src_addr_ton = src_addr_ton;
			}

			void set_src_addr_npi(unsigned char src_addr_npi)
			{
				_src_addr_npi = src_addr_npi;
			}

			void set_src_addr(const char *src_addr)
			{
				strncpy(_src_addr, src_addr, SMPP_LEN_ADDR);
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_QUERY];
				memset(_buf, 0, SMPP_LEN_QUERY);
				_len = SMPP_LEN_HEAD;

				memcpy(_buf + _len, _msg_id, strlen(_msg_id));
				_len += (unsigned long)strlen(_msg_id) + 1;

				memcpy(_buf + _len, &_src_addr_ton, sizeof(_src_addr_ton));
				_len += sizeof(_src_addr_ton);

				memcpy(_buf + _len, &_src_addr_npi, sizeof(_src_addr_npi));
				_len += sizeof(_src_addr_npi);
				
				memcpy(_buf + _len, _src_addr, strlen(_src_addr));
				_len += (unsigned long)strlen(_src_addr) + 1;				
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		private:
			char			_msg_id[SMPP_LEN_MSG_ID + 1];
			unsigned char	_src_addr_ton;
			unsigned char	_src_addr_npi;
			char			_src_addr[SMPP_LEN_ADDR + 1];
		};

		class smpp_query_resp
			: public smpp_packet
		{
		public:
			smpp_query_resp(void)
			{
				_msg_stat = 0;
				_err_code = 0;
				memset(_msg_id, 0, sizeof(_msg_id));
				memset(_final_date, 0, sizeof(_final_date));
			}

			~smpp_query_resp(void)
			{
			}

			const char* get_msg_id(void) const
			{
				return _msg_id;
			}

			const char* get_final_date(void) const
			{
				return _final_date;
			}

			unsigned char get_msg_stat(void) const
			{
				return _msg_stat;
			}

			unsigned char get_err_code(void) const
			{
				return _err_code;
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
					len = SMPP_LEN_HEAD;

					memcpy(_msg_id, buf + len, SMPP_LEN_MSG_ID);
					len += (unsigned long)strlen(_msg_id);

					memcpy(_final_date, buf + len, SMPP_LEN_TIME);
					len += (unsigned long)strlen(_final_date);

					memcpy(&_msg_stat, buf + len, sizeof(_msg_stat));
					len += sizeof(_msg_stat);
					
					memcpy(&_err_code, buf + len, sizeof(_err_code));
					len += sizeof(_err_code);
				}
				return len;
			}
		private:
			char			_msg_id[SMPP_LEN_MSG_ID + 1];
			char			_final_date[SMPP_LEN_TIME + 1];
			unsigned char	_msg_stat;
			unsigned char	_err_code;
		};

		class smpp_cancel
			: public smpp_packet
		{
		public:
			smpp_cancel(unsigned long seq_id)
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_CANCEL_SM);

				_src_addr_ton = 0;
				_src_addr_npi = 0;
				_dst_addr_ton = 0;
				_dst_addr_npi = 0;
				memset(_srv_type, 0, sizeof(_srv_type));
				memset(_msg_id, 0, sizeof(_msg_id));
				memset(_src_addr, 0, sizeof(_src_addr));
				memset(_dst_addr, 0, sizeof(_dst_addr));
			}

			~smpp_cancel(void)
			{
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_CANCEL];
				memset(_buf, 0, SMPP_LEN_CANCEL);
				_len = SMPP_LEN_HEAD;

				memcpy(_buf + _len, _srv_type, strlen(_srv_type));
				_len += (unsigned long)strlen(_srv_type) + 1;

				memcpy(_buf + _len, _msg_id, strlen(_msg_id));
				_len += (unsigned long)strlen(_msg_id) + 1;

				//	src_addr
				memcpy(_buf + _len, &_src_addr_ton, sizeof(_src_addr_ton));
				_len += sizeof(_src_addr_ton);

				memcpy(_buf + _len, &_src_addr_npi, sizeof(_src_addr_npi));
				_len += sizeof(_src_addr_npi);

				memcpy(_buf + _len, _src_addr, strlen(_src_addr));
				_len += (unsigned long)strlen(_src_addr) + 1;

				//	dst_addr
				memcpy(_buf + _len, &_dst_addr_ton, sizeof(_dst_addr_ton));
				_len += sizeof(_dst_addr_ton);

				memcpy(_buf + _len, &_dst_addr_npi, sizeof(_dst_addr_npi));
				_len += sizeof(_dst_addr_npi);

				memcpy(_buf + _len, _dst_addr, strlen(_dst_addr));
				_len += (unsigned long)strlen(_dst_addr);
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		private:
			char			_srv_type[SMPP_LEN_SRV_TYPE + 1];
			char			_msg_id[SMPP_LEN_MSG_ID + 1];
			unsigned char	_src_addr_ton;		
			unsigned char	_src_addr_npi;		
			char			_src_addr[SMPP_LEN_ADDR + 1];

			unsigned char	_dst_addr_ton;
			unsigned char	_dst_addr_npi;
			char			_dst_addr[SMPP_LEN_ADDR + 1];
		};

		class smpp_cancel_resp
			: public smpp_packet
		{
		public:
			smpp_cancel_resp(void)
			{

			}

			~smpp_cancel_resp(void)
			{
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
					len = SMPP_LEN_HEAD;
				}
				return len;
			}
		};

		class smpp_query_last_msgs
			: public smpp_packet
		{
		public:
			smpp_query_last_msgs(unsigned long seq_id)
			{
			}

			~smpp_query_last_msgs(void)
			{
			}
		public:
			unsigned long encode(void)
			{
				return 0;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		};

		class smpp_query_last_msgs_resp
			: public smpp_packet
		{
		public:
			smpp_query_last_msgs_resp(void)
			{
			}

			~smpp_query_last_msgs_resp(void)
			{
			}
		public:
			unsigned long encode(void)
			{
				return 0;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		};

		class smpp_replace
			: public smpp_packet
		{
		public:
			smpp_replace(unsigned long seq_id)
			{
				set_seq_id(seq_id);
				set_cmd_id(SMPP_CMD_REPLACE_SM);

				_src_addr_ton	= 0;
				_src_addr_npi	= 0;
				_report			= 0;
				_default_msg_id	= 0;
				_sm_len			= 0;
				memset(_msg_id, 0, sizeof(_msg_id));
				memset(_src_addr, 0, sizeof(_src_addr));
				memset(_sched_dt, 0, sizeof(_sched_dt));
				memset(_valid_dt, 0, sizeof(_valid_dt));
				memset(_sm_cont, 0, sizeof(_sm_cont));
			}

			~smpp_replace(void)
			{
			}
		public:
			unsigned long encode(void)
			{
				_buf = new char[SMPP_LEN_REPLACE];
				memset(_buf, 0, SMPP_LEN_REPLACE);
				_len = SMPP_LEN_HEAD;

				memcpy(_buf + _len, _msg_id, strlen(_msg_id));
				_len += (unsigned long)strlen(_msg_id) + 1;

				//	src_addr
				memcpy(_buf + _len, &_src_addr_ton, sizeof(_src_addr_ton));
				_len += sizeof(_src_addr_ton);

				memcpy(_buf + _len, &_src_addr_npi, sizeof(_src_addr_npi));
				_len += sizeof(_src_addr_npi);

				memcpy(_buf + _len, _src_addr, strlen(_src_addr));
				_len += (unsigned long)strlen(_src_addr) + 1;

				//	sched_dt
				unsigned long dt_len = (unsigned long)strlen(_sched_dt);
				memcpy(_buf + _len, _sched_dt, dt_len);
				if (dt_len == 0)
				{
					++_len;
				}
				else
				{
					_len += dt_len + 1;
				}

				dt_len = (unsigned long)strlen(_valid_dt);
				memcpy(_buf + _len, _valid_dt, dt_len);
				if (dt_len == 0)
				{
					++_len;
				}
				else
				{
					_len += dt_len + 1;
				}

				//	
				memcpy(_buf + _len, &_report, sizeof(_report));
				_len += sizeof(_report);

				memcpy(_buf + _len, &_default_msg_id, sizeof(_default_msg_id));
				_len += sizeof(_default_msg_id);

				memcpy(_buf + _len, &_sm_len, sizeof(_sm_len));
				_len += sizeof(_sm_len);

				//	sm_cont
				memcpy(_buf + _len, _sm_cont, _sm_len);
				_len += _sm_len;
				
				set_pkt_len(_len);
				encode_head(_buf);

				return _len;
			}

			unsigned long decode(const char *buf)
			{
				return 0;
			}
		private:
			char			_msg_id[SMPP_LEN_MSG_ID + 1];
			unsigned char	_src_addr_ton;		
			unsigned char	_src_addr_npi;		
			char			_src_addr[SMPP_LEN_ADDR + 1];
			char			_sched_dt[SMPP_LEN_TIME + 1];	// or unsigned char for NULL
			char			_valid_dt[SMPP_LEN_TIME + 1];	// or unsigned char for NULL
			unsigned char	_report;
			unsigned char	_default_msg_id;			
			unsigned char	_sm_len;				
			char			_sm_cont[SMPP_LEN_CONT + 1];
		};

		class smpp_replace_resp
			: public smpp_packet
		{
		public:
			smpp_replace_resp(void)
			{
			}

			~smpp_replace_resp(void)
			{
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
					len = SMPP_LEN_HEAD;
				}
				return len;
			}
		};

		// Protocol
		class smpp_protocol
		{
		public:
			smpp_protocol(void)
			{
			}

			~smpp_protocol(void)
			{
			}

			PACKET_STATE decode(buffer &buf, smpp_head &head);
		protected:
			bool find_cmd(unsigned long cmd_id);
		};

        class smpp_seq_id
        {
        public:
			smpp_seq_id(unsigned long seq_id)
				: _seq_id(seq_id)
			{
			}

			~smpp_seq_id(void)
			{
			}

			unsigned long get_seq_id(void) const
			{
				return _seq_id;
			}

			unsigned long inc_seq_id(void)
			{
				return (unsigned long)::InterlockedIncrement((LPLONG)&_seq_id);
			}

			void set_seq_id(smpp_packet *pack)
			{
				pack->set_seq_id(get_seq_id());
			}
		private:
			unsigned long	_seq_id;
        };
	};
};

#endif
