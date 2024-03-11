#pragma once

#include <type_traits>
// EXPERIMENTAL.  Also, uses dynamic allocation
#include <stream-quantiles/ckms.h>

#include "fwd.h"

namespace embr::prometheus { inline namespace v1 {

// https://grafana.com/blog/2022/03/01/how-summary-metrics-work-in-prometheus/

template <class T, class Counter = unsigned>
class Summary : metric_tag
{
    T sum_;
    Counter count_;

public:
    void observe(const T& value)
    {
        sum_ += value;
        ++count_;
    }
};

}}
