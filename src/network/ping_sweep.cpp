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
    std::vector<unsigned char> reply_buf(reply_size);

    DWORD ret = IcmpSendEcho(icmp_handle, dest.s_addr,
                            const_cast<LPVOID>(static_cast<const void*>(send_data)),
                            sizeof(send_data),
                            nullptr,
                            reply_buf.data(),
                            reply_size,
                            static_cast<DWORD>(timeout_ms));

    IcmpCloseHandle(icmp_handle);

    if (ret > 0) {
        PICMP_ECHO_REPLY echo_reply = reinterpret_cast<PICMP_ECHO_REPLY>(reply_buf.data());
        return echo_reply->Status == IP_SUCCESS;
    }

    return false;

}

std::vector<Device> ping_sweep(const AdapterInfo& adapter,
                                    int timeout_ms,
                                    ProgressCallback progress) {
    // Calculate number of hosts on the subnet
    uint32_t first_host = adapter.network_addr + 1;
    uint32_t last_host = adapter.broadcast - 1;

    if (first_host >= last_host) {
        return read_arp_table();
    }

    int total = static_cast<int>(last_host - first_host + 1);

    // Cap at /20 (4094 hosts) to avoid superduper long scans
    if (total > 4094) {
        total = 4094;
        last_host = first_host + 4093;
    }

    int idx = 0;
    for (uint32_t host_ip = first_host; host_ip <= last_host; ++host_ip) {
        std::string ip = uint32_to_ip(host_ip);
        ping_host(ip, timeout_ms);  // Don't care about result here, just fill ARP cache
        
        ++idx;
        
        if (progress) {
            progress(idx, total);
        }
    }

    // Now, we read the ARP table which should be fully populated with responses :)
    std::vector<Device> devices = read_arp_table();

    // Mark the gateway and local device
    for (auto& dev : devices) {
        if (dev.ip_address == adapter.gateway_ip) {
            dev.is_gateway = true;
        }

        if (dev.ip_address == adapter.ip_address) {
            dev.is_local = true;
        }
    }

    return devices;
}

}  // namespace netscanner