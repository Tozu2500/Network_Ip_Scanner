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

}  // namespace netscanner