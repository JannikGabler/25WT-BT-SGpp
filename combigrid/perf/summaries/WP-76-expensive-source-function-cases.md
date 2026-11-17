# WP-76 — Judged suite: cases with expensive source functions

Status: PROPOSED · Date: 2026-11-17 · Base commit: 59f2899e4
Recommendation: accept. The new group `expensive/` delivers the following:
- 21 judged and 4 cached cases, 18.8 s per pass;
- reproducible (8-round A/A all within noise, sums 1.000x);
- exact references, refcheck OK;
- it exposes load-balance and evaluation-count effects that the cheap suite cannot see.

Machine: `perf.py calibrate` ratios e2e 1.004 / 1.000, micro 1.003 / 1.007 (start); e2e 1.019 / 1.010, micro 1.000 / 1.003 (before the measurements)

## Change
- Owner's decisions (asked in-session, plan `results/WP-76/plan.md` §6–7):
  - D1: FMA-chain cost, c1us + c100us, uniform + `ramp` on 3 cases (ramp = deterministic factor 0.1–2.8 over x₀);
  - D2: full list;
  - D3: separate group `expensive/`, own sum line, ranking unchanged;
  - D4: no baseline values, but a history over baseline / RP-1…RP-5 / HEAD;
  - D5: plain judged + cached cases in the group (not ranked) + micro extras for vector-valued functions;
  - D6: into `perf_e2e.cpp` / `perf_problems.hpp`.
