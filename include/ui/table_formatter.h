#ifndef TABLE_FORMATTER_H
#define TABLE_FORMATTER_H

#include <string>
#include <vector>

namespace netscanner {

class TableFormatter {
public:
    // Set column headers
    void set_headers(const std::vector<std::string>& headers);

    // Add row of values (must match header count)
    void add_row(const std::vector<std::string>& row);

    // Clear all rows but keep headers
    void clear_rows();

    // Render table to a string using box-drawing chars
    std::string render() const;

    // Render using simple ascii chars
    std::string render_ascii() const;

private:
    std::vector<std::string> headers_;
    std::vector<std::vector<std::string>> rows_;

    // Compute the max width per column
    std::vector<size_t> compute_widths() const;

    // Pad or truncate a string to exactly "width" chars
    static std::string pad(const std::string& s, size_t width);
};

} // Namespace netscanner

#endif // TABLE_FORMATTER_H