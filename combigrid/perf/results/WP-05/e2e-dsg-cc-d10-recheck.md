A/B `e2e`: A=`WP-05-before` B=`WP-05-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.891s | 7.077s | 0.978 | [0.964, 0.993] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.890s | 7.076s | 0.978 | [0.964, 0.993] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 433.50us | 273.18us | 1.243 | [0.707, 2.185] | ~ (noise) |

Sum of medians (total, common cases): A = 6.891s, B = 7.077s, saved = -185227772.0ns (0.974x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/cc-uq/b0/d10/L7`: A 2.83e-11 -> B 2.85e-11 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/cc-uq/b0/d10/L7 | 29.9 MiB | 30.1 MiB | 1.006 | 8.8% / 5.0% | ~ |

Peak memory (common cases): geomean B/A = 1.006; largest B/A: `quad/dsg/cc-uq/b0/d10/L7` 29.9 MiB -> 30.1 MiB (1.006x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.1 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 28, 28, 28, 28, 29, 28, 28, 28 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 226s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
