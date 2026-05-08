#ifndef OUI_LOOKUP_H
#define OUI_LOOKUP_H

#include <string>
#include <unordered_map>

namespace netscanner {

class OuiLookup {
public:
    // Load OUI prefixes from a CSV file
    bool load_from_file(const std::string& filepath);

    // Load a built in minimal database, no file needed
    void load_builtin();

    // Loop up vendor by MAC address, return "Unknown" if not found
    std::string lookup(const std::string& mac) const;

    // Number of loaded prefixes
    size_t size() const {
        return db_.size();
    }

private:
    // Key, first 3 octets uppercase, "AA:BB:CC"
    std::unordered_map<std::string, std::string> db_;

    // Extract the first 3 octets from a MAC string, uppercased and colon separated
    static std::string extract_prefix(const std::string& mac);
};

}  // namespace netscanner

#endif // OUI_LOOKUP_H