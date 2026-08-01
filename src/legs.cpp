#include "qc/legs.hpp"

namespace qc {

namespace {
Leg make_leg(const Series& s, std::size_t a, std::size_t b) {
    Leg leg;
    leg.start_index = a;
    leg.end_index = b;
    leg.start_close = s[a].close;
    leg.end_close = s[b].close;
    leg.pct = (leg.start_close > 0.0)
                  ? (leg.end_close - leg.start_close) / leg.start_close
                  : 0.0;
    return leg;
}
}  // namespace

std::vector<Leg> segment_down_legs(const Series& s,
                                   std::size_t peak_index,
                                   std::size_t trough_index,
                                   double rally_threshold) {
    std::vector<Leg> legs;
    if (trough_index <= peak_index) return legs;

    enum Dir { Down, Up };
    Dir dir = Down;
    std::size_t last_pivot = peak_index;   // start of the current down-leg
    double ext = s[peak_index].close;      // extreme in current direction
    std::size_t ext_idx = peak_index;

    for (std::size_t i = peak_index + 1; i <= trough_index; ++i) {
        const double c = s[i].close;
        if (dir == Down) {
            if (c < ext) {
                ext = c;
                ext_idx = i;  // new lower low
            } else if (ext > 0.0 && (c - ext) / ext >= rally_threshold) {
                // Relief rally confirmed: the swing low at ext_idx ends a leg.
                legs.push_back(make_leg(s, last_pivot, ext_idx));
                last_pivot = ext_idx;
                dir = Up;
                ext = c;
                ext_idx = i;
            }
        } else {  // Up
            if (c > ext) {
                ext = c;
                ext_idx = i;  // new higher high
            } else if (ext > 0.0 && (ext - c) / ext >= rally_threshold) {
                // Roll-over confirmed: the swing high at ext_idx starts a leg.
                last_pivot = ext_idx;
                dir = Down;
                ext = c;
                ext_idx = i;
            }
        }
    }

    // Close the final down-leg into the trough.
    if (dir == Down && trough_index > last_pivot)
        legs.push_back(make_leg(s, last_pivot, trough_index));

    return legs;
}

}  // namespace qc
