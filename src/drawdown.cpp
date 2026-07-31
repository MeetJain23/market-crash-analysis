#include "qc/drawdown.hpp"

namespace qc {

std::vector<double> drawdown_series(const Series& s) {
    std::vector<double> dd(s.size(), 0.0);
    double peak = 0.0;
    bool have_peak = false;
    for (std::size_t i = 0; i < s.size(); ++i) {
        const double c = s[i].close;
        if (!have_peak || c > peak) {
            peak = c;
            have_peak = true;
        }
        dd[i] = (peak > 0.0) ? (c - peak) / peak : 0.0;
    }
    return dd;
}

MaxDrawdown max_drawdown(const Series& s) {
    MaxDrawdown result;
    if (s.empty()) return result;

    double peak = s[0].close;
    std::size_t peak_idx = 0;
    for (std::size_t i = 0; i < s.size(); ++i) {
        const double c = s[i].close;
        if (c > peak) {
            peak = c;
            peak_idx = i;
        }
        const double dd = (peak > 0.0) ? (c - peak) / peak : 0.0;
        if (dd < result.depth) {
            result.depth = dd;
            result.peak_index = peak_idx;
            result.trough_index = i;
        }
    }
    return result;
}

}  // namespace qc
