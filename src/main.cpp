#include <cstdio>
#include <exception>
#include <string>
#include <vector>

#include "qc/backtest.hpp"
#include "qc/crash.hpp"
#include "qc/csv_loader.hpp"
#include "qc/drawdown.hpp"
#include "qc/legs.hpp"
#include "qc/precrash.hpp"
#include "qc/series.hpp"
#include "qc/volume.hpp"

namespace {

void print_header(const qc::Series& s, const std::string& path) {
    const auto& first = s[0];
    const auto& last = s[s.size() - 1];
    const auto mdd = qc::max_drawdown(s);
    std::printf("quantcrash — %s\n", path.c_str());
    std::printf("  bars   : %zu   range: %s -> %s\n", s.size(),
                first.date.c_str(), last.date.c_str());
    std::printf("  close  : %.2f -> %.2f\n", first.close, last.close);
    std::printf("  maxDD  : %.2f%%  (%s -> %s)\n", mdd.depth * 100.0,
                s[mdd.peak_index].date.c_str(),
                s[mdd.trough_index].date.c_str());
}

void print_episode(const qc::Series& s, const qc::CrashEpisode& e, int n) {
    std::printf("\n--- crash #%d ---------------------------------------\n", n);
    std::printf("  peak    : %s (%.2f)\n", s[e.peak_index].date.c_str(),
                s[e.peak_index].close);
    std::printf("  trough  : %s (%.2f)\n", s[e.trough_index].date.c_str(),
                s[e.trough_index].close);
    std::printf("  depth   : %.2f%%   over %zu bars   %s\n", e.depth * 100.0,
                e.trough_index - e.peak_index,
                e.recovered ? "(recovered)" : "(still under water)");

    // How many legs did it make on the way down?
    const auto legs = qc::segment_down_legs(s, e.peak_index, e.trough_index);
    std::printf("  legs    : %zu\n", legs.size());
    for (std::size_t i = 0; i < legs.size(); ++i) {
        const auto& L = legs[i];
        std::printf("     leg %zu: %s -> %s  %.2f%%\n", i + 1,
                    s[L.start_index].date.c_str(), s[L.end_index].date.c_str(),
                    L.pct * 100.0);
    }

    // Where was the bottom made — did volume capitulate at the low?
    const auto cap = qc::analyze_capitulation(s, e.peak_index, e.trough_index);
    std::printf("  volume  : trough z=%.2f, heaviest z=%.2f on %s  %s\n",
                cap.trough_volume_z, cap.max_volume_z,
                s[cap.max_volume_index].date.c_str(),
                cap.capitulation_at_trough ? "[capitulation at low]"
                                           : "[no capitulation at low]");

    // What did it look like before the fall?
    const auto pre = qc::scan_precrash(s, e.peak_index);
    std::printf("  pre-fall: distribution days=%d, ATR expansion x%.2f\n",
                pre.distribution_days, pre.vol_expansion);
}

void print_backtest(const qc::Series& s) {
    const auto closes = s.closes();
    const auto sig = qc::rsi_oversold_signal(closes, 14, 30.0);
    const auto bt = qc::run_signal_backtest(s, sig, 10);
    std::printf("\n=== backtest: buy RSI(14)<30, hold 10 bars ==========\n");
    if (bt.trades == 0) {
        std::printf("  no qualifying trades in this window\n");
        return;
    }
    std::printf("  trades=%zu  mean=%.2f%%  median=%.2f%%  win=%.0f%%\n",
                bt.trades, bt.mean_return * 100.0, bt.median_return * 100.0,
                bt.win_rate * 100.0);
    std::printf("  best=%.2f%%  worst=%.2f%%\n", bt.best * 100.0,
                bt.worst * 100.0);
}

}  // namespace

int main(int argc, char** argv) {
    const std::string path = (argc > 1) ? argv[1] : "data/sample_spx.csv";
    try {
        const qc::Series s = qc::load_csv(path);
        if (s.empty()) {
            std::fprintf(stderr, "No usable rows in %s\n", path.c_str());
            return 1;
        }

        print_header(s, path);

        const auto crashes = qc::detect_crashes(s, 0.10);
        std::printf("\ncrashes >= 10%%: %zu\n", crashes.size());
        int n = 1;
        for (const auto& e : crashes) print_episode(s, e, n++);

        print_backtest(s);
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "error: %s\n", e.what());
        return 1;
    }
}
