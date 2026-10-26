## Notes

- **Clenshaw-Curtis weights on master and e6a21c331 are the naive O(n²) formula.** In both
  states the FFT (Waldvogel) branches of `clenshaw_curtis_quadrature_rule.cpp` do not compile
  (const member call to a free function, `std::vector` → `DataVector`, undeclared `len`; the
  mirror loop bound `m < b.size()` would never terminate), so every build of these states that
  compiled used the naive fallback. The worktree builds disable the `__has_include` detection
  (which today finds `<armadillo>` in `/usr/include` and fails to compile) to reproduce exactly
  that. The FFT works since 91dbd3b1b (before the baseline). Affected: every `cc` quadrature
  case (`quad/*/cc-uq`, `mivec`); interpolation does not use quadrature weights.
- **`quad/sg/cc-uq/b0/d2/L20` was not run on master / e6a21c331**: its level-20 1D rule has
  2,097,153 nodes, the naive formula needs (2²⁰)² ≈ 1.1·10¹² dependent recurrence steps for that
  single weight vector, measured at 2.3 ns/step (2.0 GHz) → ≥ 42 min per run (estimate).
- **DSG quadrature of e6a21c331** is the first implementation (before 45fa2f1c8 "factorizing the
  global weights along a coefficient trie"). Cases that did not finish within 45 s are lower
  bounds; for `quad/dsg/equi-exp/b0/d7/L10` an earlier attempt ran 521 s for warm-up + sample
  without finishing (→ > 4.3 min per run).
- **Build changes needed only to compile the old states with today's GCC 15.2** (no effect on
  the generated code of the library): master `-include cstdint` (GCC 15 no longer includes it
  transitively). `perf_e2e` did not exist then: the protected sources were copied unchanged
  except master without the DSG cases and e6a21c331 with a `quadrature()` → `quadratureDelta()`
  forwarder; on master only `perf_e2e.cpp` itself is compiled with `-std=c++23` (no `-DNDEBUG`).
- **Comparability**: `perf.py calibrate` before the measurements: baseline binaries at 1.007 /
  1.003 (16-thread e2e, burst / sustained) and 1.004 / 0.999 (1-thread micro) of
  `baseline/calibration.json`, i.e. the machine state equals the baseline recording within 1 %.
  With 2 processes per state the numbers carry process-to-process noise of a few % (bimodal
  cases up to ~9 %); differences below ~5 % between master / e6a21c331 / baseline are not
  meaningful. Millisecond-scale `generation` times depend on the measurement pattern (the
  baseline's 5 in-process samples run warm, the other columns use fresh processes): compare
  `total` / `operator`.
- **What lies between the columns**: master → e6a21c331: the owner's work after the thesis
  (DSG quadrature, refactorings, site_scons `-DNDEBUG`/C++23); e6a21c331 → baseline: DSG trie
  factorization, CC FFT backend fix, benchmark work; baseline → WP-13: WP-01 (SG quadrature
  per-grid-point work) and WP-13 (`-fno-semantic-interposition`); WP-11 only measured.
- Raw data: `run-master{,-2}.json`, `run-dev{,-2}.json`, `run-dev-dsg.jsonl`, `calibrate.txt` in this
  directory; snapshots `report-master` / `report-dev` in `combigrid/perf/.work/snapshots/`.
