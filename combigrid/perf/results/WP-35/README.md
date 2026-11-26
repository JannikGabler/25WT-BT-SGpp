# WP-35 — evidence (fragile tests)

Machine and builds as in `combigrid/perf/README.md`; tests-only WP (library `.text` of
`WP-35-before` / `WP-35-after` byte-identical: combigrid 613 742 B, base 2 764 910 B, same SHA-256).

## Tools (`tools/`)

- `shim/fake_time.c`: `LD_PRELOAD` shim, `time()` returns `$FAKE_TIME`. The clock-seeded tests seed
  `base::RandomNumberGenerator` with `std::time`, so `FAKE_TIME=<seed>` replays a reported seed
  without code changes: `gcc -O2 -shared -fPIC -o fake_time.so fake_time.c`, then
  `FAKE_TIME=1793379453 LD_PRELOAD=.../fake_time.so ./test_combigrid_boost --run_test=...`.
- `fuzz.sh <bindir> <run_test filter> <first seed> <count> <out>`: one process per seed
  (seed = first + i * 7919, 8 processes x 2 OpenMP threads), appends `seed<TAB>failing case`.
  `bindir` holds a copy of `test_combigrid_boost` + both libraries.
- `seeded_cases.py`: prints the `--run_test` filter of every clock-seeded case (run in
  `combigrid/tests/`).
- `build_variant.sh <name> <before|after|keep> <normal|off|fma>`: test binary whose two cache-test
  TUs are compiled from HEAD / the working tree with the normal build's flags, plus
  `-ffp-contract=off` or with the shifted domain computed by `std::fma(0.1, k, -0.5)` (needs
  `acc_cmd.txt` / `cc_cmd.txt` / `link_cmd.txt` taken from a `perf.py build normal` log).

## Results

| What | Before (HEAD) | After |
|---|---|---|
| 12 SG quadrature cases (equi + cc: sin interval / cube, linear unit / interval, monomial cube, random polynomial), 10 000 seeds (debug before, normal after) | 100 seeds fail (1.0 %): `seed-sweep-quadrature-before.txt` | 0 (incl. 28 pinned cases per run) |
| other 105 clock-seeded cases, 500 seeds (debug) | 0 | – |
| all 127 clock-seeded cases, 200 seeds | – | normal 0, debug 0 |
| margin of the new magnitude-scaled checks, 3000 seeds | – | max error 0.21 of the tolerance (`margins-quadrature.txt`) |
| cache TUs, 30 runs each (`cache-tu-variants-30runs.txt`) | normal 0/30, `std::fma` domain 30/30 fail (`diskHits > 0`) | normal / `-ffp-contract=off` / `std::fma` 0/30 |
| shared cache test statistics | 6778 in memory, 7 spilled, 56 disk hits (PLAN) | 2048 in memory (16 x 128), 4737 beyond, 75-86 k disk hits |
| `grid_storage_pinning`, debug builds | Armadillo only: 55 failed checks; no FFT: 141 (all cc / mixed bitwise hashes) | 0 in Eigen-only, Armadillo-only, no-FFT |
| full debug suite, no FFT backend | 38 failed checks in 2 CC-rule tests (`fullsuite-debug-no-fft.log`): direct-sum CC weights up to 1.4e4 eps / m (bound 8) | 566/566 cases pass (`fullsuite-debug-no-fft-after.log`); CC weights <= 2.61 eps / m for all 28 sizes up to 4097 nodes, weight sums within 2.7e-15 of 1 |
| `grid_storage_pinning`, portable `OPT=1` build (Eigen + Armadillo, no `-march=native`) | 184 of 1585 checks fail (optimized-build hashes) | pass: the 252 optimized-build hash checks are skipped outside the recorded configuration |

Adaptive policy, normal build, cost estimates (`CacheStatistics`, every evaluation timed):
- concurrent cold start (4 x 4 threads, cc b1 d4 L5): `cheapFunc` 48-90 ns vs. hit 110-300 ns
  (contention), switch-off rule c_f < 0.8 E;
- 1-thread cold first call: `cheapFunc` / miss overhead 0.10-0.14 (20/20 switched off), trivial
  function 0.001-0.008; afterwards the concurrent calls add 8 switches (probes) in 20/20 runs;
- 1-thread probes (cc b0 d3 L5, 21 calls): `cheapFunc` / (1.25 x hit) = 0.57-0.95 (rule < 1, latent
  flake of `cheap_function_is_switched_off_and_probed`), trivial function 0.05-0.09.

Follow-up round (no-FFT CC fallback, optimized-build hash gate): `refcheck-followup.txt` (same
single known WARN), `e2e-followup.md` / `.json` (A/B `WP-35-after` vs `WP-35-followup`, full e2e
suite: 1.000x, neutral; the library `.text` changed only through LTO inlining budgets).

Third round (pinning without stored bit patterns; `grid_storage_pinning` golden values with
tolerance 1e-12 (1 + |g|), recorded with the debug build + Eigen):

| build | pinning + `quadrature_weights` | values differing bitwise from the table | max \|r − g\| / (1 + \|g\|) |
|---|---|---|---|
| debug, Eigen + Armadillo (recording) | 8181/8181 | 0 of 4896 | 0 |
| debug, no FFT | 8181/8181 | 206 | 5.2e-16 |
| debug, Armadillo only | 8181/8181 | 101 | 5.2e-16 |
| `OPT=1`, no `-march=native` (CI-like) | 8181/8181 | 0 | 0 |
| normal (judged) | 8181/8181 | 1641 | 1.8e-15 |

Tolerance vs. real differences: pairs of configurations that give the same grid differ by
≤ 1e-15 (round-off of another order); pairs of different grids (other boundary offset / MI set /
grid type) by ≥ 5.4e-9 (`cc b0` vs. `b1`, unit d2, sg). The tolerance has ≥ 550x headroom above
the build spread and ≥ 5000x below the smallest real difference.
