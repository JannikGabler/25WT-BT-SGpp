# WP-76: judged group `expensive/` (patch for the owner)

`judged-cases.patch` applies to HEAD 59f2899e4 with `git apply` (`--check` verified). It changes
these protected files:

- **`combigrid/benchmarks/perf/perf_problems.hpp`** (+60 lines), expensive source functions:
  - `COST_1US_ITERATIONS = 490` and `COST_100US_ITERATIONS = 49000`;
  - `burnChain(seed, iterations)`: a `noinline` dependent `std::fma` chain, every step pinned by
    an empty `asm volatile` (`"+x"` on x86, `"+w"` on AArch64, `"+r,m"` elsewhere);
  - `CostShape {uniform, ramp}`; ramp = factor 0.1 + 2.7 x₀² on the unit cube;
  - `expensive<F, ITERATIONS, SHAPE>(x)`: returns exactly `F(x)` and burns the work beside it.
- **`combigrid/benchmarks/perf/perf_e2e.cpp`** (+183 lines; the existing cases and functions are
  unchanged):
  - `ExpensiveFunc`, `addExpensiveQuadrature<Grid>`, `addExpensiveMIVec<Grid>`,
    `addExpensiveInterpolation<Grid>` and `addExpensiveCases`;
  - 25 cases `expensive/...`: 21 judged and 4 `-cached`. All use `perf::expSum` on [0, 1]^d with
    Clenshaw-Curtis (quadrature, global interpolation) or equidistant (multilinear) nodes and
    boundary offset 1;
  - `-cached` cases wrap the function in a `CachedSourceFunc` (default configuration, built in the
    `setup` phase of every run).
- **`combigrid/benchmarks/perf/perf_reference.cpp`** (+135 lines; existing cases unchanged):
  - `expensiveMivecCase<Grid>`, `expensiveInterpCase<Grid>`, `cachedQuadCase`,
    `addExpensiveQuadCases` and `addExpensiveInterpCases`;
  - 14 cases `expensive/...` (132 → 146) with the 1 µs integrands at reduced sizes:
    - quad sg/dsg cc b1 d10 L3 and d20 L2;
    - quad/sg d10 L3 cached;
    - mivec sg/dsg d15 S5 n300, plus sg ramp;
    - interp sg/dsg cc b1 d5 L4, plus sg ramp;
    - interplin sg/dsg equi b1 d5 L4, plus sg ramp.
  - info `nEvals`; the cached case records `cacheInserts` instead (concurrent misses of one point
    may evaluate it twice: its evaluation count varied 10001 → 10005 at 16 threads).
- **`combigrid/perf/reference/reference.jsonl`**: the 14 records, generated with this patch at
  1 thread (`perf_reference --filter '^expensive/' --threads 1`), appended.
- **`combigrid/perf/scripts/perf.py`** (+36 / −7 lines): `case_group()` and `group_sum_lines()`.
  `run e2e` and `ab e2e` print one sum line per group: suite, `expensive/`, `expensive/ cached`.
  A run without expensive cases keeps the historical single sum line.
- `tolerances.json`, `perf_harness.hpp`, `baseline/`: unchanged.

After applying: `perf.py protect --authorized-by-owner`. The new cases have no baseline values
(absolute reporting, as `interp/dsg`); their history over earlier states is in `history/`
(decision D4).

Unprotected companion: `micro-extras.patch` (`perf_micro.cpp`, +74 lines; compiles only with the
patched `perf_problems.hpp`, applied by the work session after the owner's patch):
- `micro/expensive/integrand/exp/d10/{c1us,c100us}`: drift monitor of the cost per evaluation;
- `micro/expensive/quad_{vec,scalar_x_m}/{sg,dsg}/cc/b1/{d7/L4/m4/c1us,d5/L3/m4/c100us}`:
  vector-valued expensive functions.

## Checks in the scratch worktree (HEAD + patch, `perf.py build normal`)

- Build: 0 new compiler / style warnings (the pre-existing cpplint line-length message for
  `perf_cache.hpp:109` reappears once when its lint target reruns).
- `perf.py refcheck` (`judged-refcheck.txt`):
  - 1 thread: 145 OK + the known WARN `interp/sg/equi/b0/d2/L5/cos-shifted`;
  - 16 threads: the same; no INFO.
  - New reference errors: quad 2.3e-6 (d10) / 1.1e-3 (d20), mivec 4.2e-3, interp 3.2e-4,
    interplin 1.4e-4 (relative).
  - The c1us values are bitwise equal to the cheap function's values at 1 thread (checked on the
    interplin cases against `interplin/{sg,dsg}/equi/b1/d5/L4/cos-shifted` before the switch to
    `exp`). At 16 threads SG multilinear interpolation reassociates its `reduction(+)`.
- Per-evaluation cost (`judged-micro-run1.json`, 1 thread): c1us 0.901 µs, c100us 99.84 µs.
- `perf.py run e2e --filter '^expensive/'`, two runs (16 threads, one process per case;
  `judged-e2e-run{1,2}.json`). The two runs differ by ≤ 1.1 % per case. Sums: 17.76 / 17.80 s
  (21 judged cases), 1.05 / 1.05 s (4 cached). Evaluation counts from `counts.md`. Efficiency =
  evaluations × 1-thread cost / 16 / total; at c1us it includes the SMT slowdown of the integrand
  (≈ +10–14 % at 16 threads).

The measured table is in `judged-table.md`. Highlights:
- quadrature on complete grids: 0.99;
- `mivec` at 100 µs: SG 0.42, DSG 0.62;
- multilinear DSG interpolation d5: 0.48;
- `-ramp` vs. uniform: mivec/sg 2.48 vs. 2.15 s, interplin/sg 1.29 vs. 0.84 s;
- cached interplin d8 at 1 µs: 0.20 (the cache's own cost dominates);
- peak RSS 11.9–15.9 MiB (cached ≤ 59.4 MiB).

Added runtime per suite pass: ≈ 17.8 s judged + 1.05 s cached = **18.8 s** (suite 36.65 s →
≈ 55.5 s). A 5-round `ab e2e --filter '^expensive/'` takes ≈ 6.5 min.
