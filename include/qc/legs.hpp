#pragma once

#include <cstddef>
#include <vector>

#include "qc/series.hpp"

namespace qc {

// One down-wave within a larger decline: a high-to-low segment, bounded by
// relief rallies on either side.
struct Leg {
    std::size_t start_index = 0;
    std::size_t end_index = 0;
    double start_close = 0.0;
    double end_close = 0.0;
    double pct = 0.0;  // (end - start) / start, negative for a down-leg
};

// Splits the decline from `peak_index` to `trough_index` into down-legs using a
// ZigZag: a counter-move of at least `rally_threshold` (positive fraction, e.g.
// 0.05 for 5%) off a local low is treated as a relief rally that ends the
// current leg. The number of legs answers "how many legs did it make falling?".
std::vector<Leg> segment_down_legs(const Series& s,
                                   std::size_t peak_index,
                                   std::size_t trough_index,
                                   double rally_threshold = 0.05);

}  // namespace qc
