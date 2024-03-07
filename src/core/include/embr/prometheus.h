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

class Summary : metric_tag
{

};


}

}
