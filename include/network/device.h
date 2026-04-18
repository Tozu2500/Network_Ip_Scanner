#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <cstdint>
#include <ctime>

namespace netscanner {

// This struct represents a single device, that is discovered on the network
struct Device {
    std::string ip_address;
    std::string mac_address;
    std::string hostname;
    std::string vendor;
    bool is_gateway = false;
    bool is_local = false;
    time_t first_seen = 0;
    time_t last_seen = 0;
};

// Local Adapter information
struct AdapterInfo {
    std::string name;
    std::string description;
    std::string ip_address;
    std::string subnet_mask;
    std::string gateway_ip;
    std::string mac_address;
    uint32_t ip_numeric = 0;    // Host-byte-order IP
    uint32_t mask_numeric = 0;  // Host-byte-order mask
    uint32_t network_addr = 0;  // IP & Mask
    uint32_t broadcast = 0;     // network | ~mask
};

}  // namespace netscanner

#endif // DEVICE_H