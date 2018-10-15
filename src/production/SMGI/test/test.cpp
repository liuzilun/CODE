// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include "..\SMGI\SMGI.h"

class mysmgi 
	: public CSMGI
{
public:
	mysmgi(void) {} 
	~mysmgi(void) {}
protected:
	void OnSubmitResp(SUBMIT_RESP &resp)
	{
		printf("OnSubmitResp:msg_id=%I64u,seq_id=%lu\n", 
			resp._msg_id, resp._seq_id);
	}

	void OnDeliver(DELIVER_SM &deliver)
	{
		printf("OnDeliver:mobile=%s,sp_num=%s,cont=%s,link_id=%S\n", 
			deliver._mobile, deliver._sp_num,deliver._cont, deliver._link_id);
	}

	void OnReport(REPORT_SM &report)
	{
		printf("OnReport:mobile=%s,msg_id=%I64u,stat=%d,err_code=%s.\n", report._mobile, report._msg_id, report._stat, report._err_code);
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	{
		mysmgi x;
		CONNECT_INFO cn_info;

	#ifdef _SGIP_
		strcpy(cn_info._corp_id, "31008");
		cn_info._interval = 0;
		cn_info._links = 1;
		cn_info._src_id = 3075531008;
		cn_info._smg_type = 1;
		strcpy(cn_info._login_name, "zfyj");
		strcpy(cn_info._login_pswd, "zfyj");
		cn_info._seq_id = 1;
		cn_info._slip_size = 32;
		strcpy(cn_info._smg_ip, "10.123.10.1");
		cn_info._smg_port_mo = 9881;
		cn_info._smg_port_mt = 7890;
		cn_info._sp_port = 8801;
		strcpy(cn_info._sp_num, "430");
	#elif _SMPP_
		strcpy(cn_info._corp_id, "919559");
		cn_info._interval = 10;
		cn_info._links = 1;
		cn_info._smg_type = 43;
		strcpy(cn_info._login_name, "333");
		strcpy(cn_info._login_pswd, "0555");
		cn_info._seq_id = 1;
		cn_info._slip_size = 32;
		strcpy(cn_info._smg_ip, "127.0.0.1");
		cn_info._smg_port_mo = 2775;
		cn_info._smg_port_mt = 7890;
		cn_info._sp_port = 8802;
		strcpy(cn_info._sp_num, "2000");
	#elif _CMPP_
		//CMPP
		//连接网关
		strcpy(cn_info._corp_id, "919559");
		cn_info._interval = 10;
		cn_info._links = 3;
		cn_info._smg_type = 10;//华为
		strcpy(cn_info._login_name, "919559");
		strcpy(cn_info._login_pswd, "919559");
		cn_info._seq_id = 0;
		cn_info._slip_size = 32;
		strcpy(cn_info._smg_ip, "127.0.0.1");
		cn_info._smg_port_mo = 7891;
		cn_info._smg_port_mt = 7890;
		strcpy(cn_info._sp_num, "01850");
	#elif _CNGP_
		strcpy(cn_info._corp_id, "919559");
		cn_info._interval = 10;
		cn_info._links = 1;
		cn_info._smg_type = 30;
		strcpy(cn_info._login_name, "333");
		strcpy(cn_info._login_pswd, "0555");
		cn_info._seq_id = 1;
		cn_info._slip_size = 32;
		strcpy(cn_info._smg_ip, "127.0.0.1");
		cn_info._smg_port_mo = 9890;
		cn_info._smg_port_mt = 9890;
		cn_info._sp_port = 8802;
		strcpy(cn_info._sp_num, "2000");
	#elif _SMGP_
		strcpy(cn_info._corp_id, "919559");
		cn_info._interval = 10;
		cn_info._links = 1;
		cn_info._smg_type = 20;
		strcpy(cn_info._login_name, "333");
		strcpy(cn_info._login_pswd, "0555");
		cn_info._seq_id = 1;
		cn_info._slip_size = 32;
		strcpy(cn_info._smg_ip, "127.0.0.1");
		cn_info._smg_port_mo = 8890;
		cn_info._smg_port_mt = 9890;
		cn_info._sp_port = 8802;
		strcpy(cn_info._sp_num, "2000");
	#endif

		x._startup(cn_info);

		getch();

		SUBMIT_SM submit;
		time_t t;
		time(&t);
		tm* st = localtime(&t);
		printf("begin:%s\n", asctime(st));
		DWORD dwStart = GetTickCount();
		DWORD dwStop = 0;
		for( int i = 0; i < 5000; i++)
		{
			sprintf(submit._cont, "深圳荣迪SP测试50条/秒：%d", i);
			strcpy(submit._fee_type, "1");
			strcpy(submit._fee_value, "0");
			strcpy(submit._link_id, "");
			//strcpy(submit._sched_time, "200503261101");
			strcpy(submit._mobile, "8613312345678");
			submit._prio = 0;
			strcpy(submit._sp_num, "430");
			submit._report = 1;
			strcpy(submit._srv_type, "TZ");
			
			unsigned long sys_seq = x._submit(submit, false);
			//printf("sys_seq:%d\r\n",sys_seq);
			Sleep(10);
		}
		dwStop = GetTickCount();
		time(&t);
		st = localtime(&t);
		printf("end:%s\n", asctime(st));
		printf("Time is %d", dwStop - dwStart);
		getch();
	}
	
	getch();

	return 0;
}

