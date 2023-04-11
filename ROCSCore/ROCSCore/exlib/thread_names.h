#pragma once

#include "exlib/win32/declspec.h"

#include <map>
#include <string>
#include <pthread.h>
#include <stdexcept>
#include "exlib/format.h"


namespace ex {

#ifdef _WIN32
// Win32 pthreads doesn't support assignment or comparison of pthread_t, so
// this implementation is just a stub.
class EXLIB_API ThreadNamesImpl
{
public:
    ThreadNamesImpl() {}

    ~ThreadNamesImpl() {}

    std::string get_current_thread_name() { return "(no thread name available)"; }

    void set_current_thread_name(const std::string& name) { return; }

    std::string get_thread_name(pthread_t id) { return "(no thread name available)"; }
};
#else
// This is only for debugging, and is not compatible with Win32 pthreads.
class EXLIB_API ThreadNamesImpl
{
public:
    ThreadNamesImpl();
    ~ThreadNamesImpl();
    std::string get_current_thread_name();
    void set_current_thread_name(const std::string& name);
    std::string get_thread_name(pthread_t id);

private:
    pthread_mutex_t lock_;
    std::map<pthread_t, std::string> thread_names_;
};
#endif // _WIN32


class EXLIB_API ThreadNames
{
public:
    ThreadNames()
    {
        pimpl = new ThreadNamesImpl();
    }
    
    ~ThreadNames()
    {
        delete pimpl;
    }
    
    std::string get_current_thread_name()
    {
        return pimpl->get_current_thread_name();
    }
    
    void set_current_thread_name(const std::string& name)
    {
        pimpl->set_current_thread_name(name);
    }
    
    std::string get_thread_name(pthread_t id)
    {
        return pimpl->get_thread_name(id);
    }
    
private:
    ThreadNamesImpl *pimpl;
};

EXLIB_API
extern
ThreadNames thread_names;

}
