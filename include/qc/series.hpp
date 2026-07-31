#pragma once

#include <cstddef>
#include <vector>

#include "qc/bar.hpp"

namespace qc {

// An ordered price series (oldest -> newest). Thin wrapper over a vector of
// Bars that hands back column views the indicator/analysis code wants.
class Series {
public:
    Series() = default;
    explicit Series(std::vector<Bar> bars) : bars_(std::move(bars)) {}

    std::size_t size() const { return bars_.size(); }
    bool empty() const { return bars_.empty(); }

    const Bar& operator[](std::size_t i) const { return bars_[i]; }
    Bar& operator[](std::size_t i) { return bars_[i]; }

    const std::vector<Bar>& bars() const { return bars_; }
    void push_back(const Bar& b) { bars_.push_back(b); }

    // Column extractors — allocate a fresh vector each call, so cache the
    // result if you need it in a hot loop.
    std::vector<double> closes() const { return column(&Bar::close); }
    std::vector<double> opens()  const { return column(&Bar::open); }
    std::vector<double> highs()  const { return column(&Bar::high); }
    std::vector<double> lows()   const { return column(&Bar::low); }
    std::vector<double> volumes() const { return column(&Bar::volume); }

private:
    std::vector<double> column(double Bar::*field) const {
        std::vector<double> out;
        out.reserve(bars_.size());
        for (const auto& b : bars_) out.push_back(b.*field);
        return out;
    }

    std::vector<Bar> bars_;
};

}  // namespace qc
