#pragma once

#include "exlib/win32/declspec.h"

#include <ostream>

#include <pthread.h>

namespace ex
{

class EXLIB_API ThreadSafeOStream
{
public:
    ThreadSafeOStream(std::ostream &inStream) : outputStream_(inStream) {}

    template< typename T >
    ThreadSafeOStream& operator<<(T val)
    {
        ScopeLock lock(lock_);
        outputStream_ << val;
        return (*this);
    }
    
    ThreadSafeOStream &flush() 
    {
        ScopeLock lock(lock_);
        this->outputStream_.flush();
        return (*this);
    }

private:
    class ScopeLock
    {
    public:
        ScopeLock(pthread_mutex_t &inLock)
            :
            lock_(inLock)
        {
            pthread_mutex_lock(&lock_);
        }

        ~ScopeLock()
        {
            pthread_mutex_unlock(&lock_);
        }

    private:
        pthread_mutex_t &lock_;
    };

private:
    pthread_mutex_t lock_;
    std::ostream &outputStream_;
};

}
