# WP-47 — drafted judged cases `interplin/dsg/...` (owner's deliverable)

`judged-cases.patch` (applies to HEAD 6df60cb50 with `git apply`; `--check` verified) changes the
protected files:
- `combigrid/benchmarks/perf/perf_e2e.cpp`: `addLinearInterpolationDSG` and 4 cases
  `interplin/dsg/equi/b1/{d2/L16/p16384, d5/L8/p1024, d8/L5/p256, d10/L5/p32}` (same problems,
  points and sizes as the 4 `interplin/sg/...` cases; grid = `DeltasSparseGrid`).
- `combigrid/benchmarks/perf/perf_reference.cpp`: `interpCase` templated on the grid type; 16
  cases `interplin/dsg/{equi,cc}/b{0,1}/{d1/L6,d2/L6,d3/L5,d5/L4}/cos-shifted` next to their
  `interplin/sg/...` counterparts (104 → 120 cases).
- `combigrid/perf/reference/reference.jsonl`: the 16 records, generated with the WP-47 code at
  1 thread (`perf_reference --filter '^interplin/dsg/' --threads 1`). Their values agree with the
  `interplin/sg/...` reference values (recorded on the unoptimized code, same points) within
  7.6e-15; evaluations per case (8 points): d1 16 (= sg), d2 224 (sg 416), d3 1024 (2944),
  d5 8016 (32256).
- `tolerances.json`, `perf_problems.hpp`: unchanged (no override needed).

After applying: `perf.py protect --authorized-by-owner`. The new e2e cases have no baseline.

## Checks in a scratch git worktree (WP-47 tree + patch, `perf.py build normal`)
- `perf.py refcheck`: 120 OK at 1 and 16 threads (`judged-refcheck.txt`).
- `perf.py run e2e --filter '^interplin/'`, 2 runs (16 threads; one process per case;
  `judged-e2e-run{1,2}.json`):

| case | total run 1 | total run 2 | operator run 1 | peak RSS | sg counterpart (total) |
|---|---|---|---|---|---|
| interplin/dsg/equi/b1/d2/L16/p16384 | 182.6 ms | 184.2 ms | 177.8 ms | 20.3 MiB | 112.2 / 112.9 ms, 19.5 MiB |
| interplin/dsg/equi/b1/d5/L8/p1024 | 183.2 ms | 253.5 ms | 180.6 ms | 13.0 MiB | 361.4 / 358.8 ms, 13.0 MiB |
| interplin/dsg/equi/b1/d8/L5/p256 | 374.9 ms | 370.3 ms | 372.5 ms | 13.1 MiB | 1.214 / 1.282 s, 12.7 MiB |
| interplin/dsg/equi/b1/d10/L5/p32 | 438.3 ms | 432.4 ms | 436.3 ms | 16.6 MiB | 1.841 / 1.839 s, 13.6 MiB |

Added e2e time per suite pass ≈ 1.18 s (sum of the run-1 totals; d5 is bimodal: 183 vs. 254 ms).
The operator phase of the DSG cases includes building the grid's index on the first call of
every sample (each sample builds a new grid): d2 L16 ≈ 7.7 ms at 1 thread; at d5 – d10 it is
within the call-to-call spread of a single call (≤ 3 ms).

Note: these worktree runs used the tree before the index moved from a dedicated `DeltasSparseGrid`
member into the generic `OperatorDataCache`. The operator code is identical; the cache adds one
lock and lookup per call. The patch touches only public headers and still applies to HEAD
(`git apply --check`).
