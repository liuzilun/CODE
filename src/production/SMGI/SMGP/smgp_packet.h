#pragma once

/*
** Copyright (c) 2005
** Shen Zhen RongDi Information Technology Co., Ltd.
** All rights reserved.
**
** smgp_packet.h
** class SMGP_xxx :Implement the encoding & decoding code of the whole packet 
**				that sp->smg & smg->sp communication using.
**
**
**
**
** Version:	1.0
** Author:	qsf
** Time:	2005/04/06
*/


#include <memory.h>         //  memcpy,memset
#include <stdio.h>			//  sprintf
#include <winsock.h>		//  htonl, ntohl
#include <math.h>			// 	pow
#include "..\SMGI\SMGDefine.h"		//  
#include "..\common\sync.h" // 	CCriticalSection

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
static const int MAX_CLIENT_ID_LEN		= 8;
static const int MAX_AUTH_CLIENT_LEN	= 16;
static const int MAX_TIME_STAMP_LEN		= 10;
static const int MAX_SHARED_SECRET_LEN	= 15;
static const int MAX_MD5_SRC_LEN		= 50;

//  login_resp
static const int MAX_AUTH_SERVER_LEN	= 16;

//	submit
static const int MAX_SERVICE_ID_LEN		= 10;
static const int MAX_FEE_CODE_LEN		= 6;
static const int MAX_FEE_TYPE_LENGTH	= 2;
static const int MAX_FIXED_FEE_LEN		= 6;
static const int MAX_VALID_TIME_LEN		= 17;
static const int MAX_AT_TIME_LEN		= 17;
static const int MAX_SRC_TERM_ID_LEN	= 21;
static const int MAX_CHARGE_TERM_ID_LEN = 21;
static const int MAX_DEST_TERM_ID_LEN	= 21;
static const int MAX_MSG_CONT_LEN		= 252;
static const int MAX_RESERVE_LEN		= 8;
static const int MAX_SUBMTI_MSG_TYPE	= 1;
static const int MAX_SP_DEAL_RESULT		= 1;

//	MSG_ID
static const int MAX_MSG_ID_LEN			= 10;
static const int MAX_SMG_ID_LEN			= 3;
static const int MAX_MSG_ID_SEQ			= 7;

//	deliver
static const int MAX_RECV_TIME_LEN		= 14;

//  query
static const int MAX_QRY_TIME_LEN		= 8;
static const int MAX_QRY_CODE_LEN		= 10;

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
#define FEE_TYPE_FREE  "00"
#define FEE_TYPE_ITEM  "01"
#define FEE_TYPE_MONTH "02"
#define FEE_TYPE_FIXED "03"

static UINT8 bcd2int(const BYTE src_str[], UINT4 src_len);

namespace sms
{
	namespace smgp 
	{
		class smgp_def
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
				QUERY               = 0X00000007,	//  ��ѯ
				QUERY_RESP          = 0X80000007,	//  ��ѯ��Ӧ
			};
			
			//  ��¼ģʽ
			enum LOGIN_MODE
			{
				MT		= 0,	//  MO
				MO		= 1,	//  MT
				MO_MT   = 2		//  MO & MT 
			};
			//  ��Ϣ��ʽ
			enum MSG_FMT   
			{
				ASCII	  = 0,							//  ASCII����
				WRT_CARD3 = 3,							//  ����д��������
				BIN		  = 4,							//  ��������Ϣ��
				UCS2	  = 8,							//  UCS2���룻
				GB2312	  = 15,							//  ��GB����
			};
			
			//	SUBMIT MSG_TYPE
			enum MSG_TYPE
			{
				CANCEL_ORDER	= 1,					//ȡ������
				ORDER_REQ		= 2,					//��������
				ORDER_PROG		= 3,					//�㲥
				ORDER			= 4,					//����
				COMM_OPRT		= 5,					//��������
				QUERY_REQ		= 6	,					//��ѯ	
				SMC_ORDER		= 0X98,
				SMC_CANCEL		= 0X99,
				
			};

			//��������
			enum 
			{
				HEAD_LEN		= 12,					//	ͷ������
				MSG_ID_LEN		= 10,					//  MSG_ID����
				IS_REPORT		= 1,					//	��Ҫ(��)����
				SUBMIT_OK		= 0,					//	�ύ�ɹ�
				DELVIER_OK		= 0						//  ���ͳɹ�
			};

			//	MSG_ID�ĸ�ʽ
			typedef struct MSG_ID
			{
				BYTE smg_id[3];
				BYTE seq_id[7];
			}MSG_ID;

