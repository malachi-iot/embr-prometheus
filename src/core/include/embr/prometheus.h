#pragma once

#include <estd/string.h>

#include <estd/ios.h>
// DEBT: Do a fwd for this guy
#include <estd/iomanip.h>

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

    void label(const Labels& labels, int label_count)
    {
        for(int i = 0; i < label_count; ++i)
            label(labels.names[i], labels.values[i]);
    }

    template <class T>
    void metric(const Gauge<T>& value)
    {
        finalize_label();

        out_ << ' ' << value.value();
    }

    template <class T, T... buckets>
    void metric(const Histogram<T, buckets...>& value, unsigned idx,
        const char* n,
        T bucket,   // DEBT: Do this compile time
        const Labels& labels, int label_count)
    {
        estd::layer1::string<8> s;

        estd::to_string(s, bucket);

        name(n, "_bucket");
        label("le", s.c_str());
        label(labels, label_count);
        finalize_label();

        out_ << ' ' << value.buckets_[idx];
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

    template <class T, T... buckets>
    void histogram_metric(const Histogram<T, buckets...>& value, unsigned idx, T bucket)
    {
        oa_.metric(value, idx, name_, bucket, labels_, label_count);
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

    template <class T, T... buckets>
    constexpr void metric(const Histogram<T, buckets...>& value)
    {
        int i = 0;

        (!(histogram_metric(value, i, buckets), i++ < sizeof...(buckets)) || ...);
    }
};



}

}