#ifndef ADAPTER_INFO_H
#define ADAPTER_INFO_H

#include "device.h"
#include <vector>

namespace netscanner {

// Enumerates all active network adapters and returns their info.
// Filters to adapters that have a valid IPv4 gateway (i.e. connected to a network).
std::vector<AdapterInfo> get_active_adapters();

// Convert a dotted-decimal IP string to a host-byte-order uint32_t.
uint32_t ip_to_uint32(const std::string& ip);

// Convert a host-byte-order uint32_t to a dotted-decimal IP string.
std::string uint32_to_ip(uint32_t ip);

} // namespace netscanner

#endif // ADAPTER_INFO_H
