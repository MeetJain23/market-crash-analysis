#include "qc/volume.hpp"

#include <cmath>
#include <cstdlib>

namespace qc {

Capitulation analyze_capitulation(const Series& s,
                                  std::size_t peak_index,
                                  std::size_t trough_index,
                                  std::size_t near_days) {
    Capitulation cap;
    if (trough_index < peak_index || s.empty()) return cap;

    const std::size_t lo = peak_index;
    const std::size_t hi = trough_index;
    const std::size_t count = hi - lo + 1;

    // Mean / std of volume across the episode window.
    double sum = 0.0, sum_sq = 0.0;
    double max_vol = -1.0;
    std::size_t max_idx = lo;
    for (std::size_t i = lo; i <= hi; ++i) {
        const double v = s[i].volume;
        sum += v;
        sum_sq += v * v;
        if (v > max_vol) {
            max_vol = v;
            max_idx = i;
        }
    }
    const double mean = sum / count;
    const double var = std::max(0.0, sum_sq / count - mean * mean);
    const double sd = std::sqrt(var);

    auto z = [&](double v) { return sd > 0.0 ? (v - mean) / sd : 0.0; };

    cap.trough_volume_z = z(s[trough_index].volume);
    cap.max_volume_index = max_idx;
    cap.max_volume_z = z(max_vol);

    const std::size_t dist = (max_idx > trough_index) ? (max_idx - trough_index)
                                                      : (trough_index - max_idx);
    cap.capitulation_at_trough = dist <= near_days;
    return cap;
}

}  // namespace qc
