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

}  // namespace netscanner