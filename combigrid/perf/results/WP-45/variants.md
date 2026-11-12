# WP-45 — design decision and variants of the DSG global interpolation

## Design decision (owner, 2026-11-12, in session)

Options presented before implementing:
- (A) per-TG gather over a per-call value store (no interface change; per-TG results bitwise equal
  to the SparseGrid kernel; keeps per-TG sums). Estimate from the WP-36 prototype: cc 1.7–3.9x.
- (B) global weights through the DSG quadrature's row loop, with the 1D quadrature weights replaced
  by 1D basis values at x. Needs the basis values of a 1D rule, which `InterpolationMethod` did
  not expose (black-box probing costs O(n²) per (dimension, level) and call: cc d2 L12,
  n = 8193, ≈ 180M flops per call) → new public virtual `InterpolationMethod::basisValues`.

Owner chose (B). Final (B) vs. WP-36's gather prototype "dedup" (`explore-t16.md`, 16 t, same
run): d2 0.051 vs. 0.104 s, d3 0.042 vs. 0.093 s, d5 0.172 vs. 0.395 s, d7 0.149 vs. 0.337 s,
d10 0.392 vs. 0.725 s, equi d3 0.0066 vs. 0.047 s (2.0–7.2x).

## Variants (scratch driver `gate_driver.cpp`, 16 threads, median per call, cosProd)

| step | change | effect |
|---|---|---|
| v1 | first version: serial basis values, parallel `omp for dynamic` over the delta TGs, reduction | explore 16 t: d2 2.69x, d3 4.23x, d5 7.27x, d7 7.23x, d10 4.01x, equi d3 3.05x vs. SG op. |
| v2 | delta-TG sums stored and added in grid order (deterministic) | 16 t vs. 1 t differed by up to 0.37 on equi d2 L5 / d3 L5 (Lebesgue regime) in v1; now 0 everywhere (`numerics-sg-vs-dsg.md`); runtime neutral |
| v2 | gate 512 → 128 grid points | crossover serial / parallel: 49 pts 9.6 / 10.4 µs, 65 pts 7.3 / 10.0 µs, 113 pts 14.7 / 11.0 µs, 257 pts (d1) 12.8 / 12.5 µs, 225 pts (d3) 27 / 13 µs |
| v3 | basis values inside the parallel region (rows largest first, then the delta TGs after the barrier) | basis values were 16 % (cc d2 L12: 159 of 1008 µs) and 55 % (equi d3 L5: 158 of 286 µs) of a call; per call cc d2 L12 1008 → 940 µs, equi d3 L5 286 → 118 µs; small grids +2–3 µs (one more barrier) |
| v4 | delta TGs scheduled largest first (precomputed order) | cc d5 L3 188 → 138 µs, d3 L9 888 → 670 µs, d2 L12 944 → 817 µs, d5 L6 3112 → 2697 µs, equi d3 L5 120 → 106 µs; small grids unchanged (two interleaved rounds) |
| v5 | `OptBarycentricFormula::basisValues` vectorizable (divisions in their own loop, multiply by the inverse sum) | basis values cc d2 L12 164 → 143 µs serial; per call d2 L12 816 → 800 µs (≈ 2 %, minimal), others neutral |

v1–v5 are all kept (v5 is minimal but costs no code size). Gate rows (final, `gate_driver.cpp`):

| case | grid points | basis values (serial) | serial | parallel | api |
|---|---|---|---|---|---|
| cc d1 L3 / L5 / L7 | 17 / 65 / 257 | 2.4 / 3.3 / 7.1 µs | 6.6 / 6.6 / 15.8 µs | 9.6 / 10.3 / 15.8 µs | 4.6 / 6.8 / 16.2 µs |
| cc d2 L2 / L3 / L4 / L5 | 49 / 113 / 257 / 577 | 2.4–4.7 µs | 10.2 / 15.7 / 26.2 / 36.2 µs | 12.9 / 13.5 / 15.0 / 18.0 µs | 10.5 / 15.8 / 15.3 / 18.3 µs |
| cc d3 L2 / L3 / L4 | 225 / 593 / 1505 | 2.9–3.8 µs | 27.7 / 47.1 / 114 µs | 13.9 / 16.3 / 22.9 µs | 14.3 / 16.7 / 23.3 µs |
| cc d2 L12 / d3 L9 / d5 L6 | 131k / 115k / 283k | 163 / 31 / 11 µs | 5.33 / 7.06 / 28.9 ms | 825 / 674 / 2700 µs | 820 / 670 / 2698 µs |
| equi d3 L5 | 3713 | 158 µs | 413 µs | 103 µs | 108 µs |

(d2 L3 with 113 points stays serial at the gate 128: 15.8 vs. 13.5 µs; d1 L7 with 257 points is
even. The gate is a compromise over d; per-point cost depends on d and the integrand.)

## Not attempted / left for follow-ups
- Splitting large delta TGs across threads: cc d2 L12 is bounded by its largest delta TG
  ((12, 0): 12,288 of 131,073 points ≈ 0.5 ms of 0.82 ms per call) — needs a row-range split in
  `dsg_quadrature::forEachGPWithGlobalWeight` (shared with the DSG quadrature).
- Generic barycentric weights (equi): the largest row's O(n²) log-based weights (65 nodes ≈ 85 µs)
  are the critical path of equi d3 L5 (108 µs per call) → WP-07 (cache the weights).
