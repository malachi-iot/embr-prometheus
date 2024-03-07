#pragma once

// DEBT: Don't always include this, since not everyone has esp_timer available
#include "esp_timer.h"

#include "../../internal/metric_put.h"

#include "../../fwd.h"

namespace embr::prometheus {

// metric_put_core like this inlines the counter vs a const ref
// DEBT: Not 100% sure I like system auto appending "_total" to name after all
internal::metric_put_core<Counter<unsigned> >
    put_metric_uptime(const char* name = "uptime", const char* help = "Uptime in seconds")
{
    auto uptime = (unsigned)(esp_timer_get_time() / 1000000);

    return { { uptime }, name, help };
}

}