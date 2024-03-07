#include <stdio.h>

#include <embr/prometheus.h>
#include <embr/posix/streambuf.h>
#include <embr/prometheus/internal/server.h>

using namespace embr::prometheus;

Counter<unsigned> request_count;

extern "C" void app_main(void)
{
    MiniHttpServer server;

    // TODO: Still have to pull in esp_helper or something else to get WiFi & TCP online
    // Until that point, we just abort
    return;

    server.start(9100);

    for(;;)
    {
        embr::Socket client_fd = server.accept();

        if (!client_fd.valid())
        {
            perror("accept failed");
            continue;
        }

        request_count.inc();

        posix_ostream client_out(client_fd);

        http_respond_ok(client_out);

        client_out << put_metric(request_count, "request_count");

        client_fd.close();
    }
}
