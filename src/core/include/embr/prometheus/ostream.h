#pragma once

#include <estd/string.h>

#include <estd/ios.h>
// DEBT: Do a fwd for this guy
#include <estd/iomanip.h>

#include "internal/metric_put.h"

#if ESP_PLATFORM
#include "platform/esp-idf/ostream.h"
#endif

#include "feature.h"
#include "fwd.h"

// DEBT: Pick better name, constexpr if we can
#define HTTP_ENDL   "\r\n"

namespace embr { namespace prometheus {

// Guidance from
// https://sysdig.com/blog/prometheus-metrics/
// https://prometheus.io/docs/instrumenting/writing_exporters/
// https://www.robustperception.io/how-does-a-prometheus-histogram-work/

// Gonna need to be smarter and more complex due to the way buckets work
// (i.e. every bucket entry probably wants custom labels too)

namespace internal {

// Technique lifted from
// https://blog.tartanllama.xyz/exploding-tuples-fold-expressions/

template <std::size_t... Idx>
auto make_index_dispatcher(std::index_sequence<Idx...>) {
    return [] (auto&& f) { (f(std::integral_constant<std::size_t,Idx>{}), ...); };
}

template <std::size_t N>
auto make_index_dispatcher() {
    return make_index_dispatcher(std::make_index_sequence<N>{});
}

template <typename Tuple, typename Func>
void for_each(Tuple&& t, Func&& f) {
    constexpr auto n = std::tuple_size<std::decay_t<Tuple>>::value;
    auto dispatcher = make_index_dispatcher<n>();
    dispatcher([&f,&t](auto idx) { f(std::get<idx>(std::forward<Tuple>(t))); });
}

template <class Streambuf, class Base, class T>
void write_one_label(estd::detail::basic_ostream<Streambuf, Base>& out,
    const char* name, const T& value)
{
    out << name << "=\"" << value << '"';
}


}

// NOTE: Someone else renders curly braces.  This way, we can add 'le' easily for buckets
template <class Streambuf, class Base, typename ...LabelValues>
void write(estd::detail::basic_ostream<Streambuf, Base>& out,
    // DEBT: Bug https://github.com/malachi-iot/estdlib/issues/32 keeps
    // us from const'ing this
    Labels2<LabelValues...>& labels)
{
    // NOTE: Would prefer explicit c++20 generic syntax, but we want
    // to retain c++17 compatibility
    labels.values.visit([&](const auto& instance)
    {
        if(instance.index > 0) out << ", ";

        internal::write_one_label(out,
            labels.names[instance.index],
            instance.value);

        return false;
    });
    // NOTE: Fold expressions are amazing, but my estd::tuple visitor is
    // more convenient here
    /*
    internal::for_each(labels.values, [&](const auto& value)
    {
        write_one_label(out, )
    }); */
}

template <class Stream>
class OutAssist
{
#//if UNIT_TESTING
public:
//#endif
    Stream& out_;
    // # of labels written so far for this line
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

    // DEBT: Due to https://github.com/malachi-iot/estdlib/issues/32
    void label(const Labels2<>&)
    {

    }


    template <class ...Args>
    void label(const Labels2<Args...>& labels)
    {
        /*
        // FIX: gonna need estd::get<> on the tuple, which isn't
        // going to work here unless there's a constexpr for waiting for us
        for(int i = 0; i < sizeof...(Args); ++i)
        {
            // TODO: Detect and skip over monostates/nullptrs
            label(labels.names[i], labels.values[i]);
        }*/

        prep_label();
        // DEBT: See comments in write
        write(out_, (Labels2<Args...>&)labels);

        labels_ += sizeof... (Args);
    }

    template <class T>
    void metric(const Gauge<T>& value)
    {
        finalize_label();

        out_ << ' ' << value.value();
    }

    template <class T>
    void metric(const Counter<T>& value)
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

    template <class T, class Bucket, typename ...LabelValues>
    void metric_histogram(const T& value,
        const char* n,
        Bucket bucket,   // DEBT: Do this compile time
        const Labels2<LabelValues...>& labels)
    {
        name(n, "_bucket");
        label(labels);
        label("le", bucket);
        finalize_label();

        out_ << ' ' << value;
    }

    void reset()
    {
        labels_ = 0;
    }
};

#if UPGRADING_LABELS
template <class Stream, typename ...LabelValueTypes>
#else
template <class Stream, unsigned label_count = 0>
#endif
class OutAssist2
{
    OutAssist<Stream> oa_;
    const char* name_;

#if UPGRADING_LABELS
    using labels_type = Labels2<LabelValueTypes...>;
    Labels2<LabelValueTypes...> labels_;
#else
    Labels labels_;
#endif

