#include "qc/indicators.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>

#include "qc/util.hpp"

namespace qc {

std::vector<double> sma(const std::vector<double>& x, int period) {
    const std::size_t n = x.size();
    std::vector<double> out(n, nan_value());
    if (period <= 0 || n < static_cast<std::size_t>(period)) return out;

    double sum = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        sum += x[i];
        if (i >= static_cast<std::size_t>(period)) sum -= x[i - period];
        if (i + 1 >= static_cast<std::size_t>(period)) out[i] = sum / period;
    }
    return out;
}

std::vector<double> ema(const std::vector<double>& x, int period) {
    const std::size_t n = x.size();
    std::vector<double> out(n, nan_value());
    if (period <= 0 || n < static_cast<std::size_t>(period)) return out;

    const double k = 2.0 / (period + 1.0);
    // Seed with the SMA of the first `period` values.
    double seed = 0.0;
    for (int i = 0; i < period; ++i) seed += x[static_cast<std::size_t>(i)];
    seed /= period;

    double prev = seed;
    out[static_cast<std::size_t>(period - 1)] = seed;
    for (std::size_t i = static_cast<std::size_t>(period); i < n; ++i) {
        prev = (x[i] - prev) * k + prev;
        out[i] = prev;
    }
    return out;
}

std::vector<double> rsi(const std::vector<double>& closes, int period) {
    const std::size_t n = closes.size();
    std::vector<double> out(n, nan_value());
    if (period <= 0 || n <= static_cast<std::size_t>(period)) return out;

    const std::size_t p = static_cast<std::size_t>(period);

    // Seed average gain/loss over the first `period` deltas (closes[1..p]).
    double avg_gain = 0.0, avg_loss = 0.0;
    for (std::size_t i = 1; i <= p; ++i) {
        const double d = closes[i] - closes[i - 1];
        if (d > 0) avg_gain += d;
        else avg_loss += -d;
    }
    avg_gain /= period;
    avg_loss /= period;

    auto to_rsi = [](double g, double l) {
        if (l == 0.0) return 100.0;
        const double rs = g / l;
        return 100.0 - 100.0 / (1.0 + rs);
    };
    out[p] = to_rsi(avg_gain, avg_loss);

    // Wilder smoothing for the rest.
    for (std::size_t i = p + 1; i < n; ++i) {
        const double d = closes[i] - closes[i - 1];
        const double gain = d > 0 ? d : 0.0;
        const double loss = d < 0 ? -d : 0.0;
        avg_gain = (avg_gain * (period - 1) + gain) / period;
        avg_loss = (avg_loss * (period - 1) + loss) / period;
        out[i] = to_rsi(avg_gain, avg_loss);
    }
    return out;
}

std::vector<double> true_range(const Series& s) {
    const std::size_t n = s.size();
    std::vector<double> tr(n, nan_value());
    if (n == 0) return tr;

    tr[0] = s[0].high - s[0].low;
    for (std::size_t i = 1; i < n; ++i) {
        const double prev_close = s[i - 1].close;
        const double hl = s[i].high - s[i].low;
        const double hc = std::fabs(s[i].high - prev_close);
        const double lc = std::fabs(s[i].low - prev_close);
        tr[i] = std::max({hl, hc, lc});
    }
    return tr;
}

std::vector<double> atr(const Series& s, int period) {
    const std::size_t n = s.size();
    std::vector<double> out(n, nan_value());
    if (period <= 0 || n < static_cast<std::size_t>(period)) return out;

    const std::vector<double> tr = true_range(s);
    const std::size_t p = static_cast<std::size_t>(period);

    double seed = 0.0;
    for (std::size_t i = 0; i < p; ++i) seed += tr[i];
    seed /= period;

    out[p - 1] = seed;
    double prev = seed;
    for (std::size_t i = p; i < n; ++i) {
        prev = (prev * (period - 1) + tr[i]) / period;
        out[i] = prev;
    }
    return out;
}

std::vector<double> rolling_std(const std::vector<double>& x, int period) {
    const std::size_t n = x.size();
    std::vector<double> out(n, nan_value());
    if (period <= 1 || n < static_cast<std::size_t>(period)) return out;

    const std::size_t p = static_cast<std::size_t>(period);
    double sum = 0.0, sum_sq = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        sum += x[i];
        sum_sq += x[i] * x[i];
        if (i >= p) {
            sum -= x[i - p];
            sum_sq -= x[i - p] * x[i - p];
        }
        if (i + 1 >= p) {
            const double mean = sum / period;
            const double var = std::max(0.0, sum_sq / period - mean * mean);
            out[i] = std::sqrt(var);
        }
    }
    return out;
}

std::vector<double> zscore(const std::vector<double>& x, int period) {
    const std::size_t n = x.size();
    std::vector<double> out(n, nan_value());
    if (period <= 1 || n < static_cast<std::size_t>(period)) return out;

    const std::size_t p = static_cast<std::size_t>(period);
    const std::vector<double> sd = rolling_std(x, period);
    double sum = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        sum += x[i];
        if (i >= p) sum -= x[i - p];
        if (i + 1 >= p && is_valid(sd[i]) && sd[i] > 0.0) {
            const double mean = sum / period;
            out[i] = (x[i] - mean) / sd[i];
        }
    }
    return out;
}

}  // namespace qc
