threads: 16, repetitions: 7 (median, after 1 warm-up)

| case | TGs | sum nGP | max nGP / (sum/threads) | evals per call | evals p calls | evals multi-point | p calls / multi | regenerated nodes / integrand coordinates per call |
|---|---|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 33 | 6553617 | 0.80 | 132 | 2162688 | 1832019 | 1.18 | - |
| interplin/sg/equi/b1/d5/L8/p1024 | 1231 | 3715715 | 0.09 | 39392 | 40337408 | 13559391 | 2.97 | - |
| interplin/sg/equi/b1/d8/L5/p256 | 1287 | 747079 | 0.07 | 329472 | 84344832 | 44497943 | 1.90 | - |
| interplin/sg/equi/b1/d10/L5/p32 | 3003 | 2304931 | 0.02 | 3075072 | 98402304 | 92224292 | 1.07 | - |

| case | p | api [s] | kernel [s] (api/x) | kernel cached weights | sol_api [s] (share of api) | multi | multi cached weights | sol_multi [s] | api + cache [hit rate] | max diff kernel / kernel w / multi / cache |
|---|---|---|---|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 16384 | 0.1055 | 0.1086 (0.97) | - | 0.1013 (0.96) | - | - | - | 0.1769 (0.60) [0.590] | 6.2e-15 / - / - / 4.9e-15 |
| interplin/sg/equi/b1/d5/L8/p1024 | 1024 | 0.3600 | 0.3570 (1.01) | - | 0.3549 (0.99) | - | - | - | 0.7901 (0.46) [0.962] | 1.2e-13 / - / - / 9.9e-14 |
| interplin/sg/equi/b1/d8/L5/p256 | 256 | 1.2176 | 1.2041 (1.01) | - | 1.2082 (0.99) | - | - | - | 2.2029 (0.55) [0.939] | 1.2e-13 / - / - / 3.1e-14 |
| interplin/sg/equi/b1/d10/L5/p32 | 32 | 1.9714 | 1.8739 (1.05) | - | 1.8694 (0.95) | - | - | - | 3.7953 (0.52) [0.878] | 5.7e-14 / - / - / 1.3e-14 |

| case (nested nodes) | distinct points per call | dedup [s] (api/x) | multi + dedup [s] (api/x) | max diff dedup / multi + dedup |
|---|---|---|---|---|

| case (multilinear) | slots | store per call (values + marks) | evals per call (api) | distinct evals per call | api / distinct | lin dedup [s] (api/x) | lin dedup all [s] (api/x) | max diff lin dedup / all | dsg api [s] (api/x) | max diff dsg api |
|---|---|---|---|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 188 | 1.7 KiB | 132 | 68 | 1.94 | 0.3358 (0.31) | 0.3270 (0.32) | 6.0e-15 / 5.1e-15 | 0.1752 (0.60) | 8.2e-15 |
| interplin/sg/equi/b1/d5/L8/p1024 | 5908 | 51.9 KiB | 39392 | 5852 | 6.73 | 0.2401 (1.50) | 0.1544 (2.33) | 1.6e-13 / 2.3e-13 | 0.1814 (1.98) | 1.4e-13 |
| interplin/sg/equi/b1/d8/L5/p256 | 47264 | 415.4 KiB | 329472 | 47264 | 6.97 | 0.4910 (2.48) | 0.3595 (3.39) | 6.4e-14 / 6.0e-14 | 0.3712 (3.28) | 7.2e-14 |
| interplin/sg/equi/b1/d10/L5/p32 | 370688 | 3258.0 KiB | 3075072 | 370688 | 8.30 | 0.5586 (3.53) | 0.4097 (4.81) | 2.9e-14 / 5.0e-14 | 0.4322 (4.56) | 4.7e-14 |
