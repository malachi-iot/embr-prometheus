#pragma once

namespace embr::prometheus {

template <class T>
class Gauge : metric_tag
{
    T value_ {};

public:
    void add(T value)
    {
        value_ += value;
    }

    constexpr const T& value() const { return value_; }

    Gauge& operator+=(const T& rhs)
    {
        value_ += rhs;
        return *this;
    }
};

}