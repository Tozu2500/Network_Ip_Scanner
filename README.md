# Network_Ip_Scanner

A Windows console network scanner written in C++. Reads the ARP cache, ping-sweeps
the local subnet, looks up device vendors by MAC (OUI), resolves hostnames, and
flags unknown/intrusive devices against a trusted list.

## Requirements

- Windows
- A C++17 compiler (MinGW-w64 `g++` or MSVC)
- Windows SDK libraries `ws2_32` and `iphlpapi` (present by default with either toolchain)

## Build & Run

**Quickest way:** double-click `run.bat`, or from a terminal:

```
run.bat
```

**Manual build (MinGW):**

```powershell
mingw32-make
.\build\netscanner.exe
```

**Manual build (CMake, if installed):**

```powershell
cmake -B build -G "MinGW Makefiles"
cmake --build build
.\build\netscanner.exe
```

> Run from an elevated (Administrator) terminal if Full Scan (ping sweep) doesn't
> return results — it uses raw ICMP/ARP APIs that some environments restrict.

## Menu Options

| # | Action |
|---|--------|
| 1 | Quick Scan — read ARP cache (instant) |
| 2 | Full Scan — ping sweep + ARP (thorough) |
| 3 | Show Adapter Info |
| 4 | Show Last Scan Results |
| 5 | Intrusion Check — compare against trusted list |
| 6 | Mark Current Devices as Trusted |
| 7 | Export Results to CSV |
| 0 | Exit |
