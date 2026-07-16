#include "../../include/ui/console_ui.h"
#include "../../include/ui/table_formatter.h"
#include "../../include/network/arp_scanner.h"
#include "../../include/network/ping_sweep.h"
#include "../../include/utils/string_utils.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

namespace netscanner {

// Windows Console Colors
enum ConsoleColor {
    COLOR_DEFAULT    = 7,
    COLOR_RED        = 12,
    COLOR_GREEN      = 10,
    COLOR_YELLOW     = 14,
    COLOR_CYAN       = 11,
    COLOR_MAGENTA    = 13,
    COLOR_WHITE      = 15,
    COLOR_DARK_GRAY  = 8,
    COLOR_BLUE       = 9,
};

// Construction / Destruction

ConsoleUI::ConsoleUI() {}

ConsoleUI::~ConsoleUI() {
    WSACleanup();
}

// Console Helpers

void ConsoleUI::set_color(int color) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(h, static_cast<WORD>(color));
}

void ConsoleUI::reset_color() {
    set_color(COLOR_DEFAULT);
}

void ConsoleUI::clear_screen() {
    // Use system("cls") — simple, reliable on Windows console
    system("cls");
}

void ConsoleUI::pause() {
    std::cout << "\n";
    set_color(COLOR_DARK_GRAY);
    std::cout << "  Press Enter to continue...";
    reset_color();
    std::string dummy;
    std::getline(std::cin, dummy);
}

// Initialization

void ConsoleUI::init_console() {
    // Enable virtual terminal sequences for Windows 10+
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    // Set console title
    SetConsoleTitleA("WiFi Network Scanner v1.0");

    // Initialize WinSock
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
}

void ConsoleUI::load_oui_database() {
    // Try loading from file first (look next to executable, then in data/)
    if (oui_.load_from_file("data\\oui_prefixes.csv") ||
        oui_.load_from_file("oui_prefixes.csv")) {
        return;
    }
    // Fall back to built-in database
    oui_.load_builtin();
}

void ConsoleUI::detect_adapters() {
    adapters_ = get_active_adapters();
}

// Banner

