#pragma once

namespace embr {

namespace prometheus {

void synthetic();

// Trying for
// https://stackoverflow.com/questions/38370986/how-to-pass-variadic-amount-of-stdpair-with-different-2nd-types-to-a-functio
// https://stackoverflow.com/questions/56367136/expand-variadic-template-pack-by-pairs

template <class ...A>
struct tester;

template <>
struct tester<>
{

};


/*
template <class T> //, class ...A>
struct tester<T> //, A...> : tester<A...>
{
    //using base_type = tester<A...>;
    using kvp = std::pair<const char*, T>;

    kvp value_;

    //template <class ...Args>
    constexpr explicit tester(kvp v) : //, Args&&...args) :
        value_{v}//,
        //base_type(std::forward<Args>(args)...)
    {}
};  */

template <class T, class ...A>
struct tester<T, A...> : tester<A...>
{
    using base_type = tester<A...>;
    using kvp = std::pair<const char*, T>;

    kvp value_;

    // DEBT: I think c++17 onward lets us get rid of 'template' part of this syntax
    template <class ...Args>
    constexpr explicit tester(kvp v, Args&&...args) :
        value_{v},
        base_type(std::forward<Args>(args)...)
    {}
};

//template <class T>
//tester(std::pair<const char*, T>) -> tester<T>;

template <class T, class ...A, class ...Args>
tester(std::pair<const char*, T>, Args&&...) -> tester<T, A...>;


}

}
