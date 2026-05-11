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

        ss << std::uppercase << std::hex << std::setfill("0") << std::setw(2)
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
}

}  // namespace netscanner