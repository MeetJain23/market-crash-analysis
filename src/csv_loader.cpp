#include "qc/csv_loader.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace qc {
namespace {

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    const auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

std::vector<std::string> split(const std::string& line, char delim) {
    std::vector<std::string> out;
    std::string cell;
    std::stringstream ss(line);
    while (std::getline(ss, cell, delim)) out.push_back(trim(cell));
    return out;
}

// Parse a double; returns false if the cell is empty or non-numeric.
bool parse_double(const std::string& s, double& out) {
    if (s.empty()) return false;
    try {
        std::size_t pos = 0;
        out = std::stod(s, &pos);
        return pos == s.size();
    } catch (...) {
        return false;
    }
}

}  // namespace

Series load_csv(const std::string& path, bool sort_ascending) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("csv_loader: cannot open file: " + path);

    std::string header_line;
    if (!std::getline(in, header_line))
        throw std::runtime_error("csv_loader: empty file: " + path);

    const auto header = split(header_line, ',');
    std::unordered_map<std::string, int> col;
    for (int i = 0; i < static_cast<int>(header.size()); ++i)
        col[to_lower(header[i])] = i;

    auto require = [&](const std::string& name) -> int {
        auto it = col.find(name);
        if (it == col.end())
            throw std::runtime_error("csv_loader: missing '" + name +
                                     "' column in " + path);
        return it->second;
    };

    const int ci_date  = require("date");
    const int ci_open  = require("open");
    const int ci_high  = require("high");
    const int ci_low   = require("low");
    const int ci_close = require("close");
    const int ci_vol   = col.count("volume") ? col["volume"] : -1;

    std::vector<Bar> bars;
    std::string line;
    while (std::getline(in, line)) {
        if (trim(line).empty()) continue;
        const auto f = split(line, ',');
        const int need = std::max({ci_date, ci_open, ci_high, ci_low, ci_close});
        if (static_cast<int>(f.size()) <= need) continue;  // ragged row

        Bar b;
        b.date = f[ci_date];
        if (!parse_double(f[ci_open],  b.open))  continue;
        if (!parse_double(f[ci_high],  b.high))  continue;
        if (!parse_double(f[ci_low],   b.low))   continue;
        if (!parse_double(f[ci_close], b.close)) continue;
        if (ci_vol >= 0 && ci_vol < static_cast<int>(f.size()))
            parse_double(f[ci_vol], b.volume);  // volume is optional/best-effort
        bars.push_back(std::move(b));
    }

    if (sort_ascending)
        std::sort(bars.begin(), bars.end(),
                  [](const Bar& a, const Bar& c) { return a.date < c.date; });

    return Series(std::move(bars));
}

}  // namespace qc
