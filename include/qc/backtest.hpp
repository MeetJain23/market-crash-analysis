#pragma once

#include <cstddef>
#include <vector>

#include "qc/series.hpp"

namespace qc {

// Outcome of holding for `horizon` bars after every entry signal.
struct BacktestResult {
    std::size_t trades = 0;
    double mean_return = 0.0;
    double median_return = 0.0;
    double win_rate = 0.0;   // fraction of trades with positive return
    double best = 0.0;
    double worst = 0.0;
    int horizon = 0;
};

// Buys on each true entry in `signal` and holds `horizon` bars, measuring the
// forward close-to-close return. `signal` must be the same length as `s`
// (use char as bool: 1 = entry). This measures whether a rule *would have*
// worked historically — it is not a live trading system.
BacktestResult run_signal_backtest(const Series& s,
                                   const std::vector<char>& signal,
                                   int horizon);

// Entry rule: RSI crossing down through `threshold` (a fresh oversold reading,
// not every bar it stays oversold). Returns a signal vector for `closes`.
std::vector<char> rsi_oversold_signal(const std::vector<double>& closes,
                                      int period = 14,
                                      double threshold = 30.0);

}  // namespace qc
