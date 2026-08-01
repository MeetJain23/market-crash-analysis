#include "qc/crash.hpp"

namespace qc {

std::vector<CrashEpisode> detect_crashes(const Series& s, double min_depth) {
    std::vector<CrashEpisode> episodes;
    if (s.empty()) return episodes;

    const double thresh = -std::abs(min_depth);

    double peak = s[0].close;
    std::size_t peak_idx = 0;
    double trough = s[0].close;
    std::size_t trough_idx = 0;
    bool in_decline = false;

    auto current_depth = [&]() {
        return (peak > 0.0) ? (trough - peak) / peak : 0.0;
    };

    for (std::size_t i = 0; i < s.size(); ++i) {
        const double c = s[i].close;

        if (c >= peak) {
            // New all-time high — closes any qualifying episode below us.
            if (in_decline && current_depth() <= thresh) {
                CrashEpisode e;
                e.peak_index = peak_idx;
                e.trough_index = trough_idx;
                e.recovery_index = i;
                e.depth = current_depth();
                e.recovered = true;
                episodes.push_back(e);
            }
            peak = c;
            peak_idx = i;
            trough = c;
            trough_idx = i;
            in_decline = false;
        } else {
            in_decline = true;
            if (c < trough) {
                trough = c;
                trough_idx = i;
            }
        }
    }

    // Series ended while still under water.
    if (in_decline && current_depth() <= thresh) {
        CrashEpisode e;
        e.peak_index = peak_idx;
        e.trough_index = trough_idx;
        e.recovery_index = s.size() - 1;
        e.depth = current_depth();
        e.recovered = false;
        episodes.push_back(e);
    }

    return episodes;
}

}  // namespace qc
