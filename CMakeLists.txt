cmake_minimum_required(VERSION 3.15)
project(NetworkIpScanner CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT WIN32)
    message(FATAL_ERROR "This project uses the Windows networking APIs (winsock2, iphlpapi) and can only be built for Windows.")
endif()

add_executable(netscanner
    src/main.cpp
    src/ui/console_ui.cpp
    src/ui/table_formatter.cpp
    src/network/adapter_info.cpp
    src/network/arp_scanner.cpp
    src/network/ping_sweep.cpp
    src/oui/oui_lookup.cpp
)

target_include_directories(netscanner PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(netscanner PRIVATE ws2_32 iphlpapi)

if(MSVC)
    target_compile_options(netscanner PRIVATE /W4)
else()
    target_compile_options(netscanner PRIVATE -Wall -Wextra)
endif()
