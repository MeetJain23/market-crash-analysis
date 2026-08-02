#pragma once

#include <cstddef>

#include "qc/series.hpp"

namespace qc {

// Warning-sign readings taken from the window leading into a market peak.
struct PrecrashSignals {
    int distribution_days = 0;    // down days on rising volume (institutional selling)
    double atr_at_peak = 0.0;     // volatility right at the top
    double atr_baseline = 0.0;    // volatility over an earlier, calmer window
    double vol_expansion = 0.0;   // atr_at_peak / atr_baseline (>1 = vol rising in)
};

// Scans the `lookback` bars before `peak_index` for distribution days, and
// compares volatility at the peak against an earlier `baseline`-length window.
PrecrashSignals scan_precrash(const Series& s,
                              std::size_t peak_index,
                              int lookback = 20,
                              int baseline = 60);

}  // namespace qc
