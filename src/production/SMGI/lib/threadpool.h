// ============================================================================
// FILE: threadpool.h
//
//	Definition the interface of the thread pool.
//
//	On many occasions we need to utilize mutiple threads to boost the system 
//	performance. The logic for each thread is almost the same, but we need to 
//	manage these threads. If the system is busy, we create more threads, 
//	otherwise we kill some thread to avoid the extra overhead. 
//	
//	This class can dynamically allocate threads and assign jobs to these worker 
//	threads. You derive your own classes and you do not need to know the 
//	underlying machanism to handle the mutiple threading and synchronization 
//	between these threads. However, you need to make your worker classes thread 
//	safe since your objects may be assigned to different threads each time. 
//
// Requirements:
//	using "Multi-Threaded DLL" or "Multi-Threaded Debug DLL" if in Debug
//
// Author:
//	Chris
//
// Revision History:
//	9 Oct 2002: first release
//	30 Mar 2006: second release
//	
// Samples:
//	see following:
//
//	class myJob : public Process::CJob {
//	public:
//		myJob() : i(0) {}
//		~myJob() { 
//			delete i; 
//			i = 0; 
//		}
//
//		int* i;
//	};
//
//	class myWorker : public Process::CWorker {
//	protected:
//		void do_execute(process::job* p) {
//			myJob* j = static_cast<myJob* >(p);
//			printf("%d %d\n", *(j->i), ::GetCurrentThreadId());
//			::Sleep(10);
//		}
//	};
//
//	Process::threadpool x(100);
//	// wait thread pool creatation
//	Sleep(10000);
//	for(int i = 0; i < 1000; ++i) {
//		myWorker* w = new myWorker;
//		myJob* j = new myJob;
//		j->i = new int(i);
//		x.push_back(w, j);
//	}
//
//	printf("size: %d\n", x.size());
//  //	x.grow(8);
//	printf("size: %d\n", x.size());
//	Sleep(1000);
//	x.clear();
//	printf("size: %d\n", x.size());
#pragma once

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the THREADPOOLS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// THREADPOOLS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef THREADPOOLS_EXPORTS
#define THREADPOOLS_API __declspec(dllexport)
#else
#define THREADPOOLS_API __declspec(dllimport)
#endif

// if under other platform than windows, comment this
#define	OS_WIN

namespace Process
{
	/** the worker thread's parameter
	 * implementor must derivered from this class
	 */
	class THREADPOOLS_API CJob
	{
	public:
		CJob(void)			{}
		virtual ~CJob(void)	{}
	};

	/// the worker when thread start to run
	class THREADPOOLS_API CWorker
	{
	public:
		CWorker(void)			{}
		virtual ~CWorker(void)	{}

		void execute(CJob* p) { do_execute(p); }
	protected:
		virtual void do_execute(CJob* p) throw() = 0;
	};

	class CThreadPoolImpl;
	/// the interface of the thread pool
	class THREADPOOLS_API CThreadPool
	{
	public:
		/// construct the threadpool object and create thread pool
		CThreadPool(int count = 0)  throw();
		/// destruct the threadpool and shutdown all of the thread in pool
		~CThreadPool(void) throw();

		/** 
		 * queue the work executed later
		 *	@param	w	a class that is responsible for handling requests
		 *				queued on the thread pool. it must devired from
		 *				the base class worker which hava a method execute().
		 *	@param	j	the worker's parameter
		 *	@see	class CWorker
		 */
		bool push_back(CWorker* w, CJob* j) throw();
		/**
		 * change the number of the threads on the pool
		 *	@param	number	the number of the threads on the pool<br>
		 *					if < 0, the threads number per proc<br>
		 *					if = 0, the default value
		 */
		bool grow(int number) throw();
		/// shutdown all of the threads on the pool
		bool clear(unsigned int timeout = 0) throw();
		/// get the number of the threads on the pool
		size_t size(void) const throw();
	private:
		CThreadPoolImpl	*impl_;
	};
}