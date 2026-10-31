(Variant V1, `updateCoeffSums` defined in the .cpp; snapshot `WP-29b-after` of V1, renamed `WP-29b-v1b` afterwards. Not the final state, see the summary.)

A/B `e2e`: A=`WP-29b-before` B=`WP-29b-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 95.59ms | 93.12ms | 1.020 | [1.001, 1.040] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 92.17ms | 91.67ms | 1.013 | [0.990, 1.037] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.17ms | 1.11ms | 1.361 | [0.638, 2.906] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 201.72ms | 204.11ms | 0.986 | [0.972, 1.000] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.09ms | 201.50ms | 0.986 | [0.972, 1.000] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.03ms | 1.10ms | 1.041 | [0.807, 1.344] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.142s | 1.149s | 0.994 | [0.982, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.134s | 1.141s | 0.994 | [0.983, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 2.97ms | 3.00ms | 0.902 | [0.674, 1.208] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.425s | 5.490s | 0.989 | [0.985, 0.994] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.417s | 5.482s | 0.990 | [0.985, 0.994] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.01ms | 3.42ms | 0.901 | [0.678, 1.196] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.224s | 1.235s | 0.995 | [0.988, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.151s | 1.156s | 0.996 | [0.991, 1.000] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.04ms | 45.10ms | 0.997 | [0.854, 1.163] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.226s | 1.230s | 0.995 | [0.989, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.127s | 1.129s | 0.997 | [0.991, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 59.22ms | 60.72ms | 0.968 | [0.940, 0.997] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.02ms | 250.86ms | 0.988 | [0.980, 0.996] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.50ms | 247.00ms | 0.989 | [0.981, 0.996] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.27ms | 2.35ms | 0.966 | [0.837, 1.115] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.449s | 1.463s | 0.990 | [0.984, 0.995] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.440s | 1.453s | 0.990 | [0.984, 0.995] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.82ms | 4.47ms | 0.948 | [0.593, 1.516] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.812s | 6.849s | 0.995 | [0.989, 1.000] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.803s | 6.840s | 0.995 | [0.990, 0.999] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.52ms | 4.58ms | 1.016 | [0.594, 1.738] | ~ (noise) |

Sum of medians (total, common cases): A = 17.824s, B = 17.965s, saved = -141061715.0ns (0.992x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/cc-uq/b0/d5/L12`: A 8.91e-14 -> B 1.43e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.8 MiB | 62.4 MiB | 0.994 | 2.5% / 1.6% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 25.8 MiB | 25.8 MiB | 1.000 | 3.1% / 5.5% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.0 MiB | 41.5 MiB | 0.988 | 3.8% / 4.5% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.7 MiB | 42.2 MiB | 1.013 | 2.5% / 2.5% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 149.7 MiB | 152.9 MiB | 1.022 | 3.0% / 2.0% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.0 MiB | 217.4 MiB | 1.002 | 0.7% / 0.3% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.3 MiB | 25.9 MiB | 1.027 | 2.7% / 1.8% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 41.5 MiB | 42.0 MiB | 1.011 | 3.9% / 4.7% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.3 MiB | 41.9 MiB | 0.989 | 4.0% / 4.9% | ~ |

Peak memory (common cases): geomean B/A = 1.005; largest B/A: `quad/dsg/cc-uq/b0/d5/L12` 25.3 MiB -> 25.9 MiB (1.027x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.8%, 0.7%, 0.8%, 0.7%, 0.6%
Round durations (s): 74, 74, 74, 74, 74 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 368s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
