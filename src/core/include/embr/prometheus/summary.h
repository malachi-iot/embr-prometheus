#pragma once

#include <type_traits>

#include "fwd.h"

namespace embr::prometheus {

template <class T>
class Summary : metric_tag
{
public:
    void observe(const T& value)
    {

    }
};

}
