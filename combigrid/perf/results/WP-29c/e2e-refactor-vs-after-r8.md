A/B `e2e`: A=`WP-29c-after` B=`WP-29c-refactor`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d5/L6/p64 | total | 1.455s | 1.450s | 1.006 | [0.995, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.455s | 1.450s | 1.007 | [0.996, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 117.89us | 109.48us | 0.719 | [0.272, 1.902] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.543s | 1.555s | 1.006 | [0.943, 1.073] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.540s | 1.553s | 1.005 | [0.943, 1.072] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 3.01ms | 2.25ms | 1.224 | [0.805, 1.862] | ~ (noise) |

Sum of medians (total, common cases): A = 2.999s, B = 3.006s, saved = -7165916.0ns (0.998x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d5/L6/p64`: A 7.11e-08 -> B 7.11e-08 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.0 MiB | 0.996 | 2.0% / 3.2% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.8 MiB | 12.7 MiB | 0.996 | 3.4% / 4.1% | ~ |

Peak memory (common cases): geomean B/A = 0.996; largest B/A: `interp/sg/cc/b0/d5/L6/p64` 12.1 MiB -> 12.0 MiB (0.996x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.4%, 0.5%, 0.8%, 0.4%, 0.5%, 0.4%, 0.3%
Round durations (s): 13, 12, 13, 13, 12, 13, 12, 12 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 100s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
