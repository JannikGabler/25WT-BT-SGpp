A/B `e2e`: A=`WP-77-after` B=`WP-77-interplin-dyn`, 16 thread(s), 12 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | total | 102.24ms | 106.68ms | 0.962 | [0.937, 0.989] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 95.27ms | 100.01ms | 0.959 | [0.932, 0.987] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.40ms | 2.30ms | 1.024 | [0.982, 1.069] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 369.04ms | 373.04ms | 0.988 | [0.976, 1.001] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 368.50ms | 372.48ms | 0.988 | [0.975, 1.002] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 95.89us | 94.25us | 0.972 | [0.905, 1.043] | ~ (noise) |

Sum of medians (total, common cases): A = 471.28ms, B = 479.71ms, saved = -8437302.5ns (0.982x)
Result sanity (rel. error vs exact): worst growth in `interplin/sg/equi/b1/d2/L16/p16384`: A 1.92e-10 -> B 1.92e-10 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 20.7 MiB | 20.5 MiB | 0.989 | 9.2% / 8.4% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.8 MiB | 12.8 MiB | 1.001 | 1.3% / 4.2% | ~ |

Peak memory (common cases): geomean B/A = 0.995; largest B/A: `interplin/sg/equi/b1/d5/L8/p1024` 12.8 MiB -> 12.8 MiB (1.001x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.5 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.3%, 0.2%, 0.2%, 0.2%, 0.1%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 2, 2, 3, 2, 2, 2, 2, 2, 2, 3, 2, 2 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 30s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
