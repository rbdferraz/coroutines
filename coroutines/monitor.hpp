// (c) 2013 Maciej Gajewski, <maciej.gajewski0@gmail.com>
#ifndef COROUTINES_MONITOR_HPP
#define COROUTINES_MONITOR_HPP

#include "coroutine.hpp"
#include "thread_safe_queue.hpp"

#include <functional>

namespace coroutines {

// monitor is a syncronisation tool.
// it allows one corotunie to wait for singla from another.
class monitor
{
public:

    typedef std::function<void ()> epilogue_type;

    monitor();
    monitor(const monitor&) = delete;
    ~monitor();

    // called from corotunie context. Will cause the corountine to yield
    // Epilogue will be called after the coroutine is preemted
    void wait(epilogue_type epilogue = epilogue_type());

    // wakes all waiting corotunies
    void wake_all();

    // wakes one of the waiting corountines
    void wake_one();


private:

    thread_safe_queue<coroutine_ptr> _waiting;
};

}

#endif