#include <stdio.h>
#include <tchar.h>
#include <string>
#include <conio.h>
#include "..\Lib\log.h"
#include "..\Lib\Timer.h"
#include "..\Lib\Fasparse.h"
#include "IniCfg.h"
#include "MyService.h"
#include "FilePath.h"

rpt::log			g_log("SMGI");
Process::CTimer		g_timer;
CIniCfg				g_ini;
FASPARSE			g_fasparse;

int _tmain(int argc, _TCHAR* argv[])
{
	char svc_path[MAX_PATH]	= {0};
	char file_name[MAX_PATH]= {0};

	//	取程序路径
	GetModuleFileName(NULL, svc_path, sizeof(svc_path));
	strncpy(svc_path, ExtractFilePath(string(svc_path)).c_str(), MAX_PATH);		

	//	打开配置文件
	sprintf(file_name, "%s\\SMGI.ini", svc_path);
	g_ini.set_file_name(file_name);

	CreateDirectory(g_ini.get_cfg()->_log_dir, NULL);
	CreateDirectory(g_ini.get_cfg()->_data_dir, NULL);
	
	g_log.add(new rpt::journal(rpt::debug, 
							   std::string(g_ini.get_cfg()->_log_dir) + "\\SMGI", 
							   new rpt::simplelayout("%t|%s|%e|%m")));

	if (argc == 2) 
	{
		if ((strncmp(argv[1], "/?", 2) == 0)
			|| (strncmp(argv[1], "-?", 2) == 0))
		{
			printf("SMG Interface of NextPower VMS Platform(V3.0.7.15)\n");
			printf("Copyright (C) 2004-2006 深圳市荣迪信息技术有限公司.All rights reserved.\n");
			printf("For ShenZhen Unicom use only.\n");
			printf("Usage: P2PSMPSI [options]\n\n");
			printf("	-i		Install the service. \n");
			printf("	-u		Uninstall the service.\n");
			printf("	-s		Start the service.\n");
			printf("	-e		End a currently running service.\n");
			printf("	-d		Debug the service.\n");
			printf("	-f		Start service as a faceless application(win95/98 mode).\n");
			printf("	-M		Keep the maintenance of SMGI.\n");
			printf("	-P[Idx]	Test push capacity of the idx SMGI(eg:P2PSMGI -P1).\n");

			return 0;
		}
		else if (strncmp(argv[1], "-M", 2) == 0)
		{
			//CPauseSMGI x;
			printf("Press any key to end...\n");
			getch();
			printf("Stopping...\n");
			return true;
		}
		else if (strncmp(argv[1], "-P", 2) == 0)
		{
			//CTestCapSMGI x;
		}
	}
	
	CMyService x;
	return x.RegisterService(argc, argv);
}