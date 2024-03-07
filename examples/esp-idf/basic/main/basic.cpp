#include "esp_log.h"
#include "nvs_flash.h"

#include <embr/prometheus.h>
#include <embr/posix/streambuf.h>
#include <embr/prometheus/internal/server.h>

void wifi_init_sta();

using namespace embr::prometheus;

Counter<unsigned> request_count;

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

        ESP_LOGD(TAG, "Connection received");

        ++request_count;

        posix_ostream client_out(client_fd);

        http_respond_ok(client_out);

        client_out << put_metric(request_count, "request_count");
        client_out << put_metric_uptime();

        client_fd.shutdown();     // Doesn't seem to really change things
        client_fd.close();
    }
}
