#pragma once

#include <estd/tuple.h>

// DEBT: These labels helpers need better names and obviously
// a mismatch for 'internal' is going on here

namespace embr::prometheus {

// DEBT: This is just clumsy + needs documentation
extern const char** label_names;


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
    //std::tuple<Args...> values;

    Labels2(const Labels2&) = default;

    constexpr Labels2(const char** n, const estd::tuple<Args...>& v) :
        names{n},
        values{v}
    {}

    explicit constexpr Labels2(const char** n, Args&&... v) :
        names{n},
        values(std::forward<Args>(v)...)
    {}
};


}
