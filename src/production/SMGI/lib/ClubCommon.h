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
	//	�澯����
	enum ALARM_LEVEL
	{
		ALARM_LEVEL_EVENT		= 0,		//	�¼�
		ALARM_LEVEL_WARN		= 1,		//	��ͨ����
		ALARM_LEVEL_FATAL		= 2			//	��������
	};

	//	��������
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

	//	ģ������״̬
	enum MODULE_RUN_STAT
	{
		MODULE_RUN_STAT_CLOSED	= 0,		//	�رա���ֹ
		MODULE_RUN_STAT_WORKING	= 1,		//	����������
		MODULE_RUN_STAT_WARN	= 2			//	���ϡ��澯
	};

	static const int MAX_MODULE_NAME_LEN	= 64;		//	ģ������󳤶�
	static const int MAX_SQL_LEN			= 4096;		//	SQL��䳤��
	static const int MAX_SQL_SP_LEN			= 254;
	static const int MAX_TAG_NAME_LEN		= 32;		//	��ǩ����
	static const int MAX_IDEN_LEN			= 30;		//	��Ʒ���ű�ʶ���ݳ���
	static const int MAX_SIGN_LEN			= 10;		//	ǩ������
	static const int MAX_XMS_WM_LEN			= 32;		//	ˮӡ����
	static const int MAX_CONT_LEN			= 160;		//	��Ϣ������󳤶�
	static const int MAX_LONG_CONT_LEN		= 1024;		//	�����ţ���󳤶�
	static const int MAX_TOPICS_LEN			= 128;		//	�������󳤶�
	static const int MAX_CACHE_SIZE			= 65536;	//	��д�ļ������С
	static const int MAX_ERR_CODE			= 7;        //  ������볤��
	static const int MAX_SP_LEN				= 21;		//	��ַ��󳤶�

	// This class is exported from the ClubCommon.dll
	class CLUBCOMMON_API CFuncCommon {
	public:
		CFuncCommon(void);
		~CFuncCommon(void);

	public:
		/*
			���ܺ��������ݿ��������ת��
			Input: src
			Ouput: dst
		*/
		bool do_encode_pwd(const char	*src,
						  char			*dst);

		/*
			���ܺ��������ݿ��������ת��
			Input: src
			Ouput: dst
		*/
		bool do_decode_pwd(const char	*src,
						   char			*dst);

		/*
			��ȡ��������
		*/
		unsigned char get_net_type(__int64 mobile);

		/*
			�ֻ��������
			input:	mobile	64λ����(�ƶ����룬С��ͨ�������ת����SP���벻��ת��)
			output	msisdn	����������,ֱ�����ã���������Ϊ���ַ���
		*/
		const char* do_encode_isdn(__int64		mobile,
								   char			*msisdn,
								   const char	*inter_code = "86");

		/*
			�ֻ��������
			input:	msisdn	�ַ���
			output:	mobile, 
					net_type��������{0-SP_Num;1-GSM(unicom);2-GSM(mobile);3-CDMA;4-PHS;5-3G;6-Others}

		*/
		__int64 do_decode_isdn(const char		*msisdn,
							   unsigned char	&net_type);

		/*
			���ļ�·���ط����·����
		*/
		char* get_file_path_from_name(const char	*file_name,
									  char			*file_path);

		/*
			���ļ�·���ط�����ļ���
		*/
		char* get_file_name_from_path(const char	*file_path,
									  char			*file_name);

		/*
			���ָ��·���Ƿ����
		*/
		bool get_dir_is_exist(const char *dir);

		/*
			�����༶Ŀ¼
		*/
		bool set_mkdir_ex(const char *dir);	

		/*
			ȡ�÷���������ڵ�·��
		*/
		bool get_service_path(const char *serv_name, char *serv_path);

		/*
			����Ƿ����������֣��绰���룩�������ų���������
		*/
		bool has_tel_num(const char *cont, int tel_nums);

		/*
			�������Ƿ�ΪASCII����
		*/
		bool is_ascii(const char *sms_cont);

		/*
			��ȡָ�����ȵĲ�������
			Input:	cont		ԭ��
					trunc_len	��ȡ����
			Output:	��ȷ��ȡ���ȣ���ֹ�������룬Ӣ����һ���ֽڣ�����2���ֽڣ�����
		*/
		int get_trunc_len(const char	*cont,
						  int			trunc_len);
	};

	class CLUBCOMMON_API CDBCommon {
	public:
		CDBCommon(CMYSQLDB *db, const char *db_name);
		~CDBCommon(void);
	
		/*
			�����¼�/�澯��¼��ͨ��Email/SMS֪ͨ����Ա��
			Input: level �澯����{0�¼���1��ͨ����澯��2��������澯}
				   subject  �澯����(�澯ģ��)
				   cont		�澯��ϸ����
		*/
		bool do_alarm(unsigned char	level,
					  const char	*subject,
					  const char	*cont);

		/*
			����ϵͳ����״̬
			input:	run_stat	����״̬
					pid_name	�����Ψһ��־
					subject		��������
					detail		����״̬����,
					
		*/
		bool do_rpt_run(unsigned char	run_stat,
						const char		*pid_name, 
						const char		*subject,
						const char		*detail);
	private:
		CMYSQLDB	*_db;
		char		_db_name[MAX_PATH];
	};

	//	����ͬ��״̬��
	class CLUBCOMMON_API CProcSync
	{
	public:
		CProcSync(CMYSQLDB *db);
		~CProcSync(void);

		/*
			���û����ϲ�״̬
			�����Ƿ�ɹ�
		*/
		bool do_set_status_cdr_decoder(void);

		/*	
			����Ƿ�Ϊ�����ϲ�״̬
			�����Ƿ�Ϊ��״̬
		*/
		bool do_check_status_cdr_decoder(void);

		/*
			���������ھ�״̬
			�����Ƿ�ɹ�
		*/
		bool do_set_status_cont_miller(void);

		/*	
			����Ƿ�Ϊ�����ھ�״̬
			�����Ƿ�Ϊ��״̬
		*/
		bool do_check_status_cont_miller(void);

		/*
			����ͬ������ID״̬
			�����Ƿ�ɹ�
		*/
		bool do_set_status_sms_id_sync(void);

		/*	
			����Ƿ�Ϊ����IDͬ��״̬
			�����Ƿ�Ϊ��״̬
		*/
		bool do_check_status_sms_id_sync(void);

		/*
			��������ʶ��״̬
			�����Ƿ�ɹ�
		*/
		bool do_set_status_cont_identifier(void);

		/*	
			����Ƿ�Ϊ����ʶ��״̬
			�����Ƿ�Ϊ��״̬
		*/
		bool do_check_status_cont_identifier(void);

		/*
			���û��ּ���״̬
			�����Ƿ�ɹ�
		*/
		bool do_set_status_score_proc(void);

		/*	
			����Ƿ�Ϊ���ּ���״̬
			�����Ƿ�Ϊ��״̬
		*/
		bool do_check_status_score_proc(void);

		/*
			��������״̬
			�����Ƿ�ɹ�
		*/
		bool do_set_status_normal(void);

		/*	
			����Ƿ�Ϊ����״̬
			�����Ƿ�Ϊ��״̬
		*/
		bool do_check_status_normal(void);

		/*
			�����쳣��ֹ״̬
		*/
		bool do_set_status_abend(void);

	private:
		CMYSQLDB	*_db;
	};

	//	�Ŷι���
	class CLUBCOMMON_API CSegment
	{
	public:
#pragma pack (push, 1)
		struct SSegment
		{
			unsigned long	_seg_id;					//	��ID
			unsigned long	_segment;					//	��Ŷ�
			unsigned short	_area_id;					//	��������
			unsigned char	_site_id;					//	վ��(site_id�ٱ�վΪmt_out)
			unsigned char	_seg_net;					//	����(1-GSM/2-CDMA/3-PHS/4-3G)
		};

		//	Key: segment OR seg_id
		typedef std::hash_map<unsigned long, SSegment>	MAP_Segment;
#pragma pack (pop, 1)
	public:
		CSegment(CMYSQLDB *db);
		~CSegment(void);
		
		/*
			���غŶ���Ϣ
		*/
		bool load(void);

		/*
			���ҺŶ���Ϣ: By mobile
			����NULL��ʾδ�ҵ������򷵻����š�վ�㡢������Ϣ
		*/
		SSegment* do_find_by_mobile(__int64 mobile);

		/*
			���һ�ԱID
			����0��ʾδ�ҵ��������ԱID
		*/
		unsigned long do_get_mbr_id(__int64 mobile);
		unsigned long do_get_mbr_id(__int64 mobile, unsigned short &area_id);

		/*
			���ҺŶ���Ϣ: By mbr_id
			����NULL��ʾδ�ҵ������򷵻����š�վ�㡢������Ϣ
		*/
		SSegment* do_find_by_mbr_id(unsigned long mbr_id);

		/*
			�����ֻ�����
			����0��ʾδ�ҵ��������ֻ�����
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