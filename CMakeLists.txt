cmake_minimum_required(VERSION 2.8.9)
project (monitoring_server)

set(monitoring_server_VERSION_MAJOR 0)
set(monitoring_server_VERSION_MINOR 3)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic")

# find_package (Threads REQUIRED)

add_library(serverConfig STATIC src/serverConfig.c)

#find_library(LIBUTILS utils)

#if ( NOT LIBUTILS )
#  message(FATAL_ERROR "utils library not found")
#endif()

# add_subdirectory(modules/module)

add_executable(monitoring_server src/main.c)
# add_dependencies(monitoring_server )
target_link_libraries(monitoring_server serverConfig)
target_link_libraries(monitoring_server pthread)
# target_link_libraries(monitoring_server -lutils)

# install(TARGETS monitoring_server DESTINATION bin)
