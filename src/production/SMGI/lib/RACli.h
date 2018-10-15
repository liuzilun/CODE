#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the RACLI_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// RACLI_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef RACLI_EXPORTS
#define RACLI_API __declspec(dllexport)
#else
#define RACLI_API __declspec(dllimport)
#endif

#pragma comment(lib, "rpcrt4.lib")

// This class is exported from the RACli.dll
extern "C"  
{
	RACLI_API bool k_Startup(const char	*lpProt = "ncacn_ip_tcp",
							 const char	*lpAddr = "127.0.0.1",
							 const char	*lpPort = "8181");
	RACLI_API bool k_Cleanup(void);
    RACLI_API HRESULT k_Switch(UINT		nMode,
							   HRESULT	&hRet);
}
