#include <string>
#include <winsvc.h>

#pragma once

#ifndef FILE_PATH_H
#define FILE_PATH_H

using namespace std;

//	从文件路径重分离出路径名
static string ExtractFilePath(string &FileName)
{
	string::size_type pos = 0;
	string ret;
	pos = FileName.find_last_of("\\", FileName.size());
	ret = FileName.substr(0, pos);
	return ret;
}

//	从文件路径重分离出文件名
static string ExtractFileName(string &FileName)
{
	string::size_type pos = 0;
	string ret;
	pos = FileName.find_last_of("\\", FileName.size());
	ret = FileName.substr(pos + 1,FileName.size() - pos);
	return ret;
}

//	检查指定路径是否存在
static bool ExistDir(const char *Dir)
{
    WIN32_FIND_DATA fd; 
	bool ret = false;

    HANDLE hFind = FindFirstFile(Dir, &fd); 
    if ((hFind != INVALID_HANDLE_VALUE) && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
    { 
        ret = true;
    } 
    FindClose(hFind); 

	return ret;
}

//	创建多级目录
static bool MkDirEx(const char *Dir)
{
	string DirPath(Dir);
	string::size_type pos = 0, off = 0;
	string SubDir;
	bool ret = true;

	pos = DirPath.find("\\", off);

	while(pos != string::npos)
	{
		SubDir = DirPath.substr(0, pos);
	
		if(SubDir.length() >= 3)
		{
			if(!ExistDir(SubDir.c_str()))
			{					
				ret =  CreateDirectory(SubDir.c_str(), NULL) == TRUE;
				if(!ret)
				{
					//	创建目录失败
					break;
				}
			}
		}
		off = pos + 1;
		pos = DirPath.find("\\", off);
	}
    //	创建最后一级目录
	if(ret && !ExistDir(DirPath.c_str()))
	{
		ret =  CreateDirectory(DirPath.c_str(), NULL) == TRUE;
	}
	return ret;
}

//	取得服务程序所在的路径
static bool GetServicePath(const char *ServiceName, char* ServicePath)
{
	LPQUERY_SERVICE_CONFIG lpServiceCfg = NULL;
	DWORD dwBytesNeeded = 0;

	SC_HANDLE schSCManager = NULL;
	SC_HANDLE schService  = NULL;

	schSCManager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);

	if(schSCManager == NULL)
	{
		return false;
	}
	schService = OpenService( 
		schSCManager,           // SCManager database 
		ServiceName,            // name of service 
		SERVICE_QUERY_CONFIG);  // need QUERY access 

	if (schService == NULL) 
	{
		CloseServiceHandle(schSCManager);
		int x= GetLastError();
		return false;
	}

	// Allocate a buffer for the configuration information. 
	lpServiceCfg = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LPTR, 4096); 
	if (lpServiceCfg == NULL) 
	{
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);
		return false;
	}

	// Get the configuration information. 

	if (! QueryServiceConfig( 
		schService, 
		lpServiceCfg, 
		4096, 
		&dwBytesNeeded) ) 
	{
		CloseServiceHandle(schSCManager);
		CloseServiceHandle(schService);
		LocalFree(lpServiceCfg);
		return false;
	}

	strcpy(ServicePath, lpServiceCfg->lpBinaryPathName);
	CloseServiceHandle(schSCManager);
	CloseServiceHandle(schService);
	LocalFree(lpServiceCfg);

	return true;
}

#endif