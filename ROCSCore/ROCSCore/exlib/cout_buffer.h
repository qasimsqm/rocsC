#pragma once

#include "exlib/win32/declspec.h"

#include <pthread.h>
#include <sstream>
#include <iostream>
#include "exlib/cxx11.h"

namespace ex {

#ifdef CXX11

namespace ex_internal
{

// This class does not support recursive/reentrant locks, and is not intended
// for use outside of this file. Instead, use Lockable (exlib/Lockable.h).
// The internal_lockable class only exists so that the more sophisticated lock
// classes defined in exlib/ex_lock.h can use thread-safe cout without
// ludicrous forward declarations.
class EXLIB_API internal_lockable
{
public:
    internal_lockable()
    {
        pthread_mutex_init(&lock_, NULL);
    }

    ~internal_lockable()
    {
        pthread_mutex_destroy(&this->lock_);
    }

    void lock()
    {
        pthread_mutex_lock(&this->lock_);
    }

    void unlock()
    {
        pthread_mutex_unlock(&this->lock_);
    }

protected:
    class internal_scope_lock
    {
    private:
        internal_lockable &lockable_;

    public:
        internal_scope_lock(internal_lockable *inLockable)
            :
            lockable_(*inLockable)
        {
            this->lockable_.lock();
        }

        ~internal_scope_lock()
        {
            this->lockable_.unlock();
        }
    };

private:
    pthread_mutex_t lock_;
};

};

struct EndlT {};

struct FlushT {};

struct RollbackT {};

extern EXLIB_API EndlT endl;

extern EXLIB_API FlushT flush;

extern EXLIB_API FlushT &commit;

extern EXLIB_API RollbackT rollback;

class EXLIB_API ThreadSafeOStream : private ex_internal::internal_lockable
{
public:
    ThreadSafeOStream(std::ostream &outputStream) : outputStream_(outputStream) {}

    template <typename T>
    ThreadSafeOStream& operator<<(T val)
    {
        internal_scope_lock lock(this);
        outputStream_ << val;
        return (*this);
    }

    ThreadSafeOStream &flush()
    {
        internal_scope_lock lock(this);
        this->outputStream_.flush();
        return (*this);
    }

private:
    std::ostream &outputStream_;
};

EXLIB_API
extern ThreadSafeOStream thread_safe_cout;

EXLIB_TEMPLATE_DECLSPEC(std::basic_ostringstream<char>);

template <class O>
class EXLIB_API OutBuffer
{
public:
    OutBuffer(O &outputStream) : outputStream_(outputStream) {}

    OutBuffer(O &outputStream, const std::string& val)
        :
        outputStream_(outputStream)
    {
        (*this) << val;
    }

    ~OutBuffer()
    {
        this->commit();
    }

    OutBuffer& operator<<(EndlT)
    {
        (*this) << '\n';
        return *this;
    }

    OutBuffer& operator<<(FlushT)
    {
        this->flush();
        return *this;
    }

    OutBuffer& operator<<(RollbackT)
    {
        this->rollback();
        return *this;
    }

    template <typename T>
    OutBuffer& operator<<(T val)
    {
        this->buffer_ << val;
        return *this;
    }

    void commit()
    {
        this->outputStream_ << this->buffer_.str();
        this->buffer_.clear();
    }

    void rollback()
    {
        this->buffer_.clear();
    }

    // For backward-compatibility.
    void write()
    {
        this->commit();
    }

    OutBuffer& flush()
    {
        this->commit();
        this->outputStream_.flush();
        return *this;
    }

    std::string str()
    {
        return this->buffer_.str();
    }

private:
    std::ostringstream buffer_;
    O &outputStream_;
};


class EXLIB_API CoutBuffer: public OutBuffer<ThreadSafeOStream>
{
public:
    CoutBuffer() : OutBuffer(thread_safe_cout) {}

    CoutBuffer(const std::string& val) 
        : 
        OutBuffer(thread_safe_cout, val) {}

    CoutBuffer& operator<<(EndlT)
    {
        OutBuffer<ThreadSafeOStream>::operator<<("\n");
        return *this;
    }

    CoutBuffer& operator<<(FlushT)
    {
        this->flush();
        return *this;
    }

    CoutBuffer& operator<<(RollbackT)
    {
        this->rollback();
        return *this;
    }

    template <typename T>
    CoutBuffer& operator<<(T val)
    {
        OutBuffer<ThreadSafeOStream>::operator<<(val);
        return *this;
    }
};

#endif

}
