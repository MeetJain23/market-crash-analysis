#include "qc/precrash.hpp"

#include <algorithm>

#include "qc/indicators.hpp"
#include "qc/util.hpp"

namespace qc {

PrecrashSignals scan_precrash(const Series& s,
                              std::size_t peak_index,
                              int lookback,
                              int baseline) {
    PrecrashSignals out;
    if (s.empty() || peak_index == 0) return out;

    // Distribution days: a lower close on higher volume than the day before,
    // counted over the `lookback` bars ending at the peak.
    const std::size_t start =
        (peak_index >= static_cast<std::size_t>(lookback))
            ? peak_index - static_cast<std::size_t>(lookback) + 1
            : 1;
    for (std::size_t i = start; i <= peak_index; ++i) {
        if (s[i].close < s[i - 1].close && s[i].volume > s[i - 1].volume)
            ++out.distribution_days;
    }

    // Volatility now vs. an earlier calmer window.
    const std::vector<double> a = atr(s, 14);
    if (is_valid(a[peak_index])) out.atr_at_peak = a[peak_index];

    // Baseline window sits before the lookback window.
    long b_end = static_cast<long>(peak_index) - lookback;
    long b_start = b_end - baseline;
    b_start = std::max<long>(b_start, 0);
    if (b_end > b_start) {
        double sum = 0.0;
        int cnt = 0;
        for (long i = b_start; i < b_end; ++i) {
            const double v = a[static_cast<std::size_t>(i)];
            if (is_valid(v)) {
                sum += v;
                ++cnt;
            }
        }
        if (cnt > 0) out.atr_baseline = sum / cnt;
    }

    if (out.atr_baseline > 0.0)
        out.vol_expansion = out.atr_at_peak / out.atr_baseline;

    return out;
}

}  // namespace qc
