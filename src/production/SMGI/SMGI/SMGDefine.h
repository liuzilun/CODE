#pragma once

namespace sms
{
	namespace smgi
	{
		static const int MAX_ADDR_LEN		= 21;	//	����ַ����
		static const int MAX_SRV_TYPE_LEN	= 10;	//	���ҵ�����ͳ���
		static const int MAX_FEE_TYPE_LEN	= 2;	//	���Ʒ����ͳ���
		static const int MAX_FEE_VALUE_LEN	= 6;	//	���Ʒ�ֵ���ȡ�
		static const int MAX_TIME_LEN		= 17;	//	���ʱ�䳤��
		static const int MAX_CONT_LEN		= 160;	//	������ݳ���
		static const int MAX_CONT_ENC_LEN   = 140;	//	������ݱ���󳤶�
		static const int MAX_LONG_SM_LEN	= 1024;	//	��󳤶��ų���
		static const int MAX_LINK_ID_LEN	= 20;	//	���link_id����
		static const int MAX_ERR_CODE_LEN	= 7;	//	��������볤��

		static const int MAX_IP_LEN			= 15;	//	���IP��ַ����
		static const int MAX_CORP_ID_LEN	= 10;	//	�����ҵ���볤��
		static const int MAX_UID_LEN		= 16;	//	����û�������
		static const int MAX_PSWD_LEN		= 16;	//	����û����볤��

		static const int IDLE_TIME			= 60;	//	���ӿ���ʱ�䣬ȱʡ60��
		static const int SLIP_WINDOW		= 32;	//	�������ڴ�С
		static const int WAIT_TIME			= 30;	//	�����Ӧ��֮���ʱ��������ܳ���30��
		static const int RESEND_TIMES		= 5;	//	�ط�����

		#pragma warning(disable : 4103)
		#pragma pack(push, 1)

		/*
			��������
			0-SGIP��׼��1-SGIP���죻2-SGIPӢ˹�ˣ�3-SGIP˹���棻4-SGIP��Ϊ����������
			10-CMPP��׼(��Ϊ)��11-CMPP ˫�˿�(�廪��Ѷ)��12-CMPP ͬ�˿�˫����(����)
			20-SMGP��׼(ͬ�˿�˫����)
			30-CNGP��׼(ͬ�˿�˫����)
			40-SMPPV3.3(������)��41-SMPPV3.3(˫����)��42-SMPPV3.4(������)��43-SMPPV3.4(˫����)
		*/
		enum SMG_TYPE
		{
			SMG_TYPE_SGIP_STD		= 0,
			SMG_TYPE_SGIP_OPENET	= 1,
			SMG_TYPE_SGIP_INTRINT	= 2,
			SMG_TYPE_SGIP_SITECH	= 3,
			SMG_TYPE_SGIP_HUAWEI	= 4,
			SMG_TYPE_CMPP_STD		= 10,
			SMG_TYPE_CMPP_DP_QHSX	= 11,
			SMG_TYPE_CMPP_DL_YX		= 12,
			SMG_TYPE_SMGP_STD		= 20,
			SMG_TYPE_CNGP_STD		= 30,
			SMG_TYPE_SMPP_SL_V33	= 40,
			SMG_TYPE_SMPP_DL_V33	= 41,
			SMG_TYPE_SMPP_SL_V34	= 42,
			SMG_TYPE_SMPP_DL_V34	= 43
		};

		//	Msg_coding
		enum SMG_CODING   
		{
			SMG_CODING_ASCII		= 0,		//	ASCII��
			SMG_CODING_WRT_CARD3	= 3,		//	����д������
			SMG_CODING_BIN			= 4,		//	��������Ϣ
			SMG_CODING_UCS2			= 8,		//	UCS2����
#ifdef	_HKUNICOM_
			SMG_CODING_GB2312		= 8			//	Unicode����
#else
			SMG_CODING_GB2312		= 15		//	��GB����
#endif
		};

		//	Long SM type	V4.0
		enum SMG_LONG_SM
		{
			SMS_LONG_SM_NO			= 0,		//	��֧�ֳ�����
			SMG_LONG_SM_6			= 6,		//	6�ֽڱ��볤����
			SMG_LONG_SM_7			= 7			//	7�ֽڱ��볤����
		};

		struct CONNECT_INFO
		{
			unsigned char	_smg_type;							//	��������
			unsigned char	_links;								//	��������
			unsigned short	_slip_size;							//	�������ڴ�С
			unsigned short	_resp_wait_time;					//	�����Ӧ��֮���ʱ��������ܳ���30��
			unsigned short	_resp_resends;						//	�ط�����
			unsigned short	_sp_port;							//	���ض˿�(��SGIPʹ��)
			unsigned short	_smg_port_mo;						//	���ض˿�(MO)
			unsigned short	_smg_port_mt;						//	���ض˿�(MT)
			unsigned short	_interval;							//	����������ÿ��Submit����ļ��ʱ�䣬��λ��
			unsigned long	_seq_id;							//	��ʼ��ϢID
			unsigned long	_min_seq_id;						//	��С��ϢID
			unsigned long   _max_seq_id;                        //  �����ϢID
			unsigned long	_src_id;							//	Դ�ڵ����(��SGIPʹ��)
			unsigned long	_long_sm;							//	�����ű�־ V4.0
			char			_smg_ip[MAX_IP_LEN + 1];			//	����IP��ַ
			char			_login_name[MAX_UID_LEN + 1];		//	�û���
			char			_login_pswd[MAX_PSWD_LEN + 1];		//	����
			char			_corp_id[MAX_CORP_ID_LEN + 1];		//	��ҵ����
			char			_sp_num[MAX_ADDR_LEN + 1];			//	�����

			
			CONNECT_INFO(void)
			{
				memset(this, 0, sizeof(CONNECT_INFO));
			}

