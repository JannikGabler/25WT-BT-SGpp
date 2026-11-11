A/B `e2e`: A=`WP-08-after` B=`WP-08-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.213s | 1.232s | 0.994 | [0.928, 1.066] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.210s | 1.227s | 0.995 | [0.928, 1.067] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.13ms | 2.99ms | 0.741 | [0.561, 0.979] | SLOWER |

Sum of medians (total, common cases): A = 1.213s, B = 1.232s, saved = -19109262.5ns (0.984x)
Result sanity (rel. error vs exact): worst growth in `interplin/sg/equi/b1/d8/L5/p256`: A 3.19e-02 -> B 3.19e-02 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 13.1 MiB | 1.004 | 1.2% / 1.4% | ~ |

Peak memory (common cases): geomean B/A = 1.004; largest B/A: `interplin/sg/equi/b1/d8/L5/p256` 13.0 MiB -> 13.1 MiB (1.004x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.4%, 0.4%, 0.4%, 0.5%, 0.4%, 0.4%, 0.4%
Round durations (s): 5, 5, 5, 5, 5, 5, 5, 5 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 41s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
