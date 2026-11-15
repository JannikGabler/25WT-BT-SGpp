A/B `e2e`: A=`WP-05-before` B=`rp-5`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.431s | 5.435s | 1.001 | [0.998, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.430s | 5.434s | 1.001 | [0.998, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 463.36us | 478.41us | 0.974 | [0.701, 1.354] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.890s | 6.888s | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.890s | 6.887s | 1.001 | [0.997, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 279.12us | 401.52us | 0.877 | [0.559, 1.377] | ~ (noise) |

Sum of medians (total, common cases): A = 12.322s, B = 12.323s, saved = -1027359.0ns (1.000x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/cc-uq/b0/d10/L7`: A 2.79e-11 -> B 2.83e-11 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d10/L7 | 29.9 MiB | 29.7 MiB | 0.995 | 5.5% / 6.7% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 29.8 MiB | 29.9 MiB | 1.005 | 5.5% / 5.2% | ~ |

Peak memory (common cases): geomean B/A = 1.000; largest B/A: `quad/dsg/cc-uq/b0/d10/L7` 29.8 MiB -> 29.9 MiB (1.005x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.3%, 0.4%, 0.2%, 0.2%, 0.2%, 0.1%
Round durations (s): 50, 50, 50, 50, 50, 50, 50, 50 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 398s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
