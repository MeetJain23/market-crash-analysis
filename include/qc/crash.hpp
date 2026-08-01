#pragma once

#include <cstddef>
#include <vector>

#include "qc/series.hpp"

namespace qc {

// A peak-to-trough decline that exceeded a depth threshold. `recovered` is
// false when the series ends before price climbs back to the pre-crash peak
// (as in a dataset that stops mid-recovery).
struct CrashEpisode {
    std::size_t peak_index = 0;
    std::size_t trough_index = 0;
    std::size_t recovery_index = 0;  // == last index if not recovered
    double depth = 0.0;              // trough-vs-peak fraction (<= 0)
    bool recovered = false;
};

// Finds every distinct decline whose depth reached at least `min_depth`
// (given as a positive fraction, e.g. 0.10 for -10%).
std::vector<CrashEpisode> detect_crashes(const Series& s, double min_depth = 0.10);

}  // namespace qc
