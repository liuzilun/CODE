#pragma once

#include <list>
#include <hash_map>
#pragma warning (disable : 4275)
#include "..\SMGI\SMGI.h"
#include "..\SMGI\SMGDefine.h"

class CTestSMGI
	: public CSMGI
{
public:
	CTestSMGI(void);
	~CTestSMGI(void);
protected:
	void OnSubmitResp(SUBMIT_RESP &resp);
	void OnDeliver(DELIVER_SM &deliver);
	void OnReport(REPORT_SM &report);
};