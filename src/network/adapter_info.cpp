#include "include/network/adapter_info.h"

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

uint32_t ip_to_uint32(const std::string& ip) {
    struct in_addr addr;
    if (inet_pton(AF_INET, ip.c_str(), &addr) == 1) {
        return ntohl(addr.s_addr);
    }
    return 0;
}

std::string uint32_to_ip(uint32_t ip) {
    struct in_addr addr;
    addr.s_addr = htonl(ip);
    char buf[INET_ADDRSTRLEN];

    if (inet_ntop(AF_INET, &addr, buf, sizeof(buf))) {
        return std::string(buf);
    }
    return "0.0.0.0";
}

std::vector<AdapterInfo> get_active_adapters() {
    std::vector<AdapterInfo> result;

    ULONG buf_len = 15000;
    PIP_ADAPTER_INFO adapter_list = nullptr;

    // Allocate and call GetAdaptersInfo and resizing if required
    for (int attempt = 0; attempt < 3; ++attempt) {
        adapter_list = reinterpret_cast<PIP_ADAPTER_INFO>(new unsigned char[buf_len]);
        DWORD ret = GetAdaptersInfo(adapter_list, &buf_len);

        if (ret == ERROR_SUCCESS) {
            break;
        }

        delete[] reinterpret_cast<unsigned char*>(adapter_list);
        adapter_list = nullptr;

        if (ret != ERROR_BUFFER_OVERFLOW) {
            return result;
        }
    }

    if (!adapter_list) return result;

    PIP_ADAPTER_INFO current = adapter_list;

    while (current) {
        std::string gateway = current->GatewayList.IpAddress.String;

        // Skip the adapters without a real gateway
        if (!gateway.empty() && gateway != "0.0.0.0") {
            AdapterInfo info;
            info.name = current->AdapterName;
            info.description = current->Description;
            info.ip_address = current->IpAddressList.IpAddress.String;
            info.subnet_mask = current->IpAddressList.IpMask.String;
            info.gateway_ip = gateway;

            // MAC Format
            std::ostringstream mac_ss;
            for (UINT i = 0; i < current->AddressLength; ++i) {
                if (i > 0) mac_ss << ":";
                mac_ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2)
                        << static_cast<int>(current->Address[i]);
            }

            info.mac_address = mac_ss.str();

            info.ip_numeric = ip_to_uint32(info.ip_address);
            info.mask_numeric = ip_to_uint32(info.subnet_mask);
            info.network_addr = info.ip_numeric & info.mask_numeric;
            info.broadcast = info.network_addr | (~info.mask_numeric);

            result.push_back(info);
        }

        current = current->Next;
    }

    delete[] reinterpret_cast<unsigned char*>(adapter_list);
    return result;
}

}  // namespace netscanner