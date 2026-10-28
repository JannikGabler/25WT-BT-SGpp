# RP-1 — cumulative progress per operator group (A = baseline fed2cdb4e, B = rp-1 f48d2bbc1)

From `e2e.json` (full suite, 16 threads, 5 interleaved rounds; medians of `total`; memory = geomean of the per-case median peak-RSS ratios). Verdicts (total): quad/sg 13 faster; quad/dsg 3 faster, 3 ~, 3 SLOWER (b1 d15 0.968, b1 d20 0.962, cc-uq d10 0.993); interp 8 faster; interplin 3 faster, 1 ~; mivec 2 faster, 2 SLOWER (d15 0.902, d20 0.924).

| group | cases | A total | B total | speedup | A total d>=7 | B total d>=7 | speedup d>=7 | peak RSS geomean B/A |
|---|---|---|---|---|---|---|---|---|
| quad/sg | 13 | 22.35 s | 9.19 s | 2.432x | 18.51 s | 6.93 s | 2.670x | 1.080 |
| quad/dsg | 9 | 26.72 s | 27.10 s | 0.986x | 26.12 s | 26.53 s | 0.985x | 0.999 |
| interp | 8 | 6.61 s | 5.70 s | 1.161x | 3.72 s | 3.25 s | 1.142x | 1.006 |
| interplin | 4 | 5.01 s | 4.52 s | 1.108x | 4.39 s | 3.96 s | 1.107x | 0.993 |
| mivec | 4 | 9.27 s | 6.91 s | 1.340x | 7.05 s | 6.16 s | 1.144x | 1.011 |
| all | 38 | 69.95 s | 53.41 s | 1.310x | 59.78 s | 46.84 s | 1.276x | |

mivec phases: generation 5.07 → 5.65 s (0.897x), operator 4.19 → 1.25 s (3.34x). Peak memory over all 38 cases: geomean 1.028, 3 MEM+ (all quad/sg: equi-exp/b0/d2/L20 363.7 → 619.9 MiB, cc-uq/b0/d2/L20 786.2 → 1079.1 MiB, cc-uq/b0/d3/L14 37.8 → 40.5 MiB), 0 MEM-; floors 10.2 / 10.3 MiB.

## rp-1 cases by total time

