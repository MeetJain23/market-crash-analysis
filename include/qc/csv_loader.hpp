#pragma once

#include <string>

#include "qc/series.hpp"

namespace qc {

// Loads a daily OHLCV CSV into a Series.
//
// Expected header (case-insensitive), as produced by Stooq and Yahoo:
//   Date,Open,High,Low,Close,Volume
// An optional "Adj Close" column (Yahoo) is tolerated and ignored.
//
// Rows are kept in file order; if `sort_ascending` is true the result is
// sorted oldest-first regardless of source ordering. Blank lines and rows
// with non-numeric OHLC fields are skipped.
//
// Throws std::runtime_error if the file cannot be opened or the header is
// unrecognised.
Series load_csv(const std::string& path, bool sort_ascending = true);

}  // namespace qc
