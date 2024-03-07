#pragma once

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

    constexpr Labels2(const char** n, const estd::tuple<Args...>& v) :
        names{n},
        values{v}
    {}

    explicit constexpr Labels2(const char** n, Args... v) :
        names{n},
        values{v...}
    {}
};


}