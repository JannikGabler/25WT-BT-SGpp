# WP-76 — Plan: judged-suite cases with expensive source functions (phase 1)

Status: plan for the owner's decisions D1–D6 (§6). Measured on 2026-11-17, HEAD 59f2899e4,
snapshot `WP-76-before`, `perf.py build normal`, 16 threads (`OMP_PROC_BIND=close`,
`OMP_PLACES=threads`), boost off. `perf.py calibrate`: e2e 1.004 burst / 1.000 sustained, micro
1.003 / 1.007.

Files in this directory: `expensive_proto.cpp` + `build_proto.sh` (prototype driver, public API,
judged flags, not part of the build), `proto-cost.txt` (cost generators), `proto-count.md`
(evaluation counts), `proto-case-*.md` / `proto-candidates.md` (case timings),
`inventory-e2e.json` (`run e2e` of the current suite).

## 1. Inventory of the current suite

`run e2e --snapshot WP-76-before` (one process per case): **48 cases, sum of `total` 36.65 s**.

| group | cases | sum `total` | share | integrand (1 t cost per evaluation through `SourceFunc`, `cheap-cost.txt`) |
|---|---|---|---|---|
| quad/sg | 13 | 8.28 s | 22.6 % | `exp` 17 (d2) – 18 ns (d10), 24 ns (d20); `uq` 19–27 ns (d2–d10); `sin` d7 114 ns |
| quad/dsg | 9 | 17.83 s | 48.7 % | `exp` 17–24 ns, `uq` 20–27 ns |
| interp/sg | 8 | 4.23 s | 11.5 % | `cos` 35 (d2) – 230 ns (d10) |
| interp/dsg | 6 | 0.83 s | 2.3 % | `cos` as above |
| interplin/sg | 4 | 3.52 s | 9.6 % | `cos` 35 – 230 ns |
| interplin/dsg | 4 | 0.80 s | 2.2 % | `cos` as above |
| mivec | 4 | 1.15 s | 3.1 % | `exp` 17–24 ns (d5–d20) |

All integrands cost 17–230 ns (all but `sin` / `cos` at d ≥ 5 below 50 ns), i.e. the operators' own
work (weights, basis values, tries, index building) dominates almost every case: the suite judges
framework overhead. The evaluation count, the parallel efficiency of the evaluation loops and the
cache are not judged.

## 2. Cost model (prototype `proto-cost.txt`, 1 / 8 / 16 threads)

Value and cost are separated: the integrand returns **exactly** the cheap function (`uqIntegrand`,
`cosProd`, closed-form references stay valid) and burns a fixed amount of work beside it, whose
result is pinned with an empty `asm volatile` (cannot be folded, vectorized, reassociated or
removed; `noinline` burner).

| generator | how | 1 t cost per iteration | 16 t / 1 t per-evaluation cost (1 / 10 / 100 µs) | properties |
|---|---|---|---|---|
| **chain** (recommended) | dependent `std::fma` chain, `asm volatile("" : "+x"(w))` per step | 2.042 ns = 4.08 cycles at 1.996 GHz (FMA latency) | 1.14 / 1.02 / 1.015 | deterministic work: cost in cycles (fixed at boost off), latency-bound → barely competes with the SMT sibling (16 t speedup 14.0 / 15.6 / 15.8x vs. 1 t); independent of the compiler (latency chain cannot be reordered); scales with the clock like real code |
| mix | 4 independent chains + `sqrt` | 12.5 ns | 1.25 / 1.04 / 1.02 | closer to real numerics (port pressure), same scaling within 1–2 % |
| spin | `rdtscp` loop until a TSC deadline | 0.50 ns per tick | 1.03 / 1.01 / 1.01 | exact wall time independent of clock and code; but hides frequency effects, 37 ns granularity, a preempted spin still ends at its deadline (masks preemption noise) |

