#pragma once

#include <estd/string.h>

#include <estd/ios.h>
// DEBT: Do a fwd for this guy
#include <estd/iomanip.h>

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
    const char* name_;

    using labels_type = Labels<LabelValueTypes...>;
    Labels<LabelValueTypes...> labels_;

    // Bucket has distinct type to flow through "+Int" string
    template <class T, class Bucket = T>
    void histogram_metric(T value, Bucket bucket)
    {
        oa_.metric_histogram(value, name_, bucket, labels_);
        oa_.reset();

        oa_.out_ << HTTP_ENDL;
    }

public:
    OutAssist2(Stream& out, const char* name,
        const char** label_names = embr::prometheus::label_names,
        LabelValueTypes&&...values) :
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

    template <class Impl>
    void help(const estd::detail::basic_string<Impl>& s)
    {

    }

    template <class T>
    void metric(const Gauge<T>& value, const char* help = nullptr)
    {
        if(help)    oa_.help(name_, help);

        oa_.out_ << "# TYPE " << name_ << " gauge" << HTTP_ENDL;
        oa_.name(name_);
        oa_.label(labels_);
        oa_.metric(value);
        oa_.out_ << HTTP_ENDL;
    }

    template <class T>
    void metric(const Counter<T>& value, const char* help = nullptr)
    {
        if(help)    oa_.help(name_, help, "_total");

        oa_.out_ << "# TYPE " << name_ << "_total counter" << HTTP_ENDL;
        oa_.name(name_, "_total");
        oa_.label(labels_);
        oa_.metric(value);
        oa_.out_ << HTTP_ENDL;
    }

    // TODO: Still need to output HELP and TYPE
    template <class T, typename Bucket, Bucket... buckets>
    void metric(const Histogram<T, Bucket, buckets...>& value, const char* help = nullptr)
    {
        if(help)    oa_.help(name_, help);
        oa_.out_ << "# TYPE " << name_ << " histogram" << HTTP_ENDL;

        int i = 0;
        // DEBT: Easy to forget "+Inf" slot
        T calced[sizeof...(buckets) + 1];

        value.get(calced);

        // DEBT: Let's resolve this constant warning
        (!(histogram_metric(calced[i], buckets), i++ < sizeof...(buckets)) || ...);

        histogram_metric(calced[i], "+Inf");

        oa_.name(name_, "_sum");
        oa_.label(labels_);
        oa_.metric(Gauge(value.sum()));
        oa_.out_ << HTTP_ENDL;
        oa_.reset();

        // DEBT: With presence of +Inf, isn't this superfluous?  Perhaps I'm not
        // doing this right
        oa_.name(name_, "_count");
        oa_.label(labels_);
        oa_.metric(Gauge(calced[i]));
        oa_.out_ << HTTP_ENDL;
    }
};

namespace internal {

template <class Metric, class ...Args>
template <class Streambuf, class Base>
inline void metric_put_core<Metric, Args...>::operator()(estd::detail::basic_ostream<Streambuf, Base>& out) const
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
    const Labels<LabelValues...>& labels)
{
    return { metric, name, help, labels };
}

// DEBT: THis one collides with above Label one, so temporarily disabling
/*
template <class Metric, class ...LabelValues>
constexpr internal::metric_put<Metric, LabelValues...>
put_metric(const Metric& metric, const char* name, const char* help,
    LabelValues&&... label_values)
{
    return { metric, name, help, Labels<LabelValues...>(label_names,
        estd::tuple<LabelValues...>(std::forward<LabelValues>(label_values)...)) };
}
*/

}}
