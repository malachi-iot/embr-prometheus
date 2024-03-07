#include <iostream>

#include <estd/internal/streambuf.h>
#include <estd/ostream.h>

#include <embr/prometheus.h>
#include <embr/prometheus/internal/server.h>
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

        oa.metric(request_count);
        client_fd.shutdown(SHUT_RDWR);
        client_fd.close();
    }

    return 0;
}
