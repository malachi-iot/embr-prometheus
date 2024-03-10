#include <chrono>
#include <iostream>

#include <estd/internal/streambuf.h>
#include <estd/ostream.h>

#include <embr/prometheus.h>
#include <embr/prometheus/internal/server.h>
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

    using time_point = std::chrono::system_clock::time_point;
    time_point last_request = time_point::max();

    embr::prometheus::Counter<unsigned> request_count;
    embr::prometheus::Histogram<unsigned, unsigned, 0, 5, 30, 60> request_interval;

    for(;;)
    {
        out << "Awaiting incoming client" << estd::endl;

        embr::Socket client_fd = server.accept();

        if (!client_fd.valid())
        {
            perror("accept failed");
            continue;
        }

        const time_point now = std::chrono::system_clock::now();

        if(last_request != time_point::max())
        {
            auto interval = std::chrono::duration_cast<std::chrono::seconds>(now - last_request);

            request_interval.observe(interval.count());
        }

        last_request = now;

        posix_ostream client_out(client_fd);

        embr::prometheus::posix_istream client_in(client_fd);

        //static char buf[1024];

        //client_in.read(buf, sizeof(buf));

        http_respond_ok(client_out);

        client_out << put_metric(++request_count, "request", "help");
        client_out << put_metric(request_interval, "request_interval",
            "How much time passed between incoming request",
            {"inst", "str"}, 0, (const char*)"hello2u");

        client_fd.shutdown(SHUT_RDWR);
        //client_fd.close();
    }

    return 0;
}
