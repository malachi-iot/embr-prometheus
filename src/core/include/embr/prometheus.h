#pragma once

#include <estd/string.h>

namespace embr {

namespace prometheus {

void synthetic();

struct metric_tag {};

// buckets MUST appear in ascending order
template <typename T, T... buckets>
class Histogram : metric_tag
{
#if UNIT_TESTING
public:
#endif
    static constexpr unsigned bucket_count = sizeof...(buckets);

    T buckets_[bucket_count] {};

public:
    void observe_idx(unsigned bucket_idx)
    {
        ++buckets_[bucket_idx];
    }

    bool observe(T value)
    {
        // Lots of help from
        // https://www.foonathan.net/2020/05/fold-tricks/

        unsigned i = 0;
        unsigned bucket;
        bool valid = (((i++ < bucket_count && value < buckets) && (bucket = i, true)) || ...);

        if(valid)   observe_idx(bucket - 1);

        return valid;
    }
};


template <class T>
class Counter : metric_tag
{
    T value_;

public:
    void inc()
    {
        ++value_;
    }
};


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

// Guidance from
// https://sysdig.com/blog/prometheus-metrics/
// https://prometheus.io/docs/instrumenting/writing_exporters/

// Gonna need to be smarter and more complex due to the way buckets work
// (i.e. every bucket entry probably wants custom labels too)

template <class Stream>
class OutAssist
{
    Stream& out_;
    int labels_ = 0;
    const char* name_;

    void finalize_label()
    {
        if(labels_ > 0) out_ << '}';
    }

public:
    OutAssist(Stream& out, const char* metric_name) : out_{out},
        name_{metric_name}
    {
        // FIX: Need to do this as base name and append bucket, count, sum, total, etc
        out_ << metric_name;
    }

    void label(const char* label, const char* value)
    {
        if(labels_ == 0)
            out_ << '{';
        else
            out_ << ", ";

        out_ << label << "=\"";
        out_ << value;
        out_ << '"';

        ++labels_;
    }

    template <class T>
    void metric(const Gauge<T>& value)
    {
        finalize_label();

        out_ << ' ' << value.value();
    }

    template <class T, T... buckets>
    void metric(const Histogram<T, buckets...>& value)
    {
        finalize_label();

        //out_ << ' ' << value.value();
    }
};


}

}