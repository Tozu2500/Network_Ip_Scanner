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
    
}

}  // namespace netscanner