A/B `e2e`: A=`WP-29b-before` B=`WP-29b-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 92.77ms | 92.75ms | 1.001 | [0.991, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 91.30ms | 91.42ms | 1.000 | [0.991, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.15ms | 1.03ms | 1.008 | [0.824, 1.233] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 202.02ms | 201.51ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.32ms | 198.97ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.16ms | 1.13ms | 1.049 | [0.813, 1.354] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.135s | 1.132s | 1.006 | [0.989, 1.024] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.127s | 1.124s | 1.007 | [0.989, 1.025] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.00ms | 3.04ms | 0.991 | [0.975, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.410s | 5.400s | 1.003 | [0.999, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.401s | 5.392s | 1.003 | [0.999, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.22ms | 3.17ms | 0.969 | [0.678, 1.385] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.215s | 1.223s | 0.995 | [0.985, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.143s | 1.146s | 0.996 | [0.986, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.39ms | 43.02ms | 0.975 | [0.835, 1.138] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.225s | 1.224s | 1.000 | [0.995, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.122s | 1.123s | 1.002 | [0.995, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 59.51ms | 60.45ms | 0.966 | [0.851, 1.097] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 248.31ms | 247.95ms | 1.002 | [0.995, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.45ms | 244.48ms | 1.002 | [0.995, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.28ms | 2.37ms | 0.945 | [0.863, 1.035] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.442s | 1.437s | 1.003 | [0.999, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.432s | 1.427s | 1.002 | [0.998, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.84ms | 4.53ms | 1.117 | [0.975, 1.279] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.778s | 6.778s | 1.000 | [0.994, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.769s | 6.769s | 1.000 | [0.996, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.53ms | 4.59ms | 1.030 | [0.674, 1.575] | ~ (noise) |

Sum of medians (total, common cases): A = 17.748s, B = 17.738s, saved = 10.72ms (1.001x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/cc-uq/b0/d7/L10`: A 2.26e-12 -> B 2.47e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.9 MiB | 62.2 MiB | 0.989 | 3.7% / 2.6% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 25.9 MiB | 25.9 MiB | 1.003 | 2.5% / 2.0% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.7 MiB | 42.4 MiB | 0.993 | 1.6% / 4.1% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.8 MiB | 41.3 MiB | 0.989 | 3.5% / 2.8% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 150.5 MiB | 153.0 MiB | 1.017 | 2.8% / 2.9% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.1 MiB | 217.3 MiB | 1.001 | 0.8% / 1.2% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.3 MiB | 25.4 MiB | 1.002 | 3.1% / 2.0% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 41.9 MiB | 41.6 MiB | 0.991 | 4.5% / 5.7% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.5 MiB | 42.7 MiB | 1.006 | 3.2% / 5.3% | ~ |

Peak memory (common cases): geomean B/A = 0.999; largest B/A: `quad/dsg/equi-exp/b1/d15/L7` 150.5 MiB -> 153.0 MiB (1.017x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.0 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.3%, 0.2%, 0.2%
Round durations (s): 73, 73, 73, 73, 73 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 365s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
