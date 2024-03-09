#pragma once

#include "labels.h"

namespace embr::prometheus::internal {

// EXPERIMENTAL, for optimization
// Remember usually Metric is a const ref but once in a while it's an inline class
template <class Metric, class ...Args>
struct Context
{
    static constexpr bool has_labels = true;

    const char* name_;
    Metric metric_;
    const Labels<Args...>& labels_;
};

template <class Metric>
struct Context<Metric>
{
    static constexpr bool has_labels = false;

    const char* name_;
    Metric metric_;
};


// DEBT: Inconsistent convention
template <class Metric, class ...Args>
struct metric_put_core : estd::internal::ostream_functor_tag
{
    const Metric metric_;
    const char* name_;
    const char* help_ = nullptr;
    // DEBT: Make this a pointer
    Labels<Args...> labels_;

    constexpr metric_put_core(const Metric& metric, const char* name, const char* help) :
        metric_{metric},
        name_{name},
        help_{help},
        // DEBT: Probably wanna null label names out here
        labels_{label_names}
    {}

    constexpr metric_put_core(const Metric& metric, const char* name, const char* help,
        const Labels<Args...>& labels) :
        metric_{metric},
        name_{name},
        help_{help},
        labels_{labels}
    {}

    template <class Streambuf, class Base>
    void operator()(estd::detail::basic_ostream<Streambuf, Base>& out) const;
};


}
