#pragma once

namespace sms
{
	namespace smgi
	{
		static const int MAX_ADDR_LEN		= 21;	//	最大地址长度
		static const int MAX_SRV_TYPE_LEN	= 10;	//	最大业务类型长度
		static const int MAX_FEE_TYPE_LEN	= 2;	//	最大计费类型长度
		static const int MAX_FEE_VALUE_LEN	= 6;	//	最大计费值长度、
		static const int MAX_TIME_LEN		= 17;	//	最大时间长度
		static const int MAX_CONT_LEN		= 160;	//	最大内容长度
		static const int MAX_CONT_ENC_LEN   = 140;	//	最大内容编码后长度
		static const int MAX_LONG_SM_LEN	= 1024;	//	最大长短信长度
		static const int MAX_LINK_ID_LEN	= 20;	//	最大link_id长度
		static const int MAX_ERR_CODE_LEN	= 7;	//	最大错误代码长度

		static const int MAX_IP_LEN			= 15;	//	最大IP地址长度
		static const int MAX_CORP_ID_LEN	= 10;	//	最大企业代码长度
		static const int MAX_UID_LEN		= 16;	//	最大用户名长度
		static const int MAX_PSWD_LEN		= 16;	//	最大用户密码长度

		static const int IDLE_TIME			= 60;	//	连接空闲时间，缺省60秒
		static const int SLIP_WINDOW		= 32;	//	滑动窗口大小
		static const int WAIT_TIME			= 30;	//	命令及其应答之间的时间间隔最大不能超过30秒
		static const int RESEND_TIMES		= 5;	//	重发次数

		#pragma warning(disable : 4103)
		#pragma pack(push, 1)

		/*
			网关类型
			0-SGIP标准；1-SGIP傲天；2-SGIP英斯克；3-SGIP斯特奇；4-SGIP华为；其他保留
			10-CMPP标准(华为)；11-CMPP 双端口(清华深讯)；12-CMPP 同端口双连接(亚信)
			20-SMGP标准(同端口双连接)
			30-CNGP标准(同端口双连接)
			40-SMPPV3.3(单连接)；41-SMPPV3.3(双连接)；42-SMPPV3.4(单连接)；43-SMPPV3.4(双连接)
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
			SMG_CODING_ASCII		= 0,		//	ASCII串
			SMG_CODING_WRT_CARD3	= 3,		//	短信写卡操作
			SMG_CODING_BIN			= 4,		//	二进制信息
			SMG_CODING_UCS2			= 8,		//	UCS2编码
#ifdef	_HKUNICOM_
			SMG_CODING_GB2312		= 8			//	Unicode编码
#else
			SMG_CODING_GB2312		= 15		//	含GB汉字
#endif
		};

		//	Long SM type	V4.0
		enum SMG_LONG_SM
		{
			SMS_LONG_SM_NO			= 0,		//	不支持长短信
			SMG_LONG_SM_6			= 6,		//	6字节编码长短信
			SMG_LONG_SM_7			= 7			//	7字节编码长短信
		};

		struct CONNECT_INFO
		{
			unsigned char	_smg_type;							//	网关类型
			unsigned char	_links;								//	连接数量
			unsigned short	_slip_size;							//	滑动窗口大小
			unsigned short	_resp_wait_time;					//	命令及其应答之间的时间间隔最大不能超过30秒
			unsigned short	_resp_resends;						//	重发次数
			unsigned short	_sp_port;							//	本地端口(供SGIP使用)
			unsigned short	_smg_port_mo;						//	网关端口(MO)
			unsigned short	_smg_port_mt;						//	网关端口(MT)
			unsigned short	_interval;							//	控制流量，每条Submit间隔的间隔时间，单位秒
			unsigned long	_seq_id;							//	初始消息ID
			unsigned long	_min_seq_id;						//	最小消息ID
			unsigned long   _max_seq_id;                        //  最大消息ID
			unsigned long	_src_id;							//	源节点编码(供SGIP使用)
			unsigned long	_long_sm;							//	长短信标志 V4.0
			char			_smg_ip[MAX_IP_LEN + 1];			//	网关IP地址
			char			_login_name[MAX_UID_LEN + 1];		//	用户名
			char			_login_pswd[MAX_PSWD_LEN + 1];		//	密码
			char			_corp_id[MAX_CORP_ID_LEN + 1];		//	企业代码
			char			_sp_num[MAX_ADDR_LEN + 1];			//	接入号

			
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

		//	各网关差异：fee_type,mt_flag,like_id
		struct SUBMIT_SM
		{
			char			_mobile[MAX_ADDR_LEN + 1];			//	手机号码
			char			_sp_num[MAX_ADDR_LEN + 1];			//	接入号码
			char            _charge_num[MAX_ADDR_LEN + 1];      //  付费号码
			char			_srv_type[MAX_SRV_TYPE_LEN + 1];	//	业务代码
			char			_fee_type[MAX_FEE_TYPE_LEN + 1];	//	计费类型
			char			_fee_value[MAX_FEE_VALUE_LEN + 1];	//	计费值
			unsigned char	_mt_flag;							//	下行标志(SGIP使用)
			unsigned char   _agent_flag;						//  代收费标志
			unsigned char	_prio;								//	优先级(0-9)
			unsigned char	_report;							//	状态报告{0不要，1要}
			char			_expire_time[MAX_TIME_LEN + 1];		//	超时时间
			char			_sched_time[MAX_TIME_LEN + 1];		//	定时下行时间
			unsigned char	_coding;							//	短消息的编码格式GSM3.38
																//	0：纯ASCII字符串
																//	3：写卡操作
																//	4：二进制编码
																//	8：UCS2编码
																//	15: GBK编码
			char			_cont[MAX_LONG_SM_LEN + 1];			//	消息内容  V4.0
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
			unsigned long		_seq_id;						//	序列号
			unsigned __int64	_msg_id;						//	消息ID
			unsigned long		_resp_code;						//	返回码 V4.0

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
			char			_mobile[MAX_ADDR_LEN + 1];			//	手机号码
			char			_sp_num[MAX_ADDR_LEN + 1];			//	接入号码
			unsigned char	_coding;							//	短消息的编码格式GSM3.38
																//	0：纯ASCII字符串
																//	3：写卡操作
																//	4：二进制编码
																//	8：UCS2编码
																//	15: GBK编码
			char			_cont[MAX_LONG_SM_LEN + 1];			//	消息内容 V4.0
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
			Report状态 	短消息状态
			0，发送成功	DELIVERED
			1，等待发送	ENROUTE，ACCEPTED
			2，发送失败	EXPIRED，DELETED，UNDELIVERABLE，UNKNOWN，REJECTED
		*/
		struct REPORT_SM
		{
			unsigned __int64	_msg_id;						//	消息ID
			unsigned char		_stat;							//	状态{0:发送成功, 1:等待发送, 2:发送失败}
			char				_mobile[MAX_ADDR_LEN + 1];		//	手机号码
			char				_err_code[MAX_ERR_CODE_LEN + 1];//	错误代码

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