#include "../../include/network/ping_sweep.h"
#include "../../include/network/arp_scanner.h"
#include "../../include/network/adapter_info.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <vector>
#include <string>
#include <cstring>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

namespace netscanner {

bool ping_host(const std::string& ip, int timeout_ms) {
    HANDLE icmp_handle = IcmpCreateFile();

    if (icmp_handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    struct in_addr dest;
    if (inet_pton(AF_INET, ip.c_str(), &dest) != 1) {
        return false;
    }

    const char send_data[] = "ping";
    DWORD reply_size = sizeof(ICMP_ECHO_REPLY) + sizeof(send_data) + 8;
    
}

}  // namespace netscanner