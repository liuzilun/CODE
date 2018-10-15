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
Report״̬ 	����Ϣ״̬
0�����ͳɹ�	DELIVERED
1���ȴ�����	ENROUTE��ACCEPTED
2������ʧ��	EXPIRED��DELETED��UNDELIVERABLE��UNKNOWN��REJECTED
*/
#define STAT0		 "DELIVRD"
#define STAT1		 "ENROUTE,ACCEPTD"
#define STAT2		 "EXPIRED,DELETED,UNDELIV,UNKNOWN��REJECTD"

#define STAT0_VALUE   0  //  ״̬0
#define STAT1_VALUE   1  //  ״̬1
#define STAT2_VALUE   2	 //  ״̬2
#define STAT_UNKNOW   9  //  δ֪״̬

//  ��������
#define UINT8 unsigned __int64
#define UINT4 unsigned long
#define UINT2 unsigned short
#define UINT1 unsigned char

//	��Ϣ�ֶγ��ȵĳ�������

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

//	�շ�����
#define FEE_TYPE_FREE		"00"					//	���
#define FEE_TYPE_ITEM		"01"					//	�����շ�
#define FEE_TYPE_MONTH		"02"					//	����
#define FEE_TYPE_FIXED		"03"					//	�ⶥ
#define FEE_TYPE_MONTH_REQ	"04"					//	���¿۷�����
#define FEE_TYPE_CR			"05"					//	CR����

//	tlv param_tag����
static const UINT2 PROT_ID		= 0x0100;			//	Э��ID
static const UINT2 CONG_STAT	= 0x0428;			//	ӵ��״̬	

static UINT8 bcd2int(const BYTE src_str[], UINT4 src_len);

namespace sms
{
	namespace cngp 
	{
		class cngp_def
		{
		public:
			//  �����ʶ
			enum REQ_ID														
			{
				LOGIN				= 0X00000001,   //  ��¼
				LOGIN_RESP			= 0X80000001,	//  ��¼��Ӧ
				SUBMIT				= 0X00000002,	//  �ύ
				SUBMIT_RESP			= 0X80000002,	//  �ύ��Ӧ
				DELIVER				= 0X00000003,	//  ����
				DELIVER_RESP		= 0X80000003,	//  ���л�Ӧ
				ACTIVE_TEST			= 0X00000004,	//  ��·���
				ACTIVE_TEST_RESP	= 0X80000004,	//  ��·����Ӧ
				EXIT                = 0X00000006,	//  �˳�
				EXIT_RESP           = 0X80000006,	//  �˳���Ӧ
			};
			
			//  ��¼ģʽ
			enum LOGIN_MODE
			{
				MT		= 0,						//  MO
				MO		= 1,						//  MT
				MO_MT   = 2							//  MO & MT 
			};
			//  ��Ϣ��ʽ
			enum MSG_FMT   
			{
				ASCII	  = 0,						//  ASCII����
				WRT_CARD  = 3,						//  ����д��������
				BIN		  = 4,						//  ��������Ϣ��
				UCS2	  = 8,						//  UCS2���룻
				GB2312	  = 15,						//  ��GB����
			};
			
			//	SUBMIT MSG_TYPE
			enum MSG_TYPE
			{
				CANCEL_ORDER	= 0,				//ȡ������
				ORDER_REQ		= 1,				//���Ļ�㲥����
				ORDER_MT		= 2,				//�㲥�·�
				ORDER_SER_MT	= 3,				//�����·�
			};
			//��������
			enum 
			{
				HEAD_LEN		= 16,				//	ͷ������
				MSG_ID_LEN		= 10,				//  MSG_ID����
				IS_REPORT		= 1,				//	��Ҫ(��)����
				SUBMIT_OK		= 0,				//	�ύ�ɹ�
				DELVIER_OK		= 0					//  ���ͳɹ�
			};

