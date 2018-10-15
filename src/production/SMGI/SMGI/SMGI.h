#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SMGI_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SMGI_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SMGI_EXPORTS
#define SMGI_API __declspec(dllexport)
#else
#define SMGI_API __declspec(dllimport)
#endif

#ifdef _SGIP_
	#include "..\SGIP\SGIP.h"
#elif _CMPP_
	#include "..\CMPP\CMPP.h"
#elif _SMPP_
	#include "..\SMPP\SMPP.h"
#elif _SMGP_
	#include "..\SMGP\SMGP.h"
#elif _CNGP_
	#include "..\CNGP\CNGP.h"
#else
	#include "..\SGIP\SGIP.h"
#endif

#include <windows.h>
#include "SMGDefine.h"

// This class is exported from the SMGI.dll
class SMGI_API CSMGI
	#ifdef _SGIP_
		: public CSgip
	#elif _CMPP_	
		: public CCmpp
	#elif _SMPP_
		: public CSmpp
	#elif _SMGP_
		: public CSmgp
	#elif _CNGP_
		: public CCngp
	#else
		: public CSgip
	#endif
{
public:
	CSMGI(void)
	{
	}

	~CSMGI(void)
	{
	}

	bool _startup(CONNECT_INFO &info)
	{
		return Startup(info);
	}

	unsigned long _submit(SUBMIT_SM &submit, bool prio)
	{
		return Submit(submit, prio);
	}

	unsigned long _get_seq_id(void) const
	{
		return GetSeqID();
	}
protected:
	virtual void OnSubmitResp(SUBMIT_RESP &resp) = 0;
	virtual void OnDeliver(DELIVER_SM &deliver) = 0;
	virtual void OnReport(REPORT_SM &report) = 0;
};