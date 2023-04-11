#pragma once

#include "exlib/win32/declspec.h"

#include <exception>

#include "exlib/ex_lock.h"

namespace ex
{

class EXLIB_API BasicLockable
{
public:
    virtual void Lock() = 0;

    virtual void Unlock() = 0;

    template<typename RESULT_T = void, class LAMBDA_T>
    RESULT_T LambdaLock(LAMBDA_T fn)
    {
        ScopeLock lock(this);
        return fn();
    }

    class ScopeLock
    {
    private:
        BasicLockable *lockable_;

    public:
        ScopeLock(BasicLockable &lockable)
            :
            lockable_(&lockable)
        {
            this->lockable_->Lock();
        }

        ScopeLock(BasicLockable *lockable)
            :
            lockable_(lockable)
        {
            this->lockable_->Lock();
        }

        ~ScopeLock()
        {
            this->Release();
        }

        template<typename LAMBDA_T>
        void Intermission(LAMBDA_T fn)
        {
            if (this->lockable_ == NULL)
            {
                throw std::logic_error("Attempted to release a null lock.");
            }

            this->lockable_->Unlock();

            fn();

            if (this->lockable_ == NULL)
            {
                throw std::logic_error("Attempted to obtain a null lock.");
            }

            this->lockable_->Lock();
        };

        void Release()
        {
            if (this->lockable_ == NULL)
            {
                // The lock has already been released.
                return;
            }

            this->lockable_->Unlock();
            this->lockable_ = NULL;
        }
    };
};

class EXLIB_API RLockable
    :
    public BasicLockable
{
public:
    // The following method definitions are joined (to one line each) in
    // solidarity to protest having to sacrifice a less verbose template base
    // class implementation to appease the __declspec(dllimport) gods.
    RLockable(const std::string &lockName) { this->lock_.set_name(lockName); }
    virtual void Lock() { this->lock_.lock(); }
    virtual void Unlock() { this->lock_.unlock(); }

private:
    ex::RLock lock_;
};

class EXLIB_API Lockable
    :
    public BasicLockable
{
public:
    // The following method definitions are joined (to one line each) in
    // solidarity to protest having to sacrifice a less verbose template base
    // class implementation to appease the __declspec(dllimport) gods.
    Lockable(const std::string &lockName) { this->lock_.set_name(lockName); }
    virtual void Lock() { this->lock_.lock(); }
    virtual void Unlock() { this->lock_.unlock(); }

private:
    ex::Lock lock_;
};

}
