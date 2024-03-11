#pragma once

#include <estd/string.h>

#include "internal/metric_put.h"
#include "internal/ostream.h"

#if ESP_PLATFORM
#include "platform/esp-idf/ostream.h"
#endif

#include "fwd.h"

namespace embr { namespace prometheus {


// NOTE: Someone else renders curly braces.  This way, we can add 'le' easily for buckets
template <class Streambuf, class Base, typename ...LabelValues>
void write(estd::detail::basic_ostream<Streambuf, Base>& out,
    // DEBT: Bug https://github.com/malachi-iot/estdlib/issues/32 keeps
    // us from const'ing this
    Labels<LabelValues...>& labels)
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



template <class Stream, typename ...LabelValueTypes>
class OutAssist2
{
    internal::OutAssist<Stream> oa_;

    using context_type = internal::ContextBase<LabelValueTypes...>;
    const context_type* context_;

    using labels_type = Labels<LabelValueTypes...>;

    // Bucket has distinct type to flow through "+Int" string
    template <class T, class Bucket = T>
    void histogram_metric(T value, Bucket bucket)
    {
        oa_.metric_histogram(value, *context_, bucket);
        oa_.reset();

        oa_.out_ << HTTP_ENDL;
    }

public:
    OutAssist2(Stream& out, const context_type* context)
        :
        oa_(out),
        context_(context)
    {}

    template <class Impl>
    void help(const estd::detail::basic_string<Impl>& s)
    {

    }

    template <class T>
    void metric(const Gauge<T>& value, const char* help = nullptr)
    {
        if(help)    oa_.help(context_->name_, help);

        oa_.out_ << "# TYPE " << context_->name_ << " gauge" << HTTP_ENDL;
        oa_.name(context_->name_);
        if constexpr (context_type::has_labels)
            oa_.label(context_->labels_);
        oa_.metric(value);
        oa_.out_ << HTTP_ENDL;
    }

    template <class T>
    void metric(const Counter<T>& value, const char* help = nullptr)
    {
        if(help)    oa_.help(context_->name_, help, "_total");

        oa_.out_ << "# TYPE " << context_->name_ << "_total counter" << HTTP_ENDL;
        oa_.name_and_labels(*context_, "_total");
        oa_.metric(value);
        oa_.out_ << HTTP_ENDL;
    }

    // TODO: Still need to output HELP and TYPE
    template <class T, typename Bucket, Bucket... buckets>
    void metric(const Histogram<T, Bucket, buckets...>& value, const char* help = nullptr)
    {
        if(help)    oa_.help(context_->name_, help);
        oa_.out_ << "# TYPE " << context_->name_ << " histogram" << HTTP_ENDL;

        int i = 0;
        // DEBT: Easy to forget "+Inf" slot
        T calced[sizeof...(buckets) + 1];

        value.get(calced);

        // DEBT: Let's resolve this constant warning
        (!(histogram_metric(calced[i], buckets), i++ < sizeof...(buckets)) || ...);

        histogram_metric(calced[i], "+Inf");

        oa_.name_and_labels(*context_, "_sum");
        oa_.metric(Gauge(value.sum()));
        oa_.out_ << HTTP_ENDL;
        oa_.reset();

        // "A histogram MUST have a bucket with {le="+Inf"}.
        // Its value must be identical to the value of x_count." [1]
        oa_.name_and_labels(*context_, "_count");
        oa_.metric(Gauge(calced[i]));
        oa_.out_ << HTTP_ENDL;
    }
};

namespace internal {

template <class Metric, class ...Args>
template <class Streambuf, class Base>
inline void metric_put_core<Metric, Args...>::operator()(estd::detail::basic_ostream<Streambuf, Base>& out) const
{
    OutAssist2 oa(out, &context_);

    oa.metric(context_.metric_, help_);
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

template <class T, class ...Args>
constexpr internal::metric_put_core<Gauge<T>, Args...>
put_metric_gauge(const T& value, const char* name, const char* help,
        const std::initializer_list<const char*>& label_names,
        Args&&...args)
{
    return { { value }, name, help, Labels{ label_names, std::forward<Args>(args)...} };
}

template <class T, class ...Args>
constexpr internal::metric_put_core<Gauge<T>, Args...>
put_metric_gauge(const T& value, const char* name, const char* help,
    const char* label_names[],
    Args&&...label_values)
{
    return { Gauge{ value }, name, help, Labels{ label_names, std::forward<Args>(label_values)...} };
}

template <class Metric, class ...LabelValues>
constexpr internal::metric_put<Metric, LabelValues...>
put_metric(const Metric& metric, const char* name, const char* help,
    const Labels<LabelValues...>& labels)
{
    return { metric, name, help, labels };
}

template <class Metric, class ...LabelValues>
constexpr internal::metric_put<Metric, LabelValues...>
put_metric(const Metric& metric, const char* name, const char* help,
    const std::initializer_list<const char*>& label_names,
    LabelValues&&... label_values)
{
    return { metric, name, help, Labels(label_names,
        std::forward<LabelValues>(label_values)...) };
}

}}
