#pragma once

#include "labels.h"

namespace embr::prometheus::internal {

// Remember usually Metric is a const ref but once in a while it's an inline class
template <class ...Args>
struct ContextBase
{
    static constexpr bool has_labels = true;

    using labels_type = Labels<Args...>;

    const char* name_;
    const labels_type labels_;

    constexpr ContextBase(const char* name, const labels_type& labels) :
        name_{name},
        labels_{labels}
    {}

    constexpr ContextBase(const char* name, const char** label_names, Args&&...args) :
        name_{name},
        labels_{label_names, std::forward<Args>(args)...}
    {}
};

template <>
struct ContextBase<>
{
    static constexpr bool has_labels = false;

    const char* name_;

    constexpr explicit ContextBase(const char* name) : name_{name}    {}
};

template <class Metric, class ...Args>
struct Context : ContextBase<Args...>
{
    using base_type = ContextBase<Args...>;

    const Metric metric_;

    constexpr Context(const Metric& metric, const char* name, const Labels<Args...>& labels) :
        base_type(name, labels),
        metric_{metric}
    {}

    constexpr Context(const Metric& metric, const char* name) :
        base_type(name),
        metric_{metric}
    {}
};


// DEBT: Inconsistent convention
template <class Metric, class ...Args>
struct metric_put_core : estd::internal::ostream_functor_tag
{
    Context<Metric, Args...> context_;
    const char* help_ = nullptr;

    constexpr metric_put_core(const Metric& metric, const char* name, const char* help,
        const Labels<Args...>& labels) :
        context_{metric, name, labels},
        help_{help}
    {}

    constexpr metric_put_core(const Metric& metric, const char* name, const char* help)
        :
        context_{metric, name},
        help_{help}
    {}

    template <class Streambuf, class Base>
    void operator()(estd::detail::basic_ostream<Streambuf, Base>& out) const;
};


}
