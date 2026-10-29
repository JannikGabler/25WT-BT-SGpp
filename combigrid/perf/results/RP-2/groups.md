# RP-2 — cumulative progress per operator group (A = baseline fed2cdb4e, B = rp-2 5215417ea; rp-1 column = B side of RP-1's e2e.json)

From `e2e.json` (full suite, 16 threads, 5 interleaved rounds; medians of `total`; memory = geomean of the per-case median peak-RSS ratios baseline → rp-2). rp-1 numbers come from another session (RP-1, calibrate 1.004/1.002; RP-2 1.003/0.999); direct rp-1 → rp-2 A/Bs: `e2e-rp-1-vs-rp-2-*.md`.

| group | cases | baseline | rp-1 (RP-1 report) | rp-2 | baseline → rp-2 | rp-1 → rp-2 | d ≥ 7: baseline → rp-2 | peak RSS geomean B/A |
|---|---|---|---|---|---|---|---|---|
| quad/sg | 13 | 22.29 s | 9.19 s | 9.18 s | 2.43x | 1.001x | 18.53 → 6.93 s (2.67x) | 1.064 |
| quad/dsg | 9 | 26.73 s | 27.10 s | 17.74 s | 1.51x | 1.528x | 26.13 → 17.20 s (1.52x) | 0.979 |
| interp | 8 | 6.64 s | 5.70 s | 5.73 s | 1.16x | 0.993x | 3.72 → 3.27 s (1.14x) | 0.999 |
| interplin | 4 | 5.09 s | 4.52 s | 4.65 s | 1.09x | 0.971x | 4.41 → 4.10 s (1.08x) | 1.009 |
| mivec | 4 | 9.28 s | 6.91 s | 6.35 s | 1.46x | 1.088x | 7.08 → 5.61 s (1.26x) | 1.004 |
| all | 38 | 70.03 s | 53.41 s | 43.65 s | 1.604x | 1.224x | 59.87 → 37.10 s (1.61x) | |
mivec generation: baseline 5.107 s, rp-1 5.653 s, rp-2 5.100 s
quad/dsg rp-2: b0 15.30 s, b1 2.44 s

