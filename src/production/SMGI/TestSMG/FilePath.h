#include <string>
#include <winsvc.h>

#pragma once

#ifndef FILE_PATH_H
#define FILE_PATH_H

using namespace std;

//	���ļ�·���ط����·����
static string ExtractFilePath(string &FileName)
{
	string::size_type pos = 0;
	string ret;
	pos = FileName.find_last_of("\\", FileName.size());
	ret = FileName.substr(0, pos);
	return ret;
}

//	���ļ�·���ط�����ļ���
static string ExtractFileName(string &FileName)
{
	string::size_type pos = 0;
	string ret;
	pos = FileName.find_last_of("\\", FileName.size());
	ret = FileName.substr(pos + 1,FileName.size() - pos);
	return ret;
}

//	���ָ��·���Ƿ����
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

//	�����༶Ŀ¼
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
					//	����Ŀ¼ʧ��
					break;
				}
			}
		}
		off = pos + 1;
		pos = DirPath.find("\\", off);
	}
    //	�������һ��Ŀ¼
	if(ret && !ExistDir(DirPath.c_str()))
	{
		ret =  CreateDirectory(DirPath.c_str(), NULL) == TRUE;
	}
	return ret;
}

//	ȡ�÷���������ڵ�·��
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