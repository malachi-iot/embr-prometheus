#include "esp_log.h"
#include "nvs_flash.h"

#include <embr/prometheus.h>
#include <embr/posix/streambuf.h>
#include <embr/prometheus/internal/server.h>

void wifi_init_sta();

using namespace embr::prometheus;

Counter<unsigned> request_count;
int arbitrary_gauge = 0;

static const char* TAG = "prometheus::app";

extern "C" void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // NOTE: Prometheus scrapes imply a fixed IP address.  You might consider
    // a DHCP reservation for your ESP32.
    wifi_init_sta();

    MiniHttpServer server;

    ESP_LOGI(TAG, "Starting prometheus listener");

    if(server.start(9100) != EXIT_SUCCESS)
        ESP_LOGW(TAG, "Couldn't start listener");

    for(;;)
    {
        embr::Socket client_fd = server.accept();

        if (!client_fd.valid())
        {
            perror("accept failed");
            continue;
        }

        ++request_count;
        arbitrary_gauge = -request_count.value();

        posix_ostream client_out(client_fd);

        // We'll just treat any old request content as valid
        //static char buf[4096];
        //int sz = client_out.rdbuf()->sgetn(buf, sizeof(buf));
        int sz = 0;

        ESP_LOGD(TAG, "Connection received: (req sz=%d)", sz);

        http_respond_ok(client_out);

        client_out << put_metric(request_count, "request_count");
        client_out << put_metric_uptime();
        client_out << put_metric_gauge(arbitrary_gauge, "arbitrary");

        // NOTE: SHUT_RDWR seems to wait for input side of socket to empty.  Totally
        // reasonable, but we don't want to pay attention.
        client_fd.shutdown(SHUT_WR);
        client_fd.close();
    }
}
