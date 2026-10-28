A/B `e2e`: A=`WP-02-before` B=`WP-02-v1`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.583s | 5.556s | 1.005 | [0.999, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.574s | 5.547s | 1.005 | [0.999, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.11ms | 4.21ms | 0.965 | [0.727, 1.281] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.830s | 2.294s | 1.671 | [1.665, 1.677] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.770s | 2.233s | 1.689 | [1.683, 1.696] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 35.63ms | 35.26ms | 1.011 | [0.866, 1.180] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.526s | 2.667s | 2.449 | [2.440, 2.458] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.436s | 2.578s | 2.499 | [2.490, 2.508] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 49.86ms | 50.13ms | 1.001 | [0.972, 1.032] | ~ (noise) |

Sum of medians (total, common cases): A = 15.939s, B = 10.517s, saved = 5.422s (1.516x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d15/L7`: A 8.75e-10 -> B 1.11e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d10/L7 | 36.5 MiB | 36.7 MiB | 1.006 | 3.3% / 2.7% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 231.2 MiB | 232.8 MiB | 1.007 | 1.7% / 3.1% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 325.9 MiB | 327.8 MiB | 1.006 | 1.3% / 1.3% | ~ |

Peak memory (common cases): geomean B/A = 1.006; largest B/A: `quad/dsg/equi-exp/b1/d15/L7` 231.2 MiB -> 232.8 MiB (1.007x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.1 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.4%, 0.4%, 0.4%, 0.4%
Round durations (s): 54, 54, 54, 54, 54 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 270s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
