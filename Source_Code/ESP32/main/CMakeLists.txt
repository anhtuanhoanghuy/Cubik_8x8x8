# Quét tất cả file .c trong thư mục main/
file(GLOB COMPONENT_SRCS "Monitoring.c" "uart.c" "Bluetooth.c" "app.c" "control.c" "Utils.c" "Wifi.c" "*.c")

# Đăng ký component với ESP-IDF
idf_component_register(
    SRCS "main.c"
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "../Include"
    REQUIRES
        esp_wifi
        esp_event
        esp_http_server
        nvs_flash
        json
        bt
        mqtt
        driver
)