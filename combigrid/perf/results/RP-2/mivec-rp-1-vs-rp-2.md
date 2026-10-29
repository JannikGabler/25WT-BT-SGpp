# RP-2 — mivec generation faster in rp-2 than in rp-1 without a source change

Direct A/B `ab e2e rp-1 rp-2 --filter '^mivec/|^interplin/'` (`e2e-rp-1-vs-rp-2-mivec-interplin.md`,
5 rounds, foreign load ≤ 0.5 %), `generation`:
- d10 1.512 → 1.437 s (1.063 [1.033, 1.095]);
- d15 2.074 → 1.841 s (1.131);
- d20 2.090 → 1.848 s (1.130).

`total` d10/d15/d20 1.046 / 1.130 / 1.131 faster; d5 noise. interplin: all four noise (0.98–1.00);
the cross-session rp-1 → rp-2 group ratio 0.971 is drift.

WP-20 and WP-02 touched no code of `genMIVecDownwardsClosure`, but LTO compiled it differently.

`nm -S` of `genMIVecDownwardsClosure<unsigned int> [clone ._omp_fn.0]`: rp-1 0x2044 B (8.3 KB),
rp-2 0x2aff B (11.0 KB).

`perf stat`, `mivec/sg/cc-exp/b1/d20/S7/n300`, 16 threads, 1 warm-up + 2 samples:

| | rp-1 | rp-2 | rp-2 / rp-1 |
|---|---|---|---|
| instructions | 3.527e11 | 3.451e11 | 0.979 |
| cycles | 2.006e11 | 1.776e11 | 0.885 |
| op-cache accesses | 7.06e10 | 6.07e10 | 0.859 |
| op-cache misses | 1.66e8 (2.3 ‰) | 1.49e8 (2.5 ‰) | 0.900 |
| instruction-cache misses | 2.88e7 | 2.65e7 | 0.919 |
| stalled-cycles-frontend | 1.02e10 (5.1 % of cycles) | 7.25e9 (4.1 %) | 0.714 |

The gain is not placement: op-cache miss rates are low and equal. It is a different code shape:
2 % fewer instructions at a higher IPC (1.76 → 1.94).

The hot loop still has the RP-1 structure: the 32-byte alignment test `test $0x1f` is the hottest
instruction (7.3 %), plus the AVX2 `vptest` early-break loop and the per-test `imul` index
multiply.

Conclusion for WP-03: the vectorized dominance test is fragile. Its speed moves by 13 % with
inlining decisions elsewhere in the library. A scalar early-exit test, or the algorithmic change,
makes the closure's cost independent of such decisions.