Cost levels: `c1us` = 490 iterations, `c100us` = 49 000 iterations (fixed constants; nominal at
2.0 GHz; measured 1 t cost 0.90 µs + integrand, 99.9 µs). The extra 1 t → 16 t cost at 1 µs
(+14 %) comes from the integrand's `exp` and the loop around it sharing the core, not from the
chain. Requires an FMA instruction (`-march=native` / `-mfma`; without it `std::fma` is a slow
libm call — the judged configuration has it).

**Non-uniform cost** (`ramp`): factor 0.1 + 2.7·t², t = position of x₀ in the domain's first
interval (mean 1 over the domain; range 0.1–2.8, 28x; CC nodes cluster at the ends → node mean
≈ 1.14). Models a simulation whose cost depends on a parameter (stiffness, convergence).

## 3. Findings (prototype timings at 16 threads; eff = ideal / measured, ideal = Σ cost / 16)

Evaluation counts (`proto-count.md`): SG quadrature evaluates Σ nGP (duplicates included), DSG
the distinct points (b1 d10 L4: 244 361 vs. 90 561 = 2.7x; d20 L3 2.0x); global interpolation
evaluates every grid point **per query point** (SG Σ nGP·p, DSG distinct·p); multilinear SG 2^d
corners per TG per query point (d10 L4 p16: 16.4 M evaluations vs. 0.24 M grid points), DSG the
distinct corners (2.65 M). Boundary offset 0 is infeasible at high d (b0 d10 L2: 2.4 M distinct
points) → all high-d cases use **b1**.

| case (prototype name) | evals | c100us: time / eff | c100us ramp: eff | c1us: time / eff |
|---|---|---|---|---|
| quad/sg/cc/b1/d10/L4 | 244 361 | 1.54 s / 0.99 | 0.99 | |
| quad/dsg/cc/b1/d10/L4 | 90 561 | 0.57 s / 0.99 | 0.99 | |
| quad/sg/cc/b1/d20/L3 | 164 971 | 1.05 s / 0.98 | 0.99 | |
| quad/dsg/cc/b1/d20/L3 | 82 401 | 0.52 s / 0.99 | 0.98 | |
| quad/sg/cc/b1/d15/L5 | 17 984 096 | | | 1.15 s / 0.98 |
| quad/dsg/cc/b1/d15/L5 | 5 235 393 | | | 0.35 s / 0.94 |
| mivec/sg/cc/b1/d15/S6/n300 | 145 512 | 2.16 s / **0.42** | 0.43 | |
| mivec/dsg/cc/b1/d15/S6/n300 | 87 789 | 0.88 s / **0.62** | | |
| mivec/sg/cc/b1/d20/S10/n300 | 7 679 632 | | | 0.67 s / **0.72** |
| mivec/dsg/cc/b1/d20/S10/n300 | 4 767 673 | | | 0.33 s / 0.89 |
| interp/sg/cc/b1/d7/L3/p8 | 56 064 | 0.41 s / 0.86 | **0.72** | |
| interp/sg/cc/b1/d10/L3/p8 | 163 888 | 1.08 s / 0.95 | 0.95 | |
| interp/dsg/cc/b1/d10/L3/p8 | 80 008 | 0.51 s / 0.99 | 0.98 | |
| interp/sg/cc/b1/d10/L4/p16 | 3 909 776 | | | 0.32 s / 0.75 |
| interp/dsg/cc/b1/d10/L4/p16 | 1 448 976 | | | 0.12 s / 0.73 |
| interplin/sg/equi/b1/d5/L5/p16 | 128 512 | 0.82 s / 0.98 | **0.79** | |
| interplin/dsg/equi/b1/d5/L5/p16 | 26 912 | 0.35 s / **0.49** | | |
| interplin/sg/equi/b1/d10/L4/p16 | 16 400 384 | | | 1.44 s / 0.71 |
| interplin/dsg/equi/b1/d10/L4/p16 | 2 648 064 | | | 0.27 s / 0.61 |

- **Complete-grid quadrature is evaluation-bound and balanced** (98–99 % at 100 µs, 94–98 % at
  1 µs): only fewer evaluations (SG → distinct points) can gain there.
