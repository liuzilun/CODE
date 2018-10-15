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
	**	����һ���Զ�����ʵ��. ��ʵ�ʳ�����, �ܶ�ط�����Ҫʹ���ٽ��������߳�ͬ��.
	**	�����Ϳ�����һ��������Ķ���. ��ȷ�������ǽ�����Ҫͬ�����ʵ�����������, 
	**	�뿪ʱ����. ����ö���û�ж�Ӧ����, ���򽫳��ֲ���Ԥ�ϵ����.
	**	����, ��������г��ںܶ�, ����ִ��������, ����ִ���з����쳣��, ��������
	**	���Կ����Ķ���. AutoLock�ܹ����뿪�������ʱ���Զ�ִ�п����Ķ���.
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