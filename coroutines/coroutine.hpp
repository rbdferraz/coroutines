// (c) 2013 Maciej Gajewski, <maciej.gajewski0@gmail.com>
#ifndef COROUTINES_COROUTINE_HPP
#define COROUTINES_COROUTINE_HPP

#include "coroutines/mutex.hpp"

#include <boost/context/all.hpp>

#include <functional>
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>

namespace coroutines {

class scheduler;

class coroutine
{
public:
    typedef std::function<void()> function_type;
    typedef std::function<void(coroutine*)> epilogue_type;

    coroutine(scheduler& parent, std::string name, function_type&& fun);
    ~coroutine();

    coroutine(const coroutine&) = delete;
    coroutine(coroutine&&) = delete;

    void run();

    // returns currently runnig coroutine
    static coroutine* current_corutine();

    void yield(const std::string& checkpoint_name, epilogue_type epilogue = epilogue_type());

    std::string name() const { return _name; }
    std::string last_checkpoint() const { return _last_checkpoint; }
    void set_checkpoint(const std::string& cp) { _last_checkpoint = cp; }

private:


    static void static_context_function(intptr_t param);
    void context_function();

    std::function<void()> _function;

    boost::context::fcontext_t _caller_context;
    boost::context::fcontext_t* _new_context = nullptr;

    char* _stack = nullptr;
    epilogue_type _epilogue;
    mutex _run_mutex;
    scheduler& _parent;
    std::string _name;
    std::string _last_checkpoint = "just created";
};

template <typename Callable>
class callable_wrapper
{
public:
    callable_wrapper(Callable&& c)
    : _callable(std::move(c))
    {
    }

    void operator()()
    {
        _callable();
    }

private:

    Callable _callable;
};

typedef std::unique_ptr<coroutine> coroutine_ptr;
typedef coroutine* coroutine_weak_ptr; // this couldbe made smarter later on

template<typename Callable>
coroutine_ptr make_coroutine(scheduler& parent, std::string name, Callable&& c)
{
    callable_wrapper<Callable>* wrapper = new callable_wrapper<Callable>(std::move(c));

    return coroutine_ptr(new coroutine(parent, std::move(name), [wrapper]()
    {
        try
        {
            (*wrapper)();
            delete wrapper;
        }
        catch(...)
        {
            delete wrapper;
            throw;
        }

    }));
}


}

#endif
