# quantcrash

A pure C++17 toolkit for dissecting how markets crash and recover which is built to
study episodes like 2008 and COVID-2020 from daily OHLCV data.

The goal is *characterisation, not prediction*: measure what historical bottoms
had in common (drawdown depth, capitulation volume, indicator extremes, how many
down-legs a decline made) and back-test whether simple rules would have caught
them. It does **not** claim to call the next bottom in advance or anything that
does is fooling you.

## Status

Foundation in place:

- `Bar` / `Series` core data structures
- CSV loader (Stooq / Yahoo `Date,Open,High,Low,Close,Volume` format)
- Drawdown engine (running drawdown, worst peak-to-trough)
- CLI that prints a summary + max drawdown for a dataset

See the roadmap below for what's next.

## Build & run

```bash
cmake -S . -B build
cmake --build build
./build/quantcrash data/sample_spx.csv
```

## Data

Free daily history with no API key: [Stooq](https://stooq.com/). Example: the
S&P 500 index:

```
https://stooq.com/q/d/l/?s=^spx&i=d
```

Save downloads under `data/` (bulk files under `data/full/`, which is
git-ignored). A small synthetic COVID-window sample ships in
`data/sample_spx.csv` so the repo runs out of the box.

## Roadmap

- [x] Project scaffolding
- [x] `Bar` + `Series`
- [x] CSV loader
- [x] Drawdown engine
- [ ] Indicators: SMA / EMA
- [ ] RSI
- [ ] ATR + rolling volatility (z-score)
- [ ] Crash detector (drawdown episodes past a threshold)
- [ ] Leg segmentation (down-legs via relief-rally threshold)
- [ ] Capitulation / volume analysis around troughs
- [ ] Pre-crash signal scan
- [ ] Backtest harness
- [ ] CLI report / per-episode tables
- [ ] Unit tests

## License

MIT
