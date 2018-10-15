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
	// CWorkThread
	// This class suports WINDOW/FreeBSD/LINUX operating system.
	// This class creates a worker thread that waits on kernel
	// object handles and executes a specified client
	// function when the handle is signaled.
	// To run the thread, call BeginThread
	// To end the thread, call EndThread
	// To suspend the thread, call Suspend
	// To Resume the thread, call Resume
	//
	class CWorkThread
	{
	public:
		CWorkThread ();
		virtual ~CWorkThread ();

		void Start();
		void Stop(unsigned long timeout = 5);

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
		virtual unsigned int Execute(void) = 0;
	
		bool			stopped_; 	//	Is the thread terminated
		bool			working_;	// Is the thread working	

	private:
		thread_t		handle_;
		unsigned int	threadid_;
	};           
}