			//  ������ְ��ṹ
			//  ��ͷ
			typedef struct smgp_head 
			{
				UINT4 pkt_len;
				UINT4 req_id;
				UINT4 seq_id;			
				smgp_head(void)
				{		
					memset(this, 0, sizeof(smgp_head));
				}
			}smgp_head;

			//  ��¼��
			typedef struct smgp_login
			{
				smgp_head head;
				char client_id[MAX_CLIENT_ID_LEN];
				char authenticator_client[MAX_AUTH_CLIENT_LEN];
				UINT1 login_mode;
				UINT4 time_stamp;
				UINT1 version;
				smgp_login(void)
				{
					memset(this, 0, sizeof(smgp_login));
				}
			}smgp_login;

			//  ��¼��Ӧ��
			typedef struct smgp_login_resp
			{	
				smgp_head head;
				UINT4 status;
				char  authenticator_server[MAX_AUTH_SERVER_LEN];
				UINT1 version;
				smgp_login_resp(void)
				{
					memset(this, 0, sizeof(smgp_login_resp));
				}
			}smgp_login_resp;

			//  �ύ��
			typedef struct smgp_submit
			{
				smgp_head head;
				UINT1 msg_type;
				UINT1 need_report;
				UINT1 priority;
				char  service_id[MAX_SERVICE_ID_LEN];
				char  fee_type[MAX_FEE_TYPE_LENGTH];
				char  fee_code[MAX_FEE_CODE_LEN];
				char  fixed_fee[MAX_FIXED_FEE_LEN];
				UINT1 msg_fmt;
				char  valid_time[MAX_VALID_TIME_LEN];
				char  at_time[MAX_AT_TIME_LEN];
				char  src_term_id[MAX_SRC_TERM_ID_LEN];
				char  charge_term_id[MAX_CHARGE_TERM_ID_LEN];
				UINT1 dest_term_id_count;
				char  dest_term_id[MAX_DEST_TERM_ID_LEN];
				UINT1 msg_len;
				char  msg_content[MAX_MSG_CONT_LEN];            //  ���58������
				char  reserve[MAX_RESERVE_LEN];
#ifdef _SMGP3_ 
				UINT2 link_id_tag;
				UINT2 link_id_len; 
				char  link_id[MAX_LINK_ID_LEN];

				UINT2 submit_msg_type_tag;
				UINT2 submit_msg_type_len;
				UINT1 submit_msg_type;

				UINT2 sp_deal_result_tag;
				UINT2 sp_deal_result_len;
				UINT1 sp_deal_result;
#endif
				smgp_submit(void)
				{
					memset(this, 0, sizeof(smgp_submit));
				}
			}smgp_submit;

			//  �ύ��Ӧ��
			typedef struct smgp_submit_resp
			{
				smgp_head head;
				union
				{
					MSG_ID		  msg_id;
					unsigned char sz_msg_id[MAX_MSG_ID_LEN];
				};
				UINT4 status;
				smgp_submit_resp(void)
				{
					memset(this, 0, sizeof(smgp_submit_resp));
				}
			}smgp_submit_resp;

			//  ���а�
			typedef struct smgp_deliver
			{
				smgp_head head;
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
				char reserve[MAX_RESERVE_LEN];
#ifdef _SMGP3_
				UINT2 link_id_tag;
				UINT2 link_id_len; 
				char  link_id[MAX_LINK_ID_LEN];

				UINT2 submit_msg_type_tag;
				UINT2 submit_msg_type_len;
				UINT1 submit_msg_type;

				UINT2 sp_deal_result_tag;
				UINT2 sp_deal_result_len;
				UINT1 sp_deal_result;
#endif
				smgp_deliver(void)
				{
					memset(this, 0, sizeof(smgp_deliver));
				}
			}smgp_deliver;

			//  ���л�Ӧ��
			typedef struct smgp_deliver_resp
			{
				smgp_head head;
				union
				{
					MSG_ID			msg_id;
					unsigned char	sz_msg_id[MAX_MSG_ID_LEN];
				};
				UINT4 status;
				smgp_deliver_resp(void)
				{
					memset(this, 0, sizeof(smgp_deliver_resp));
				}
			}smgp_deliver_resp;

			//  ��ѯ��
			typedef struct smgp_query
			{
				smgp_head head;
				char time[MAX_QRY_TIME_LEN];
				UINT1 query_type;
				char query_code[MAX_QRY_CODE_LEN];
				smgp_query(void)
				{
					memset(this, 0, sizeof(smgp_query));
				}
			}smgp_query;

