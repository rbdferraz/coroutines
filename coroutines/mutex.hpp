// Copyright (c) 2013 Maciej Gajewski

#ifndef COROUTINES_MUTEX_HPP
#define COROUTINES_MUTEX_HPP

#include "profiling/profiling.hpp"

#include <mutex>
#include <atomic>

namespace coroutines {


class spinlock
{
public:

    spinlock() 
    : _flag(ATOMIC_FLAG_INIT)
    { }

    spinlock(const char* name)
    : _flag(ATOMIC_FLAG_INIT)
    {
        #ifdef COROUTINES_SPINLOCKS_PROFILING
            CORO_PROF("spinlock", this, "created", name);
        #else
            (void)name;
        #endif
    }

    void lock()
    {
        #ifdef COROUTINES_SPINLOCKS_PROFILING
            // only report contested lock events
            if (try_lock())
                return;
            CORO_PROF("spinlock", this, "spinning begin");
        #endif
        while(_flag.test_and_set(std::memory_order_acquire))
            ; // spin
        #ifdef COROUTINES_SPINLOCKS_PROFILING
            CORO_PROF("spinlock", this, "spinning end");
        #endif
    }

    bool try_lock()
    {
        return !_flag.test_and_set(std::memory_order_acquire);
    }

    void unlock()
    {
        _flag.clear(std::memory_order_release);
    }


private:

    std::atomic_flag _flag;
};


//typedef std::mutex mutex;
typedef spinlock mutex;

}

#endif