    // Bucket has distinct type to flow through "+Int" string
    template <class T, class Bucket = T>
    void histogram_metric(T value, Bucket bucket)
    {
#if UPGRADING_LABELS
        oa_.metric_histogram(value, name_, bucket, labels_);
#else
        oa_.metric_histogram(value, name_, bucket, labels_, label_count);
#endif
        oa_.reset();

        oa_.out_ << HTTP_ENDL;
    }

public:
#if UPGRADING_LABELS
    OutAssist2(Stream& out, const char* name,
        const char** label_names, LabelValueTypes&&...values) :
        oa_(out),
        name_(name),
        labels_(label_names, std::forward<LabelValueTypes>(values)...)
    {
    }

    OutAssist2(Stream& out, const char* name,
        const labels_type& labels) :
        oa_(out),
        name_(name),
        labels_(labels)
    {
    }
#else
    OutAssist2(Stream& out, const char* name,
        const char** label_names = nullptr,
        const char** label_values = nullptr) :
        oa_(out),
        name_(name),
        labels_{label_names, label_values}
    {
    }
#endif

    template <class Impl>
    void help(const estd::detail::basic_string<Impl>& s)
    {

    }

    template <class T>
    void metric(const Gauge<T>& value, const char* help = nullptr)
    {
        if(help)
            oa_.out_ << "# HELP " << name_ << help << HTTP_ENDL;

        oa_.out_ << "# TYPE " << name_ << " gauge" << HTTP_ENDL;
        oa_.name(name_);
        oa_.metric(value);
#if UPGRADING_LABELS
        oa_.label(labels_);
#else
        oa_.label(labels_, label_count);
#endif
        oa_.out_ << HTTP_ENDL;
    }

    template <class T>
    void metric(const Counter<T>& value, const char* help = nullptr)
    {
        if(help)
            oa_.out_ << "# HELP " << name_ << "_total " << help << HTTP_ENDL;

        oa_.out_ << "# TYPE " << name_ << "_total counter" << HTTP_ENDL;
        oa_.name(name_, "_total");
        oa_.metric(value);
#if UPGRADING_LABELS
        oa_.label(labels_);
#else
        oa_.label(labels_, label_count);
#endif
        oa_.out_ << HTTP_ENDL;
    }

    template <class T, typename Bucket, Bucket... buckets>
    void metric(const Histogram<T, Bucket, buckets...>& value, const char* help = nullptr)
    {
        int i = 0;
        // DEBT: Easy to forget "+Inf" slot
        T calced[sizeof...(buckets) + 1];

        value.get(calced);

        // DEBT: Let's resolve this constant warning
        (!(histogram_metric(calced[i], buckets), i++ < sizeof...(buckets)) || ...);

        histogram_metric(calced[i], "+Inf");
    }
};

namespace internal {

template <class Metric, class ...Args>
template <class Streambuf, class Base>
void metric_put_core<Metric, Args...>::operator()(estd::detail::basic_ostream<Streambuf, Base>& out) const
{
    OutAssist2 oa(out, name_, labels_);

    oa.metric(metric_, help_);
}

}

template <class Metric>
constexpr internal::metric_put<Metric>
    put_metric(const Metric& metric, const char* name, const char* help = nullptr)
{
    return { metric, name, help };
}

template <class T>
constexpr internal::metric_put_core<Counter<T> >
    put_metric_counter(const T& count, const char* name, const char* help = nullptr)
{
    return { { count }, name, help };
}

template <class T>
constexpr internal::metric_put_core<Gauge<T> >
put_metric_gauge(const T& value, const char* name, const char* help = nullptr)
{
    return { { value }, name, help };
}

template <class Metric, class ...LabelValues>
constexpr internal::metric_put<Metric, LabelValues...>
put_metric(const Metric& metric, const char* name, const char* help,
    Labels2<LabelValues...>& labels)
{
    return { metric, name, help, labels };
}

template <class Metric, class ...LabelValues>
constexpr internal::metric_put<Metric, LabelValues...>
put_metric(const Metric& metric, const char* name, const char* help,
    LabelValues... label_values)
{
    return { metric, name, help, Labels2<LabelValues...>(label_names,
        estd::tuple<LabelValues...>(std::forward<LabelValues>(label_values)...)) };
}


}}
