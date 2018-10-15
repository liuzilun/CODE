#include "WorkThread.h"

using namespace Process;

CWorkThread::CWorkThread ()
	: working_(false)
	, stopped_(false)
	, handle_(0)
	, threadid_(0)
{
}

CWorkThread::~CWorkThread () 
{
	if (working_)
		Stop();
		
	if (0 != handle_)
	{
		#ifdef WIN32
			CloseHandle(handle_);
		#endif
	}
}

void CWorkThread::Start(void)
{
	if (working_)
		return;

	stopped_ = false;

	#ifdef WIN32
		handle_ = (void*)_beginthreadex(NULL, 0, InvokeThreadEntry, this, 0, &threadid_);
	#else
		pthread_create(&handle_, NULL, InvokeThreadEntry, this);
		threadid_ = handle_;
	#endif
	
	if (0 != handle_)
		working_ = true;
}

void CWorkThread::Stop(unsigned long timeout)
{
	if (working_)
	{
		stopped_ = true;
		#ifdef WIN32
			if (WAIT_TIMEOUT == WaitForSingleObject(handle_, timeout * 1000))
			{
				TerminateThread(handle_, -1);
			}
		#else
			pthread_join(handle_, NULL);
		#endif
		working_ = false;
	}
}

void CWorkThread::Suspend(void)
{
	stopped_ = true;
	#ifdef WIN32
		SuspendThread(handle_);
	#else
		pthread_kill(handle_, SIGSTOP);
	#endif
}

void CWorkThread::Resume(void)
{
	stopped_ = false;
	#ifdef WIN32
		ResumeThread(handle_);
	#else
		pthread_kill(handle_, SIGCONT);
	#endif
}

unsigned int CWorkThread::InvokeThreadEntry(void* p)
{
	CWorkThread *pWorkerThread = (CWorkThread*)p;

	if (pWorkerThread)
		return pWorkerThread->Execute();
	return 0;
}