#include <esp_log.h>

#include "mdns.h"

void initialize_mdns()
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(mdns_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(mdns_hostname_set(CONFIG_LWIP_LOCAL_HOSTNAME));
    ESP_ERROR_CHECK_WITHOUT_ABORT(mdns_instance_name_set("embr prometheus test app"));
    ESP_ERROR_CHECK_WITHOUT_ABORT(mdns_service_add("prometheus exporter",
        "prometheus_http", "_tcp", 9100, nullptr, 0));
}