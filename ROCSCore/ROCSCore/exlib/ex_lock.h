#pragma once

#include "exlib/win32/declspec.h"

#include <string>
#include <pthread.h>
// #include <sys/time.h>
// #include <ctime>
#include <stdexcept>
#include <type_traits>
#if defined(__APPLE__) || defined(__linux)
#include <unistd.h>
#endif // __APPLE || __linux
#include "exlib/xplatform_types.h"
#include "exlib/cout_buffer.h"
#include "exlib/thread_names.h"

namespace ex {

// http://msdn.microsoft.com/en-us/library/3tdb471s.aspx
MSC_DISABLE_WARNING(4275);
class EXLIB_API ExLockError : public std::runtime_error
{
public:
    ExLockError(const std::string& what): std::runtime_error(what) {}
};
MSC_RESTORE_WARNING(4275);

class EXLIB_API LockImpl_
{
public:
    LockImpl_();
    virtual ~LockImpl_();
    virtual void lock();
    virtual void unlock();
    virtual bool timedlock(double seconds);
    virtual bool trylock();
    virtual void set_name(const std::string& name);
    pthread_mutex_t& get_lock();
    virtual void set_owner(pthread_t) {}
    virtual int count() { return 0; }

protected:
    LockImpl_(bool val); 
    pthread_mutex_t lock_;
    MSC_DISABLE_WARNING(4251); 
    std::string name_;
    MSC_RESTORE_WARNING(4251);
};


#ifdef _WIN32
// DebugLockImpl_ isn't compatible with Win32 pthreads,
// so just pass through to the non-debug implementation.
class EXLIB_API DebugLockImpl_ : public LockImpl_
{
public:
    DebugLockImpl_(bool verbose = true) : LockImpl_(verbose) {}
};
#else // !_WIN32
class EXLIB_API DebugLockImpl_ : public LockImpl_
{
public:
    DebugLockImpl_(bool verbose=true);
    virtual ~DebugLockImpl_();
    virtual void lock();
    virtual void unlock();
    virtual void set_owner(pthread_t owner);

protected:
    DebugLockImpl_(bool val, bool verbose);
    pthread_mutex_t debug_lock_;
    pthread_t owner_;
    bool verbose_;
};
#endif // _WIN32

class EXLIB_API RLockImpl_ : public LockImpl_
{
public:
    RLockImpl_();
};


#ifdef _WIN32
// DebugLockImpl_ isn't compatible with Win32 pthreads,
// so just pass through to the non-debug implementation.
class EXLIB_API DebugRLockImpl_ : public RLockImpl_
{
public:
    DebugRLockImpl_(bool verbose=true) : RLockImpl_() {}
};
#else
class EXLIB_API DebugRLockImpl_ : public DebugLockImpl_
{
public:
    DebugRLockImpl_(bool verbose=true);
    virtual void lock();
    virtual void unlock();
    virtual int count();

protected:
    std::map<pthread_t, int> counts_;
};
#endif // _WIN32

template<bool debug>
class basic_lock
{
public:
    basic_lock();

    basic_lock(const basic_lock& other)
    {
        CoutBuffer cb;
        cb << "basic_lock move constructor, other = " << &other << ex::endl;
        cb.flush();
        pimpl = other.pimpl;
        const_cast<basic_lock &>(other).pimpl = NULL;
    }
    
    basic_lock& operator=(const basic_lock& other)
    {
        CoutBuffer cb;
        cb << "basic_lock move operator=, other = " << &other << ex::endl;
        cb.flush(); 
        pimpl = const_cast<basic_lock &>(other).pimpl;
        const_cast<basic_lock &>(other).pimpl = NULL;
        return *this;
    }

    ~basic_lock() { delete pimpl; }

    void lock() { pimpl->lock(); }

    void unlock() { pimpl->unlock(); }

    bool timedlock(double seconds) { return pimpl->timedlock(seconds); }

    bool trylock() { return pimpl->trylock(); }

    void set_name(const std::string& name) { pimpl->set_name(name); }

    pthread_mutex_t& get_lock() { return pimpl->get_lock(); }

    void print_addresses()
    {
        CoutBuffer cb;
        cb << "basic_lock this=" << this << ", pimpl=" << pimpl << ex::endl;
        cb.flush();
    }

    /** Do not use this directly.  I would make it protected, but the friend semantics
        for the templated cond_wait function are tricky. **/
#ifndef _WIN32
    void    set_owner(pthread_t owner) { pimpl->set_owner(owner); }
#endif // !_WIN32

protected:
    basic_lock(bool) {}
    LockImpl_ *pimpl; 
};

template<>
inline
basic_lock<true>::basic_lock()
{
    #ifdef EXLIB_DEBUG_LOCKS_VERBOSE
    pimpl = new DebugLockImpl_(true);
    #else
    pimpl = new DebugLockImpl_(false);
    #endif
}

template<>
inline
basic_lock<false>::basic_lock()
{
    pimpl = new LockImpl_();
}


template<bool debug>
class basic_rlock : public basic_lock<debug>
{
public:
    basic_rlock();
    int count() { return this->pimpl->count(); }
};

template<>
inline
basic_rlock<true>::basic_rlock()
    :
    basic_lock<true>(false)
{
    #ifdef EXLIB_DEBUG_LOCKS_VERBOSE
    this->pimpl = new DebugRLockImpl_(true);
    #else
    this->pimpl = new DebugRLockImpl_(false);
    #endif
}

template<>
inline
basic_rlock<false>::basic_rlock()
    :
    basic_lock<false>(false)
{
    this->pimpl = new RLockImpl_();
}

template<class LockClass>
void cond_wait(pthread_cond_t *cond, LockClass& lock)
{
    lock.lock();
    pthread_cond_wait(cond, &lock.get_lock());
    #ifndef _WIN32
    lock.set_owner(pthread_self());
    #endif // _WIN32
    lock.unlock();
}

#ifdef EXLIB_DEBUG_LOCKS
typedef basic_lock<true> Lock;
typedef basic_rlock<true> RLock;
EXLIB_TEMPLATE_DECLSPEC(basic_lock<true>);
EXLIB_TEMPLATE_DECLSPEC(basic_rlock<true>);
#else
typedef basic_lock<false> Lock;
typedef basic_rlock<false> RLock;
EXLIB_TEMPLATE_DECLSPEC(basic_lock<false>);
EXLIB_TEMPLATE_DECLSPEC(basic_rlock<false>);
#endif

}

