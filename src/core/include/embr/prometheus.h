#pragma once

#include <estd/string.h>

#include <estd/ios.h>
// DEBT: Do a fwd for this guy
#include <estd/iomanip.h>

#include "internal/histogram.h"

namespace embr {

namespace prometheus {

void synthetic();

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
// https://www.robustperception.io/how-does-a-prometheus-histogram-work/

// Gonna need to be smarter and more complex due to the way buckets work
// (i.e. every bucket entry probably wants custom labels too)

struct Labels
{
    const char** names;
    const char** values;
};

template <class ...Args>
struct Labels2
{
    const char** names;
    estd::tuple<Args...> values;
};


template <class Stream>
class OutAssist
{
#if UNIT_TESTING
public:
#endif
    Stream& out_;
    int labels_ = 0;

    void prep_label()
    {
        if(labels_ == 0)
            out_ << '{';
        else
            out_ << ", ";
    }

    void finalize_label()
    {
        if(labels_ > 0) out_ << '}';
    }

public:
    OutAssist(Stream& out) : out_{out}
    {
    }

    void name(const char* name, const char* suffix = nullptr)
    {
        out_ << name;
        if(suffix)  out_ << suffix;
    }

    template <class T>
    void label(const char* label, const T& value)
    {
        prep_label();

        out_ << label << "=\"";
        out_ << value;
        out_ << '"';

        ++labels_;
    }

    void label(const Labels& labels, int label_count)
    {
        for(int i = 0; i < label_count; ++i)
            label(labels.names[i], labels.values[i]);
    }

    template <class ...Args>
    void label(const Labels2<Args...>& labels)
    {
        // FIX: gonna need estd::get<> on the tuple, which isn't
        // going to work here unless there's a constexpr for waiting for us
        for(int i = 0; i < sizeof...(Args); ++i)
            label(labels.names[i], labels.values[i]);
    }

    template <class T>
    void metric(const Gauge<T>& value)
    {
        finalize_label();

        out_ << ' ' << value.value();
    }

    template <class Counter, class T, T... buckets>
    void metric(const Histogram<Counter, T, buckets...>& value, unsigned idx,
        const char* n,
        T bucket,   // DEBT: Do this compile time
        const Labels& labels, int label_count)
    {
        name(n, "_bucket");
        label("le", bucket);
        label(labels, label_count);
        finalize_label();

        out_ << ' ' << value.buckets_[idx];
    }

    template <class T>
    void metric_histogram(const T& value,
        const char* n,
        T bucket,   // DEBT: Do this compile time
        const Labels& labels, int label_count)
    {
        name(n, "_bucket");
        label("le", bucket);
        label(labels, label_count);
        finalize_label();

        out_ << ' ' << value;
    }

    void reset()
    {
        labels_ = 0;
    }
};

template <class Stream, unsigned label_count = 0>
class OutAssist2
{
    OutAssist<Stream> oa_;
    const char* name_;

    Labels labels_;

    template <class T>
    void histogram_metric(T value, T bucket)
    {
        oa_.metric_histogram(value, name_, bucket, labels_, label_count);
        oa_.reset();

        oa_.out_ << estd::endl;
    }

public:
    OutAssist2(Stream& out, const char* name,
        const char** label_names = nullptr,
        const char** label_values = nullptr) :
        oa_(out),
        name_(name),
        labels_{label_names, label_values}
    {
    }

    template <class T>
    void metric(const Gauge<T>& value)
    {
        oa_.metric(value);
        oa_.label(labels_);
        oa_.out_ << estd::endl;
    }

    template <class T, typename Bucket, Bucket... buckets>
    void metric(const Histogram<T, Bucket, buckets...>& value)
    {
        int i = 0;
        T calced[sizeof...(buckets)];

        value.get(calced);

        (!(histogram_metric(calced[i], buckets), i++ < sizeof...(buckets)) || ...);
    }
};



}

}
