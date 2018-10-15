#include "StdAfx.h"
#include "workerthread.h"


CWorkerThread::CWorkerThread() 
	: running_(false)
	, terminated_(false)
	, handle_(0)
	, threadid_(0)
{
}

CWorkerThread::~CWorkerThread()
{
	if (running_)
		EndThread();
	
	if (0 != handle_)
	{
		#ifdef WIN32
			CloseHandle(handle_);
		#endif
	}
}

void CWorkerThread::BeginThread(void)
{
	if (running_)
		return;

	terminated_ = false;

	#ifdef WIN32
		handle_ = (void*)_beginthreadex(NULL, 0, InvokeThreadEntry, this, 0, &threadid_);
	#else
		pthread_create(&handle_, NULL, InvokeThreadEntry, this);
		threadid_ = handle_;
	#endif
	
	if (0 != handle_)
		running_ = true;
}

void CWorkerThread::EndThread(void)
{
	if (running_)
	{
		terminated_ = true;
		#ifdef WIN32
			WaitForSingleObject(handle_, INFINITE);
		#else
			pthread_join(handle_, NULL);
		#endif
		running_ = false;
	}
}

void CWorkerThread::Suspend(void)
{
	terminated_ = true;
	#ifdef WIN32
		SuspendThread(handle_);
	#else
		pthread_kill(handle_, SIGSTOP);
	#endif
}

void CWorkerThread::Resume(void)
{
	terminated_ = false;
	#ifdef WIN32
		ResumeThread(handle_);
	#else
		pthread_kill(handle_, SIGCONT);
	#endif
}

unsigned int CWorkerThread::InvokeThreadEntry(void* p)
{
	CWorkerThread *pWorkerThread = (CWorkerThread*)p;

	if (pWorkerThread)
		return pWorkerThread->ThreadEntry();
	return 0;
}