# RP-3 — cumulative progress per operator group (A = baseline fed2cdb4e, B = rp-3 0c6ecfc4b; rp-2 column = B side of RP-2's e2e.json)

From `e2e.json` (full suite, 16 threads, 5 interleaved rounds; medians of `total`; memory = geomean of the per-case median peak-RSS ratios). rp-2 numbers come from another session (RP-2, calibrate 1.003/0.999; RP-3 1.002/1.001); direct rp-2 → rp-3 A/B of interp + interplin: `e2e-rp-2-vs-rp-3-interp.md` (all noise except interp cheb1 d5 0.980x).

| group | cases | baseline | rp-2 (RP-2 report) | rp-3 | baseline → rp-3 | rp-2 → rp-3 | d ≥ 7: baseline → rp-3 | peak RSS geomean B/A | rp-2 → rp-3 RSS |
|---|---|---|---|---|---|---|---|---|---|
| quad/sg | 13 | 22.28 s | 9.18 s | 9.20 s | 2.421x | 0.997x | 18.50 → 6.93 s (2.67x) | 1.064 | 0.994 |
| quad/dsg | 9 | 26.70 s | 17.74 s | 17.75 s | 1.505x | 1.000x | 26.10 → 17.20 s (1.52x) | 0.973 | 0.996 |
| interp | 8 | 6.60 s | 5.73 s | 5.70 s | 1.157x | 1.006x | 3.70 → 3.25 s (1.14x) | 1.003 | 1.000 |
| interplin | 4 | 5.02 s | 4.65 s | 4.55 s | 1.103x | 1.023x | 4.40 → 4.00 s (1.10x) | 0.995 | 0.986 |
| mivec | 4 | 9.32 s | 6.35 s | 1.26 s | 7.418x | 5.054x | 7.05 → 0.51 s (13.71x) | 0.995 | 0.996 |
| all | 38 | 69.92 s | 43.65 s | 38.45 s | 1.818x | 1.135x | 59.76 → 31.90 s (1.87x) | 1.014 | 0.995 |

mivec rp-3: generation 13.8 ms, operator 1.241 s
quad/dsg rp-3: b0 15.30 s, b1 2.45 s

