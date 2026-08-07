// Minimal dependency-free test harness. Returns non-zero on any failure so it
// plugs straight into CTest / CI.
#include <cmath>
#include <cstdio>
#include <vector>

#include "qc/backtest.hpp"
#include "qc/crash.hpp"
#include "qc/drawdown.hpp"
#include "qc/indicators.hpp"
#include "qc/legs.hpp"
#include "qc/series.hpp"
#include "qc/util.hpp"

namespace {

int g_failures = 0;

void check(bool cond, const char* what) {
    if (!cond) {
        std::printf("  FAIL: %s\n", what);
        ++g_failures;
    }
}

void check_near(double a, double b, double tol, const char* what) {
    if (std::fabs(a - b) > tol) {
        std::printf("  FAIL: %s (%.6f vs %.6f)\n", what, a, b);
        ++g_failures;
    }
}

qc::Series make_series(const std::vector<double>& closes) {
    std::vector<qc::Bar> bars;
    for (std::size_t i = 0; i < closes.size(); ++i) {
        qc::Bar b;
        b.date = std::to_string(i);
        b.open = b.high = b.low = b.close = closes[i];
        b.volume = 1.0;
        bars.push_back(b);
    }
    return qc::Series(std::move(bars));
}

void test_sma() {
    const std::vector<double> x{1, 2, 3, 4, 5};
    const auto m = qc::sma(x, 3);
    check(!qc::is_valid(m[0]) && !qc::is_valid(m[1]), "sma warmup is NaN");
    check_near(m[2], 2.0, 1e-9, "sma[2]");
    check_near(m[3], 3.0, 1e-9, "sma[3]");
    check_near(m[4], 4.0, 1e-9, "sma[4]");
}

void test_ema() {
    const std::vector<double> x{1, 2, 3, 4, 5};
    const auto e = qc::ema(x, 3);
    check_near(e[2], 2.0, 1e-9, "ema seed == sma of first 3");
    check_near(e[3], 3.0, 1e-9, "ema[3]");  // (4-2)*0.5+2
    check_near(e[4], 4.0, 1e-9, "ema[4]");  // (5-3)*0.5+3
}

void test_rsi_bounds() {
    std::vector<double> up;
    for (int i = 0; i < 30; ++i) up.push_back(100.0 + i);
    const auto r = qc::rsi(up, 14);
    check_near(r[29], 100.0, 1e-6, "rsi of pure uptrend == 100");
    for (double v : r)
        if (qc::is_valid(v)) check(v >= 0.0 && v <= 100.0, "rsi in [0,100]");
}

void test_drawdown_monotonic() {
    const auto s = make_series({1, 2, 3, 4, 5});
    const auto mdd = qc::max_drawdown(s);
    check_near(mdd.depth, 0.0, 1e-12, "no drawdown when only rising");
}

void test_drawdown_vshape() {
    const auto s = make_series({100, 80, 60, 90, 120});
    const auto mdd = qc::max_drawdown(s);
    check_near(mdd.depth, -0.40, 1e-9, "v-shape max drawdown -40%");
    check(mdd.peak_index == 0 && mdd.trough_index == 2, "peak/trough indices");
}

void test_crash_and_legs() {
    // Down to 60, bounce to ~78 (>5%), down to 50, recover above 100.
    const auto s = make_series({100, 80, 60, 78, 65, 50, 70, 95, 110});
    const auto crashes = qc::detect_crashes(s, 0.10);
    check(crashes.size() == 1, "one crash episode detected");
    if (!crashes.empty()) {
        const auto& e = crashes[0];
        check(e.recovered, "episode recovered");
        check_near(e.depth, -0.50, 1e-9, "episode depth -50%");
        const auto legs = qc::segment_down_legs(s, e.peak_index, e.trough_index);
        check(legs.size() == 2, "decline made two legs");
    }
}

void test_backtest_shapes() {
    const auto s = make_series({10, 11, 12, 13, 14, 15});
    std::vector<char> sig(s.size(), 0);
    sig[0] = 1;  // buy at 10, hold 2 -> exit 12 -> +20%
    const auto bt = qc::run_signal_backtest(s, sig, 2);
    check(bt.trades == 1, "one trade");
    check_near(bt.mean_return, 0.20, 1e-9, "forward return +20%");
}

}  // namespace

int main() {
    std::printf("running qc tests...\n");
    test_sma();
    test_ema();
    test_rsi_bounds();
    test_drawdown_monotonic();
    test_drawdown_vshape();
    test_crash_and_legs();
    test_backtest_shapes();

    if (g_failures == 0) {
        std::printf("all tests passed\n");
        return 0;
    }
    std::printf("%d test(s) failed\n", g_failures);
    return 1;
}
