A/B `e2e`: A=`WP-56-c-lazy` B=`WP-56-c`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | total | 369.60ms | 374.81ms | 0.985 | [0.969, 1.002] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 295.29ms | 302.23ms | 0.977 | [0.960, 0.994] | SLOWER |
| quad/sg/cc-uq/b0/d2/L20 | generation | 69.20ms | 69.78ms | 1.020 | [0.925, 1.124] | ~ (noise) |

Sum of medians (total, common cases): A = 369.60ms, B = 374.81ms, saved = -5217565.0ns (0.986x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d2/L20`: A 8.08e-16 -> B 2.69e-16 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/cc-uq/b0/d2/L20 | 195.1 MiB | 194.6 MiB | 0.997 | 12.1% / 14.4% | ~ |

Peak memory (common cases): geomean B/A = 0.997; largest B/A: `quad/sg/cc-uq/b0/d2/L20` 195.1 MiB -> 194.6 MiB (0.997x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.1%, 0.1%, 0.1%, 0.1%, 0.3%, 0.1%, 0.1%
Round durations (s): 2, 2, 2, 2, 2, 2, 2, 2 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 14s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
