#include "qc/backtest.hpp"

#include <algorithm>
#include <cstddef>

#include "qc/indicators.hpp"
#include "qc/util.hpp"

namespace qc {

BacktestResult run_signal_backtest(const Series& s,
                                   const std::vector<char>& signal,
                                   int horizon) {
    BacktestResult r;
    r.horizon = horizon;
    if (horizon <= 0 || s.empty() || signal.size() != s.size()) return r;

    std::vector<double> rets;
    for (std::size_t i = 0; i + static_cast<std::size_t>(horizon) < s.size(); ++i) {
        if (!signal[i]) continue;
        const double entry = s[i].close;
        const double exit = s[i + static_cast<std::size_t>(horizon)].close;
        if (entry > 0.0) rets.push_back(exit / entry - 1.0);
    }

    if (rets.empty()) return r;

    r.trades = rets.size();
    double sum = 0.0;
    int wins = 0;
    r.best = rets[0];
    r.worst = rets[0];
    for (double x : rets) {
        sum += x;
        if (x > 0) ++wins;
        r.best = std::max(r.best, x);
        r.worst = std::min(r.worst, x);
    }
    r.mean_return = sum / rets.size();
    r.win_rate = static_cast<double>(wins) / rets.size();

    std::sort(rets.begin(), rets.end());
    const std::size_t mid = rets.size() / 2;
    r.median_return =
        (rets.size() % 2 == 0) ? (rets[mid - 1] + rets[mid]) / 2.0 : rets[mid];

    return r;
}

std::vector<char> rsi_oversold_signal(const std::vector<double>& closes,
                                      int period,
                                      double threshold) {
    const std::vector<double> r = rsi(closes, period);
    std::vector<char> sig(closes.size(), 0);
    for (std::size_t i = 1; i < r.size(); ++i) {
        if (is_valid(r[i]) && is_valid(r[i - 1]) &&
            r[i] < threshold && r[i - 1] >= threshold) {
            sig[i] = 1;  // fresh cross below the threshold
        }
    }
    return sig;
}

}  // namespace qc
