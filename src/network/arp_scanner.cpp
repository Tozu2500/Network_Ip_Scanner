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

}  // namespace netscanner