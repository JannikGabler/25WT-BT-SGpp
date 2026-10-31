A/B `e2e`: A=`WP-26-before` B=`WP-26-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.36ms | 93.46ms | 0.999 | [0.989, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 91.94ms | 92.03ms | 1.000 | [0.990, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.03ms | 1.12ms | 0.951 | [0.830, 1.089] | ~ (noise) |

Sum of medians (total, common cases): A = 93.36ms, B = 93.46ms, saved = -96066.0ns (0.999x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b0/d3/L16`: A 1.15e-12 -> B 1.15e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 63.0 MiB | 62.8 MiB | 0.997 | 3.1% / 1.6% | ~ |

Peak memory (common cases): geomean B/A = 0.997; largest B/A: `quad/dsg/equi-exp/b0/d3/L16` 63.0 MiB -> 62.8 MiB (0.997x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.0%, 0.1%, 0.1%, 0.3%, 0.1%, 0.1%, 0.1%
Round durations (s): 1, 1, 1, 1, 1, 1, 1, 1 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 5s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
