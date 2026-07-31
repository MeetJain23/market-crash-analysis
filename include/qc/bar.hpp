#pragma once

#include <string>

namespace qc {

// A single daily OHLCV observation.
struct Bar {
    std::string date;   // ISO date as read from source, e.g. "2020-03-16"
    double open  = 0.0;
    double high  = 0.0;
    double low   = 0.0;
    double close = 0.0;
    double volume = 0.0;
};

}  // namespace qc
