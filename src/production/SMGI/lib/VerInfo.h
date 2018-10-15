/*
* Copyright (c) 2001гн2003
* All rights reserved.
*
* VerInfo.h
* interface for the VerInfo class.
* 
* Version:	1.0
* Author:	Christ
* Time:		2003/05/07
*
*/

#pragma once
#pragma comment (lib, "version.lib")

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the VERINFOS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// VERINFOS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef VERINFOS_EXPORTS
#define VERINFOS_API __declspec(dllexport)
#else
#define VERINFOS_API __declspec(dllimport)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

namespace Process {
	class VERINFOS_API CVerInfo {
	public:
		CVerInfo(LPCTSTR imageName);
		~CVerInfo();

		char* ProductName(char* ptr) const;
		char* ProductVersion(char* ptr) const;
		char* CompanyName(char* ptr) const;
		char* Copyright(char* ptr) const;
		char* Comments(char* ptr) const;
		char* FileDescription(char* ptr) const;
		char* FileVersion(char* ptr) const;
		char* InternalName(char* ptr) const;
		char* LegalTrademarks(char* ptr) const;
		char* PrivateBuild(char* ptr) const;
		char* SpecialBuild(char* ptr) const;
	private: 
		void InitVer(LPCTSTR imageName);
		char* GetValue(LPCTSTR key, char* value) const;

		bool m_Existed;

		LPVOID m_ResourceData;
		char m_LangID[MAX_PATH];
	};
}