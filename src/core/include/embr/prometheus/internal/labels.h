#pragma once

#include <estd/tuple.h>

// DEBT: These labels helpers need better names and obviously
// a mismatch for 'internal' is going on here

namespace embr::prometheus {

// DEBT: This is just clumsy + needs documentation
extern const char** label_names;


template <class ...Args>
struct Labels
{
    const char* const* names;
    estd::tuple<Args...> values;
    //std::tuple<Args...> values;

    Labels(const Labels&) = default;

    constexpr Labels(const char* n[], const estd::tuple<Args...>& v) :
        names{n},
        values{v}
    {}

    explicit constexpr Labels(const char* n[], Args&&... v) :
        names{n},
        values(std::forward<Args>(v)...)
    {}

    constexpr Labels(
        const std::initializer_list<const char*>& n,
        Args&&...v) :
        names{data(n)},
        values{std::forward<Args>(v)...}
    {
        assert(n.size() >= sizeof...(Args));
    }
};

//template <class ...Args> Labels(const char**, Args&&...args) ->
    //Labels<const char**, Args...>;

template <class ...Args>
using Labels2 = Labels<Args...>;

template <class ...Args>
Labels<Args...> make_labels(const char** n, Args&&...args)
{
    return Labels<Args...>(n, std::forward<Args>(args)...);
}

template <class ...Args>
struct Labels3
{
    //estd::tuple<Args...> name_and_value_pairs;

    //Labels3(const std::initializer_list<estd::pair<const char*, auto> >& list)
    //{

    //}
};


}
