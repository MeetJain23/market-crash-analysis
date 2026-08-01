#pragma once

#include <vector>

#include "qc/series.hpp"

// Technical indicators. Every function returns a vector the same length as its
// input; warm-up positions that don't yet have a value are filled with NaN
// (see qc::is_valid). Nothing here predicts anything — these are the raw
// measurements the analysis layer builds on.
namespace qc {

// Simple moving average over `period` closes.
std::vector<double> sma(const std::vector<double>& x, int period);

// Exponential moving average, seeded with the SMA of the first `period` values.
std::vector<double> ema(const std::vector<double>& x, int period);

// Wilder's RSI (default period 14). Values in [0, 100].
std::vector<double> rsi(const std::vector<double>& closes, int period = 14);

// True Range per bar (needs high/low/close).
std::vector<double> true_range(const Series& s);

// Wilder's Average True Range (default period 14).
std::vector<double> atr(const Series& s, int period = 14);

// Rolling population standard deviation over `period` values.
std::vector<double> rolling_std(const std::vector<double>& x, int period);

// Rolling z-score: (x - rolling_mean) / rolling_std over `period` values.
std::vector<double> zscore(const std::vector<double>& x, int period);

}  // namespace qc