			enum FEE_USER_TYPE
			{
                FEE_DEST_TERM_ID	= 0,			//	��Ŀ���ն˼Ʒ�
				FEE_SRC_TERM_ID		= 1,			//	��Դ�ն˼Ʒ�
				FEE_SP				= 2,			//	��SP�Ʒ�
				FEE_CHARGE_TERM_ID	= 3				//	�ԼƷ��û�����Ʒ�
			};
			//	MSG_ID�ĸ�ʽ
			typedef struct MSG_ID
			{
				BYTE smg_id[3];
				BYTE seq_id[7];
			}MSG_ID;

			//  ������ְ��ṹ
			
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

			//  ��ͷ
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

			//  ��¼��
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

			//  ��¼��Ӧ��
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

			//  �ύ��
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
				char  msg_content[MAX_MSG_CONT_LEN];     //  ���58������
				#ifdef _TLV
					cngp_tlv prot_id;
				#endif				
				cngp_submit(void)
				{
					memset(this, 0, sizeof(cngp_submit));
				}
			}cngp_submit;

			//  �ύ��Ӧ��
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

			//  ���а�
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

			//  ���л�Ӧ��
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

			//  ״̬����
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

			//  ��·����
			typedef struct cngp_active_test 
			{
				cngp_head head;
			}cngp_active_test;

			//  ��·����Ӧ��
			typedef struct cngp_active_test_resp
			{
				cngp_head head;
			}cngp_active_test_resp ;

			//  �˳�
			typedef struct cngp_exit
			{
				cngp_head head;
			}cngp_exit;

			//  �˳���Ӧ��
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
			//	�ͷŻ�����
			virtual void release(void) = 0;

			//	�������к�
			virtual UINT4 get_seq_id(void) const = 0;

			//	����Ϣ����
			static void encode_head(cngp_def::cngp_head &head);

			//	����Ϣͷ����
			static void decode_head(cngp_def::cngp_head & head);

		public:
			//	���ػ�������ַ
			virtual const char* buf(void)
			{
				return NULL;
			}

			//	������Ϣ����
			virtual UINT4 len(void) const
			{
				return 0;
			}

			//	������Ϣ���к�
			virtual void set_seq_id(UINT4)
			{ 
				return;
			}

			//	����
			virtual void decode(const char *buf)
			{
				return;
			}

			//	����
			virtual void encode(void)
			{
				return;
			}

			//	tlv����
			static void encode_tlv(cngp_def::cngp_tlv & tlv);
		};
		
		//  ��¼
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
			//  ������ܴ�
			UINT4 set_authenticator_client(void);

		private:
			cngp_def::cngp_login *_data;

			char _shared_secret[MAX_SHARED_SECRET_LEN];

		};

		//  ��¼��Ӧ
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

		//  �ύ
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
			//	����󳤶�
			UINT4					_enc_len;
		};

		//  �ύ��Ӧ
		class cngp_submit_resp :
			public cngp_packet
		{
		public:
			cngp_submit_resp(void);
			~cngp_submit_resp(void);
		private:
			cngp_def::cngp_submit_resp *_data;
			//  msg_id�����кŲ���
			UINT8	_msg_id;   
		public:
			void release(void);
			void decode(const char *buf);

		public:
			UINT4 get_seq_id(void) const
			{
				return _data->head.seq_id;
			}

			//  ȡMSG_ID�����кŲ���
			UINT8 get_msg_id_seq(void) const
			{
				return _msg_id;
			}

			UINT4 get_status(void) const
			{
				return _data->head.req_stat;
			}
		};

		//  ����
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

			//	msg_id�����кŲ���
			UINT8 _msg_id;

		};

		//  ���л�Ӧ
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

		//  ����
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

		//  ��·���
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

		//  ��·���ظ�
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

		//  �˳�
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

		//  �˳���Ӧ
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

		//  ��Ϣͷ
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

		//  seq��,����������Ϣ���к�
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
