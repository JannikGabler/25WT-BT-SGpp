A/B `e2e`: A=`WP-08-after` B=`WP-08-export`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.201s | 1.288s | 0.948 | [0.919, 0.979] | SLOWER |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.198s | 1.284s | 0.948 | [0.918, 0.979] | SLOWER |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.26ms | 2.26ms | 1.027 | [0.687, 1.538] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.879s | 1.850s | 1.014 | [0.983, 1.046] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.877s | 1.847s | 1.014 | [0.983, 1.046] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.73ms | 1.68ms | 1.036 | [0.983, 1.091] | ~ (noise) |

Sum of medians (total, common cases): A = 3.080s, B = 3.138s, saved = -57533118.0ns (0.982x)
Result sanity (rel. error vs exact): worst growth in `interplin/sg/equi/b1/d8/L5/p256`: A 3.19e-02 -> B 3.19e-02 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d8/L5/p256 | 13.1 MiB | 13.1 MiB | 1.001 | 3.5% / 2.6% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.6 MiB | 13.7 MiB | 1.007 | 5.3% / 3.6% | ~ |

Peak memory (common cases): geomean B/A = 1.004; largest B/A: `interplin/sg/equi/b1/d10/L5/p32` 13.6 MiB -> 13.7 MiB (1.007x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.5 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.5%, 0.3%, 0.4%, 0.5%, 0.4%, 0.4%, 0.3%
Round durations (s): 13, 13, 13, 13, 13, 13, 13, 13 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 102s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
