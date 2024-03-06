#include "fwd.h"

namespace embr { namespace prometheus {

template <class T>
class Counter : metric_tag
{
    T value_{};

public:
    void inc()
    {
        ++value_;
    }

    constexpr const T& value() const { return value_; }
};

}}