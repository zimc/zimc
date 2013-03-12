#ifndef __Lock_H__
#define __Lock_H__


#ifdef _DEBUG
	#include <assert.h>
#endif

#include <windows.h>


class CBaseLockObject
{
public:
	virtual void Lock()   = 0;
	virtual void Unlock() = 0;
};


class CGenericLockHandler
{
public:
	CGenericLockHandler(CBaseLockObject & lock)
		: m_lock(lock)
		, m_locked(false)
	{
		Lock();
	}

	virtual ~CGenericLockHandler()
	{
		Unlock();
	}

	// dangerous ???
	// Unlock(): 'if(m_locked)' maybe multithread 'm_locked' state not sync.
	virtual void Lock()   { m_lock.Lock(); m_locked = true; }
	virtual void Unlock() { if(m_locked) { m_locked = false; m_lock.Unlock(); } }
	virtual BOOL IsLock() { return m_locked; }


protected:
	CBaseLockObject & m_lock;
	BOOL              m_locked;
};


class CMutexObject : public CBaseLockObject
{
public:
	CMutexObject()
	{
		m_mutex = CreateMutex(0, false, NULL);
		assert(m_mutex != NULL);
	}

	~CMutexObject()
	{
		CloseHandle(m_mutex);
	}

	void Lock()
	{
		WaitForSingleObject(m_mutex, INFINITE);
	}

	void Unlock()
	{
		ReleaseMutex(m_mutex);
	}


private:
	HANDLE           m_mutex;
};


class CCriticalSetionObject : public CBaseLockObject
{
public:
	CCriticalSetionObject()
	{
		InitializeCriticalSection(&m_cs);
	}

	~CCriticalSetionObject()
	{
		DeleteCriticalSection(&m_cs);
	}

	void Lock()
	{
		EnterCriticalSection(&m_cs);
	}

	void Unlock()
	{
		LeaveCriticalSection(&m_cs);
	}


private:
	CRITICAL_SECTION m_cs;
};


#endif
