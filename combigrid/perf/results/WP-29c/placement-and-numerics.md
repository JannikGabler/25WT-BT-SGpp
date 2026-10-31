# WP-29c: code placement (scalar path) and vector-vs-scalar round-off

## `perf stat` of `micro/interp/global_tg/cc/b0/d10/L2` (1 thread, core 4, 10 samples, 2 runs each)

| snapshot | cycles:u | instructions:u | op_cache_miss:u | branch-misses:u |
|---|---|---|---|---|
| WP-29c-before | 2.204 G / 2.224 G | 5.5262 G | 2.99 M / 2.57 M | 18.4 M / 19.0 M |
| WP-29c-after (V3) | 2.235 G / 2.240 G | 5.5272 G | 7.61 M / 8.81 M | 18.9 M / 19.7 M |
| WP-29c-v4 (V3 + aligned(64)) | 2.240 G / 2.314 G | 5.5272 G | 18.45 M / 17.77 M | 19.0 M / 18.9 M |

Instructions +0.02 % (process setup: more registered micro cases); the scalar collapse kernels are
instruction-identical to before (normalized `objdump` diff: padding, one `endbr`, operand order of
two `cmp`). The +0.7–1.4 % cycles come from op-cache misses, i.e. code placement. aligned(64) on
both collapse templates made it worse (V4, dropped).

## Vector vs. scalar results (`OptBarycentricFormula`, n = 3 CC nodes, 100000 random cases)

- Multi-output code at -O0 == scalar formula bitwise (0 mismatches): the arithmetic is the same.
- Optimized build: 13 % of the m = 1 calls differ by 1 ulp. Cause: GCC contracts inconsistently
  (C++ default `-ffp-contract=fast`): the numerator loop of the multi-output formula is unrolled
  by 2 and only its odd remainder uses `vfmadd231sd`; the scalar `barycentricFormula` has no FMA,
  but its inlined `hasTrivialResult` (n = 2) has one.
- Effect on the production kernels (cosProdK, m = 32, all tensor grids): max |vector − scalar|
  = 1.8e-17 (d10 L2; 10.9 eps × max |value| 7.5e-3) and 8.3e-17 (d5 L6; 4.9 eps × 7.6e-2). The
  large `max_ulp` counters of vector-gain.md (up to 23396 at d10 m = 32) are values near 0.
