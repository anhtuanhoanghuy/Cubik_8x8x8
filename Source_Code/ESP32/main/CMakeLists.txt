# Quét tất cả file .c trong thư mục main/
file(GLOB COMPONENT_SRCS "uart.c" "WebServer.c" "*.c")

# Đăng ký component với ESP-IDF
idf_component_register(
    SRCS "main.c"
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "../Include"
    EMBED_TXTFILES 
        "Admin.html" 
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