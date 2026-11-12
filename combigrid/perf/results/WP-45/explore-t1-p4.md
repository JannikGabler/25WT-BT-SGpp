threads: 1, repetitions: 3 (median, after 1 warm-up)

| case | TGs | sum nGP | max nGP / (sum/threads) | evals per call | evals p calls | evals multi-point | p calls / multi | regenerated nodes / integrand coordinates per call |
|---|---|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 25 | 360465 | 0.07 | 360465 | 1441860 | 360465 | 4.00 | 49194 / 720930 = 0.0682 |
| interp/sg/cc/b0/d3/L9/p64 | 136 | 557030 | 0.02 | 557030 | 2228120 | 557030 | 4.00 | 21714 / 1671090 = 0.0130 |
| interp/sg/cc/b0/d5/L6/p64 | 456 | 2202350 | 0.00 | 2202350 | 8809400 | 2202350 | 4.00 | 17070 / 11011750 = 0.0016 |
| interp/sg/cc/b0/d7/L4/p16 | 330 | 5055102 | 0.00 | 5055102 | 20220408 | 5055102 | 4.00 | 10178 / 35385714 = 0.0003 |
| interp/sg/cc/b0/d10/L2/p8 | 66 | 10195794 | 0.02 | 10195794 | 40783176 | 10195794 | 4.00 | 2240 / 101957940 = 0.0000 |

| case | p | api [s] | kernel [s] (api/x) | kernel cached weights | sol_api [s] (share of api) | multi | multi cached weights | sol_multi [s] | api + cache [hit rate] | max diff kernel / kernel w / multi / cache |
|---|---|---|---|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 4 | 0.0660 | 0.0566 (1.17) | - | 0.0501 (0.76) | 0.0192 (3.44) | - | 0.0125 | 0.1414 (0.47) [0.909] | 0.0e+00 / - / 0.0e+00 / 0.0e+00 |
| interp/sg/cc/b0/d3/L9/p64 | 4 | 0.1398 | 0.1271 (1.10) | - | 0.1148 (0.82) | 0.0403 (3.47) | - | 0.0286 | 0.2375 (0.59) [0.949] | 0.0e+00 / - / 0.0e+00 / 0.0e+00 |
| interp/sg/cc/b0/d5/L6/p64 | 4 | 0.9244 | 0.8284 (1.12) | - | 0.7616 (0.82) | 0.2521 (3.67) | - | 0.1905 | 1.1376 (0.81) [0.968] | 0.0e+00 / - / 0.0e+00 / 0.0e+00 |
| interp/sg/cc/b0/d7/L4/p16 | 4 | 3.4313 | 2.9790 (1.15) | - | 2.7473 (0.80) | 0.8654 (3.96) | - | 0.6869 | 3.1281 (1.10) [0.967] | 0.0e+00 / - / 0.0e+00 / 0.0e+00 |
| interp/sg/cc/b0/d10/L2/p8 | 4 | 10.1522 | 8.9878 (1.13) | - | 8.5373 (0.84) | 2.5683 (3.95) | - | 2.1374 | 11.5075 (0.88) [0.941] | 5.6e-17 / - / 0.0e+00 / 0.0e+00 |

| case (nested nodes) | distinct points per call | dedup [s] (api/x) | multi + dedup [s] (api/x) | max diff dedup / multi + dedup | dsg api [s] (api/x) | max diff dsg api |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 131073 | 0.0313 (2.11) | 0.0121 (5.47) | 0.0e+00 / 0.0e+00 | 0.0215 (3.07) | 6.1e-15 |
| interp/sg/cc/b0/d3/L9/p64 | 114689 | 0.0454 (3.08) | 0.0197 (7.09) | 0.0e+00 / 0.0e+00 | 0.0283 (4.93) | 1.9e-14 |
| interp/sg/cc/b0/d5/L6/p64 | 282625 | 0.2205 (4.19) | 0.1003 (9.21) | 0.0e+00 / 0.0e+00 | 0.1165 (7.94) | 2.5e-15 |
| interp/sg/cc/b0/d7/L4/p16 | 676161 | 0.7402 (4.64) | 0.3082 (11.13) | 0.0e+00 / 0.0e+00 | 0.4184 (8.20) | 4.7e-15 |
| interp/sg/cc/b0/d10/L2/p8 | 2421009 | 3.0342 (3.35) | 1.0460 (9.71) | 0.0e+00 / 0.0e+00 | 2.1820 (4.65) | 1.5e-16 |

| case (multilinear) | slots | store per call (values + marks) | evals per call (api) | distinct evals per call | api / distinct | lin dedup [s] (api/x) | lin dedup all [s] (api/x) | max diff lin dedup / all | dsg api [s] (api/x) | max diff dsg api |
|---|---|---|---|---|---|---|---|---|---|---|
