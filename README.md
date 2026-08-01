# quantcrash

A pure C++17 toolkit for dissecting how markets crash and recover — built to
study episodes like 2008 and COVID-2020 from daily OHLCV data.

The goal is *characterisation, not prediction*: measure what historical bottoms
had in common (drawdown depth, capitulation volume, indicator extremes, how many
down-legs a decline made) and back-test whether simple rules would have caught
them. It does **not** claim to call the next bottom in advance — anything that
does is fooling you.

## What it computes

- **Drawdown engine** — running drawdown and the worst peak-to-trough decline.
- **Crash detection** — every distinct decline past a depth threshold, with peak,
  trough, recovery and whether it has recovered yet.
- **Leg segmentation** — how many down-legs a crash made, via a ZigZag that
  treats a relief rally past a threshold as a leg boundary.
- **Capitulation analysis** — whether the heaviest volume of the episode landed
  at the price low (a classic bottom signature).
- **Pre-crash scan** — distribution days and volatility expansion heading into
  the top.
- **Backtest harness** — forward-return stats for a rule (ships with RSI<30 buy).
- **Indicators** — SMA, EMA, RSI, ATR/True Range, rolling std, z-score.

## Build & run

```bash
cmake -S . -B build
cmake --build build
./build/quantcrash data/sample_spx.csv
ctest --test-dir build --output-on-failure
```

## Data

Free daily history with no API key: [Stooq](https://stooq.com/). Example — the
S&P 500 index:

```
https://stooq.com/q/d/l/?s=^spx&i=d
```

Save downloads under `data/` (bulk files under `data/full/`, which is
git-ignored). A small synthetic COVID-window sample ships in
`data/sample_spx.csv` so the repo runs out of the box.

> Note: the pre-crash volatility-expansion reading needs ~60 bars *before* the
> peak, so it reads 0 on the tiny bundled sample. Point it at a full multi-year
> dataset and it populates.

## Roadmap

- [x] Project scaffolding
- [x] `Bar` + `Series`
- [x] CSV loader
- [x] Drawdown engine
- [x] Indicators: SMA / EMA / RSI / ATR / rolling std / z-score
- [x] Crash detector
- [x] Leg segmentation
- [x] Capitulation / volume analysis
- [x] Pre-crash signal scan
- [x] Backtest harness
- [x] CLI report
- [x] Unit tests
- [ ] Real 2008 / 2020 datasets committed + written-up findings
- [ ] News / sentiment layer (separate, likely a Python sidecar)

## License

MIT
