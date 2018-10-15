#pragma once

#include <string>
#include <string.h>
#include <hash_map>
#include "..\Lib\MYSQLDB.h"
#include "..\Lib\Sync.h"

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CLUBCOMMON_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CLUBCOMMON_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CLUBCOMMON_EXPORTS
#define CLUBCOMMON_API __declspec(dllexport)
#else
#define CLUBCOMMON_API __declspec(dllimport)
#endif

#pragma warning(disable : 4251)
namespace RdiClubCommon
{
	//	告警级别
	enum ALARM_LEVEL
	{
		ALARM_LEVEL_EVENT		= 0,		//	事件
		ALARM_LEVEL_WARN		= 1,		//	普通错误
		ALARM_LEVEL_FATAL		= 2			//	致命错误
	};

	//	网络类型
	enum NET_TYPE
	{
		NET_TYPE_SP				= 0,		//	SP
		NET_TYPE_GSM_UNICOM		= 1,		//	GSM--Unicom
		NET_TYPE_GSM_MOBILE		= 2,		//	GSM--Mobile
		NET_TYPE_CDMA			= 3,		//	CDMA
		NET_TYPE_PHS			= 4,		//	PHS
		NET_TYPE_3G				= 5,		//	3G
		NET_TYPE_OTHER			= 6			//	Other
	};

	//	模块运行状态
	enum MODULE_RUN_STAT
	{
		MODULE_RUN_STAT_CLOSED	= 0,		//	关闭、终止
		MODULE_RUN_STAT_WORKING	= 1,		//	启动工作中
		MODULE_RUN_STAT_WARN	= 2			//	故障、告警
	};

	static const int MAX_MODULE_NAME_LEN	= 64;		//	模块名最大长度
	static const int MAX_SQL_LEN			= 4096;		//	SQL语句长度
	static const int MAX_SQL_SP_LEN			= 254;
	static const int MAX_TAG_NAME_LEN		= 32;		//	标签长度
	static const int MAX_IDEN_LEN			= 30;		//	作品短信标识内容长度
	static const int MAX_SIGN_LEN			= 10;		//	签名长度
	static const int MAX_XMS_WM_LEN			= 32;		//	水印长度
	static const int MAX_CONT_LEN			= 160;		//	消息内容最大长度
	static const int MAX_LONG_CONT_LEN		= 1024;		//	长短信：最大长度
	static const int MAX_TOPICS_LEN			= 128;		//	主题词最大长度
	static const int MAX_CACHE_SIZE			= 65536;	//	读写文件缓存大小
	static const int MAX_ERR_CODE			= 7;        //  错误代码长度
	static const int MAX_SP_LEN				= 21;		//	地址最大长度

	// This class is exported from the ClubCommon.dll
	class CLUBCOMMON_API CFuncCommon {
	public:
		CFuncCommon(void);
		~CFuncCommon(void);

	public:
		/*
			加密函数：数据库密码进行转换
			Input: src
			Ouput: dst
		*/
		bool do_encode_pwd(const char	*src,
						  char			*dst);

		/*
			解密函数：数据库密码进行转换
			Input: src
			Ouput: dst
		*/
		bool do_decode_pwd(const char	*src,
						   char			*dst);

		/*
			获取网络类型
		*/
		unsigned char get_net_type(__int64 mobile);

		/*
			手机号码编码
			input:	mobile	64位整型(移动号码，小灵通号码进行转换，SP号码不用转换)
			output	msisdn	带国际区号,直接设置，否则设置为空字符串
		*/
		const char* do_encode_isdn(__int64		mobile,
								   char			*msisdn,
								   const char	*inter_code = "86");

		/*
			手机号码解码
			input:	msisdn	字符串
			output:	mobile, 
					net_type网络类型{0-SP_Num;1-GSM(unicom);2-GSM(mobile);3-CDMA;4-PHS;5-3G;6-Others}

		*/
		__int64 do_decode_isdn(const char		*msisdn,
							   unsigned char	&net_type);

		/*
			从文件路径重分离出路径名
		*/
		char* get_file_path_from_name(const char	*file_name,
									  char			*file_path);

		/*
			从文件路径重分离出文件名
		*/
		char* get_file_name_from_path(const char	*file_path,
									  char			*file_name);

		/*
			检查指定路径是否存在
		*/
		bool get_dir_is_exist(const char *dir);

		/*
			创建多级目录
		*/
		bool set_mkdir_ex(const char *dir);	

		/*
			取得服务程序所在的路径
		*/
		bool get_service_path(const char *serv_name, char *serv_path);

		/*
			检查是否含有连续数字（电话号码），用于排除垃圾短信
		*/
		bool has_tel_num(const char *cont, int tel_nums);

		/*
			检查短信是否为ASCII编码
		*/
		bool is_ascii(const char *sms_cont);

