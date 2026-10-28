A/B `e2e`: A=`WP-20-nsa` B=`WP-20-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 98.43ms | 96.29ms | 1.020 | [1.012, 1.027] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 96.96ms | 95.04ms | 1.020 | [1.012, 1.028] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 982.24us | 997.33us | 1.017 | [0.831, 1.245] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 211.44ms | 209.90ms | 1.006 | [1.001, 1.011] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 208.67ms | 207.29ms | 1.005 | [1.002, 1.009] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.24ms | 1.29ms | 1.034 | [0.696, 1.537] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.191s | 1.196s | 0.993 | [0.982, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.184s | 1.188s | 0.993 | [0.986, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.06ms | 3.21ms | 0.946 | [0.374, 2.392] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.521s | 5.586s | 0.990 | [0.985, 0.994] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.512s | 5.577s | 0.990 | [0.985, 0.994] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.12ms | 3.62ms | 0.987 | [0.681, 1.431] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.890s | 3.820s | 1.018 | [1.011, 1.025] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.828s | 3.761s | 1.019 | [1.013, 1.024] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 35.53ms | 35.09ms | 0.975 | [0.836, 1.138] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.659s | 6.524s | 1.020 | [1.016, 1.024] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.569s | 6.435s | 1.020 | [1.016, 1.024] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 50.80ms | 49.66ms | 1.020 | [0.990, 1.051] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 257.70ms | 255.14ms | 1.010 | [1.006, 1.014] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 253.92ms | 251.28ms | 1.011 | [1.007, 1.015] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.55ms | 2.31ms | 1.035 | [0.963, 1.112] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.489s | 1.488s | 1.002 | [0.996, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.480s | 1.479s | 1.002 | [0.997, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.18ms | 4.25ms | 1.057 | [0.801, 1.394] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.948s | 6.922s | 1.003 | [0.997, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.939s | 6.914s | 1.003 | [0.999, 1.008] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.67ms | 3.48ms | 1.146 | [0.750, 1.752] | ~ (noise) |

Sum of medians (total, common cases): A = 26.266s, B = 26.099s, saved = 167.52ms (1.006x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d15/L7`: A 9.45e-10 -> B 1.25e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.4 MiB | 62.7 MiB | 1.005 | 2.1% / 1.7% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 24.7 MiB | 24.1 MiB | 0.977 | 6.3% / 1.7% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 37.5 MiB | 37.5 MiB | 1.001 | 2.3% / 5.2% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 37.0 MiB | 36.4 MiB | 0.984 | 3.6% / 4.4% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 233.1 MiB | 231.1 MiB | 0.991 | 1.1% / 1.2% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 328.6 MiB | 329.0 MiB | 1.001 | 1.3% / 2.1% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 24.1 MiB | 24.3 MiB | 1.010 | 4.4% / 5.2% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 37.3 MiB | 37.1 MiB | 0.996 | 6.1% / 3.4% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 36.9 MiB | 37.4 MiB | 1.015 | 3.1% / 7.9% | ~ |

Peak memory (common cases): geomean B/A = 0.998; largest B/A: `quad/dsg/cc-uq/b0/d10/L7` 36.9 MiB -> 37.4 MiB (1.015x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.0 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.3%, 0.3%, 0.3%
Round durations (s): 107, 107, 107, 107, 107 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 535s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
