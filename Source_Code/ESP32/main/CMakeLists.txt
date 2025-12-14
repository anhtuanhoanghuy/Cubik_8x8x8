# Quét tất cả file .c trong thư mục main/
file(GLOB COMPONENT_SRCS "*.c")

# Đăng ký component với ESP-IDF
idf_component_register(
    SRCS "main.c"
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "../Include"
)