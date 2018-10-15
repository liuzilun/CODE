#ifndef SYNC_H_
#define SYNC_H_

#if defined(WIN32)
	#include <windows.h>
	typedef CRITICAL_SECTION	critical_section_t;
	#define initial_cs(x)		InitializeCriticalSection(x);
	#define destroy_cs(x)		DeleteCriticalSection(x);
	#define lock_cs(x)			EnterCriticalSection(x);
	#define unlock_cs(x)		LeaveCriticalSection(x);
#elif (defined(__FreeBSD__) || defined(__LINUX__))
	#include <unistd.h>
	#include <pthread.h>
	typedef pthread_mutex_t		critical_section_t;
	#define	initial_cs(x)		pthread_mutex_init(x);
	#define destroy_cs(x)		pthread_mutex_destroy;
	#define lock_cs(x)			pthread_mutex_lock;
	#define unlock_cs(x)		pthread_mutex_unlock;
#endif

namespace Process
{
	//
	// CCriticalSection
	// This class suports WINDOW/FreeBSD/LINUX operating system.
	// And it creates a criticalsection object, 
	// To use it, Lock function enter a criticalsection,
	// Unlock function leave a criticalsection.
	// When it destroys, it first Unlock.
	//

	class CCriticalSection
	{
	public:
		CCriticalSection() {
			initial_cs(&cs_);
		}

		~CCriticalSection() {
			destroy_cs(&cs_);
		}

		void Lock(void) {
			lock_cs(&cs_);
		}

		void Unlock(void) {
			unlock_cs(&cs_);
		}
	private:
		critical_section_t	cs_;
	};

	/*
	**	这是一个自动锁的实现. 在实际程序中, 很多地方都需要使用临界区来对线程同步.
	**	锁定和开锁是一个相关联的动作. 正确的做法是进入需要同步访问的区域是锁定, 
	**	离开时开锁. 如果该动作没有对应起来, 程序将出现不可预料的情况.
	**	但是, 如果程序中出口很多, 或者执行中跳出, 或者执行中发生异常等, 都很容易
	**	忽略开锁的动作. AutoLock能够在离开作用域的时候自动执行开锁的动作.
	*/

	class CAutoLock
	{
	public:
		CAutoLock(CCriticalSection *cs) 
			: _cs(cs)
		{
			_cs->Lock();
		}

		~CAutoLock(void)
		{
			_cs->Unlock();
		}
	private:
		CAutoLock() { }

		CCriticalSection	*_cs;
	};
}

#endif