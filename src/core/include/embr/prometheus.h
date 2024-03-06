#pragma once

#include <estd/string.h>

#include <estd/ios.h>
// DEBT: Do a fwd for this guy
#include <estd/iomanip.h>

#include "internal/histogram.h"
#include "prometheus/fwd.h"
#include "prometheus/counter.h"
#include "prometheus/gauge.h"
#include "prometheus/ostream.h"

namespace embr {

namespace prometheus {

void synthetic();

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
};

class Summary : metric_tag
{

};


}

}
