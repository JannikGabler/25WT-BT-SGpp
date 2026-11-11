A/B `e2e`: A=`WP-08-after` B=`WP-08-export`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | total | 113.47ms | 112.09ms | 1.014 | [0.976, 1.053] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 107.39ms | 105.94ms | 1.015 | [0.974, 1.057] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.72ms | 1.67ms | 1.024 | [0.942, 1.113] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 363.61ms | 359.38ms | 1.016 | [0.990, 1.042] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 360.43ms | 356.59ms | 1.015 | [0.989, 1.042] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.27ms | 2.10ms | 1.134 | [0.811, 1.586] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.198s | 1.210s | 0.997 | [0.891, 1.115] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.193s | 1.207s | 0.996 | [0.890, 1.115] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 3.29ms | 2.14ms | 1.380 | [0.912, 2.087] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.898s | 1.906s | 0.979 | [0.936, 1.025] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.896s | 1.904s | 0.979 | [0.935, 1.025] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.74ms | 1.71ms | 1.154 | [0.840, 1.585] | ~ (noise) |

Sum of medians (total, common cases): A = 3.573s, B = 3.588s, saved = -14828124.0ns (0.996x)
Result sanity (rel. error vs exact): worst growth in `interplin/sg/equi/b1/d5/L8/p1024`: A 1.71e-04 -> B 1.71e-04 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.8 MiB | 1.005 | 3.7% / 1.9% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.4 MiB | 13.2 MiB | 0.987 | 4.2% / 3.1% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 13.1 MiB | 1.005 | 2.7% / 3.3% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.6 MiB | 13.5 MiB | 0.994 | 3.6% / 4.6% | ~ |

Peak memory (common cases): geomean B/A = 0.998; largest B/A: `interplin/sg/equi/b1/d8/L5/p256` 13.0 MiB -> 13.1 MiB (1.005x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.6 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.4%, 0.4%, 0.4%, 0.4%
Round durations (s): 15, 15, 16, 15, 15 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 77s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
