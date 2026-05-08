#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include <cctype>

namespace netscanner {
namespace utils {

inline std::string to_upper(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return result;
}

inline std::string to_lower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

inline std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}



}  // Namespace utils
}  // Namespace netscanner

#endif // STRING_UTILS_H