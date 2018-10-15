#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MSISDN_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MSISDN_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MSISDN_EXPORTS
#define MSISDN_API __declspec(dllexport)
#else
#define MSISDN_API __declspec(dllimport)
#endif

// This class is exported from the MSISDN.dll
enum MSISDN_API EUSER_TYPE
{
	USER_TYPE_UNICOM_GSM	= 0,				//	��ͨG��
	USER_TYPE_UNICOM_CDMA	= 1,				//	��ͨC��
	USER_TYPE_MOBILE_GSM	= 2,				//	�ƶ�G��
	USER_TYPE_TELECOM_PHS	= 3,				//	����С��ͨ
	USER_TYPE_SP			= 4,				//	SP
	USER_TYPE_OTHER			= 5					//	����					
};

class MSISDN_API CMSIsdn
{
public:
	CMSIsdn(void);
	~CMSIsdn(void);

	//	input the number saved in db, output the format string(include china code).
	const char* encode(char	*msisdn,
					   unsigned long number,
					   unsigned short city_id,
					   bool have_china_code);

	//	msisdn convert to mobile stored into db, and output mobile type
	unsigned long decode(const char		*msisdn, 
						 unsigned short &city_id,
						 unsigned char	&type);

	//	С��ͨ���룺����+����
	//	�������Ż��֣�01��02��ͷ��3λ����010��020��021
	//	��������4λ����0755
	unsigned long decode_phs(const char		*msisdn,
							 unsigned short &city_id);
	unsigned char get_type(unsigned long mobile);
};
