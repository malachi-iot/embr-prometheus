#pragma once

#include "fwd.h"

namespace embr::prometheus { inline namespace v1 {

namespace internal {

template <class T>
class CounterBase : metric_tag
{
protected:
    T value_;

public:
    constexpr CounterBase(T initial) : value_{initial} {}

    void inc()
    {
        ++value_;
    }

    // [1.1] demands this, so we'll do it
    void inc(const T& v)
    {
        value_ += v;
    }

    constexpr const T& value() const { return value_; }

};

}

template <class T>
class Counter : public internal::CounterBase<T>
{
    using base_type = internal::CounterBase<T>;

public:
    constexpr explicit Counter(T initial = {}) : base_type{initial} {}

    Counter& operator++()
    {
        ++base_type::value_;
        return *this;
    }
};

}}