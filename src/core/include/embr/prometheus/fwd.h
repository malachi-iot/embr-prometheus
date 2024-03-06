#pragma once

namespace embr { namespace prometheus {

struct metric_tag {};

template <typename Counter, typename Bucket, Bucket... buckets>
class Histogram;

template <class T>
class Counter;

template <class T>
class Gauge;


}}