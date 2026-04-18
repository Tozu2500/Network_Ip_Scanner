#ifndef ADAPTER_INFO_H
#define ADAPTER_INFO_H

#include "device.h"
#include <vector>

namespace netscanner {

// This function enumerates all network adapters and filters to the adapters that have a valid
// IPv4 gateway (mainly when connected to a network)
std::vector<AdapterInfo> get_active_adapters();ecvt

uint32_t ip_to_uint32(const std::string& ip);

std::string uint32_to_ip(uint32_t ip);

} // netscanner (namespace)

#endif // ADAPTER_INFO_H