#ifndef PING_SWEEP_H
#define PING_SWEEP_H

#include "device.h"
#include <vector>
#include <functional>

namespace netscanner {

// Callback fired after each IP probed. Parameters: IDX, TOTAL COUNT
using ProgressCallback = std::function<void(int current, int total)>;

// Ping sweeping all hosts in the subnet, after sweeping read ARP table for results.
// Optional callback reports progress.
std::vector<Device> ping_sweep(const AdapterInfo& adapter,
                                int timeout_ms = 100,
                                ProgressCallback progress = nullptr);

// Ping a single IP, return true if host responds
bool ping_host(const std::string& ip, int timeout_ms = 100);

}  // Namespace netscanner

#endif // PING_SWEEP_H