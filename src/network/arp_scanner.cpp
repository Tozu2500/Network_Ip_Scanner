#include "../../include/network/adapter_info.h"
#include "../../include/network/arp_scanner.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <cstring>
#include <sstream>
#include <iomanip>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

namespace netscanner {

std::string format_mac(const unsigned char* mac, size_t len) {
    std::ostringstream ss;

    for (size_t i = 0; i < len; ++i) {
        if (i > 0) ss << ":";

        ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(mac[i]);
    }

    return ss.str();
}

std::vector<Device> read_arp_table() {
    std::vector<Device> devices;

    ULONG table_size = 0;
    // First call to determine required buffer size
    GetIpNetTable(nullptr, &table_size, FALSE);
    if (table_size == 0) return devices;

    std::vector<unsigned char> buffer(table_size);
    PMIB_IPNETTABLE arp_table = reinterpret_cast<PMIB_IPNETTABLE>(buffer.data());

    if (GetIpNetTable(arp_table, &table_size, TRUE) != NO_ERROR) {
        return devices;
    }

    for (DWORD i = 0; i < arp_table->dwNumEntries; ++i) {
        MIB_IPNETROW& row = arp_table->table[i];

        // Here, skip invalid entries and only keep dynamic (learned) and static entries
        if (row.dwType != MIB_IPNET_TYPE_DYNAMIC && row.dwType != MIB_IPNET_TYPE_STATIC) {
            continue;
        }

        Device dev;

        // IP
        struct in_addr addr;
        addr.s_addr = static_cast<u_long>(row.dwAddr);
        char ip_buf[INET_ADDRSTRLEN];

        if (inet_ntop(AF_INET, &addr, ip_buf, sizeof(ip_buf))) {
            dev.ip_address = ip_buf;
        }

        // MAC
        dev.mac_address = format_mac(row.bPhysAddr, row.dwPhysAddrLen);

        // Skip multicast (224.0.0.0/4) and broadcast entries — these aren't
        // real devices and would otherwise flood the intrusion check with
        // false "unknown device" alerts on every scan.
        unsigned char first_octet = reinterpret_cast<unsigned char*>(&addr.s_addr)[0];
        bool is_multicast_ip = first_octet >= 224 && first_octet <= 239;
        bool is_broadcast_ip = dev.ip_address == "255.255.255.255";
        bool is_broadcast_mac = dev.mac_address == "FF:FF:FF:FF:FF:FF";
        if (is_multicast_ip || is_broadcast_ip || is_broadcast_mac) {
            continue;
        }

        dev.last_seen = time(nullptr);
        dev.first_seen = dev.last_seen;

        devices.push_back(dev);
    }

    return devices;
}

std::string send_arp_request(const std::string& ip) {
    struct in_addr dest_addr;
    if (inet_pton(AF_INET, ip.c_str(), &dest_addr) != 1) {
        return "";
    }

    ULONG mac_addr[2] = {0};
    ULONG mac_len = 6;

    DWORD ret = SendARP(dest_addr.s_addr, 0, mac_addr, &mac_len);
    if (ret == NO_ERROR && mac_len > 0) {
        unsigned char* mac_bytes = reinterpret_cast<unsigned char*>(mac_addr);
        return format_mac(mac_bytes, mac_len);
    }

    return "";
}

std::string resolve_hostname(const std::string& ip) {
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &sa.sin_addr);

    char host[NI_MAXHOST];
    if (getnameinfo(reinterpret_cast<struct sockaddr*>(&sa), sizeof(sa),
                host, sizeof(host), nullptr, 0, NI_NAMEREQD) == 0) {
        return std::string(host);
    }
    return "N/A";
}

}  // namespace netscanner