| case | total | generation | operator | peak RSS (A → B) |
|---|---|---|---|---|
| quad/dsg/equi-exp/b1/d20/L6 | 7.035 s | 50.8 ms | 6.943 s | 331.3 → 328.8 MiB |
| quad/dsg/cc-uq/b0/d10/L7 | 7.020 s | 3.6 ms | 7.011 s | 37.2 → 37.0 MiB |
| quad/dsg/equi-exp/b0/d10/L7 | 5.657 s | 4.2 ms | 5.649 s | 37.4 → 37.2 MiB |
| quad/dsg/equi-exp/b1/d15/L7 | 4.091 s | 35.5 ms | 4.032 s | 232.4 → 233.4 MiB |
| interplin/sg/equi/b1/d10/L5/p32 | 2.468 s | 1.8 ms | 2.465 s | 13.3 → 13.2 MiB |
| quad/sg/equi-sin/b0/d7/L7 | 2.410 s | 1.8 ms | 2.407 s | 14.5 → 14.8 MiB |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 2.102 s | 2077.4 ms | 0.024 s | 12.7 → 12.8 MiB |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 2.068 s | 2065.3 ms | 0.002 s | 12.3 → 12.3 MiB |
| interp/sg/cc/b0/d10/L2/p8 | 1.994 s | 0.0 ms | 1.994 s | 24.2 → 24.5 MiB |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 1.988 s | 1505.3 ms | 0.476 s | 17.3 → 17.7 MiB |
| quad/dsg/cc-uq/b0/d7/L10 | 1.512 s | 4.4 ms | 1.502 s | 37.8 → 37.5 MiB |
| interplin/sg/equi/b1/d8/L5/p256 | 1.497 s | 2.0 ms | 1.495 s | 12.8 → 12.9 MiB |
| interp/sg/cc/b0/d5/L6/p64 | 1.448 s | 0.1 ms | 1.448 s | 12.0 → 12.1 MiB |
| quad/sg/cc-uq/b0/d10/L4 | 1.311 s | 3.8 ms | 1.308 s | 12.7 → 13.0 MiB |
| interp/sg/cc/b0/d7/L4/p16 | 1.259 s | 0.1 ms | 1.259 s | 12.9 → 13.0 MiB |
| quad/dsg/equi-exp/b0/d7/L10 | 1.216 s | 3.0 ms | 1.209 s | 37.3 → 37.5 MiB |
| quad/sg/cc-uq/b0/d2/L20 | 1.189 s | 82.4 ms | 1.101 s | 786.2 → 1079.1 MiB |
| quad/sg/equi-exp/b0/d10/L4 | 0.937 s | 4.2 ms | 0.933 s | 12.8 → 12.8 MiB |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 0.757 s | 5.1 ms | 0.751 s | 16.0 → 16.1 MiB |
| quad/sg/cc-uq/b0/d7/L7 | 0.704 s | 1.9 ms | 0.701 s | 14.9 → 14.8 MiB |
| quad/sg/equi-exp/b1/d15/L6 | 0.526 s | 17.5 ms | 0.498 s | 53.3 → 54.8 MiB |
| quad/sg/equi-exp/b0/d7/L7 | 0.502 s | 1.8 ms | 0.499 s | 14.6 → 14.6 MiB |
| interplin/sg/equi/b1/d5/L8/p1024 | 0.442 s | 2.3 ms | 0.439 s | 13.3 → 12.9 MiB |
| quad/sg/equi-exp/b0/d2/L20 | 0.403 s | 31.4 ms | 0.325 s | 363.7 → 619.9 MiB |
| interp/sg/cheb1/b0/d5/L4/p64 | 0.338 s | 0.0 ms | 0.338 s | 12.1 → 12.1 MiB |
| quad/sg/cc-uq/b0/d5/L10 | 0.326 s | 2.1 ms | 0.324 s | 17.2 → 17.4 MiB |
| quad/sg/equi-exp/b1/d10/L7 | 0.309 s | 4.6 ms | 0.300 s | 25.9 → 26.1 MiB |
| quad/dsg/cc-uq/b0/d5/L12 | 0.258 s | 2.4 ms | 0.255 s | 23.8 → 24.0 MiB |
| quad/sg/equi-exp/b0/d5/L10 | 0.253 s | 2.1 ms | 0.250 s | 15.3 → 15.9 MiB |
| interp/sg/cc/b0/d3/L9/p64 | 0.246 s | 0.1 ms | 0.246 s | 12.0 → 12.0 MiB |
| quad/sg/equi-exp/b1/d20/L5 | 0.236 s | 18.3 ms | 0.209 s | 59.8 → 57.6 MiB |
| quad/dsg/equi-exp/b0/d5/L12 | 0.214 s | 1.3 ms | 0.211 s | 24.2 → 24.3 MiB |
| interp/sg/cheb1/b0/d3/L6/p64 | 0.181 s | 0.0 ms | 0.181 s | 11.8 → 12.0 MiB |
| interp/sg/cc/b0/d2/L12/p64 | 0.176 s | 0.2 ms | 0.176 s | 14.9 → 14.9 MiB |
| interplin/sg/equi/b1/d2/L16/p16384 | 0.110 s | 1.7 ms | 0.104 s | 19.6 → 19.7 MiB |
| quad/dsg/equi-exp/b0/d3/L16 | 0.096 s | 1.0 ms | 0.094 s | 62.8 → 62.7 MiB |
| quad/sg/cc-uq/b0/d3/L14 | 0.087 s | 2.1 ms | 0.085 s | 37.8 → 40.5 MiB |
| interp/sg/equi/b0/d3/L5/p64 | 0.051 s | 0.0 ms | 0.051 s | 11.8 → 11.9 MiB |