void ConsoleUI::show_banner() {
    set_color(COLOR_CYAN);
    std::cout << R"(
   __        ___  _____ _   ____
   \ \      / (_)|  ___(_) / ___|  ___ __ _ _ __  _ __   ___ _ __
    \ \ /\ / /| || |_  | | \___ \ / __/ _` | '_ \| '_ \ / _ \ '__|
     \ V  V / | ||  _| | |  ___) | (_| (_| | | | | | | |  __/ |
      \_/\_/  |_||_|   |_| |____/ \___\__,_|_| |_|_| |_|\___|_|
    )" << "\n";
    reset_color();
    set_color(COLOR_YELLOW);
    std::cout << "    Network Intrusion Detection Scanner for Windows\n";
    std::cout << "    ================================================\n";
    reset_color();
    std::cout << "\n";
}

// Main Menu

void ConsoleUI::show_main_menu() {
    set_color(COLOR_WHITE);
    std::cout << "  MAIN MENU\n";
    reset_color();
    std::cout << "  ---------\n";
    set_color(COLOR_GREEN);
    std::cout << "  [1] ";
    reset_color();
    std::cout << "Quick Scan       (read ARP cache — instant)\n";
    set_color(COLOR_GREEN);
    std::cout << "  [2] ";
    reset_color();
    std::cout << "Full Scan        (ping sweep + ARP — thorough)\n";
    set_color(COLOR_GREEN);
    std::cout << "  [3] ";
    reset_color();
    std::cout << "Show Adapter Info\n";
    set_color(COLOR_GREEN);
    std::cout << "  [4] ";
    reset_color();
    std::cout << "Show Last Scan Results\n";
    set_color(COLOR_GREEN);
    std::cout << "  [5] ";
    reset_color();
    std::cout << "Intrusion Check  (compare with trusted list)\n";
    set_color(COLOR_GREEN);
    std::cout << "  [6] ";
    reset_color();
    std::cout << "Mark Current Devices as Trusted\n";
    set_color(COLOR_GREEN);
    std::cout << "  [7] ";
    reset_color();
    std::cout << "Export Results to CSV\n";
    set_color(COLOR_RED);
    std::cout << "  [0] ";
    reset_color();
    std::cout << "Exit\n\n";
    set_color(COLOR_CYAN);
    std::cout << "  Select> ";
    reset_color();
}

// Adapter Info Screen

void ConsoleUI::show_adapter_info() {
    clear_screen();
    show_banner();

    if (adapters_.empty()) {
        set_color(COLOR_RED);
        std::cout << "  [!] No active network adapters found.\n";
        reset_color();
        pause();
        return;
    }

    set_color(COLOR_WHITE);
    std::cout << "  NETWORK ADAPTERS (" << adapters_.size() << " found)\n";
    reset_color();
    std::cout << "  " << std::string(50, '-') << "\n\n";

    for (size_t i = 0; i < adapters_.size(); ++i) {
        const auto& a = adapters_[i];
        set_color(COLOR_CYAN);
        std::cout << "  Adapter #" << (i + 1) << "\n";
        reset_color();
        std::cout << "    Description : " << a.description << "\n";
        std::cout << "    MAC Address : " << a.mac_address << "\n";
        std::cout << "    IP Address  : " << a.ip_address << "\n";
        std::cout << "    Subnet Mask : " << a.subnet_mask << "\n";
        std::cout << "    Gateway     : " << a.gateway_ip << "\n";

        uint32_t host_count = a.broadcast - a.network_addr - 1;
        std::cout << "    Hosts in net: " << host_count << "\n\n";
    }

    pause();
}

// Scan Results

void ConsoleUI::show_scan_results() {
    clear_screen();
    show_banner();

    if (devices_.empty()) {
        set_color(COLOR_YELLOW);
        std::cout << "  [!] No scan results yet. Run a scan first.\n";
        reset_color();
        pause();
        return;
    }

    set_color(COLOR_WHITE);
    std::cout << "  SCAN RESULTS (" << devices_.size() << " devices)\n";
    reset_color();
    std::cout << "  " << std::string(50, '-') << "\n\n";

    TableFormatter table;
    table.set_headers({"#", "IP Address", "MAC Address", "Vendor", "Hostname", "Notes"});

    for (size_t i = 0; i < devices_.size(); ++i) {
        const auto& d = devices_[i];
        std::string notes;
        if (d.is_gateway) notes += "[GW] ";
        if (d.is_local)   notes += "[YOU] ";

        table.add_row({
            std::to_string(i + 1),
            d.ip_address,
            d.mac_address,
            d.vendor,
            d.hostname,
            notes
        });
    }

    std::cout << table.render();
    std::cout << "\n";

    pause();
}

// Quick Scan

void ConsoleUI::do_quick_scan() {
    clear_screen();
    show_banner();

    auto scan_start = std::chrono::steady_clock::now();

    set_color(COLOR_WHITE);
    std::cout << "  QUICK SCAN\n";
    reset_color();
    std::cout << "  " << std::string(50, '-') << "\n\n";

    // ── Phase 1: Read ARP cache ──
    set_color(COLOR_CYAN);
    std::cout << "  [Phase 1/4] Reading ARP cache...\n";
    reset_color();

    auto phase_start = std::chrono::steady_clock::now();
    devices_ = read_arp_table();
    auto phase_end = std::chrono::steady_clock::now();
    auto phase_ms = std::chrono::duration_cast<std::chrono::milliseconds>(phase_end - phase_start).count();

    set_color(COLOR_GREEN);
    std::cout << "             Found " << devices_.size() << " device(s) in ARP cache";
    set_color(COLOR_DARK_GRAY);
    std::cout << "  (" << phase_ms << " ms)\n";
    reset_color();

    if (devices_.empty()) {
        set_color(COLOR_YELLOW);
        std::cout << "\n  [!] ARP cache is empty. Try a Full Scan (option 2) to discover devices.\n";
        reset_color();
        pause();
        return;
    }

    // ── Phase 2: Vendor lookup ──
    set_color(COLOR_CYAN);
    std::cout << "\n  [Phase 2/4] Looking up device vendors...\n";
    reset_color();

    phase_start = std::chrono::steady_clock::now();
    int vendors_found = 0;
    for (size_t i = 0; i < devices_.size(); ++i) {
        auto& dev = devices_[i];
        dev.vendor = oui_.lookup(dev.mac_address);
        if (dev.vendor != "Unknown") ++vendors_found;

        std::cout << "\r             ";
        set_color(COLOR_DARK_GRAY);
        std::cout << "[" << (i + 1) << "/" << devices_.size() << "] "
                  << dev.mac_address << " -> ";
        if (dev.vendor != "Unknown") {
            set_color(COLOR_GREEN);
        } else {
            set_color(COLOR_YELLOW);
        }
        std::cout << dev.vendor << std::string(30, ' ');
        reset_color();
        std::cout.flush();
    }
    phase_end = std::chrono::steady_clock::now();
    phase_ms = std::chrono::duration_cast<std::chrono::milliseconds>(phase_end - phase_start).count();

    std::cout << "\r" << std::string(80, ' ') << "\r";
    set_color(COLOR_GREEN);
    std::cout << "             Identified " << vendors_found << "/" << devices_.size() << " vendors";
    set_color(COLOR_DARK_GRAY);
    std::cout << "  (" << phase_ms << " ms)\n";
    reset_color();

    // ── Phase 3: Hostname resolution ──
    set_color(COLOR_CYAN);
    std::cout << "\n  [Phase 3/4] Resolving hostnames (this may take a moment)...\n";
    reset_color();

    phase_start = std::chrono::steady_clock::now();
    int hostnames_resolved = 0;
    for (size_t i = 0; i < devices_.size(); ++i) {
        auto& dev = devices_[i];

        // Show which IP we're resolving with a progress bar
        int pct = static_cast<int>(((i + 1) * 100) / devices_.size());
        int bar_width = 25;
        int filled = (pct * bar_width) / 100;

        std::cout << "\r             [";
        set_color(COLOR_CYAN);
        for (int b = 0; b < bar_width; ++b) {
            std::cout << (b < filled ? '#' : '.');
        }
        reset_color();
        std::cout << "] " << std::setw(3) << pct << "%  "
                  << dev.ip_address << std::string(20, ' ');
        std::cout.flush();

        dev.hostname = resolve_hostname(dev.ip_address);
        if (dev.hostname != "N/A") ++hostnames_resolved;
    }
    phase_end = std::chrono::steady_clock::now();
    phase_ms = std::chrono::duration_cast<std::chrono::milliseconds>(phase_end - phase_start).count();

    std::cout << "\r" << std::string(80, ' ') << "\r";
    set_color(COLOR_GREEN);
    std::cout << "             Resolved " << hostnames_resolved << "/" << devices_.size() << " hostnames";
    set_color(COLOR_DARK_GRAY);
    std::cout << "  (" << phase_ms << " ms)\n";
    reset_color();

    // Phase 4: Classify devices
    set_color(COLOR_CYAN);
    std::cout << "\n  [Phase 4/4] Classifying devices...\n";
    reset_color();

    if (!adapters_.empty()) {
        for (auto& dev : devices_) {
            if (dev.ip_address == adapters_[0].gateway_ip) dev.is_gateway = true;
            if (dev.ip_address == adapters_[0].ip_address) dev.is_local = true;
        }
    }

    set_color(COLOR_GREEN);
    std::cout << "             Marked gateway & local device\n";
    reset_color();

    // Summary
    auto scan_end = std::chrono::steady_clock::now();
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(scan_end - scan_start).count();
    double total_sec = total_ms / 1000.0;

    std::cout << "\n  " << std::string(50, '=') << "\n";
    set_color(COLOR_GREEN);
    std::cout << "  [+] Quick scan complete!\n";
    reset_color();
    std::cout << "      Devices found  : " << devices_.size() << "\n";
    std::cout << "      Vendors matched: " << vendors_found << "\n";
    std::cout << "      Names resolved : " << hostnames_resolved << "\n";
    set_color(COLOR_DARK_GRAY);
    std::cout << "      Total time     : " << std::fixed << std::setprecision(1) << total_sec << "s\n";
    reset_color();
    std::cout << "  " << std::string(50, '=') << "\n";

    show_scan_results();
}

// Full Scan (Ping Sweep)

void ConsoleUI::do_full_scan() {
    clear_screen();
    show_banner();

    if (adapters_.empty()) {
        set_color(COLOR_RED);
        std::cout << "  [!] No active adapters. Cannot scan.\n";
        reset_color();
        pause();
        return;
    }

    auto scan_start = std::chrono::steady_clock::now();

    const auto& adapter = adapters_[0]; // Use first active adapter

    uint32_t host_count = adapter.broadcast - adapter.network_addr - 1;

    set_color(COLOR_WHITE);
    std::cout << "  FULL SCAN (Ping Sweep + ARP)\n";
    reset_color();
    std::cout << "  " << std::string(50, '-') << "\n\n";

    // Show scan parameters
    set_color(COLOR_DARK_GRAY);
    std::cout << "  Adapter    : " << adapter.description << "\n";
    std::cout << "  Local IP   : " << adapter.ip_address << "\n";
    std::cout << "  Subnet     : " << adapter.subnet_mask << "\n";
    std::cout << "  Gateway    : " << adapter.gateway_ip << "\n";
    std::cout << "  Hosts      : " << host_count << "\n";
    std::cout << "  Timeout    : 100 ms per host\n";
    reset_color();
    std::cout << "\n";

    // ── Phase 1: Ping sweep ──
    set_color(COLOR_CYAN);
    std::cout << "  [Phase 1/5] Ping sweeping " << host_count << " hosts...\n";
    reset_color();

    int hosts_alive = 0;
    auto phase_start = std::chrono::steady_clock::now();

    auto progress = [this, &hosts_alive](int current, int total) {
        int pct = (current * 100) / total;
        int bar_width = 35;
        int filled = (pct * bar_width) / 100;

        std::cout << "\r             [";
        set_color(COLOR_GREEN);
        for (int i = 0; i < bar_width; ++i) {
            std::cout << (i < filled ? '#' : '.');
        }
        reset_color();
        std::cout << "] " << std::setw(3) << pct << "%  ("
                  << current << "/" << total << ")";
        std::cout.flush();
    };

    devices_ = ping_sweep(adapter, 100, progress);

    auto phase_end = std::chrono::steady_clock::now();
    auto phase_ms = std::chrono::duration_cast<std::chrono::milliseconds>(phase_end - phase_start).count();
    double phase_sec = phase_ms / 1000.0;

    std::cout << "\n";
    set_color(COLOR_GREEN);
    std::cout << "             Sweep complete — " << devices_.size() << " device(s) in ARP table";
    set_color(COLOR_DARK_GRAY);
    std::cout << "  (" << std::fixed << std::setprecision(1) << phase_sec << "s)\n";
    reset_color();

    if (devices_.empty()) {
        set_color(COLOR_YELLOW);
        std::cout << "\n  [!] No devices found. Your network might be blocking ICMP/ARP.\n";
        reset_color();
        pause();
        return;
    }

    // Phase 2: Vendor lookup
    set_color(COLOR_CYAN);
    std::cout << "\n  [Phase 2/5] Looking up device vendors...\n";
    reset_color();

    phase_start = std::chrono::steady_clock::now();
    int vendors_found = 0;
    for (size_t i = 0; i < devices_.size(); ++i) {
        auto& dev = devices_[i];
        dev.vendor = oui_.lookup(dev.mac_address);
        if (dev.vendor != "Unknown") ++vendors_found;

        std::cout << "\r             ";
        set_color(COLOR_DARK_GRAY);
        std::cout << "[" << (i + 1) << "/" << devices_.size() << "] "
                  << dev.mac_address << " -> ";
        if (dev.vendor != "Unknown") {
            set_color(COLOR_GREEN);
        } else {
            set_color(COLOR_YELLOW);
        }
        std::cout << dev.vendor << std::string(30, ' ');
        reset_color();
        std::cout.flush();
    }
    phase_end = std::chrono::steady_clock::now();
    phase_ms = std::chrono::duration_cast<std::chrono::milliseconds>(phase_end - phase_start).count();

    std::cout << "\r" << std::string(80, ' ') << "\r";
    set_color(COLOR_GREEN);
    std::cout << "             Identified " << vendors_found << "/" << devices_.size() << " vendors";
    set_color(COLOR_DARK_GRAY);
    std::cout << "  (" << phase_ms << " ms)\n";
    reset_color();

    // Phase 3: Hostname resolution
    set_color(COLOR_CYAN);
    std::cout << "\n  [Phase 3/5] Resolving hostnames (this may take a moment)...\n";
    reset_color();

    phase_start = std::chrono::steady_clock::now();
    int hostnames_resolved = 0;
    for (size_t i = 0; i < devices_.size(); ++i) {
        auto& dev = devices_[i];

        int pct = static_cast<int>(((i + 1) * 100) / devices_.size());
        int bar_width = 25;
        int filled = (pct * bar_width) / 100;

        std::cout << "\r             [";
        set_color(COLOR_CYAN);
        for (int b = 0; b < bar_width; ++b) {
            std::cout << (b < filled ? '#' : '.');
        }
        reset_color();
        std::cout << "] " << std::setw(3) << pct << "%  "
                  << dev.ip_address << std::string(20, ' ');
        std::cout.flush();

        if (dev.hostname.empty() || dev.hostname == "N/A") {
            dev.hostname = resolve_hostname(dev.ip_address);
        }
        if (dev.hostname != "N/A") ++hostnames_resolved;
    }
    phase_end = std::chrono::steady_clock::now();
    phase_ms = std::chrono::duration_cast<std::chrono::milliseconds>(phase_end - phase_start).count();

    std::cout << "\r" << std::string(80, ' ') << "\r";
    set_color(COLOR_GREEN);
    std::cout << "             Resolved " << hostnames_resolved << "/" << devices_.size() << " hostnames";
    set_color(COLOR_DARK_GRAY);
    std::cout << "  (" << phase_ms << " ms)\n";
    reset_color();

    // Phase 4: Classify devices
    set_color(COLOR_CYAN);
    std::cout << "\n  [Phase 4/5] Classifying devices...\n";
    reset_color();

    // Gateway / local already marked by ping_sweep, but ensure it
    for (auto& dev : devices_) {
        if (dev.ip_address == adapter.gateway_ip) dev.is_gateway = true;
        if (dev.ip_address == adapter.ip_address) dev.is_local = true;
    }

    set_color(COLOR_GREEN);
    std::cout << "             Marked gateway & local device\n";
    reset_color();

    // ── Phase 5: Intrusion pre-check ──
    if (!known_devices_.empty()) {
        set_color(COLOR_CYAN);
        std::cout << "\n  [Phase 5/5] Quick intrusion pre-check...\n";
        reset_color();

        int new_count = 0;
        for (const auto& dev : devices_) {
            bool found = false;
            for (const auto& known : known_devices_) {
                if (dev.mac_address == known.mac_address) { found = true; break; }
            }
            if (!found) ++new_count;
        }

        if (new_count > 0) {
            set_color(COLOR_RED);
            std::cout << "             " << new_count << " unknown device(s) detected! Use option 5 for details.\n";
        } else {
            set_color(COLOR_GREEN);
            std::cout << "             All devices match trusted list.\n";
        }
        reset_color();
    } else {
        set_color(COLOR_CYAN);
        std::cout << "\n  [Phase 5/5] Intrusion pre-check skipped (no trusted list)\n";
        reset_color();
    }

    // Summary
    auto scan_end = std::chrono::steady_clock::now();
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(scan_end - scan_start).count();
    double total_sec = total_ms / 1000.0;

    std::cout << "\n  " << std::string(50, '=') << "\n";
    set_color(COLOR_GREEN);
    std::cout << "  [+] Full scan complete!\n";
    reset_color();
    std::cout << "      Devices found  : " << devices_.size() << "\n";
    std::cout << "      Vendors matched: " << vendors_found << "\n";
    std::cout << "      Names resolved : " << hostnames_resolved << "\n";
    std::cout << "      Hosts scanned  : " << host_count << "\n";
    set_color(COLOR_DARK_GRAY);
    std::cout << "      Total time     : " << std::fixed << std::setprecision(1) << total_sec << "s\n";
    reset_color();
    std::cout << "  " << std::string(50, '=') << "\n";

    show_scan_results();
}

// Intrusion Check

void ConsoleUI::show_intrusion_alerts() {
    clear_screen();
    show_banner();

    if (devices_.empty()) {
        set_color(COLOR_YELLOW);
        std::cout << "  [!] No scan results. Run a scan first.\n";
        reset_color();
        pause();
        return;
    }

    if (known_devices_.empty()) {
        set_color(COLOR_YELLOW);
        std::cout << "  [!] No trusted device list yet.\n";
        std::cout << "  [!] Run a scan and mark devices as trusted first (option 6).\n";
        reset_color();
        pause();
        return;
    }

    // Find devices in current scan that are NOT in trusted list (by MAC)
    std::vector<Device> unknown;
    for (const auto& dev : devices_) {
        bool found = false;
        for (const auto& known : known_devices_) {
            if (dev.mac_address == known.mac_address) {
                found = true;
                break;
            }
        }
        if (!found) {
            unknown.push_back(dev);
        }
    }

    // Also find trusted devices that disappeared
    std::vector<Device> missing;
    for (const auto& known : known_devices_) {
        bool found = false;
        for (const auto& dev : devices_) {
            if (dev.mac_address == known.mac_address) {
                found = true;
                break;
            }
        }
        if (!found) {
            missing.push_back(known);
        }
    }

    set_color(COLOR_WHITE);
    std::cout << "  INTRUSION CHECK REPORT\n";
    reset_color();
    std::cout << "  " << std::string(50, '-') << "\n\n";

    std::cout << "  Trusted devices  : " << known_devices_.size() << "\n";
    std::cout << "  Current devices  : " << devices_.size() << "\n\n";

    if (unknown.empty()) {
        set_color(COLOR_GREEN);
        std::cout << "  [+] No unknown devices detected. Network looks clean!\n";
        reset_color();
    } else {
        set_color(COLOR_RED);
        std::cout << "  [!!!] WARNING: " << unknown.size() << " UNKNOWN DEVICE(S) DETECTED!\n\n";
        reset_color();

        TableFormatter table;
        table.set_headers({"#", "IP Address", "MAC Address", "Vendor", "Hostname"});
        for (size_t i = 0; i < unknown.size(); ++i) {
            const auto& d = unknown[i];
            table.add_row({
                std::to_string(i + 1),
                d.ip_address,
                d.mac_address,
                d.vendor,
                d.hostname
            });
        }
        std::cout << table.render() << "\n";

        set_color(COLOR_YELLOW);
        std::cout << "  RECOMMENDED ACTIONS:\n";
        std::cout << "  - Check if these are new devices you recently added.\n";
        std::cout << "  - If not recognized, change your WiFi password immediately.\n";
        std::cout << "  - Enable MAC filtering on your router.\n";
        std::cout << "  - Update your router firmware.\n";
        reset_color();
    }

    if (!missing.empty()) {
        std::cout << "\n";
        set_color(COLOR_YELLOW);
        std::cout << "  [*] " << missing.size() << " trusted device(s) not currently visible:\n\n";
        reset_color();
        for (const auto& d : missing) {
            std::cout << "    - " << d.ip_address << "  " << d.mac_address
                      << "  (" << d.vendor << ")\n";
        }
    }

    std::cout << "\n";
    pause();
}

// Mark Trusted

void ConsoleUI::do_mark_trusted() {
    if (devices_.empty()) {
        set_color(COLOR_YELLOW);
        std::cout << "\n  [!] No devices to mark. Run a scan first.\n";
        reset_color();
        pause();
        return;
    }

    known_devices_ = devices_;

    // Persist to file
    std::ofstream file("trusted_devices.txt");
    if (file.is_open()) {
        for (const auto& d : known_devices_) {
            file << d.mac_address << ","
                 << d.ip_address << ","
                 << d.vendor << ","
                 << d.hostname << "\n";
        }
        file.close();
    }

    set_color(COLOR_GREEN);
    std::cout << "\n  [+] " << known_devices_.size()
              << " device(s) saved as trusted in trusted_devices.txt.\n";
    reset_color();
    pause();
}

// Export to CSV

void ConsoleUI::do_export_results() {
    if (devices_.empty()) {
        set_color(COLOR_YELLOW);
        std::cout << "\n  [!] No results to export. Run a scan first.\n";
        reset_color();
        pause();
        return;
    }

    // Generate filename with timestamp
    time_t now = time(nullptr);
    struct tm local_time;
    localtime_s(&local_time, &now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", &local_time);

    std::string filename = std::string("scan_") + timestamp + ".csv";

    std::ofstream file(filename);
    if (!file.is_open()) {
        set_color(COLOR_RED);
        std::cout << "\n  [!] Failed to create file: " << filename << "\n";
        reset_color();
        pause();
        return;
    }

    // CSV header
    file << "IP Address,MAC Address,Vendor,Hostname,Is Gateway,Is Local\n";

    for (const auto& d : devices_) {
        // Escape commas in fields
        auto escape = [](const std::string& s) -> std::string {
            if (s.find(',') != std::string::npos || s.find('"') != std::string::npos) {
                std::string escaped = s;
                // Double any existing quotes
                size_t pos = 0;
                while ((pos = escaped.find('"', pos)) != std::string::npos) {
                    escaped.insert(pos, 1, '"');
                    pos += 2;
                }
                return "\"" + escaped + "\"";
            }
            return s;
        };

        file << escape(d.ip_address) << ","
             << escape(d.mac_address) << ","
             << escape(d.vendor) << ","
             << escape(d.hostname) << ","
             << (d.is_gateway ? "Yes" : "No") << ","
             << (d.is_local ? "Yes" : "No") << "\n";
    }

    file.close();

    set_color(COLOR_GREEN);
    std::cout << "\n  [+] Exported " << devices_.size()
              << " device(s) to " << filename << "\n";
    reset_color();
    pause();
}

// Main Loop

void ConsoleUI::run() {
    init_console();
    load_oui_database();

    // Load previously saved trusted devices
    {
        std::ifstream tfile("trusted_devices.txt");
        if (tfile.is_open()) {
            std::string line;
            while (std::getline(tfile, line)) {
                auto parts = utils::split(line, ',');
                if (parts.size() >= 4) {
                    Device d;
                    d.mac_address = utils::trim(parts[0]);
                    d.ip_address  = utils::trim(parts[1]);
                    d.vendor      = utils::trim(parts[2]);
                    d.hostname    = utils::trim(parts[3]);
                    known_devices_.push_back(d);
                }
            }
            tfile.close();
        }
    }

    detect_adapters();

    bool running = true;
    while (running) {
        clear_screen();
        show_banner();

        // Show quick adapter summary
        if (!adapters_.empty()) {
            set_color(COLOR_DARK_GRAY);
            std::cout << "  Connected via: " << adapters_[0].description << "\n";
            std::cout << "  Local IP: " << adapters_[0].ip_address
                      << "  Gateway: " << adapters_[0].gateway_ip << "\n";
            if (!known_devices_.empty()) {
                std::cout << "  Trusted devices loaded: " << known_devices_.size() << "\n";
            }
            reset_color();
            std::cout << "\n";
        } else {
            set_color(COLOR_RED);
            std::cout << "  [!] No active network adapter detected!\n\n";
            reset_color();
        }

        show_main_menu();

        std::string input;
        if (!std::getline(std::cin, input)) {
            // stdin closed (EOF) — exit instead of spinning on empty reads
            break;
        }
        input = utils::trim(input);

        if (input == "1") {
            do_quick_scan();
        } else if (input == "2") {
            do_full_scan();
        } else if (input == "3") {
            show_adapter_info();
        } else if (input == "4") {
            show_scan_results();
        } else if (input == "5") {
            show_intrusion_alerts();
        } else if (input == "6") {
            do_mark_trusted();
        } else if (input == "7") {
            do_export_results();
        } else if (input == "0") {
            running = false;
        }
    }

    clear_screen();
    set_color(COLOR_CYAN);
    std::cout << "\n  Goodbye! Stay safe.\n\n";
    reset_color();
}

} // namespace netscanner
