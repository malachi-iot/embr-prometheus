#pragma once

#include <ctime>

#include "counter.h"

namespace embr::prometheus { inline namespace v1 {

template <class T>
class Gauge : public internal::CounterBase<T>
{
    using base_type = internal::CounterBase<T>;

public:
    constexpr explicit Gauge(const T& initial = {}) : base_type{initial} {}

    void dec(const T& v = 1)
    {
        base_type::value_ -= v;
    }

    void set(const T& v)
    {
        base_type::value_ = v;
    }

    // As per [1.1], untested
    void set_to_current_time()
    {
        std::time_t seconds = std::time(nullptr);

        set(seconds);
    }

    Gauge& operator+=(const T& rhs)
    {
        base_type::value_ += rhs;
        return *this;
    }
};

}}