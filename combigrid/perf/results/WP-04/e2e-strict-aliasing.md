A/B `e2e`: A=`WP-04-after` B=`WP-04-nsa`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.076s | 1.126s | 0.955 | [0.947, 0.962] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.069s | 1.118s | 0.954 | [0.947, 0.961] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.05ms | 3.04ms | 1.000 | [0.983, 1.016] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.126s | 5.313s | 0.965 | [0.962, 0.969] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.117s | 5.305s | 0.965 | [0.962, 0.969] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.39ms | 3.39ms | 1.021 | [0.769, 1.355] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.369s | 1.421s | 0.962 | [0.955, 0.968] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.360s | 1.411s | 0.962 | [0.955, 0.969] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.27ms | 4.53ms | 0.957 | [0.819, 1.119] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.534s | 6.838s | 0.956 | [0.950, 0.962] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.525s | 6.829s | 0.956 | [0.952, 0.960] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.57ms | 3.97ms | 1.059 | [0.693, 1.618] | ~ (noise) |

Sum of medians (total, common cases): A = 14.105s, B = 14.698s, saved = -593142781.0ns (0.960x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/cc-uq/b0/d7/L10`: A 2.02e-12 -> B 2.41e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d7/L10 | 41.5 MiB | 43.3 MiB | 1.043 | 6.6% / 4.0% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.9 MiB | 41.8 MiB | 0.998 | 2.7% / 2.6% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 43.3 MiB | 42.5 MiB | 0.981 | 6.4% / 3.9% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 41.9 MiB | 42.1 MiB | 1.004 | 1.2% / 3.4% | ~ |

Peak memory (common cases): geomean B/A = 1.006; largest B/A: `quad/dsg/equi-exp/b0/d7/L10` 41.5 MiB -> 43.3 MiB (1.043x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.3%
Round durations (s): 58, 58, 58, 58, 58 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 292s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