| case | total | generation | operator | peak RSS (baseline → rp-2) | rp-1 total |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | 6.772 s | 4.3 ms | 6.762 s | 37.3 → 42.6 MiB | 7.020 s |
| quad/dsg/equi-exp/b0/d10/L7 | 5.406 s | 3.6 ms | 5.397 s | 36.6 → 41.7 MiB | 5.657 s |
| interplin/sg/equi/b1/d10/L5/p32 | 2.515 s | 1.7 ms | 2.512 s | 13.2 → 13.5 MiB | 2.468 s |
| quad/sg/equi-sin/b0/d7/L7 | 2.419 s | 1.9 ms | 2.416 s | 14.7 → 14.6 MiB | 2.410 s |
| interp/sg/cc/b0/d10/L2/p8 | 1.988 s | 0.0 ms | 1.988 s | 24.9 → 24.7 MiB | 1.994 s |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 1.896 s | 1406.0 ms | 0.477 s | 17.4 → 17.6 MiB | 1.988 s |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 1.886 s | 1862.3 ms | 0.025 s | 12.7 → 12.7 MiB | 2.102 s |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 1.829 s | 1827.0 ms | 0.002 s | 12.1 → 12.3 MiB | 2.068 s |
| interplin/sg/equi/b1/d8/L5/p256 | 1.584 s | 2.2 ms | 1.582 s | 12.8 → 13.0 MiB | 1.497 s |
| interp/sg/cc/b0/d5/L6/p64 | 1.475 s | 0.1 ms | 1.475 s | 12.1 → 12.1 MiB | 1.448 s |
| quad/dsg/cc-uq/b0/d7/L10 | 1.440 s | 4.4 ms | 1.430 s | 37.2 → 43.0 MiB | 1.512 s |
| quad/sg/cc-uq/b0/d10/L4 | 1.309 s | 3.5 ms | 1.307 s | 13.1 → 12.9 MiB | 1.311 s |
| interp/sg/cc/b0/d7/L4/p16 | 1.279 s | 0.1 ms | 1.279 s | 13.0 → 12.9 MiB | 1.259 s |
| quad/dsg/equi-exp/b1/d20/L6 | 1.225 s | 63.1 ms | 1.122 s | 330.6 → 217.1 MiB | 7.035 s |
| quad/dsg/equi-exp/b1/d15/L7 | 1.218 s | 43.2 ms | 1.145 s | 230.7 → 150.9 MiB | 4.091 s |
| quad/sg/cc-uq/b0/d2/L20 | 1.191 s | 76.2 ms | 1.110 s | 833.3 → 1072.2 MiB | 1.189 s |
| quad/dsg/equi-exp/b0/d7/L10 | 1.135 s | 3.0 ms | 1.128 s | 37.6 → 42.5 MiB | 1.216 s |
| quad/sg/equi-exp/b0/d10/L4 | 0.935 s | 2.1 ms | 0.932 s | 12.9 → 12.7 MiB | 0.937 s |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 0.740 s | 4.6 ms | 0.735 s | 16.1 → 16.1 MiB | 0.757 s |
| quad/sg/cc-uq/b0/d7/L7 | 0.702 s | 2.0 ms | 0.699 s | 14.7 → 15.1 MiB | 0.704 s |
| quad/sg/equi-exp/b1/d15/L6 | 0.522 s | 15.4 ms | 0.498 s | 54.5 → 52.8 MiB | 0.526 s |
| quad/sg/equi-exp/b0/d7/L7 | 0.502 s | 1.8 ms | 0.499 s | 14.3 → 14.6 MiB | 0.502 s |
| interplin/sg/equi/b1/d5/L8/p1024 | 0.444 s | 2.3 ms | 0.441 s | 13.0 → 13.2 MiB | 0.442 s |
| quad/sg/equi-exp/b0/d2/L20 | 0.395 s | 31.0 ms | 0.327 s | 363.7 → 619.7 MiB | 0.403 s |
| interp/sg/cheb1/b0/d5/L4/p64 | 0.336 s | 0.0 ms | 0.336 s | 12.0 → 12.0 MiB | 0.338 s |
| quad/sg/cc-uq/b0/d5/L10 | 0.325 s | 2.1 ms | 0.323 s | 17.2 → 17.3 MiB | 0.326 s |
| quad/sg/equi-exp/b1/d10/L7 | 0.304 s | 5.2 ms | 0.295 s | 25.6 → 26.0 MiB | 0.309 s |
| quad/sg/equi-exp/b0/d5/L10 | 0.253 s | 1.9 ms | 0.250 s | 15.4 → 15.5 MiB | 0.253 s |
| quad/dsg/cc-uq/b0/d5/L12 | 0.248 s | 2.3 ms | 0.244 s | 24.3 → 25.5 MiB | 0.258 s |
| interp/sg/cc/b0/d3/L9/p64 | 0.245 s | 0.1 ms | 0.245 s | 12.1 → 12.1 MiB | 0.246 s |
| quad/sg/equi-exp/b1/d20/L5 | 0.235 s | 17.5 ms | 0.210 s | 60.0 → 56.9 MiB | 0.236 s |
| quad/dsg/equi-exp/b0/d5/L12 | 0.202 s | 1.3 ms | 0.199 s | 24.3 → 25.9 MiB | 0.214 s |
| interp/sg/cheb1/b0/d3/L6/p64 | 0.182 s | 0.0 ms | 0.182 s | 11.9 → 12.0 MiB | 0.181 s |
| interp/sg/cc/b0/d2/L12/p64 | 0.176 s | 0.2 ms | 0.176 s | 14.8 → 14.7 MiB | 0.176 s |
| interplin/sg/equi/b1/d2/L16/p16384 | 0.108 s | 1.6 ms | 0.102 s | 19.6 → 19.5 MiB | 0.110 s |
| quad/dsg/equi-exp/b0/d3/L16 | 0.093 s | 1.0 ms | 0.092 s | 62.1 → 62.6 MiB | 0.096 s |
| quad/sg/cc-uq/b0/d3/L14 | 0.088 s | 2.0 ms | 0.085 s | 38.8 → 41.3 MiB | 0.087 s |
| interp/sg/equi/b0/d3/L5/p64 | 0.051 s | 0.0 ms | 0.051 s | 11.7 → 11.9 MiB | 0.051 s |