| case | rp-3 total | generation | operator | baseline → rp-3 | rp-2 total (RP-2) | rp-2 → rp-3 | peak RSS baseline → rp-3 | rp-2 RSS |
|---|---|---|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | 6.785 s | 4.5 ms | 6.775 s | 1.023x | 6.772 s | 0.998x | 37.2 → 42.3 MiB | 42.6 MiB |
| quad/dsg/equi-exp/b0/d10/L7 | 5.396 s | 3.6 ms | 5.387 s | 1.048x | 5.406 s | 1.002x | 36.7 → 41.6 MiB | 41.7 MiB |
| interplin/sg/equi/b1/d10/L5/p32 | 2.468 s | 1.8 ms | 2.465 s | 1.091x | 2.515 s | 1.019x | 13.4 → 13.1 MiB | 13.5 MiB |
| quad/sg/equi-sin/b0/d7/L7 | 2.418 s | 1.9 ms | 2.415 s | 1.458x | 2.419 s | 1.001x | 14.3 → 14.5 MiB | 14.6 MiB |
| interp/sg/cc/b0/d10/L2/p8 | 1.990 s | 0.0 ms | 1.990 s | 1.124x | 1.988 s | 0.999x | 24.5 → 24.7 MiB | 24.7 MiB |
| interplin/sg/equi/b1/d8/L5/p256 | 1.527 s | 2.1 ms | 1.525 s | 1.122x | 1.584 s | 1.037x | 12.7 → 12.8 MiB | 13.0 MiB |
| interp/sg/cc/b0/d5/L6/p64 | 1.449 s | 0.1 ms | 1.449 s | 1.204x | 1.475 s | 1.018x | 12.1 → 12.0 MiB | 12.1 MiB |
| quad/dsg/cc-uq/b0/d7/L10 | 1.441 s | 4.1 ms | 1.431 s | 1.054x | 1.440 s | 0.999x | 37.3 → 42.3 MiB | 43.0 MiB |
| quad/sg/cc-uq/b0/d10/L4 | 1.312 s | 2.5 ms | 1.309 s | 2.756x | 1.309 s | 0.998x | 12.9 → 13.0 MiB | 12.9 MiB |
| interp/sg/cc/b0/d7/L4/p16 | 1.261 s | 0.1 ms | 1.261 s | 1.170x | 1.279 s | 1.015x | 12.8 → 13.0 MiB | 12.9 MiB |
| quad/dsg/equi-exp/b1/d20/L6 | 1.227 s | 61.7 ms | 1.124 s | 5.515x | 1.225 s | 0.999x | 330.9 → 217.5 MiB | 217.1 MiB |
| quad/dsg/equi-exp/b1/d15/L7 | 1.220 s | 43.6 ms | 1.146 s | 3.255x | 1.218 s | 0.998x | 234.0 → 150.2 MiB | 150.9 MiB |
| quad/sg/cc-uq/b0/d2/L20 | 1.201 s | 79.8 ms | 1.117 s | 1.164x | 1.191 s | 0.992x | 804.7 → 1063.6 MiB | 1072.2 MiB |
| quad/dsg/equi-exp/b0/d7/L10 | 1.135 s | 3.0 ms | 1.128 s | 1.085x | 1.135 s | 1.000x | 37.2 → 42.0 MiB | 42.5 MiB |
| quad/sg/equi-exp/b0/d10/L4 | 0.935 s | 2.4 ms | 0.933 s | 3.543x | 0.935 s | 1.000x | 13.0 → 12.9 MiB | 12.7 MiB |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 0.743 s | 3.0 ms | 0.739 s | 3.052x | 0.740 s | 0.997x | 16.0 → 16.2 MiB | 16.1 MiB |
| quad/sg/cc-uq/b0/d7/L7 | 0.702 s | 2.0 ms | 0.699 s | 2.590x | 0.702 s | 1.000x | 14.9 → 14.7 MiB | 15.1 MiB |
| quad/sg/equi-exp/b1/d15/L6 | 0.523 s | 15.6 ms | 0.498 s | 4.536x | 0.522 s | 0.998x | 54.0 → 53.1 MiB | 52.8 MiB |
| quad/sg/equi-exp/b0/d7/L7 | 0.501 s | 1.8 ms | 0.499 s | 3.253x | 0.502 s | 1.001x | 14.5 → 14.6 MiB | 14.6 MiB |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 0.484 s | 7.5 ms | 0.475 s | 6.744x | 1.896 s | 3.921x | 17.7 → 17.1 MiB | 17.6 MiB |
| interplin/sg/equi/b1/d5/L8/p1024 | 0.444 s | 2.2 ms | 0.439 s | 1.146x | 0.444 s | 1.001x | 13.0 → 13.0 MiB | 13.2 MiB |
| quad/sg/equi-exp/b0/d2/L20 | 0.405 s | 30.8 ms | 0.330 s | 1.773x | 0.395 s | 0.977x | 364.1 → 619.7 MiB | 619.7 MiB |
| interp/sg/cheb1/b0/d5/L4/p64 | 0.343 s | 0.0 ms | 0.343 s | 1.069x | 0.336 s | 0.982x | 12.0 → 12.0 MiB | 12.0 MiB |
| quad/sg/cc-uq/b0/d5/L10 | 0.326 s | 2.4 ms | 0.323 s | 2.393x | 0.325 s | 0.999x | 17.2 → 17.3 MiB | 17.3 MiB |
| quad/sg/equi-exp/b1/d10/L7 | 0.311 s | 5.3 ms | 0.302 s | 3.659x | 0.304 s | 0.978x | 25.6 → 25.7 MiB | 26.0 MiB |
| quad/sg/equi-exp/b0/d5/L10 | 0.253 s | 2.0 ms | 0.250 s | 2.788x | 0.253 s | 1.000x | 15.2 → 15.7 MiB | 15.5 MiB |
| quad/dsg/cc-uq/b0/d5/L12 | 0.248 s | 2.4 ms | 0.244 s | 1.088x | 0.248 s | 1.000x | 24.4 → 25.5 MiB | 25.5 MiB |
| interp/sg/cc/b0/d3/L9/p64 | 0.246 s | 0.1 ms | 0.246 s | 1.258x | 0.245 s | 0.997x | 12.0 → 12.0 MiB | 12.1 MiB |
| quad/sg/equi-exp/b1/d20/L5 | 0.232 s | 14.3 ms | 0.209 s | 4.939x | 0.235 s | 1.013x | 60.2 → 53.9 MiB | 56.9 MiB |
| quad/dsg/equi-exp/b0/d5/L12 | 0.202 s | 1.3 ms | 0.199 s | 1.107x | 0.202 s | 1.001x | 24.2 → 26.0 MiB | 25.9 MiB |
| interp/sg/cheb1/b0/d3/L6/p64 | 0.182 s | 0.0 ms | 0.182 s | 1.032x | 0.182 s | 1.000x | 11.9 → 11.8 MiB | 12.0 MiB |
| interp/sg/cc/b0/d2/L12/p64 | 0.176 s | 0.2 ms | 0.176 s | 1.349x | 0.176 s | 1.002x | 14.8 → 14.9 MiB | 14.7 MiB |
| interplin/sg/equi/b1/d2/L16/p16384 | 0.108 s | 1.7 ms | 0.102 s | 1.042x | 0.108 s | 0.999x | 19.6 → 19.4 MiB | 19.5 MiB |
| quad/dsg/equi-exp/b0/d3/L16 | 0.093 s | 1.0 ms | 0.091 s | 1.143x | 0.093 s | 1.002x | 62.4 → 62.3 MiB | 62.6 MiB |
| quad/sg/cc-uq/b0/d3/L14 | 0.087 s | 2.0 ms | 0.084 s | 2.060x | 0.088 s | 1.009x | 37.6 → 41.0 MiB | 41.3 MiB |
| interp/sg/equi/b0/d3/L5/p64 | 0.052 s | 0.0 ms | 0.052 s | 1.018x | 0.051 s | 0.989x | 11.9 → 12.0 MiB | 11.9 MiB |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 0.028 s | 2.4 ms | 0.025 s | 66.674x | 1.886 s | 68.502x | 12.8 → 12.8 MiB | 12.7 MiB |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 0.003 s | 0.8 ms | 0.002 s | 595.221x | 1.829 s | 593.288x | 12.3 → 12.2 MiB | 12.3 MiB |
