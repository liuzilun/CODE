#include <stdio.h>
#include <tchar.h>
#include <string>
#include <conio.h>
#include "IniCfg.h"
#include "FilePath.h"
#include "..\Lib\Log.h"
#include "MySMGI.h"

CIniCfg		g_ini;
rpt::log	g_log("TestSMG");

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

	CreateDirectory(g_ini.get()->_log_dir, NULL);
	CreateDirectory(g_ini.get()->_data_dir, NULL);
	
	g_log.add(new rpt::journal(rpt::debug, 
							   std::string(g_ini.get()->_log_dir) + "\\TestSMG", 
							   new rpt::simplelayout("%t|%s|%e|%m")));

	if (argc == 2) 
	{
		if ((strncmp(argv[1], "/?", 2) == 0)
			|| (strncmp(argv[1], "-?", 2) == 0))
		{
			printf("Test SMG Interface.\n");
			printf("Copyright (C) 2004-2007 深圳市荣迪信息技术有限公司.All rights reserved.\n");
			printf("For China Mobile/Unicom/TeleCom use only.\n");
			printf("Usage: TestSMG [options]\n\n");
			printf("	-D		Debug the service.\n");
			printf("	-M		Keep the maintenance of SMGI.\n");
			printf("	-T		Test to send a message.\n");

			return 0;
		}
		else if (strncmp(argv[1], "-D", 2) == 0)
		{
			CTestSMGI x;
			printf("Press any key to end...\n");
			getch();
			printf("Stopping...\n");
			return 0;
		}
		else if (strncmp(argv[1], "-T", 2) == 0)
		{
			CTestSMGI x;
			printf("Press any key to send sm.\n");
			getch();
			
			printf("Press any key to end...\n");
			getch();
			printf("Stopping...\n");
			return 0;
		}
	}

	return 0;
}