#ifndef ARP_SCANNER_H
#define ARP_SCANNER_H

#include "device.h"
#include <vector>
#include <string>

namespace netscanner {

// Read the current system ARP cache and return entries
std::vector<Device> read_arp_table();

// Send an ARP request for a specific IP. Returns the MAC address if reachable and empty otherwise
std::string send_arp_request(const std::string& ip);

// Resolve hostname via reverse DNS lookup
std::string resolve_hostname(const std::string& ip);

// Format a 6-byte MAC to "AA:BB:CC:DD:EE:FF"
std::string format_mac(const unsigned char* mac, size_t len);

}  // namespace netscanner

#endif // ARP_SCANNER_H