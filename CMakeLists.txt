cmake_minimum_required(VERSION 2.8.9)
project (monitoring_server)

set(monitoring_server_VERSION_MAJOR 0)
set(monitoring_server_VERSION_MINOR 5)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic")

find_package (Threads REQUIRED)

add_library(logger STATIC src/logger.c)
add_library(rw_utils STATIC src/rw_utils.c)
add_library(serverConfig STATIC src/serverConfig.c)

add_executable(monitoring_server src/main.c)
target_link_libraries(monitoring_server logger)
target_link_libraries(monitoring_server rw_utils)
target_link_libraries(monitoring_server serverConfig)

add_executable(client src/client.c)
target_link_libraries(client rw_utils)
target_link_libraries(client pthread)

# install(TARGETS monitoring_server DESTINATION bin)
