#include "../../include/oui/oui_lookup.h"
#include "../../include/utils/string_utils.h"
#include <fstream>
#include <sstream>

namespace netscanner {

std::string OuiLookup::extract_prefix(const std::string& mac) {
    // Replace dashes with colons, uppercase, take first 8 chars
    std::string normalized = utils::replace_char(mac, '-', ':');
    normalized = utils::to_upper(normalized);

    // Extracting the first 3 octets
    auto parts = utils::split(normalized, ':');
    if (parts.size() >= 3) {
        return parts[0] + ":" + parts[1] + ":" + parts[2];
    }

    return "";
}

bool OuiLookup::load_from_file(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        line = utils::trim(line);

        if (line.empty() || line[0] == '#') continue;

        auto pos = line.find(',');
        if (pos == std::string::npos) continue;

        std::string prefix = utils::to_upper(utils::trim(line.substr(0, pos)));
        std::string vendor = utils::trim(line.substr(pos + 1));

        if (!prefix.empty() && !vendor.empty()) {
            db_[prefix] = vendor;
        }
    }

    return !db_.empty();
}

void OuiLookup::load_builtin() {
    db_ = {
        {"00:1A:2B", "Ayecom Technology"},
        {"00:50:56", "VMware"},
        {"00:0C:29", "VMware"},
        {"00:15:5D", "Microsoft (Hyper-V)"},
        {"00:1E:58", "D-Link"},
        {"00:1F:33", "Netgear"},
        {"00:24:01", "D-Link"},
        {"00:26:5A", "D-Link"},
        {"08:00:27", "Oracle VirtualBox"},
        {"10:FE:ED", "Google"},
        {"18:B4:30", "Nest Labs"},
        {"1C:87:2C", "ASUSTek Computer"},
        {"20:6D:31", "Firewalla"},
        {"28:6C:07", "Xiaomi"},
        {"2C:F0:5D", "Micro-Star Intl (MSI)"},
        {"30:B5:C2", "TP-Link"},
        {"34:97:F6", "ASUSTek Computer"},
        {"38:94:ED", "Hon Hai / Foxconn"},
        {"3C:37:86", "Netgear"},
        {"3C:A6:F6", "Apple"},
        {"40:B0:76", "ASUSTek Computer"},
        {"44:07:0B", "Google"},
        {"44:D9:E7", "Ubiquiti Networks"},
        {"48:A6:B8", "Linksys (Belkin)"},
        {"50:C7:BF", "TP-Link"},
        {"54:60:09", "Google"},
        {"5C:E9:1E", "Samsung"},
        {"60:38:E0", "Belkin"},
        {"64:70:02", "TP-Link"},
        {"68:FF:7B", "TP-Link"},
        {"6C:72:E7", "Samsung"},
        {"70:3A:CB", "Google"},
        {"74:DA:88", "TP-Link"},
        {"78:8A:20", "Ubiquiti Networks"},
        {"7C:10:C9", "Apple"},
        {"80:2A:A8", "Ubiquiti Networks"},
        {"84:D8:1B", "TP-Link"},
        {"88:71:B1", "Amazon"},
        {"8C:85:90", "Apple"},
        {"90:72:40", "Apple"},
        {"94:10:3E", "Belkin"},
        {"98:DA:C4", "TP-Link"},
        {"9C:5C:8E", "Amazon"},
        {"A0:63:91", "Netgear"},
        {"A4:77:33", "Google"},
        {"A8:6A:6F", "Ralink Technology"},
        {"AC:84:C6", "TP-Link"},
        {"B0:4E:26", "TP-Link"},
        {"B4:FB:E4", "Ubiquiti Networks"},
        {"B8:27:EB", "Raspberry Pi Foundation"},
        {"BC:30:7D", "Wistron Neweb"},
        {"C0:25:E9", "TP-Link"},
        {"C0:56:27", "Belkin"},
        {"C4:E9:84", "TP-Link"},
        {"C8:3A:35", "Tenda Technology"},
        {"CC:32:E5", "TP-Link"},
        {"D0:21:F9", "Ubiquiti Networks"},
        {"D4:6E:0E", "TP-Link"},
        {"D8:07:B6", "TP-Link"},
        {"DC:A6:32", "Raspberry Pi Foundation"},
        {"E0:63:DA", "Ubiquiti Networks"},
        {"E4:5F:01", "Raspberry Pi Foundation"},
        {"E8:48:B8", "TP-Link"},
        {"EC:08:6B", "TP-Link"},
        {"EC:17:2F", "TP-Link"},
        {"F0:9F:C2", "Ubiquiti Networks"},
        {"F4:F2:6D", "TP-Link"},
        {"F8:1A:67", "TP-Link"},
        {"FC:EC:DA", "Ubiquiti Networks"},
        // Common broad ranges
        {"00:11:32", "Synology"},
        {"B8:AE:ED", "Elitegroup Computer"},
        {"00:1C:B3", "Apple"},
        {"00:03:93", "Apple"},
        {"00:0A:95", "Apple"},
        {"00:14:51", "Apple"},
        {"00:16:CB", "Apple"},
        {"00:17:F2", "Apple"},
        {"00:19:E3", "Apple"},
        {"00:1B:63", "Apple"},
        {"00:1E:C2", "Apple"},
        {"00:1F:5B", "Apple"},
        {"00:21:E9", "Apple"},
        {"00:22:41", "Apple"},
        {"00:23:12", "Apple"},
        {"00:23:6C", "Apple"},
        {"00:23:DF", "Apple"},
        {"00:24:36", "Apple"},
        {"00:25:00", "Apple"},
        {"00:25:4B", "Apple"},
        {"00:25:BC", "Apple"},
        {"00:26:08", "Apple"},
        {"00:26:4A", "Apple"},
        {"00:26:B0", "Apple"},
        {"00:26:BB", "Apple"},
        {"04:0C:CE", "Apple"},
        {"14:10:9F", "Apple"},
        {"28:CF:DA", "Apple"},
        {"34:15:9E", "Apple"},
        {"3C:07:54", "Apple"},
        {"40:6C:8F", "Apple"},
        {"48:60:BC", "Apple"},
        {"54:26:96", "Apple"},
        {"58:55:CA", "Apple"},
        {"60:C5:47", "Apple"},
        {"68:96:7B", "Apple"},
        {"70:56:81", "Apple"},
        {"78:31:C1", "Apple"},
        {"80:E6:50", "Apple"},
        {"84:38:35", "Apple"},
        {"8C:FA:BA", "Apple"},
        {"A4:B1:97", "Apple"},
        {"A8:20:66", "Apple"},
        {"AC:87:A3", "Apple"},
        {"B0:65:BD", "Apple"},
        {"B8:C7:5D", "Apple"},
        {"C0:9F:42", "Apple"},
        {"C8:2A:14", "Apple"},
        {"CC:08:8D", "Apple"},
        {"D0:25:98", "Apple"},
        {"D8:1D:72", "Apple"},
        {"DC:2B:61", "Apple"},
        {"E0:B5:2D", "Apple"},
        {"E8:06:88", "Apple"},
        {"F0:B4:79", "Apple"},
        {"F4:5C:89", "Apple"},
        {"F8:27:93", "Apple"},
        // Samsung
        {"00:07:AB", "Samsung"},
        {"00:12:47", "Samsung"},
        {"00:15:99", "Samsung"},
        {"00:16:32", "Samsung"},
        {"00:17:D5", "Samsung"},
        {"00:18:AF", "Samsung"},
        {"00:1A:8A", "Samsung"},
        {"00:1B:98", "Samsung"},
        {"00:1C:43", "Samsung"},
        {"00:1D:25", "Samsung"},
        {"00:1E:E1", "Samsung"},
        {"00:1E:E2", "Samsung"},
        {"00:21:19", "Samsung"},
        {"00:21:D1", "Samsung"},
        {"00:21:D2", "Samsung"},
        {"00:24:54", "Samsung"},
        {"00:24:90", "Samsung"},
        {"00:24:91", "Samsung"},
        {"00:25:66", "Samsung"},
        {"00:26:37", "Samsung"},
        {"08:D4:6A", "Samsung"},
        {"10:1D:C0", "Samsung"},
        {"14:49:E0", "Samsung"},
        {"18:67:B0", "Samsung"},
        {"1C:62:B8", "Samsung"},
        {"24:4B:81", "Samsung"},
        {"2C:AE:2B", "Samsung"},
        {"30:CD:A7", "Samsung"},
        {"34:23:BA", "Samsung"},
        {"38:01:97", "Samsung"},
        {"40:0E:85", "Samsung"},
        {"44:4E:1A", "Samsung"},
        {"4C:BC:A5", "Samsung"},
        {"50:01:BB", "Samsung"},
        {"54:92:BE", "Samsung"},
        {"58:C3:8B", "Samsung"},
        // Intel (common in laptops)
        {"00:03:47", "Intel"},
        {"00:04:23", "Intel"},
        {"00:07:E9", "Intel"},
        {"00:0E:0C", "Intel"},
        {"00:0E:35", "Intel"},
        {"00:11:11", "Intel"},
        {"00:12:F0", "Intel"},
        {"00:13:02", "Intel"},
        {"00:13:20", "Intel"},
        {"00:13:CE", "Intel"},
        {"00:13:E8", "Intel"},
        {"00:15:00", "Intel"},
        {"00:16:6F", "Intel"},
        {"00:16:76", "Intel"},
        {"00:16:EA", "Intel"},
        {"00:16:EB", "Intel"},
        {"00:18:DE", "Intel"},
        {"00:19:D1", "Intel"},
        {"00:19:D2", "Intel"},
        {"00:1B:21", "Intel"},
        {"00:1B:77", "Intel"},
        {"00:1C:BF", "Intel"},
        {"00:1D:E0", "Intel"},
        {"00:1D:E1", "Intel"},
        {"00:1E:64", "Intel"},
        {"00:1E:65", "Intel"},
        {"00:1F:3B", "Intel"},
        {"00:1F:3C", "Intel"},
        {"00:22:FA", "Intel"},
        {"00:22:FB", "Intel"},
        {"00:24:D6", "Intel"},
        {"00:24:D7", "Intel"},
        {"58:94:6B", "Intel"},
        {"60:57:18", "Intel"},
        {"68:05:CA", "Intel"},
        {"78:92:9C", "Intel"},
        {"80:86:F2", "Intel"},
        {"8C:EC:4B", "Intel"},
        {"A0:36:9F", "Intel"},
        {"AC:7B:A1", "Intel"},
        {"B4:6B:FC", "Intel"},
        {"C8:F7:33", "Intel"},
        {"D4:3D:7E", "Intel"},
        {"F8:63:3F", "Intel"},
    };
}

std::string OuiLookup::lookup(const std::string& mac) const {
    std::string prefix = extract_prefix(mac);
    if (prefix.empty()) return "Unknown";

    auto it = db_.find(prefix);
    if (it != db_.end()) {
        return it->second;
    }

    return "Unknown";
}

}  // namespace netscanner