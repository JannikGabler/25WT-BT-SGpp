A/B `e2e`: A=`WP-47-before` B=`WP-47-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | total | 111.09ms | 111.34ms | 0.988 | [0.942, 1.038] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.95ms | 105.13ms | 0.988 | [0.938, 1.040] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.73ms | 1.68ms | 1.030 | [0.973, 1.089] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 362.01ms | 361.23ms | 1.006 | [0.985, 1.027] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 358.97ms | 358.43ms | 1.005 | [0.984, 1.026] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.17ms | 2.09ms | 1.129 | [0.940, 1.355] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.250s | 1.234s | 1.016 | [0.953, 1.082] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.247s | 1.231s | 1.016 | [0.954, 1.082] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.24ms | 2.29ms | 0.943 | [0.670, 1.328] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.875s | 1.872s | 1.002 | [0.988, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.873s | 1.870s | 1.002 | [0.988, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.72ms | 1.74ms | 1.007 | [0.941, 1.079] | ~ (noise) |

Sum of medians (total, common cases): A = 3.598s, B = 3.579s, saved = 19.45ms (1.005x)
Result sanity (rel. error vs exact): worst growth in `interplin/sg/equi/b1/d2/L16/p16384`: A 1.92e-10 -> B 1.92e-10 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.9 MiB | 1.007 | 2.0% / 3.2% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.3 MiB | 13.3 MiB | 0.996 | 4.7% / 5.3% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 13.1 MiB | 1.005 | 2.5% / 2.4% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.7 MiB | 13.7 MiB | 0.996 | 6.6% / 1.9% | ~ |

Peak memory (common cases): geomean B/A = 1.001; largest B/A: `interplin/sg/equi/b1/d2/L16/p16384` 19.7 MiB -> 19.9 MiB (1.007x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.5 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.5%, 0.2%, 0.2%
Round durations (s): 15, 15, 15, 15, 15, 16, 15, 15 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 122s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