- **Load imbalance the cheap suite does not show**:
  - `mivec` SG 42 % / DSG 62 % at 100 µs. Hypothesis: anisotropic MI sets have few very large
    tensor grids, each evaluated by one thread.
  - DSG multilinear interpolation 49 % (half of every call idle or serial).
  - Static TG scheduling in SG multilinear interpolation (98 → 79 % with non-uniform cost) and
    global interpolation at d7 (86 → 72 %).

  Upper bounds of what a balancing WP could save: mivec/sg d15 2.16 → ≈ 0.91 s; interplin/dsg
  0.35 → ≈ 0.17 s.
- **At 1 µs the operators' overhead is still visible** (interp / interplin 61–75 %). The
  in-process spread (min–max of 3 runs) is ≤ 0.5 % for almost all cases: low-noise cases.
- **Caching** (`proto-case-cached.md`, fresh `CachedSourceFunc` MemoryOnly per run):
  - 100 µs: SG quad d10 L4 1.54 → 0.58 s (= DSG); interp/sg d10 L3 p8 1.08 → 0.070 s (15x);
    interplin/sg d8 L4 p32 25.3 → 5.5 s.
  - 1 µs: interplin/sg d8 1.7x faster; mivec/sg d20 S10 0.67 → 1.24 s **slower** (7.7 M lookups,
    4.8 M inserts, 880 MB).
- Prototype pitfall: one shared `std::atomic` evaluation counter cost 30 % at 1 µs × 16 threads
  (quad/sg d10 L6: 1.65 vs. 1.14 s). Judged cases must not count evaluations inside the timed
  region; the reference suite records them (`info.nEvals`).

## 4. Proposed design

Integrands (new header, see D6): functors `ExpensiveIntegrand{value function, cost level, shape,
domain}` usable as `SourceFunc` (lambda → `std::function`, compiles against every API since the
baseline); burner `burnChain(seed, iterations)` `noinline`; constants `C1US_ITERATIONS = 490`,
`C100US_ITERATIONS = 49000`. Values: quadrature `uqIntegrand` on [0, 1]^d (thesis forward-UQ
setting, exact `uqIntegral`), interpolation `cosProd` on the shifted domain (existing points).

Names: `expensive/<op>/<grid>/<nodes>-<integrand>/b<b>/d<d>/<L|S..n..>[/p<p>]/c<cost>[-ramp]`,
e.g. `expensive/quad/sg/cc-uq/b1/d10/L4/c100us`. The `expensive/` prefix keeps every existing
filter (`^quad/sg/`, …), report and noise entry unchanged; `^expensive/` selects the group.

Case list (D2 option "full", 21 cases; times = prototype at 16 t, one pass):

| # | case | evals | time |
|---|---|---|---|
| 1–2 | expensive/quad/{sg,dsg}/cc-uq/b1/d10/L4/c100us | 244 k / 91 k | 1.54 / 0.57 s |
| 3–4 | expensive/quad/{sg,dsg}/cc-uq/b1/d20/L3/c100us | 165 k / 82 k | 1.05 / 0.52 s |
| 5–6 | expensive/mivec/{sg,dsg}/cc-uq/b1/d15/S6/n300/c100us | 146 k / 88 k | 2.16 / 0.88 s |
| 7–8 | expensive/interp/{sg,dsg}/cc/b1/d10/L3/p8/c100us | 164 k / 80 k | 1.08 / 0.51 s |
| 9–10 | expensive/interplin/{sg,dsg}/equi/b1/d5/L5/p16/c100us | 129 k / 27 k | 0.82 / 0.35 s |
| 11 | expensive/mivec/sg/cc-uq/b1/d15/S6/n300/c100us-ramp | 146 k | 2.39 s |
| 12 | expensive/interp/sg/cc/b1/d7/L3/p8/c100us-ramp | 56 k | 0.26 s |
| 13 | expensive/interplin/sg/equi/b1/d5/L5/p16/c100us-ramp | 129 k | 1.08 s |
| 14–15 | expensive/quad/{sg,dsg}/cc-uq/b1/d15/L5/c1us | 18.0 M / 5.2 M | 1.15 / 0.35 s |
| 16–17 | expensive/mivec/{sg,dsg}/cc-uq/b1/d20/S10/n300/c1us | 7.7 M / 4.8 M | 0.67 / 0.33 s |
| 18–19 | expensive/interp/{sg,dsg}/cc/b1/d10/L4/p16/c1us | 3.9 M / 1.4 M | 0.32 / 0.12 s |
| 20–21 | expensive/interplin/{sg,dsg}/equi/b1/d10/L4/p16/c1us | 16.4 M / 2.6 M | 1.44 / 0.27 s |

