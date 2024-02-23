#include <iostream>

#include <estd/internal/streambuf.h>
#include <estd/ostream.h>

#if __WIN64__
#include <winsock2.h>
#else
#include <inaddr.h>
#include <sys/socket.h>
#endif

#include <sys/unistd.h>

//#include <embr/prometheus.h>
#include <embr/internal/histogram.h>

// existing posix_streambuf uses FILE.  Ultimately we probably prefer raw int file descriptors

namespace impl {

template <class Traits>
class posix2_streambuf : public estd::internal::impl::streambuf_base<Traits>
{
    using base_type = estd::internal::impl::streambuf_base<Traits>;
    //using base_type::streamsize;
    int fd_;

public:
    constexpr posix2_streambuf(int fd) : fd_{fd}  {}
    using typename base_type::char_type;

    estd::streamsize xsputn(const char_type* s, estd::streamsize count);
    int sputc(char_type);
};

template <class Traits>
estd::streamsize posix2_streambuf<Traits>::xsputn(const char_type* s, estd::streamsize count)
{
    int ret = write(fd_, s, count);

    // DEBT: Hmm how do we register an error or blocking condition here...
    return ret < 0 ? 0 : ret;
}

template <class Traits>
int posix2_streambuf<Traits>::sputc(char_type c)
{
    if(write(fd_, &c, 1) == 1) return c;

    return Traits::eof();
}

}

using posix2_streambuf = estd::detail::streambuf<impl::posix2_streambuf<estd::char_traits<char> > >;
using posix2_ostream = estd::detail::basic_ostream<posix2_streambuf>;

using namespace std;

// Guidance from https://dev.to/jeffreythecoder/how-i-built-a-simple-http-server-from-scratch-using-c-739

int main()
{
    int server_fd;
    struct sockaddr_in server_addr;
    posix2_ostream out(1);

    out << "Hello World! " << sizeof(out) << estd::endl;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // fails on windows, presumably a permission thing... except we aren't even listening yet
        perror("socket failed");
        return EXIT_FAILURE;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    server_addr.sin_port = 80;

    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        return EXIT_FAILURE;
    }

    if(listen(server_fd, 10) < 0)
    {
        perror("listen failed");
        return EXIT_FAILURE;
    }

    return 0;
}
