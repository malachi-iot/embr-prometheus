#pragma once

#include <estd/ostream.h>

// existing posix_streambuf uses FILE.  Ultimately we probably prefer raw int file descriptors

namespace embr::prometheus {

namespace impl {

// NOTE: Normally we like to inline these since the footprint of streambuf methods tends to
// be small.  In this case, we prefer to hide the easily-collided posix socket/file descriptor
// signatures - although less of a concern in c++

template <class Traits>
class posix_streambuf : public estd::internal::impl::streambuf_base<Traits>
{
    using base_type = estd::internal::impl::streambuf_base<Traits>;
    //using base_type::streamsize;
    int fd_;

public:
    constexpr posix_streambuf(int fd) : fd_{fd}  {}
    using typename base_type::char_type;

    estd::streamsize xsputn(const char_type* s, estd::streamsize count);
    int sputc(char_type);
};

}

template <class Traits>
using posix_streambuf = estd::detail::streambuf<impl::posix_streambuf<Traits> >;

using posix_ostream = estd::detail::basic_ostream<posix_streambuf<estd::char_traits<char> > >;

void stoker();

}
