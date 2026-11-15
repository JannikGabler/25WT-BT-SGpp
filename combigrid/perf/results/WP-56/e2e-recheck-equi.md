A/B `e2e`: A=`WP-56-before` B=`WP-56-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 341.29ms | 347.73ms | 0.988 | [0.917, 1.063] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 296.34ms | 308.69ms | 0.980 | [0.910, 1.056] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 37.81ms | 38.25ms | 0.983 | [0.914, 1.056] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 262.08ms | 265.10ms | 0.981 | [0.966, 0.996] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | operator | 260.61ms | 263.65ms | 0.980 | [0.967, 0.994] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.48ms | 1.60ms | 0.972 | [0.660, 1.432] | ~ (noise) |

Sum of medians (total, common cases): A = 603.37ms, B = 612.83ms, saved = -9461137.0ns (0.985x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d2/L20`: A 7.18e-14 -> B 7.25e-14 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 121.2 MiB | 125.6 MiB | 1.036 | 17.7% / 27.2% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.7 MiB | 12.7 MiB | 1.006 | 3.2% / 4.2% | ~ |

Peak memory (common cases): geomean B/A = 1.021; largest B/A: `quad/sg/equi-exp/b0/d2/L20` 121.2 MiB -> 125.6 MiB (1.036x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.4%, 0.4%, 0.2%, 0.4%, 0.6%, 0.2%, 0.3%
Round durations (s): 3, 3, 3, 3, 3, 3, 3, 3 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 23s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
