#include "exlib/ex_lock.h"

#include <iostream>

using namespace std;

namespace ex {

#pragma mark Lock Implementation

LockImpl_::LockImpl_()
{
    pthread_mutex_init(&lock_, NULL);
}

LockImpl_::LockImpl_(bool)
{

}

LockImpl_::~LockImpl_()
{
    pthread_mutex_destroy(&lock_);
}

void LockImpl_::lock()
{   
    pthread_mutex_lock(&lock_);
}

void LockImpl_::unlock()
{
    pthread_mutex_unlock(&lock_);
}

bool LockImpl_::timedlock(double seconds)
{
    timespec req, rem;
    unsigned int elapsed_time = 0;
    unsigned int total_time = static_cast<unsigned int>(seconds * 1.0e9);

    // Set the polling interval
    unsigned int interval = 2000;
    req.tv_sec = 0;
    req.tv_nsec = interval; // 2 microseconds

    int result;
    do {
        result = pthread_mutex_trylock(&lock_);
        if (result == EBUSY) {
            while (nanosleep(&req, &rem) < 0) {
                if (errno != EINTR) {
                    break;
                } else {
                    req = rem;
                }
            }
            elapsed_time += interval;
        } else {
            return true;
        }
    } while (elapsed_time < total_time);
    
    return false; 
    
}

bool LockImpl_::trylock()
{
    return pthread_mutex_trylock(&lock_) != 0;
}

void LockImpl_::set_name(const std::string& name)
{
    name_ = name;
}

pthread_mutex_t& LockImpl_::get_lock()
{
    return lock_;
}


#pragma mark DebugLock Implementation

#ifndef _WIN32

DebugLockImpl_::DebugLockImpl_(bool verbose): owner_(0), verbose_(verbose)
{
    pthread_mutex_init(&debug_lock_, NULL);
}

DebugLockImpl_::DebugLockImpl_(bool verbose, bool val): 
        LockImpl_(val), owner_(0), verbose_(verbose) 
{
    pthread_mutex_init(&debug_lock_, NULL);
}

DebugLockImpl_::~DebugLockImpl_()
{
    pthread_mutex_destroy(&debug_lock_);
}

void DebugLockImpl_::lock()
{
    CoutBuffer cb;
    if (verbose_) {
        cb << thread_names.get_current_thread_name() << " requesting lock " << name_ << ex::endl;
        cb.flush();
    }
    while (true) {
        if (timedlock(5.0)) {
            pthread_mutex_lock(&debug_lock_);
            owner_ = pthread_self();
            pthread_mutex_unlock(&debug_lock_);
            
            if (verbose_) {
                cb << thread_names.get_current_thread_name() << " acquired lock " << name_ << ex::endl;
                cb.flush();
            }
            
            return;
        } else {
            pthread_mutex_lock(&debug_lock_);
            cb << thread_names.get_current_thread_name() << " cannot acquire lock "
                << name_ << " because " << thread_names.get_thread_name(owner_)
                << " is not sharing." << ex::endl;
            pthread_mutex_unlock(&debug_lock_);
            cb.flush();
        }
    }
}

void DebugLockImpl_::unlock()
{
    pthread_mutex_lock(&debug_lock_);
    if (owner_ != pthread_self()) {
        pthread_mutex_unlock(&debug_lock_);
        CoutBuffer cb;
        cb << thread_names.get_current_thread_name()  <<
                            " tried to release lock " << name_ << " without first acquiring it.";
        cb.flush();
        throw ExLockError(  thread_names.get_current_thread_name()  +
                            " tried to release lock " + name_ + " without first acquiring it." );
    }
    owner_ = 0;
    pthread_mutex_unlock(&debug_lock_);
    pthread_mutex_unlock(&lock_);
    if (verbose_) {
        CoutBuffer cb;
        cb << thread_names.get_current_thread_name() << " released lock " << name_ << ex::endl;
        cb.flush();
    }
}

void DebugLockImpl_::set_owner(pthread_t owner)
{
    pthread_mutex_lock(&debug_lock_);
    owner_ = owner;
    pthread_mutex_unlock(&debug_lock_);
}


#endif // _WIN32

#pragma mark Recursive Lock Implementation

RLockImpl_::RLockImpl_(): LockImpl_(false)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock_, &attr);
    pthread_mutexattr_destroy(&attr);
}



#ifndef _WIN32

#pragma mark Debug Recursive Lock Implementation

DebugRLockImpl_::DebugRLockImpl_(bool verbose):
        DebugLockImpl_(verbose, false)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&lock_, &attr);
    pthread_mutexattr_destroy(&attr);
}


void DebugRLockImpl_::lock()
{
    pthread_t id = pthread_self();
    CoutBuffer cb;
    if (verbose_) {
        
        cb << thread_names.get_current_thread_name() << " requesting lock " << name_ << ex::endl;
        cb.flush();
    }
    while (true) {
        if (timedlock(5.0)) {
            pthread_mutex_lock(&debug_lock_);
            owner_ = pthread_self();
            counts_[id]++;
            pthread_mutex_unlock(&debug_lock_);
            
            if (verbose_) {
                cb << thread_names.get_current_thread_name() << " acquired lock " 
                    << name_ << " with count " << counts_[id] << ex::endl;
                cb.flush();
            }
            
            return;
        } else {
            pthread_mutex_lock(&debug_lock_);
            cb << thread_names.get_current_thread_name() << " cannot acquire lock "
                << name_ << " because " << thread_names.get_thread_name(owner_)
                << " has acquired it " << counts_[owner_] << " times." << ex::endl;
            pthread_mutex_unlock(&debug_lock_);
            cb.flush();
        }
    }
}

void DebugRLockImpl_::unlock()
{
    pthread_t id = pthread_self();
    pthread_mutex_lock(&debug_lock_);
    if (counts_[id] == 0) {
        pthread_mutex_unlock(&debug_lock_);
        CoutBuffer cb;
        cb << thread_names.get_current_thread_name() <<
                            " tried to release lock " << name_ << 
                            " more times than it was acquired." << endl;
        cb.flush();
        throw ExLockError(  thread_names.get_current_thread_name() +
                            " tried to release lock " + name_ + 
                            " more times than it was acquired."         );
    }
    
    pthread_mutex_unlock(&lock_);
    counts_[id]--;
    int tmp = counts_[id];
    if (tmp > 0) { // set the owner_ back to this thread
        pthread_mutex_unlock(&debug_lock_);
        if (verbose_) {
            CoutBuffer cb;
            cb << thread_names.get_current_thread_name() << " decremented rlock " 
                << name_ << " to count " << tmp << ex::endl;
            cb.flush();
        }
    } else {
        owner_ = 0;
        pthread_mutex_unlock(&debug_lock_);
        if (verbose_) {
            CoutBuffer cb;
            cb << thread_names.get_current_thread_name() << " released lock " << name_ << ex::endl;
            cb.flush();
        }
        
    }
}

int DebugRLockImpl_::count()
{
    return counts_[pthread_self()];
}

#endif // _WIN32

} // end namespace ex
