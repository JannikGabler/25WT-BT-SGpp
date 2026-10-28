A/B `e2e`: A=`WP-02-after` B=`WP-02-split-a64`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.27ms | 92.98ms | 1.005 | [0.993, 1.017] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 91.99ms | 91.64ms | 1.005 | [0.992, 1.018] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.06ms | 1.05ms | 1.015 | [0.830, 1.242] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 205.20ms | 201.60ms | 1.018 | [1.013, 1.023] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 202.72ms | 198.85ms | 1.019 | [1.016, 1.023] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.25ms | 1.09ms | 0.993 | [0.676, 1.460] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.160s | 1.148s | 1.011 | [0.986, 1.036] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.151s | 1.137s | 1.011 | [0.988, 1.033] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 5.54ms | 3.04ms | 1.120 | [0.374, 3.355] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.486s | 5.405s | 1.015 | [1.011, 1.019] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.478s | 5.396s | 1.015 | [1.012, 1.019] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.93ms | 3.63ms | 0.969 | [0.581, 1.618] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.228s | 1.215s | 1.013 | [1.003, 1.023] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.152s | 1.146s | 1.009 | [0.997, 1.020] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 44.15ms | 41.93ms | 1.083 | [0.928, 1.265] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.227s | 1.222s | 1.003 | [0.998, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.126s | 1.122s | 1.003 | [0.999, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 59.96ms | 60.52ms | 0.997 | [0.909, 1.094] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 250.62ms | 249.40ms | 1.011 | [0.987, 1.036] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 247.03ms | 245.65ms | 1.012 | [0.989, 1.036] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.34ms | 2.30ms | 0.988 | [0.843, 1.159] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.466s | 1.441s | 1.019 | [0.996, 1.043] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.456s | 1.432s | 1.019 | [0.995, 1.043] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.98ms | 4.52ms | 1.098 | [0.883, 1.365] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.834s | 6.786s | 1.006 | [1.000, 1.012] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.825s | 6.776s | 1.006 | [1.001, 1.012] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.18ms | 3.96ms | 1.052 | [0.688, 1.608] | ~ (noise) |

Sum of medians (total, common cases): A = 17.951s, B = 17.760s, saved = 190.94ms (1.011x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d15/L7`: A 3.98e-10 -> B 4.34e-10 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.7 MiB | 62.6 MiB | 0.997 | 3.0% / 1.9% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.3 MiB | 26.3 MiB | 0.999 | 6.1% / 4.2% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.0 MiB | 42.0 MiB | 1.000 | 7.1% / 2.8% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.1 MiB | 42.1 MiB | 1.000 | 2.9% / 5.8% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 154.2 MiB | 149.8 MiB | 0.971 | 4.2% / 0.7% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.3 MiB | 217.4 MiB | 1.000 | 0.6% / 0.2% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.7 MiB | 25.7 MiB | 0.999 | 1.7% / 1.7% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 41.7 MiB | 42.2 MiB | 1.012 | 6.6% / 4.2% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.5 MiB | 42.1 MiB | 0.990 | 2.6% / 1.7% | ~ |

Peak memory (common cases): geomean B/A = 0.996; largest B/A: `quad/dsg/cc-uq/b0/d7/L10` 41.7 MiB -> 42.2 MiB (1.012x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.8%, 0.4%, 0.8%, 0.5%, 0.4%
Round durations (s): 74, 73, 74, 73, 74 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 368s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
