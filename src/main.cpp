#include <cstdio>
#include <exception>
#include <string>

#include "qc/csv_loader.hpp"
#include "qc/drawdown.hpp"
#include "qc/series.hpp"

int main(int argc, char** argv) {
    const std::string path = (argc > 1) ? argv[1] : "data/sample_spx.csv";

    try {
        const qc::Series s = qc::load_csv(path);
        if (s.empty()) {
            std::fprintf(stderr, "No usable rows in %s\n", path.c_str());
            return 1;
        }

        const auto& first = s[0];
        const auto& last = s[s.size() - 1];
        const auto mdd = qc::max_drawdown(s);

        std::printf("quantcrash — loaded %s\n", path.c_str());
        std::printf("  bars      : %zu\n", s.size());
        std::printf("  range     : %s -> %s\n", first.date.c_str(), last.date.c_str());
        std::printf("  close     : %.2f -> %.2f\n", first.close, last.close);
        std::printf("  max DD    : %.2f%%\n", mdd.depth * 100.0);
        std::printf("  peak      : %s (%.2f)\n",
                    s[mdd.peak_index].date.c_str(), s[mdd.peak_index].close);
        std::printf("  trough    : %s (%.2f)\n",
                    s[mdd.trough_index].date.c_str(), s[mdd.trough_index].close);
        std::printf("  DD length : %zu bars\n",
                    mdd.trough_index - mdd.peak_index);
        return 0;
    } catch (const std::exception& e) {
        std::fprintf(stderr, "error: %s\n", e.what());
        return 1;
    }
}
