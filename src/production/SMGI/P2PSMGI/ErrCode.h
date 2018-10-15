#pragma once

//	SGIP Report错误代码表

//	英斯克网关
enum ESGIP_INTRINT_ERR_CODE
{
	SGIP_INTRINT_ERR_CODE_0		= 0,		//	发送成功
	SGIP_INTRINT_ERR_CODE_2		= 2,		//	超时，没有下发
	SGIP_INTRINT_ERR_CODE_3		= 3,		//	限制呼叫
	SGIP_INTRINT_ERR_CODE_5		= 5,		//	未知用户
	SGIP_INTRINT_ERR_CODE_254	= 254,		//	没有短信功能
	SGIP_INTRINT_ERR_CODE_4003	= 163,		//	收到错误响应，这条MT业务终止
	SGIP_INTRINT_ERR_CODE_4005	= 165,		//	Report HASH表中，原记录被新记录覆盖，导致该状态报告无法核销，这条MT业务终止。
	SGIP_INTRINT_ERR_CODE_4007	= 167		//	该SMSC的发送队列已满，无法接收MT请求，这条MT业务终止。
};

enum ESGIP_OPENET_ERR_CODE
{
	SGIP_OPENET_ERR_CODE_1		= 1,		//	非法用户
	SGIP_OPENET_ERR_CODE_5		= 5,		//	用户鉴权失败，临时错误（MT，打电话提示暂时不能接通）
	SGIP_OPENET_ERR_CODE_9		= 9,		//	主叫手机未通过短信中心的鉴权（在ORATBL中无此号段或号码）
	SGIP_OPENET_ERR_CODE_11		= 11,		//	移动设备鉴权失败（手机与网络配合问题，较少）
	SGIP_OPENET_ERR_CODE_12		= 12,		//	移动设备鉴权失败（手机与网络配合问题，较少）
	SGIP_OPENET_ERR_CODE_13		= 13,		//	呼叫受限
	SGIP_OPENET_ERR_CODE_15		= 15,		//	闭合用户
	SGIP_OPENET_ERR_CODE_21		= 21,		//	漫游的网络不支持短消息服务
	SGIP_OPENET_ERR_CODE_27		= 27,		//	用户缺席（关机或不在服务区）
	SGIP_OPENET_ERR_CODE_31		= 31,		//	用户忙
	SGIP_OPENET_ERR_CODE_33		= 33,		//	HLR等待列表满（关机或不在服务区）
	SGIP_OPENET_ERR_CODE_34		= 34,		//	不可知错误	
	SGIP_OPENET_ERR_CODE_35		= 35,		//	数据缺失
	SGIP_OPENET_ERR_CODE_36		= 36,		//	非法的数据值
	SGIP_OPENET_ERR_CODE_50		= 50,		//	用户手机存储空间满 
	SGIP_OPENET_ERR_CODE_51		= 51,		//	设备协议错误（手机与基站的配合问题，随机出现）
	SGIP_OPENET_ERR_CODE_53		= 53,		//	不可知的短消息中心（MO，短信中心号码设错）
	SGIP_OPENET_ERR_CODE_54		= 54,		//	短信中心拥塞 
	SGIP_OPENET_ERR_CODE_55		= 55,		//	非法的SME地址
	SGIP_OPENET_ERR_CODE_56		= 56,		//	用户未在短信中心开户（MO失败）
	SGIP_OPENET_ERR_CODE_60		= 60,		//	短消息超长（MO、MT） 
	SGIP_OPENET_ERR_CODE_172	= 172,		//	网络无响应
	SGIP_OPENET_ERR_CODE_93		= 93,		//	后付费用户鉴权失败
	SGIP_OPENET_ERR_CODE_94		= 94		//	预付费用户扣费失败
};