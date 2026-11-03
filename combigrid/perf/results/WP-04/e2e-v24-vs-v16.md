A/B `e2e`: A=`WP-04-v24` B=`WP-04-v16`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 90.66ms | 93.46ms | 0.976 | [0.943, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 89.37ms | 91.96ms | 0.981 | [0.955, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.13ms | 1.22ms | 0.747 | [0.387, 1.441] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 192.16ms | 197.87ms | 0.973 | [0.967, 0.979] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 190.00ms | 194.74ms | 0.976 | [0.970, 0.982] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.05ms | 1.55ms | 0.691 | [0.535, 0.891] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.066s | 1.088s | 0.983 | [0.974, 0.991] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.059s | 1.080s | 0.983 | [0.974, 0.992] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.04ms | 3.02ms | 1.004 | [0.986, 1.022] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.080s | 5.179s | 0.981 | [0.978, 0.985] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.072s | 5.169s | 0.981 | [0.978, 0.985] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.21ms | 3.36ms | 0.918 | [0.691, 1.219] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.158s | 1.165s | 0.991 | [0.980, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.086s | 1.095s | 0.990 | [0.978, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.83ms | 43.19ms | 1.003 | [0.859, 1.170] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.178s | 1.183s | 0.991 | [0.981, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.077s | 1.080s | 0.993 | [0.981, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 59.92ms | 62.89ms | 0.951 | [0.896, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 237.10ms | 241.20ms | 0.984 | [0.979, 0.990] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 233.36ms | 237.12ms | 0.984 | [0.980, 0.989] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.32ms | 2.36ms | 0.981 | [0.913, 1.055] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.363s | 1.382s | 0.985 | [0.982, 0.989] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.353s | 1.372s | 0.986 | [0.982, 0.990] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.28ms | 4.94ms | 0.888 | [0.733, 1.077] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.505s | 6.654s | 0.978 | [0.973, 0.984] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.495s | 6.645s | 0.978 | [0.974, 0.983] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.61ms | 4.07ms | 1.174 | [0.768, 1.795] | ~ (noise) |

Sum of medians (total, common cases): A = 16.870s, B = 17.183s, saved = -312524538.0ns (0.982x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d15/L7`: A 4.11e-10 -> B 4.41e-10 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.8 MiB | 62.5 MiB | 0.995 | 3.8% / 3.0% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 25.7 MiB | 25.8 MiB | 1.002 | 3.6% / 3.6% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.0 MiB | 40.0 MiB | 0.953 | 7.8% / 4.5% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.8 MiB | 38.8 MiB | 0.928 | 2.0% / 1.6% | MEM- |
| quad/dsg/equi-exp/b1/d15/L7 | 150.5 MiB | 150.8 MiB | 1.002 | 0.6% / 2.0% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 215.2 MiB | 215.3 MiB | 1.000 | 0.2% / 0.1% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.8 MiB | 24.9 MiB | 0.966 | 2.6% / 3.0% | MEM- |
| quad/dsg/cc-uq/b0/d7/L10 | 43.2 MiB | 39.7 MiB | 0.920 | 4.2% / 2.9% | MEM- |
| quad/dsg/cc-uq/b0/d10/L7 | 42.6 MiB | 39.0 MiB | 0.914 | 3.6% / 2.7% | MEM- |

Peak memory (common cases): geomean B/A = 0.964; largest B/A: `quad/dsg/equi-exp/b0/d5/L12` 25.7 MiB -> 25.8 MiB (1.002x); 0 MEM+, 4 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.2%, 0.2%, 0.2%
Round durations (s): 70, 70, 70, 70, 70 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 351s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
