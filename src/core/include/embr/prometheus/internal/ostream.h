#include "../fwd.h"

#include <estd/ostream.h>

// Not CRLF as per [1]
// DEBT: Pick better name, constexpr if we can
// DEBT: Would be nice if we could do this through a locale somehow
#define HTTP_ENDL   "\n"

namespace embr::prometheus { inline namespace v1 { namespace internal {

// Guidance from
// https://sysdig.com/blog/prometheus-metrics/
// https://prometheus.io/docs/instrumenting/writing_exporters/
// https://www.robustperception.io/how-does-a-prometheus-histogram-work/

// Gonna need to be smarter and more complex due to the way buckets work
// (i.e. every bucket entry probably wants custom labels too)

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



//template <ESTD_CPP_CONCEPT(estd::concepts::v1::OutStream) Stream>
template <class Stream>
class OutAssist
{
#//if UNIT_TESTING
public:
//#endif
    Stream& out_;

    // DEBT: Being that we use constexpr quantity for label count, we can theoretically
    // eventually get rid of this
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

    void name(const char* name)
    {
        out_ << name;
    }

    void name(const char* name, const char* suffix)
    {
        out_ << name;
        out_ << suffix;
    }

    void endl()
    {
        out_ << HTTP_ENDL;
    }

    void help(const char* name, const char* help, const char* suffix = nullptr)
    {
        out_ << "# HELP " << name;
        if(suffix) out_ << suffix;
        // DEBT: Inconsistent emitting crlf here, but nowhere else in this class
        out_ << ' ' << help << HTTP_ENDL;
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

    template <class ...Args>
    void name_and_labels(const ContextBase<Args...>& context, const char* suffix)
    {
        name(context.name_, suffix);
        if constexpr (ContextBase<Args...>::has_labels)
            label(context.labels_);
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

    template <class T, class Bucket, typename ...LabelValues>
    void metric_histogram(const T& value,
        const ContextBase<LabelValues...>& context,
        Bucket bucket    // DEBT: Do this compile time
        )
    {
        name_and_labels(context, "_bucket");
        label("le", bucket);
        finalize_label();

        out_ << ' ' << value;
    }

    template <class T, typename Counter>
    void metric(const Summary<T, Counter>& value)
    {
    }


    void reset()
    {
        labels_ = 0;
    }
};

}}}
