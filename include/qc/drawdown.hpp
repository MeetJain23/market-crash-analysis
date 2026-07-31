#pragma once

#include <cstddef>
#include <vector>

#include "qc/series.hpp"

namespace qc {

// Running drawdown from the highest close seen so far, as a fraction in
// [-1, 0] (e.g. -0.34 == 34% below the prior peak). One value per bar.
std::vector<double> drawdown_series(const Series& s);

// Summary of the single worst peak-to-trough decline in the series.
struct MaxDrawdown {
    double depth = 0.0;          // most negative drawdown fraction (<= 0)
    std::size_t peak_index = 0;  // bar index of the pre-crash high
    std::size_t trough_index = 0;// bar index of the low
};

MaxDrawdown max_drawdown(const Series& s);

}  // namespace qc