			~CONNECT_INFO(void)
			{
			}

			CONNECT_INFO(const CONNECT_INFO &info)
			{
				*this = info;
			}

			const CONNECT_INFO& operator=(const CONNECT_INFO &info)
			{
				memcpy(this, &info, sizeof(CONNECT_INFO));		
				return *this;
			}
		};

		//	�����ز��죺fee_type,mt_flag,like_id
		struct SUBMIT_SM
		{
			char			_mobile[MAX_ADDR_LEN + 1];			//	�ֻ�����
			char			_sp_num[MAX_ADDR_LEN + 1];			//	�������
			char            _charge_num[MAX_ADDR_LEN + 1];      //  ���Ѻ���
			char			_srv_type[MAX_SRV_TYPE_LEN + 1];	//	ҵ�����
			char			_fee_type[MAX_FEE_TYPE_LEN + 1];	//	�Ʒ�����
			char			_fee_value[MAX_FEE_VALUE_LEN + 1];	//	�Ʒ�ֵ
			unsigned char	_mt_flag;							//	���б�־(SGIPʹ��)
			unsigned char   _agent_flag;						//  ���շѱ�־
			unsigned char	_prio;								//	���ȼ�(0-9)
			unsigned char	_report;							//	״̬����{0��Ҫ��1Ҫ}
			char			_expire_time[MAX_TIME_LEN + 1];		//	��ʱʱ��
			char			_sched_time[MAX_TIME_LEN + 1];		//	��ʱ����ʱ��
			unsigned char	_coding;							//	����Ϣ�ı����ʽGSM3.38
																//	0����ASCII�ַ���
																//	3��д������
																//	4�������Ʊ���
																//	8��UCS2����
																//	15: GBK����
			char			_cont[MAX_LONG_SM_LEN + 1];			//	��Ϣ����  V4.0
			char			_link_id[MAX_LINK_ID_LEN + 1];		//	link_id

			SUBMIT_SM(void) 
			{
				memset(this, 0, sizeof(SUBMIT_SM));
			}

			~SUBMIT_SM(void) 
			{
			}

			SUBMIT_SM(const SUBMIT_SM &sm)
			{
				*this = sm;
			}

			const SUBMIT_SM& operator=(const SUBMIT_SM &sm)
			{
				memcpy(this, &sm, sizeof(SUBMIT_SM));		
				return *this;
			}
		};

		struct SUBMIT_RESP
		{
			unsigned long		_seq_id;						//	���к�
			unsigned __int64	_msg_id;						//	��ϢID
			unsigned long		_resp_code;						//	������ V4.0

			SUBMIT_RESP(void) 
			{
				memset(this, 0, sizeof(SUBMIT_RESP));
			}

			~SUBMIT_RESP(void) 
			{
			}

			SUBMIT_RESP(const SUBMIT_RESP &sm)
			{
				*this = sm;
			}

			const SUBMIT_RESP& operator=(const SUBMIT_RESP &sm)
			{
				memcpy(this, &sm, sizeof(SUBMIT_RESP));		
				return *this;
			}
		};

		struct DELIVER_SM
		{
			char			_mobile[MAX_ADDR_LEN + 1];			//	�ֻ�����
			char			_sp_num[MAX_ADDR_LEN + 1];			//	�������
			unsigned char	_coding;							//	����Ϣ�ı����ʽGSM3.38
																//	0����ASCII�ַ���
																//	3��д������
																//	4�������Ʊ���
																//	8��UCS2����
																//	15: GBK����
			char			_cont[MAX_LONG_SM_LEN + 1];			//	��Ϣ���� V4.0
			char			_link_id[MAX_LINK_ID_LEN + 1];		//	link_id

			DELIVER_SM(void)
			{
				memset(this, 0, sizeof(DELIVER_SM));
			}

			~DELIVER_SM(void)
			{
			}

			DELIVER_SM(const DELIVER_SM &sm)
			{
				*this = sm;
			}

			const DELIVER_SM& operator=(const DELIVER_SM &sm)
			{
				memcpy(this, &sm, sizeof(DELIVER_SM));		
				return *this;
			}
		};

		/*
			Report״̬ 	����Ϣ״̬
			0�����ͳɹ�	DELIVERED
			1���ȴ�����	ENROUTE��ACCEPTED
			2������ʧ��	EXPIRED��DELETED��UNDELIVERABLE��UNKNOWN��REJECTED
		*/
		struct REPORT_SM
		{
			unsigned __int64	_msg_id;						//	��ϢID
			unsigned char		_stat;							//	״̬{0:���ͳɹ�, 1:�ȴ�����, 2:����ʧ��}
			char				_mobile[MAX_ADDR_LEN + 1];		//	�ֻ�����
			char				_err_code[MAX_ERR_CODE_LEN + 1];//	�������

			REPORT_SM(void)
			{
				memset(this, 0, sizeof(REPORT_SM));
			}

			~REPORT_SM(void)
			{
			}

			REPORT_SM(const REPORT_SM &sm)
			{
				*this = sm;
			}

			const REPORT_SM& operator=(const REPORT_SM &sm)
			{
				memcpy(this, &sm, sizeof(REPORT_SM));		
				return *this;
			}
		};
	}
}

#pragma pack(pop)