- Protected files (patch `results/WP-76/judged-cases.patch`, applied by the owner, rehashed):
  - `perf_problems.hpp`: `burnChain` (noinline dependent `std::fma` chain, `asm volatile` per step), `COST_{1,100}US_ITERATIONS` = 490 / 49 000, `CostShape`, `expensive<F, N, SHAPE>` (returns exactly `F(x)`);
  - `perf_e2e.cpp`: 25 cases `expensive/...` on the unit cube, b1, integrand `exp`. The list is in `results/WP-76/judged-table.md`: SG/DSG pairs of quad d10 L4 / d20 L3 / d15 L5, mivec d15 S6 / d20 S10, interp cc d10 L3 p8 / d10 L4 p16, interplin equi d5 L5 p16 / d10 L4 p16; 3 `-ramp`, 4 `-cached`;
  - `perf_reference.cpp` + `reference.jsonl`: 14 cases (132 → 146);
  - `perf.py`: per-group sum lines in `run` / `ab` (suite, `expensive/`, `expensive/ cached`);
  - `baseline/noise_e2e.json` (owner's request after the summary, applied by the owner + rehash): 25 `expensive/` entries added (38 → 63, existing entries unchanged). Source: the 8-round A/A of WP-76-after (`noise-expensive.json`). `total` floors: mostly 0.1–1 %; interp/sg d10 c1us 16 %, interplin/sg d10 c1us 9.8 %, interplin/sg d5 c100us-cached 7.4 % (bimodal single processes).
- Design changes against the first plan: `uq` and `cosProd` → `exp`. With b1, `uq` had 70–100 % reference errors, which would make refcheck insensitive. `cosProd` is ≈ 0 at d7–d10 on the shifted domain and costs 230 ns itself.
- Unprotected:
  - `perf_micro.cpp`: 10 `micro/expensive/...` cases (cost-drift monitor, vector-valued);
  - `README.md` (group, reference count, micro composition and filter);
  - `CLAUDE.md` (cache gotcha: cached judged cases).
- Files: `results/WP-76/`:
  - `plan.md`, `judged-cases.{md,patch}`, `micro-extras.patch`, `judged-table.md`, `counts.md`;
  - prototype `expensive_proto.cpp` / `cheap_cost.cpp` / `count_cases.cpp` / `build_proto.sh` + `proto-*.md|txt`;
  - reports `e2e-aa-old.*`, `e2e-expensive-r8.*`, `noise-expensive.json`, `micro-after.json`, `refcheck.txt`;
  - `history/` (driver, scripts, `history.md`, `runs/`).

## Gain vs. cost
- Gain: no speedup (measurement WP). New coverage: 21 judged cases, **17.77 s** (8-round medians), plus 4 cached cases, 1.05 s.
  - Suite pass 36.65 → ≈ 55.5 s (+51 %).
  - A 5-round A/B of the group takes ≈ 7 min; 8 rounds took 10.8 min.
- Memory: new cases 11.9–15.9 MiB, cached ≤ 59.4 MiB. Old cases A/A: geomean 1.011, 0 flags (libraries bit-identical).
- API / functionality: none (library, flags and build unchanged). Maintainability: +378 lines of benchmark code (protected) + 74 (micro).

## Tests
- Pinning tests: n/a (no library change).
- Results: debug `test --repeat 3` ✓ · normal `test --repeat 3` ✓ · ASan n.a. The debug `perf_reference --filter ^expensive/` at 16 t ran without assertion failures.

## Reference check
- `perf.py refcheck --snapshot WP-76-after`: 145 OK + the known WARN `interp/sg/equi/b0/d2/L5/cos-shifted` (unchanged, pre-existing) at 1 and 16 threads; no INFO.
- New reference errors (relative): quad 2.3e-6 (d10) / 1.1e-3 (d20), mivec 4.2e-3, interp 3.2e-4, interplin 1.4e-4.
- c1us values are bitwise equal to the cheap function's at 1 t. The cached case records `cacheInserts`: its evaluation count varied 10001 → 10005 at 16 t.

## Benchmarks (16 threads; A/B = WP-76-before / WP-76-after)
- **A/A of the 48 old cases** (5 rounds; only the `perf_e2e` binary differs): 41 within noise, 7 "faster" 1.008–1.080 (`exp`-integrand quad / mivec: code placement of the binary), 0 SLOWER. Sum 36.76 → 36.45 s (1.008x).
- **New cases** (8-round A/A of WP-76-after, `e2e-expensive-r8.md`): all within noise; sums 17.768 / 17.766 s, cached 1.046 / 1.042 s.
  - c100us CIs mostly ±0.2–2 %.
  - Bimodal single processes (+7–27 %): interp/sg c1us 4 of 16, interplin/sg c1us 6 of 16, interplin/sg c100us 3 of 16. The medians are stable.
- Efficiency (evaluations × 1 t cost / 16 / time; `judged-table.md`):
  - quad 0.99;
  - interp 0.95–0.99;
  - mivec SG **0.42** / DSG **0.62** (c100us);
  - interplin/dsg d5 **0.48**;
  - c1us 0.65–0.88;
  - ramp vs. uniform: interplin/sg 0.84 → 1.29 s, mivec/sg 2.16 → 2.48 s.
- L1 micro (1 t, WP-76-after, new): integrand 0.901 µs / 99.79 µs per evaluation. m = 4 vector vs. scalar: SG c1us 61.1 vs. 212.8 ms (3.5x), c100us 255.9 vs. 1021.9 ms (4.0x); DSG 3.6x / 4.0x.
- **History** (`history/history.md`, 3 rounds per state):
  - 17 common cases: baseline 73.97 s, RP-2 71.25, RP-3 16.88, RP-5 16.60, HEAD 16.60 s. The step is MI-vector **generation** (d20 S10: 28 s → 3 ms, WP-03).
  - Evaluation-bound operators are unchanged since the baseline (±1 %).
  - c1us gains: quad/sg 1.16x, quad/dsg 1.21x, interplin/sg 1.13x.

## Attempts not kept
- Integrands `uq` (quad/mivec) and `cosProd` (interp) on the shifted domain: dropped (reference errors 0.7–1.0, e2e sum errors up to 6; `cos` costs 230 ns at d10).
- Global-interpolation reference at d10 L2: absolute error 4e-2 on values ≈ 1e-4 → moved to d5 L4.
- A shared atomic evaluation counter in timed runs: +30 % at c1us × 16 t → counts only in the reference suite.
- Note: the protection hook resolved a relative path in the scratch worktree against the repo. The worktree lies outside the repo; the drafts there used absolute paths. Repo files changed only through the owner's `git apply`.

## Profile shift / follow-ups
- Load balance of `mivec` (SG 42 %, DSG 62 % at 100 µs; bound 2.16 → ≈ 0.91 s). Hypothesis: few very large TGs per thread.
- DSG multilinear interpolation (48 %).
- Static TG scheduling in SG interplin / interp under non-uniform cost.
- **WP-56 shift** (history rp-5 → HEAD): mivec/sg operator c100us +4.6 %, ramp +3.2 %, c1us −14 %. The SG quadrature's TG-loop balance changed; RP-6 should look at it.
- SG operators evaluate duplicates: quad/sg d10 L4 1.535 s vs. DSG 0.571 s vs. cached 0.576 s. An SG dedup or cache default would be a large expensive-group lever (owner decision: functionality).
- Cache overhead at c1us × 16 t: interplin/sg d8 cached efficiency 0.20.
- Not covered by the noise model: the WP-45/47 `interp/dsg` / `interplin/dsg` cases (10) still have no entries (0.3 % floor).
- Adaptive WPs 63 ff.: add `expensive/adapt/...` with the same integrands; count evaluations per thread (`plan.md` §4).
- After the owner's commit: `perf.py verify`. All hashes match now; the only messages are the uncommitted `reference/reference.jsonl` and `baseline/noise_e2e.json`.

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`; reverts the applied patch, the micro extras and the docs)
