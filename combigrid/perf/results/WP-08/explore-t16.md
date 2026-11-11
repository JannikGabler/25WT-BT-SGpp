threads: 16, repetitions: 7 (median, after 1 warm-up)

| case | TGs | sum nGP | max nGP / (sum/threads) | evals per call | evals p calls | evals multi-point | p calls / multi | regenerated nodes / integrand coordinates per call |
|---|---|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 33 | 6553617 | 0.80 | 132 | 2162688 | 1832019 | 1.18 | - |
| interplin/sg/equi/b1/d5/L8/p1024 | 1231 | 3715715 | 0.09 | 39392 | 40337408 | 13559391 | 2.97 | - |
| interplin/sg/equi/b1/d8/L5/p256 | 1287 | 747079 | 0.07 | 329472 | 84344832 | 44497943 | 1.90 | - |
| interplin/sg/equi/b1/d10/L5/p32 | 3003 | 2304931 | 0.02 | 3075072 | 98402304 | 92224292 | 1.07 | - |

| case | p | api [s] | kernel [s] (api/x) | kernel cached weights | sol_api [s] (share of api) | multi | multi cached weights | sol_multi [s] | api + cache [hit rate] | max diff kernel / kernel w / multi / cache |
|---|---|---|---|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 16384 | 0.1025 | 0.1067 (0.96) | - | 0.1029 (1.00) | - | - | - | 0.1832 (0.56) [0.590] | 4.0e-15 / - / - / 4.0e-15 |
| interplin/sg/equi/b1/d5/L8/p1024 | 1024 | 0.3604 | 0.3500 (1.03) | - | 0.3470 (0.96) | - | - | - | 0.7902 (0.46) [0.962] | 1.4e-13 / - / - / 9.9e-14 |
| interplin/sg/equi/b1/d8/L5/p256 | 256 | 1.2057 | 1.1938 (1.01) | - | 1.1958 (0.99) | - | - | - | 2.1883 (0.55) [0.939] | 1.1e-13 / - / - / 2.1e-14 |
| interplin/sg/equi/b1/d10/L5/p32 | 32 | 1.8425 | 1.8648 (0.99) | - | 1.8548 (1.01) | - | - | - | 3.8699 (0.48) [0.878] | 6.8e-14 / - / - / 9.8e-15 |

| case (nested nodes) | distinct points per call | dedup [s] (api/x) | multi + dedup [s] (api/x) | max diff dedup / multi + dedup |
|---|---|---|---|---|

| case (multilinear) | slots | store per call (values + marks) | evals per call (api) | distinct evals per call | api / distinct | lin dedup [s] (api/x) | lin dedup all [s] (api/x) | max diff lin dedup / all |
|---|---|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 188 | 1.7 KiB | 132 | 68 | 1.94 | 0.3366 (0.30) | 0.3293 (0.31) | 4.7e-15 / 6.2e-15 |
| interplin/sg/equi/b1/d5/L8/p1024 | 5908 | 51.9 KiB | 39392 | 5852 | 6.73 | 0.2451 (1.47) | 0.1534 (2.35) | 2.0e-13 / 1.3e-13 |
| interplin/sg/equi/b1/d8/L5/p256 | 47264 | 415.4 KiB | 329472 | 47264 | 6.97 | 0.4895 (2.46) | 0.3502 (3.44) | 7.0e-14 / 6.5e-14 |
| interplin/sg/equi/b1/d10/L5/p32 | 370688 | 3258.0 KiB | 3075072 | 370688 | 8.30 | 0.5535 (3.33) | 0.4059 (4.54) | 3.2e-14 / 2.8e-14 |
