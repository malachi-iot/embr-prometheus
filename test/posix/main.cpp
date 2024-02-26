#include <iostream>

#include <estd/internal/streambuf.h>
#include <estd/ostream.h>

#include <embr/prometheus.h>
#include <embr/internal/histogram.h>

#include "embr/socket.h"

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
    struct sockaddr_in server_addr, client_addr;
    posix2_ostream out(1);
    //embr::posix::Socket ss(AF_INET, SOCK_STREAM);

    out << "Hello World! " << sizeof(out) << estd::endl;

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        // fails on windows, presumably a permission thing... except we aren't even listening yet
        perror("socket failed");
        return EXIT_FAILURE;
    }

    embr::posix::Socket ss(server_fd);

    server_addr.sin_family = AF_INET;
#if __WIN64__
    server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
    server_addr.sin_addr.s_addr = INADDR_ANY;
#endif
    server_addr.sin_port = htons(9100);

    if(ss.bind(&server_addr) < 0)
    {
        perror("bind failed");
        return EXIT_FAILURE;
    }

    if(listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        return EXIT_FAILURE;
    }

    embr::prometheus::Counter<unsigned> request_count;

    for(;;)
    {
        socklen_t client_addr_size = sizeof(client_addr);

        out << "Awaiting incoming client" << estd::endl;

        embr::Socket client_fd = ss.accept(&client_addr, &client_addr_size);

        if (!client_fd.valid())
        {
            perror("accept failed");
            continue;
        }

        request_count.inc();

        posix2_ostream client_out(client_fd);

        embr::prometheus::OutAssist2<posix2_ostream> oa(client_out, "request");

        client_out << "HTTP/1.1 200 OK" << estd::endl; // << estd::endl;
        client_out << "Content-Type: text/html" << estd::endl << estd::endl;

        //client_out << "hi2u" << estd::endl;
        oa.metric(request_count);
        client_fd.close();
    }

    return 0;
}