			//  ��ѯ��Ӧ��
			typedef struct smgp_query_resp
			{
				smgp_head head;
				char time[MAX_QRY_TIME_LEN];
				UINT1 query_type;
				char query_code[MAX_QRY_CODE_LEN];
				UINT4 mt_tl_msg;
				UINT4 mt_tl_usr;
				UINT4 mt_scs;
				UINT4 mt_wt;
				UINT4 mt_fl;
				UINT4 mo_scs;
				UINT4 mo_wt;
				UINT4 mo_fl;
				char reverse[MAX_RESERVE_LEN];
				smgp_query_resp(void)
				{
					memset(this, 0, sizeof(smgp_query_resp));
				}
			}smgp_query_resp;

			//  ״̬����
			typedef struct smgp_report
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
				smgp_report(void)
				{
					memset(this, 0, sizeof(smgp_report));
				}
			}smgp_report;

			//  ��·����
			typedef struct smgp_active_test 
			{
				smgp_head head;
			}smgp_active_test;

			//  ��·����Ӧ��
			typedef struct smgp_active_test_resp
			{
				smgp_head head;
			}smgp_active_test_resp ;

			//  �˳�
			typedef struct smgp_exit
			{
				smgp_head head;
			}smgp_exit;

			//  �˳���Ӧ��
			typedef struct smgp_exit_resp {
				smgp_head head;
			} smgp_exit_resp;

		};
		class smgp_packet
		{
		public:
			smgp_packet(void);
			~smgp_packet(void);
		
		public:
			//	�ͷŻ�����
			virtual void release(void) = 0;

			//	�������к�
			virtual UINT4 get_seq_id(void) const = 0;

			//	����Ϣ����
			static void encode_head(smgp_def::smgp_head &head);

			//	����Ϣͷ����
			static void decode_head(smgp_def::smgp_head & head);

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

		};
		
		//  ��¼
		class smgp_login :
			public smgp_packet
		{
		public:
			smgp_login(void);
			~smgp_login(void);

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
				return sizeof(smgp_def::smgp_login);
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
				_data->time_stamp = htonl(tm_stmp);
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
			smgp_def::smgp_login *_data;

			char _shared_secret[MAX_SHARED_SECRET_LEN];

		};

		//  ��¼��Ӧ
		class smgp_login_resp :
			public smgp_packet
		{
		public:
			smgp_login_resp(void);
			~smgp_login_resp(void);

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
				return _data->status;
			}

			UINT4 get_version(void) const
			{
				return _data->version;
			}

		private:
			smgp_def::smgp_login_resp *_data;
			
		};

		//  �ύ
		class smgp_submit :
			public smgp_packet
		{
		public:
			smgp_submit(void);
			~smgp_submit(void);

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

			void set_fee_code(const char *fee_code)
			{
				strncpy(_data->fee_code, fee_code, sizeof(_data->fee_code));
			}

			void set_fixed_fee(const char *fixed_fee)
			{
				strncpy(_data->fixed_fee, fixed_fee, sizeof(_data->fixed_fee));
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
				strncpy(_data->charge_term_id, charge_term_id, sizeof(_data->charge_term_id));
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

			void set_rsv(const char *rsv)
			{
				strncpy(_data->reserve, rsv, sizeof(_data->reserve));
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

#ifdef _SMGP3_
			void set_link_id(const char *link_id)
			{
				memcpy(_data->link_id, link_id, MAX_LINK_ID_LEN);              
			}

			void set_submit_msg_type(const UINT1 submit_msg_type)
			{
				_data->submit_msg_type = submit_msg_type;
			}

			void set_sp_deal_result(const UINT1 sp_deal_result)
			{
				_data->sp_deal_result = sp_deal_result;
			}
			
			void modify_enc_len(void)
			{
				_data->msg_len = _data->msg_len - 2;
			}
#endif
		private:
			smgp_def::smgp_submit * _data;
			//	����󳤶�
			UINT4					_enc_len;

		};

		//  �ύ��Ӧ
		class smgp_submit_resp :
			public smgp_packet
		{
		public:
			smgp_submit_resp(void);
			~smgp_submit_resp(void);
		private:
			smgp_def::smgp_submit_resp *_data;
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
				return _data->status;
			}
		};

		//  ����
		class smgp_deliver :
			public smgp_packet
		{
		public:
			smgp_deliver(void);
			~smgp_deliver(void);

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
#ifdef _SMGP3
			const char* get_link_id(void) const
			{
				return _data->link_id;
			}

			UINT1 get_submit_msg_type(void) const
			{
				return _data->submit_msg_type;
			}

			UINT1 get_sp_deal_result(void)
			{
				return _data->sp_deal_result;
			}
#endif
		private:
			smgp_def::smgp_deliver *_data;

			//	msg_id�����кŲ���
			UINT8 _msg_id;

		};

		//  ���л�Ӧ
		class smgp_deliver_resp :
			public smgp_packet
		{
		public:
			smgp_deliver_resp(void);
			~smgp_deliver_resp(void);

		public:
			void encode(void);
			void release(void);

		public:
			void set_status(UINT4 status)
			{
				_data->status = status;
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
				return sizeof(smgp_def::smgp_deliver_resp);
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
			smgp_def::smgp_deliver_resp *_data;

		};

		//  ����
		class smgp_report 
		{
		public:
			smgp_report(void);
			~smgp_report(void);

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
			smgp_def::smgp_report *_data;

			UINT4	_stat;

			char	_err_code[MAX_ERR_CODE_LEN + 1];

			UINT8   _msg_id;

		};

		//  ��·���
		class smgp_active_test :
			public smgp_packet
		{
		public:
			smgp_active_test(void);
			~smgp_active_test(void);

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
				return sizeof(smgp_def::smgp_active_test);
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
			smgp_def::smgp_active_test *_data;

		};

		//  ��·���ظ�
		class smgp_active_test_resp :
			public smgp_packet
		{
		public:
			smgp_active_test_resp(void);
			~smgp_active_test_resp(void);
		
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
				return sizeof(smgp_def::smgp_active_test_resp);
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

		private:
			smgp_def::smgp_active_test_resp *_data;

		};

		//  �˳�
		class smgp_exit :
			public smgp_packet
		{
		public:
			smgp_exit(void);
			~smgp_exit(void);

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
				return sizeof(smgp_def::smgp_exit);
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

		private:
			smgp_def::smgp_exit *_data;
		};

		//  �˳���Ӧ
		class smgp_exit_resp :
			public smgp_packet
		{
		public:
			smgp_exit_resp(void);
			~smgp_exit_resp(void);
        
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
				return sizeof(smgp_def::smgp_exit_resp);
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

		private:
			smgp_def::smgp_exit_resp *_data;

		};

		//  ��ѯ
		class smgp_query :
			public smgp_packet
		{
		public:
			smgp_query(void);
			~smgp_query(void);

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
				return sizeof(smgp_def::smgp_query);
			}

			void set_qry_type(UINT1 qry_type)
			{
				_data->query_type = qry_type;
			}

			void set_qry_code(const char *qry_code)
			{
				strncpy(_data->query_code, qry_code, sizeof(_data->query_code));
			}

			void set_time(const char *time)
			{
				strncpy(_data->time, time, sizeof(_data->time));
			}

			UINT4 get_seq_id() const
			{
				return ntohl(_data->head.seq_id);
			}

			void set_seq_id(UINT4 seq_id)
			{
				_data->head.seq_id = seq_id;
			}

		private:
			smgp_def::smgp_query *_data;

		};

		//  ��ѯ��Ӧ
		class smgp_query_resp :
			public smgp_packet
		{
		public:
			smgp_query_resp(void);
			~smgp_query_resp(void);

		public:
			void release(void);
			void decode(const char * buf);

		public:
			UINT4 get_qry_type(void)
			{
				return _data->query_type;
			}

			void get_time(char *buf)
			{
				memcpy(buf, _data->time, sizeof(_data->time));
			}

			void get_qry_code(char *buf)
			{
				memcpy(buf, _data->query_code, sizeof(_data->query_code));
			}

			UINT4 get_mt_tl_msg(void)
			{
				return _data->mt_tl_msg;
			}

			UINT4 get_mt_tl_user(void)
			{
				return _data->mt_tl_usr;
			}

			UINT4 get_mt_scs(void)
			{
				return _data->mo_scs;
			}

			UINT4 get_mt_wt(void)
			{
				return _data->mt_wt;
			}

			UINT4 get_mt_fl(void)
			{
				return _data->mt_fl;
			}

			UINT4 get_mo_scs(void)
			{
				return _data->mo_scs;
			}

			UINT4 get_mo_wt(void)
			{
				return _data->mo_wt;
			}

			UINT4 get_mo_fl(void)
			{
				return _data->mo_fl;
			}

			UINT4 get_seq_id(void) const
			{
				return _data->head.seq_id;
			}

		private:
			smgp_def::smgp_query_resp *_data;

		};

		//  ��Ϣͷ
		class smgp_head :
			public smgp_packet
		{
		private:
			smgp_def::smgp_head *_data;
		public:
			smgp_head(void);
			~smgp_head(void);

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
		class smgp_seq
		{
		public:
			smgp_seq(UINT4 i_seed);
			~smgp_seq(void);

		public:
			void set_seq(smgp_packet &smgp_pkt);
			void set_seq(smgp_packet *smgp_pkt);
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
