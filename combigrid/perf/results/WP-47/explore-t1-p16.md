threads: 1, repetitions: 3 (median, after 1 warm-up)

| case | TGs | sum nGP | max nGP / (sum/threads) | evals per call | evals p calls | evals multi-point | p calls / multi | regenerated nodes / integrand coordinates per call |
|---|---|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 33 | 6553617 | 0.05 | 132 | 2112 | 2112 | 1.00 | - |
| interplin/sg/equi/b1/d5/L8/p1024 | 1231 | 3715715 | 0.01 | 39392 | 630272 | 614646 | 1.03 | - |
| interplin/sg/equi/b1/d8/L5/p256 | 1287 | 747079 | 0.00 | 329472 | 5271552 | 4996223 | 1.06 | - |
| interplin/sg/equi/b1/d10/L5/p32 | 3003 | 2304931 | 0.00 | 3075072 | 49201152 | 47881375 | 1.03 | - |

| case | p | api [s] | kernel [s] (api/x) | kernel cached weights | sol_api [s] (share of api) | multi | multi cached weights | sol_multi [s] | api + cache [hit rate] | max diff kernel / kernel w / multi / cache |
|---|---|---|---|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 16 | 0.0002 | 0.0002 (1.12) | - | 0.0002 (0.82) | - | - | - | 0.0003 (0.57) [0.485] | 1.0e-15 / - / - / 0.0e+00 |
| interplin/sg/equi/b1/d5/L8/p1024 | 16 | 0.0583 | 0.0574 (1.02) | - | 0.0567 (0.97) | - | - | - | 0.0637 (0.92) [0.860] | 1.7e-14 / - / - / 0.0e+00 |
| interplin/sg/equi/b1/d8/L5/p256 | 16 | 0.7755 | 0.7803 (0.99) | - | 0.7767 (1.00) | - | - | - | 0.6615 (1.17) [0.869] | 4.5e-15 / - / - / 0.0e+00 |
| interplin/sg/equi/b1/d10/L5/p32 | 16 | 9.7142 | 10.0594 (0.97) | - | 10.0159 (1.03) | - | - | - | 10.1239 (0.96) [0.885] | 5.1e-14 / - / - / 0.0e+00 |

| case (nested nodes) | distinct points per call | dedup [s] (api/x) | multi + dedup [s] (api/x) | max diff dedup / multi + dedup |
|---|---|---|---|---|

| case (multilinear) | slots | store per call (values + marks) | evals per call (api) | distinct evals per call | api / distinct | lin dedup [s] (api/x) | lin dedup all [s] (api/x) | max diff lin dedup / all | dsg api [s] (api/x) | max diff dsg api |
|---|---|---|---|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 188 | 1.7 KiB | 132 | 68 | 1.94 | 0.0003 (0.66) | 0.0003 (0.69) | 1.8e-15 / 1.8e-15 | 0.0002 (1.11) | 1.8e-15 |
| interplin/sg/equi/b1/d5/L8/p1024 | 5908 | 51.9 KiB | 39392 | 5852 | 6.73 | 0.0283 (2.06) | 0.0179 (3.25) | 1.4e-14 / 1.4e-14 | 0.0232 (2.52) | 1.4e-14 |
| interplin/sg/equi/b1/d8/L5/p256 | 47264 | 415.4 KiB | 329472 | 47264 | 6.97 | 0.2681 (2.89) | 0.1942 (3.99) | 7.5e-15 / 7.5e-15 | 0.2253 (3.44) | 7.5e-15 |
| interplin/sg/equi/b1/d10/L5/p32 | 370688 | 3258.0 KiB | 3075072 | 370688 | 8.30 | 2.6030 (3.73) | 1.9375 (5.01) | 2.0e-14 / 2.0e-14 | 2.1845 (4.45) | 2.0e-14 |