Sum ≈ **17.9 s per pass** (+49 % on 36.65 s); a 5-round `ab` of the group ≈ 6–7 min
(the full suite A/B grows from ≈ 18–23 to ≈ 25–30 min). Peak memory: small (largest grid d15 L5,
≈ 20–60 MiB; to be measured).

Option "lean" (12 cases, ≈ 9.9 s): 3–10 (c100us pairs, quad d20 only), 13 (ramp interplin/sg),
14–15 (c1us quad).

Reference suite: one reference case per new configuration at reduced size, with the `c1us`
integrand (verifies that the burner leaves the value bitwise unchanged; ≤ 50 k evaluations, ≤ 0.1 s
at 1 t): quad sg/dsg cc b1 {d10 L3, d20 L2} uq; mivec sg/dsg cc b1 d15 S5 n300 uq; interp sg/dsg
cc b1 d10 L2 cos-shifted; interplin sg/dsg equi b1 d5 L4 cos-shifted; one `-ramp` case (value
identical, documents it) → 132 + 13 = 145 records, `nEvals` as INFO (an optimization that cuts
evaluations shows up there). Tolerances: no overrides expected.

Drift monitor (unprotected): `perf_micro` cases `micro/expensive/integrand/{uq,cos}/c{1,100}us`
(1 t cost per evaluation; must stay at ≈ 1.0 / 100 µs).

Dimension-adaptive refinement (WP-63 ff.): the integrands are plain functors usable by any
operator. Adaptive cases fit the group as `expensive/adapt/<op>/dsg/cc-uq/b1/d<d>/tol<…>/c100us`
with the same cost levels (times comparable with the fixed-grid cases). They need the evaluation
count as a result. Take it from per-thread counters summed after the call, not from a shared
atomic (§3). Adaptive DSG quadrature at 100 µs is the motivating use case. The `mivec/dsg` cases
(DSG from an adaptive-like MI set) are its fixed-grid precursor.

## 5. Phase 2 verification (after the owner applies the patch)

`perf.py verify`; `refcheck` at 1 and 16 threads (all OK; known WARN
`interp/sg/equi/b0/d2/L5/cos-shifted`); `run e2e --filter '^expensive/'` ≥ 8 rounds (CI / spread
per case, per-evaluation cost check against `micro/expensive/...`); A/A of the old cases between
`WP-76-before` and `WP-76-after` (route A of D6: the old binary is unchanged → expected
bit-identical code; route B: placement check with a hybrid snapshot as `rp-4-binWP45`).

## 6. Decisions for the owner

- **D1 cost model / levels**: chain (recommended) vs. spin vs. mix; levels c1us + c100us
  (recommended) vs. c1us/c10us/c100us vs. c100us only; non-uniform `ramp` on 3 cases (recommended).
- **D2 case list / runtime**: full 21 cases ≈ 17.9 s (recommended) vs. lean 12 cases ≈ 9.9 s.
- **D3 ranking**: separate group `expensive/` with its own sum line in `ab` reports and STATUS
  (recommended). The ranking stays "absolute time saved on the cheap suite, d ≥ 7 first". The
  expensive group's saving is reported beside it and ranks WPs that target evaluation counts /
  balance. Alternatives: one pool unweighted (the expensive group would be ≈ 33 % of the sum: its
  absolute seconds depend on the arbitrary cost level); weighted pool.
