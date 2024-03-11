#pragma once

#include "feature.h"

namespace embr { namespace prometheus {

inline namespace v1 {

struct metric_tag {};

template <typename Counter, typename Bucket, Bucket... buckets>
class Histogram;

template <class T>
class Counter;

template <class T>
class Gauge;

}

namespace internal {

template <class Metric, class ...Args>
struct metric_put_core;

// DEBT: Might be a bug in estd::add_lvalue_reference_t - seems to remove const, maybe
template <class Metric, class ...Args>
//using metric_put = metric_put_core<estd::add_lvalue_reference_t<Metric>, Args...>;
using metric_put = metric_put_core<const Metric&, Args...>;

}


}}
