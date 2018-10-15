#include <stdio.h>
#include <tchar.h>
#include <string>
#include <conio.h>
#include "..\Lib\log.h"
#include "..\Lib\Timer.h"
#include "..\Lib\Fasparse.h"
#include "..\Lib\ClubCommon.h"
#include "IniCfg.h"
#include "MyService.h"

Process::log		g_log("SMGI");
Process::CTimer		g_timer;
CIniCfg				g_ini;
Process::CFasParse	g_fasparse;

int _tmain(int argc, _TCHAR* argv[])
{
	//	取程序路径
	char svc_path[MAX_PATH]	= {0};
	GetModuleFileName(NULL, svc_path, sizeof(svc_path));
	
	char file_path[MAX_PATH] = {0};
	RdiClubCommon::CFuncCommon common;
	common.get_file_path_from_name(svc_path, file_path);	

	//	打开配置文件
	char file_name[MAX_PATH]= {0};
	sprintf(file_name, "%s\\SMGI.ini", file_path);
	g_ini.set_file_name(file_name);

	CreateDirectory(g_ini.get()->_log_dir, NULL);
	CreateDirectory(g_ini.get()->_data_dir, NULL);
	
	g_log.add(new Process::journal(Process::debug, 
							   std::string(g_ini.get()->_log_dir) + "\\SMGI", 
							   new Process::simplelayout("%t|%s|%e|%m")));

	if (argc == 2) 
	{
		if ((strncmp(argv[1], "/?", 2) == 0)
			|| (strncmp(argv[1], "-?", 2) == 0))
		{
			printf("SMG Interface of NextPower VMS Platform(V4.0.8.9)\n");
			printf("Copyright (C) 2004-2007 深圳市荣迪信息技术有限公司.All rights reserved.\n");
			printf("For ShenZhen Unicom use only.\n");
			printf("Usage: P2PSMPSI [options]\n\n");
			printf("	-i		Install the service. \n");
			printf("	-u		Uninstall the service.\n");
			printf("	-s		Start the service.\n");
			printf("	-e		End a currently running service.\n");
			printf("	-d		Debug the service.\n");
			printf("	-f		Start service as a faceless application(win95/98 mode).\n");

			return 0;
		}
	}
	
	CMyService x;
	return x.RegisterService(argc, argv);
}