		/*
			截取指定长度的部分内容
			Input:	cont		原文
					trunc_len	截取长度
			Output:	正确截取长度，防止出现乱码，英文是一个字节，中文2个字节，不齐
		*/
		int get_trunc_len(const char	*cont,
						  int			trunc_len);
	};

	class CLUBCOMMON_API CDBCommon {
	public:
		CDBCommon(CMYSQLDB *db, const char *db_name);
		~CDBCommon(void);
	
		/*
			产生事件/告警记录，通过Email/SMS通知管理员。
			Input: level 告警级别{0事件，1普通错误告警，2致命错误告警}
				   subject  告警主题(告警模块)
				   cont		告警详细内容
		*/
		bool do_alarm(unsigned char	level,
					  const char	*subject,
					  const char	*cont);

		/*
			更新系统运行状态
			input:	run_stat	运行状态
					pid_name	程序的唯一标志
					subject		程序描述
					detail		运行状态描述,
					
		*/
		bool do_rpt_run(unsigned char	run_stat,
						const char		*pid_name, 
						const char		*subject,
						const char		*detail);
	private:
		CMYSQLDB	*_db;
		char		_db_name[MAX_PATH];
	};

	//	进程同步状态机
	class CLUBCOMMON_API CProcSync
	{
	public:
		CProcSync(CMYSQLDB *db);
		~CProcSync(void);

		/*
			设置话单合并状态
			返回是否成功
		*/
		bool do_set_status_cdr_decoder(void);

		/*	
			检查是否为话单合并状态
			返回是否为该状态
		*/
		bool do_check_status_cdr_decoder(void);

		/*
			设置内容挖掘状态
			返回是否成功
		*/
		bool do_set_status_cont_miller(void);

		/*	
			检查是否为内容挖掘状态
			返回是否为该状态
		*/
		bool do_check_status_cont_miller(void);

		/*
			设置同步内容ID状态
			返回是否成功
		*/
		bool do_set_status_sms_id_sync(void);

		/*	
			检查是否为内容ID同步状态
			返回是否为该状态
		*/
		bool do_check_status_sms_id_sync(void);

		/*
			设置内容识别状态
			返回是否成功
		*/
		bool do_set_status_cont_identifier(void);

		/*	
			检查是否为内容识别状态
			返回是否为该状态
		*/
		bool do_check_status_cont_identifier(void);

		/*
			设置积分计算状态
			返回是否成功
		*/
		bool do_set_status_score_proc(void);

		/*	
			检查是否为积分计算状态
			返回是否为该状态
		*/
		bool do_check_status_score_proc(void);

		/*
			设置正常状态
			返回是否成功
		*/
		bool do_set_status_normal(void);

		/*	
			检查是否为正常状态
			返回是否为该状态
		*/
		bool do_check_status_normal(void);

		/*
			设置异常终止状态
		*/
		bool do_set_status_abend(void);

	private:
		CMYSQLDB	*_db;
	};

	//	号段管理
	class CLUBCOMMON_API CSegment
	{
	public:
#pragma pack (push, 1)
		struct SSegment
		{
			unsigned long	_seg_id;					//	段ID
			unsigned long	_segment;					//	万号段
			unsigned short	_area_id;					//	所在区号
			unsigned char	_site_id;					//	站号(site_id≠本站为mt_out)
			unsigned char	_seg_net;					//	网络(1-GSM/2-CDMA/3-PHS/4-3G)
		};

		//	Key: segment OR seg_id
		typedef std::hash_map<unsigned long, SSegment>	MAP_Segment;
#pragma pack (pop, 1)
	public:
		CSegment(CMYSQLDB *db);
		~CSegment(void);
		
		/*
			加载号段信息
		*/
		bool load(void);

		/*
			查找号段信息: By mobile
			返回NULL表示未找到，否则返回区号、站点、网络信息
		*/
		SSegment* do_find_by_mobile(__int64 mobile);

		/*
			查找会员ID
			返回0表示未找到，否则会员ID
		*/
		unsigned long do_get_mbr_id(__int64 mobile);
		unsigned long do_get_mbr_id(__int64 mobile, unsigned short &area_id);

		/*
			查找号段信息: By mbr_id
			返回NULL表示未找到，否则返回区号、站点、网络信息
		*/
		SSegment* do_find_by_mbr_id(unsigned long mbr_id);

		/*
			查找手机号码
			返回0表示未找到，否则手机号码
		*/
		__int64 do_get_mobile(unsigned long mbr_id);
		__int64 do_get_mobile(unsigned long mbr_id, unsigned short &area_id);
	private:
		CMYSQLDB	*_db;
		
		MAP_Segment					_map_segment;
		MAP_Segment					_map_seg_id;
		Process::CCriticalSection	_cs_seg;
	};
}