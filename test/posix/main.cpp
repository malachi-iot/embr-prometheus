#include <iostream>

#include <estd/internal/streambuf.h>
#include <estd/ostream.h>

#include <embr/prometheus.h>
#include <embr/internal/histogram.h>
#include <embr/posix/socket.h>
#include <embr/posix/streambuf.h>

using posix_ostream = embr::prometheus::posix_ostream;

using namespace std;

// Guidance from https://dev.to/jeffreythecoder/how-i-built-a-simple-http-server-from-scratch-using-c-739

void announce(posix_ostream& out)
{
    //embr::prometheus::stoker();

    //embr::posix::Socket ss(AF_INET, SOCK_STREAM);

    //out << "Hello World! " << sizeof(out) << estd::endl;

    out << "Starting prometheus synthetic server" << estd::endl;
}


class MiniHttpServer
{
    embr::posix::Socket ss;

public:
    int start(int port)
    {
        struct sockaddr_in server_addr;
        int server_fd;

        if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            // fails on windows, presumably a permission thing... except we aren't even listening yet
            perror("socket failed");
            return EXIT_FAILURE;
        }

        server_addr.sin_family = AF_INET;
    #if __WIN64__
        server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
    #else
        server_addr.sin_addr.s_addr = INADDR_ANY;
    #endif
        server_addr.sin_port = htons(port);

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

        ss = server_fd;

        return EXIT_SUCCESS;
    }

    embr::posix::Socket accept()
    {
        struct sockaddr_in client_addr;

        socklen_t client_addr_size = sizeof(client_addr);

        return ss.accept(&client_addr, &client_addr_size);
    }
};

template <class Streambuf, class Base>
void http_respond_ok(estd::detail::basic_ostream<Streambuf, Base>& out)
{
    out << "HTTP/1.1 200 OK" << estd::endl; // << estd::endl;
    out << "Content-Type: text/html" << estd::endl << estd::endl;
}


int main()
{
    posix_ostream out(1);

    announce(out);

    MiniHttpServer server;

    server.start(9100);

    embr::prometheus::Counter<unsigned> request_count;

    for(;;)
    {
        out << "Awaiting incoming client" << estd::endl;

        embr::Socket client_fd = server.accept();

        if (!client_fd.valid())
        {
            perror("accept failed");
            continue;
        }

        request_count.inc();

        posix_ostream client_out(client_fd);

        embr::prometheus::OutAssist2<posix_ostream> oa(client_out, "request");

        http_respond_ok(client_out);

        //client_out << "hi2u" << estd::endl;
        oa.metric(request_count);
        client_fd.close();
    }

    return 0;
}
