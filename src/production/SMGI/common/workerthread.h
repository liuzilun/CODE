#ifndef WORKERTHREAD_H_
#define WORKERTHREAD_H_

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
	// CWorkerThread
	// This class suports WINDOW/FreeBSD/LINUX operating system.
	// This class creates a worker thread that waits on kernel
	// object handles and executes a specified client
	// function when the handle is signaled.
	// To run the thread, call BeginThread
	// To end the thread, call EndThread
	// To suspend the thread, call Suspend
	// To Resume the thread, call Resume
	//

	class CWorkerThread
	{
	public:
		CWorkerThread(void);
		virtual ~CWorkerThread(void);

		void BeginThread(void);
		void EndThread(void);

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
}
#pragma pack(pop, 1)

#endif
