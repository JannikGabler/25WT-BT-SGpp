A/B `e2e`: A=`WP-02-v1` B=`WP-02-v2`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.550s | 5.468s | 1.015 | [1.011, 1.019] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.541s | 5.460s | 1.015 | [1.011, 1.019] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.13ms | 3.19ms | 1.178 | [0.767, 1.811] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 2.290s | 2.132s | 1.077 | [1.065, 1.089] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 2.228s | 2.065s | 1.081 | [1.073, 1.088] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 35.47ms | 36.13ms | 0.961 | [0.801, 1.153] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 2.683s | 2.327s | 1.154 | [1.149, 1.158] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 2.591s | 2.229s | 1.162 | [1.157, 1.166] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 50.04ms | 52.20ms | 0.963 | [0.821, 1.130] | ~ (noise) |

Sum of medians (total, common cases): A = 10.523s, B = 9.928s, saved = 595.24ms (1.060x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d20/L6`: A 5.89e-09 -> B 6.06e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d10/L7 | 36.5 MiB | 38.0 MiB | 1.042 | 1.0% / 3.5% | MEM+ |
| quad/dsg/equi-exp/b1/d15/L7 | 233.3 MiB | 238.0 MiB | 1.020 | 1.7% / 1.6% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 331.0 MiB | 336.4 MiB | 1.016 | 2.1% / 1.8% | ~ |

Peak memory (common cases): geomean B/A = 1.026; largest B/A: `quad/dsg/equi-exp/b0/d10/L7` 36.5 MiB -> 38.0 MiB (1.042x); 1 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.1 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.4%, 0.5%, 0.4%, 0.4%
Round durations (s): 42, 42, 42, 42, 42 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 210s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
