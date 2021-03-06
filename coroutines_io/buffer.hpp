// Copyright (c) 2013 Maciej Gajewski
#ifndef COROUTINES_IO_BUFFER_HPP
#define COROUTINES_IO_BUFFER_HPP

#include <memory>
#include <utility>

namespace coroutines_io
{

// used to send blocks of data around. Movable, bot not copytable.
class buffer
{
public:

    typedef char value_type;
    typedef char* iterator;
    typedef const char* const_iterator;

    // null buffer
    buffer() = default;

    // alocates buffer
    buffer(std::size_t capacity)
    : _capacity(capacity), _size(0), _data(new char[_capacity])
    {
    }

    ~buffer()
    {
    }

    buffer(const buffer&) = delete;
    buffer(buffer&& o) noexcept
    {
        swap(o);
    }

    buffer& operator=(buffer&& o)
    {
        swap(o);
        return *this;
    }

    // iterators
    iterator begin() { return _data.get(); }
    iterator end() { return _data.get() + _size; }
    const_iterator begin() const { return _data.get(); }
    const_iterator end() const { return _data.get() + _size; }

    // size/capacity
    void set_size(std::size_t s) { _size = s; }
    std::size_t size() const { return _size; }
    std::size_t capacity() const { return _capacity; }

    bool is_null() const { return !_capacity; }

    // other
    void swap(buffer& o) noexcept
    {
        std::swap(_capacity, o._capacity);
        std::swap(_size, o._size);
        std::swap(_data, o._data);
    }

private:

    std::size_t _capacity = 0;  // buffer capacity
    std::size_t _size = 0;      // amount of data in
    std::unique_ptr<char[]> _data;
};

}

namespace std {

template<>
inline
void swap<coroutines_io::buffer>(coroutines_io::buffer& a, coroutines_io::buffer& b)
{
    a.swap(b);
}

}
#endif
