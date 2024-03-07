#include <iostream>

#include "embr/posix/socket.h"
#include "embr/posix/streambuf.h"

namespace embr::prometheus {

const char** label_names = nullptr;

void synthetic()
{
    std::cout << "Hello" << std::endl;
}

namespace impl {

template <class Traits>
estd::streamsize posix_streambuf<Traits>::xsputn(const char_type* s, estd::streamsize count)
{
    int ret = write(fd_, s, count);

    // DEBT: Hmm how do we register an error or blocking condition here...
    return ret < 0 ? 0 : ret;
}

template <class Traits>
int posix_streambuf<Traits>::sputc(char_type c)
{
    if(write(fd_, &c, 1) == 1) return c;

    return Traits::eof();
}

}

void stoker()
{
    posix_streambuf<estd::char_traits<char> > s(0);
    posix_streambuf<std::char_traits<char> > s2(0);
    posix_ostream o1(0);

    o1 << "hello" << '!';
}

}


