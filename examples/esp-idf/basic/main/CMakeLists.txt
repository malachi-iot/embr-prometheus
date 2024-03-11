idf_component_register(SRCS "basic.cpp" "wifi.cpp" "mdns.cpp"
    INCLUDE_DIRS "."
    REQUIRES estd embr-prometheus nvs_flash esp_timer esp_wifi)
