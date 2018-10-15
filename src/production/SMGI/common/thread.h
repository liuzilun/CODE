#pragma once

#if defined(WIN32)
	#include <windows.h>
	#include <process.h>
	typedef	HANDLE		thread_t;
#elif (defined(__FreeBSD__) || defined(__LINUX__))
	#include <unistd.h>
	#include <pthread.h>
	typedef	pthread_t	thread_t;
#endif

#pragma warning(disable : 4103)
#pragma pack(push, 1)
namespace Process
{
	//
	// CThread
	// This class suports WINDOW/FreeBSD/LINUX operating system.
	// This class creates a worker thread that waits on kernel
	// object handles and executes a specified client
	// function when the handle is signaled.
	// To run the thread, call BeginThread
	// To end the thread, call EndThread
	// To suspend the thread, call Suspend
	// To Resume the thread, call Resume
	//

	class CThread
	{
	public:
		CThread(void);
		virtual ~CThread(void);

		void BeginThread(void);
		void EndThread(unsigned long timeout = 5);

		void Suspend(void);
		void Resume(void);

		unsigned int GetThreadID(void) {
			return threadid_;
		}

		thread_t GetHandle(void) {
			return handle_;
		}
	protected:
		static unsigned int __stdcall InvokeThreadEntry(void* p);
		virtual unsigned int ThreadEntry(void) = 0;
	
		bool			terminated_; 	//	Is the thread terminated
		bool			running_;		// Is the thread running	
	private:
		thread_t		handle_;
		unsigned int	threadid_;		
	};

	inline CThread::CThread() 
		: running_(false)
		, terminated_(false)
		, handle_(0)
		, threadid_(0)
	{
	}

	inline CThread::~CThread()
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

	inline void CThread::BeginThread(void)
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

	inline void CThread::EndThread(unsigned long timeout)
	{
		if (running_)
		{
			terminated_ = true;
			#ifdef WIN32
				if (WAIT_TIMEOUT == WaitForSingleObject(handle_, timeout * 1000))
				{
					TerminateThread(handle_, -1);
				}
			#else
				pthread_join(handle_, NULL);
			#endif
			running_ = false;
		}
	}

	inline void CThread::Suspend(void)
	{
		terminated_ = true;
		#ifdef WIN32
			SuspendThread(handle_);
		#else
			pthread_kill(handle_, SIGSTOP);
		#endif
	}

	inline void CThread::Resume(void)
	{
		terminated_ = false;
		#ifdef WIN32
			ResumeThread(handle_);
		#else
			pthread_kill(handle_, SIGCONT);
		#endif
	}

	inline unsigned int CThread::InvokeThreadEntry(void* p)
	{
		CThread *pWorkerThread = (CThread*)p;

		if (pWorkerThread)
			return pWorkerThread->ThreadEntry();
		return 0;
	}
}
#pragma pack(pop, 1)