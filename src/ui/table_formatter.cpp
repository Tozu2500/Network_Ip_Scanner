#include "../../include/ui/table_formatter.h"
#include <sstream>
#include <algorithm>

namespace netscanner {

void TableFormatter::set_headers(const std::vector<std::string>& headers) {
    headers_ = headers;
}

void TableFormatter::add_row(const std::vector<std::string>& row) {
    rows_.push_back(row);
}

void TableFormatter::clear_rows() {
    rows_.clear();
}

std::vector<size_t> TableFormatter::compute_widths() const {
    std::vector<size_t> widths(headers_.size(), 0);

    for (size_t i = 0; i < headers_.size(); ++i) {
        widths[i] = headers_[i].size();
    }

    for (const auto& row : rows_) {
        for (size_t i = 0; i < row.size() && i < widths.size(); ++i) {
            widths[i] = std::max(widths[i], row[i].size());
        }
    }

    // Minimum width of 3
    for (auto& w : widths) {
        if (w < 3) w = 3;
    }

    return widths;
}

std::string TableFormatter::pad(const std::string& s, size_t width) {
    if (s.size() >= width) return s.substr(0, width);
    return s + std::string(width - s.size(), ' ');
}

std::string TableFormatter::render() const {
    if (headers_.empty()) return "";

    auto widths = compute_widths();
    std::ostringstream out;

    // Top border
    auto make_separator = [&](char left, char mid, char right, char fill) {
        std::string line(1, left);
        for (size_t i = 0; i < widths.size(); ++i) {
            line += std::string(widths[i] + 2, fill);
            line += (i + 1 < widths.size()) ? std::string(1, mid) : std::string(1, right);
        }
        return line;
    };

    std::string top_border    = make_separator('+', '+', '+', '-');
    std::string header_sep    = make_separator('+', '+', '+', '=');
    std::string row_separator = make_separator('+', '+', '+', '-');
    std::string bottom_border = make_separator('+', '+', '+', '-');

    out << top_border << "\n";

    // Header row
    out << "|";
    for (size_t i = 0; i < headers_.size(); ++i) {
        out << " " << pad(headers_[i], widths[i]) << " |";
    }
    out << "\n";
    out << header_sep << "\n";

    // Data rows
    for (const auto& row : rows_) {
        out << "|";
        for (size_t i = 0; i < headers_.size(); ++i) {
            std::string val = (i < row.size()) ? row[i] : "";
            out << " " << pad(val, widths[i]) << " |";
        }
        out << "\n";
    }

    out << bottom_border << "\n";
    return out.str();
}

std::string TableFormatter::render_ascii() const {
    return render(); // Same implementation — already ASCII
}

}  // namespace netscanner