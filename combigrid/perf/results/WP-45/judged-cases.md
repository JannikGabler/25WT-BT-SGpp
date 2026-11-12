# WP-45 — drafted judged cases `interp/dsg/...` (owner's deliverable)

`judged-cases.patch` (applies to HEAD 6138b8b9e with `git apply`; `--check` verified) changes the
protected files:
- `combigrid/benchmarks/perf/perf_e2e.cpp`: WP-47's `addLinearInterpolationDSG` generalized to
  `addInterpolationDSG(suite, linear, ...)` (the 4 `interplin/dsg/...` cases keep their names and
  code path); 6 new cases `interp/dsg/{cc/b0/d2/L12/p64, cc/b0/d3/L9/p64, cc/b0/d5/L6/p64,
  cc/b0/d7/L4/p16, cc/b0/d10/L2/p8, equi/b0/d3/L5/p64}` — the problems, points and sizes of the
  `interp/sg/...` cases with nested nodes (the two cheb1 cases have no DSG counterpart: Chebyshev
  nodes of the first type are not nested, `DeltasSparseGrid` throws).
- `combigrid/benchmarks/perf/perf_reference.cpp`: `interpCase` calls `interpolate` /
  `interpolateLinear` for both grid types (the `if constexpr` and `<type_traits>` are gone); 12
  cases `interp/dsg/{cc/b0, cc/b1, equi/b0}/{d1/L6, d2/L5, d3/L4, d5/L3}/cos-shifted` next to
  their `interp/sg/...` counterparts (120 → 132 cases).
- `combigrid/perf/reference/reference.jsonl`: the 12 records, generated with the WP-45 code at
  1 thread (`perf_reference --filter '^interp/dsg/' --threads 1`; bitwise identical at 16
  threads), inserted after their counterparts; the existing 120 records are unchanged.
- `tolerances.json`, `perf_problems.hpp`: unchanged.

After applying: `perf.py protect --authorized-by-owner`. The new e2e cases have no baseline.

## Reference values vs. the unoptimized `interp/sg/...` references (same points; `judged-ref-compare.md`)

| case | max err SG ref | max err DSG | max abs(DSG − SG ref) | evals SG / DSG |
|---|---|---|---|---|
| cc/b0 d1 L6 / d2 L5 / d3 L4 / d5 L3 | 7.8e-16 / 2.2e-15 / 2.3e-07 / 7.0e-05 | 1.0e-15 / 1.3e-15 / 2.3e-07 / 7.0e-05 | ≤ 3.6e-15 | 1032 / 1032, 11800 / 4616, 45496 / 12040, 440784 / 96264 |
| cc/b1 d1 L6 / d2 L5 / d3 L4 / d5 L3 | 7.8e-16 / 1.2e-15 / 9.9e-07 / 0.28 | 1.0e-15 / 1.1e-15 / 9.9e-07 / 0.28 | ≤ 1.0e-15 | 1032 / 1032, 8664 / 3592, 16600 / 5896, 20448 / 9608 |
| equi/b0 d3 L4 / d5 L3 | 2.7e-08 / 4.0e-05 | 2.7e-08 / 4.0e-05 | 5.6e-10 / 1.7e-13 | 45496 / 12040, 440784 / 96264 |
| equi/b0 d1 L6 / d2 L5 (Lebesgue regime) | 0.59 / 0.32 | 0.60 / **0.70** | 7.1e-03 / 0.37 | 1032 / 1032, 11800 / 4616 |

equi d1 L6 (129 equidistant nodes) and d2 L5 (65 per dimension) are dominated by round-off in
both operators (Lebesgue constants up to ~1e16): measured against the SG reference with the
refcheck rule, equi d2 L5 would be a WARN (2.15x the SG reference error); as its own reference it
is OK. The DSG is not systematically worse there: on the e2e equi d3 L5 problem (64 points) its
summed error is 0.78 vs. 1.30 for the SG operator (`numerics-sg-vs-dsg.md`).

## Checks in a scratch git worktree (WP-45 tree + patch, `perf.py build normal`)
- `perf.py refcheck`: 132 OK at 1 and 16 threads (`judged-refcheck.txt`).
- `perf.py run e2e --filter '^interp/'`, 2 runs (16 threads; one process per case;
  `judged-e2e-run{1,2}.json`):

| case | total run 1 | total run 2 | peak RSS | sg counterpart (total, peak RSS) |
|---|---|---|---|---|
| interp/dsg/cc/b0/d2/L12/p64 | 56.3 ms | 52.7 ms | 16.1 MiB | 176.9 / 175.8 ms, 15.1 MiB |
| interp/dsg/cc/b0/d3/L9/p64 | 47.1 ms | 43.2 ms | 12.4 MiB | 248.2 / 243.3 ms, 12.4 MiB |
| interp/dsg/cc/b0/d5/L6/p64 | 170.7 ms | 172.4 ms | 12.5 MiB | 1.437 / 1.456 s, 12.0 MiB |
| interp/dsg/cc/b0/d7/L4/p16 | 149.1 ms | 150.4 ms | 12.3 MiB | 1.256 / 1.254 s, 13.1 MiB |
| interp/dsg/cc/b0/d10/L2/p8 | 393.9 ms | 394.3 ms | 12.3 MiB | 1.984 / 1.992 s, 24.7 MiB |
| interp/dsg/equi/b0/d3/L5/p64 | 6.46 ms | 6.41 ms | 12.1 MiB | 53.6 / 51.1 ms, 12.1 MiB |

Added e2e time per suite pass ≈ 0.82 s (sum of the totals; their SG counterparts: 5.16 s). The
operator phase includes building the grid's index on the first call of every sample (each sample
builds a new grid; within the call-to-call spread).
