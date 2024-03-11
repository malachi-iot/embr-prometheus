#pragma once

#include <estd/internal/platform.h>

// As per [1.1] "Client libraries MUST be thread safe."
#ifndef FEATURE_EMBR_PROMETHEUS_MT_SAFE
#define FEATURE_EMBR_PROMETHEUS_MT_SAFE 0
#endif