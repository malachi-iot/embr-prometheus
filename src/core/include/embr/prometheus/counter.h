#include "fwd.h"

namespace embr { namespace prometheus {

template <class T>
class Counter : metric_tag
{
    T value_;

public:
    constexpr Counter(T initial = {}) : value_{initial} {}

    void inc()
    {
        ++value_;
    }

    constexpr const T& value() const { return value_; }

    Counter& operator++()
    {
        ++value_;
        return *this;
    }
};

}}