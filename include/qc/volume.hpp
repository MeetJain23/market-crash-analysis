#pragma once

#include <cstddef>

#include "qc/series.hpp"

namespace qc {

// Volume signature of a decline: classic capitulation shows the heaviest volume
// of the whole episode landing at or very near the price low.
struct Capitulation {
    double trough_volume_z = 0.0;   // z-score of trough-day volume within the episode
    std::size_t max_volume_index = 0;  // day of heaviest volume in the episode
    double max_volume_z = 0.0;
    bool capitulation_at_trough = false;  // heaviest volume within `near_days` of the low
};

// Analyses volume across [peak_index, trough_index]. `near_days` is how close
// the volume peak must be to the price low to count as capitulation.
Capitulation analyze_capitulation(const Series& s,
                                  std::size_t peak_index,
                                  std::size_t trough_index,
                                  std::size_t near_days = 3);

}  // namespace qc