- **D4 baseline values**: none, reported absolutely like `interp/dsg` (recommended). Reasons: at
  fed2cdb4e the counters' iteration API (`for (tg : grid)`), DSG interpolation and multilinear DSG
  are missing → a separate compatibility build; quadrature is already ≥ 98 % efficient, so the
  baseline adds little. Alternative: a one-time informational run at fed2cdb4e (hybrid worktree,
  stored in `results/WP-76/`, not in `baseline/`). Optionally add the new cases' A/A spread to
  `baseline/noise_e2e.json` (additive).
- **D5 variants**: plain `SourceFunc` only in the judged group (recommended). Cached cases mix
  cache behavior into operator judgments; their 100 µs gains are 2.7–15x and are better tracked as
  micro / feature cases (`micro/cache/op/...` exists for 1 µs). Python callables measure GIL
  contention at 16 t (WP-34) → outside. Vector-valued: covered by the `*_vec` micro cases →
  outside. Alternative: 2–3 `-cached` cases in the group (separate sum, not ranked).
- **D6 route**: (A, recommended) new protected `perf_e2e_expensive.cpp` (own binary) + new
  protected `perf_expensive.hpp`; `perf_e2e.cpp` and `perf_problems.hpp` stay untouched → the
  old cases' binary and code placement stay identical (no ±2–6 % shifts, RP-4). `perf.py` (≈ 40
  lines): `e2e` = union of both binaries (case → binary), snapshots copy the new binary, old
  snapshots without it list no expensive cases ("new case" lines), per-group sum lines.
  (B) as WP-45/47: cases in `perf_e2e.cpp`, integrands in `perf_problems.hpp`; `perf.py` only the
  sum lines; costs an A/A + hybrid placement check, and later comparisons with pre-WP-76 snapshots
  carry the binary difference.

## 7. Owner's decisions (2026-11-17) and the implemented design

- **D1**: FMA chain; c1us + c100us; uniform + `ramp` on 3 cases (asked: ramp is deterministic, no
  random variance; uniform cost = identical work per evaluation).
- **D2**: full list (21 judged cases).
- **D3**: separate group `expensive/`, own sum line; ranking unchanged.
- **D4**: no baseline values; instead a **history**: the new cases built against representative
  past states (baseline, RP-1 … RP-5, HEAD) → `history/`.
- **D5**: plain cases judged + **cached cases in the group** (own sum line, not ranked) + **micro
  extras** for vector-valued expensive functions.
- **D6**: into `perf_e2e.cpp` / `perf_problems.hpp` (asked: both routes allow running cheap only /
  expensive only / both via `--filter`).

Changes against §4 found during implementation:
- `uq` → `exp` (`expSum`) for quadrature / mivec: with boundary offset 1 the CT misses the
  boundary peak of 4(1−x)³; reference errors were 70–100 %, which would make `refcheck` (1.1 ×
  reference error) insensitive. With `exp`: 2.3e-6 … 4.2e-3.
- `cosProd` → `exp` on the unit cube for interpolation: on the shifted domain at d7–d10,
  `cosProd` is nearly zero (relative errors of the e2e sums 0.9–6), and it costs 230 ns itself at
  d10 (distorts c1us). With `exp`: 2.3e-5 … 1.3e-4. Ramp now on the unit cube only.
- Cached cases: 4 (quad/sg d10 L4, interp/sg d10 L3 p8, interplin/sg d5 L5 p16 at c100us,
  interplin/sg d8 L4 p32 at c1us; 59 MiB).
- Reference: 14 cases. Global interpolation at d5 L4 instead of d10 L2: d10 L2 had absolute
  errors of 4e-2 on values of ~1e-4. The cached case records `cacheInserts`, since its evaluation
  count varied 10001 → 10005 at 16 threads.
- Implementation, checks and measurements: `judged-cases.md`, the summary
  `summaries/WP-76-expensive-source-function-cases.md`.
