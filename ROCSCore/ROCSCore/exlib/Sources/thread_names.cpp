#include "exlib/thread_names.h"

// This is only for debugging, and isn't compatible with Win32 pthreads.
namespace ex {

ThreadNames thread_names;


#ifndef _WIN32

ThreadNamesImpl::ThreadNamesImpl()
{
    pthread_mutex_init(&lock_, NULL);
}


ThreadNamesImpl::~ThreadNamesImpl()
{
    pthread_mutex_destroy(&lock_);
}


std::string ThreadNamesImpl::get_current_thread_name()
{
    pthread_mutex_lock(&lock_);
    std::string result;
    try {
        result = thread_names_.at(pthread_self());
    } catch(std::out_of_range&) {
        result = ex::format("Thread %d", pthread_self());
    }
    pthread_mutex_unlock(&lock_);
    return result;
}

void ThreadNamesImpl::set_current_thread_name(const std::string& name)
{
    pthread_mutex_lock(&lock_);
    thread_names_[pthread_self()] = name;
    pthread_mutex_unlock(&lock_);
}

std::string ThreadNamesImpl::get_thread_name(pthread_t id)
{
    pthread_mutex_lock(&lock_);
    std::string result;
    try {
        result = thread_names_.at(id);
    } catch(std::out_of_range&) {
        result = ex::format("Unknown thread %d", id);
    }
    pthread_mutex_unlock(&lock_);
    return result;
}

#endif // _WIN32

}
