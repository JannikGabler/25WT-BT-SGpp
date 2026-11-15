A/B `e2e`: A=`WP-56-eager-dyn` B=`WP-56-after`, 16 thread(s), 6 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d5/L10 | total | 264.70ms | 265.90ms | 1.000 | [0.986, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 262.84ms | 263.01ms | 1.001 | [0.991, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.49ms | 1.61ms | 0.902 | [0.485, 1.678] | ~ (noise) |

Sum of medians (total, common cases): A = 264.70ms, B = 265.90ms, saved = -1199487.0ns (0.995x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 3.32e-13 -> B 2.77e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d5/L10 | 12.7 MiB | 12.8 MiB | 1.009 | 1.6% / 3.3% | ~ |

Peak memory (common cases): geomean B/A = 1.009; largest B/A: `quad/sg/equi-exp/b0/d5/L10` 12.7 MiB -> 12.8 MiB (1.009x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 1, 1, 1, 1, 1, 1 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 7s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
