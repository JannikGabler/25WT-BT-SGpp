A/B `e2e`: A=`WP-02-before` B=`WP-02-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 98.20ms | 94.29ms | 1.023 | [0.992, 1.055] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 96.60ms | 93.01ms | 1.020 | [0.986, 1.055] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.53ms | 1.03ms | 1.222 | [0.999, 1.495] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 210.14ms | 206.39ms | 1.021 | [1.011, 1.032] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 207.71ms | 202.95ms | 1.021 | [1.016, 1.025] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.19ms | 2.06ms | 0.884 | [0.431, 1.811] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.211s | 1.157s | 1.046 | [1.028, 1.065] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.197s | 1.149s | 1.046 | [1.028, 1.064] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 5.46ms | 3.33ms | 1.148 | [0.436, 3.021] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.578s | 5.467s | 1.018 | [1.013, 1.023] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.569s | 5.459s | 1.018 | [1.013, 1.023] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.16ms | 3.24ms | 1.126 | [0.848, 1.495] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.825s | 1.223s | 3.135 | [3.104, 3.167] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.764s | 1.151s | 3.272 | [3.259, 3.284] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 35.48ms | 43.59ms | 0.915 | [0.698, 1.199] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.543s | 1.226s | 5.338 | [5.312, 5.365] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.450s | 1.126s | 5.734 | [5.697, 5.772] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 50.97ms | 59.54ms | 0.852 | [0.806, 0.902] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | total | 255.22ms | 250.88ms | 1.014 | [1.004, 1.025] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 251.40ms | 247.19ms | 1.015 | [1.005, 1.024] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.54ms | 2.49ms | 1.016 | [0.854, 1.209] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.494s | 1.456s | 1.025 | [1.019, 1.032] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.484s | 1.447s | 1.026 | [1.019, 1.032] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.93ms | 4.82ms | 1.023 | [0.879, 1.191] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.923s | 6.827s | 1.018 | [1.011, 1.025] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.913s | 6.819s | 1.018 | [1.011, 1.025] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.60ms | 3.95ms | 1.071 | [0.701, 1.637] | ~ (noise) |

Sum of medians (total, common cases): A = 26.137s, B = 17.909s, saved = 8.228s (1.459x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/cc-uq/b0/d5/L12`: A 1.19e-13 -> B 1.36e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.9 MiB | 62.5 MiB | 0.995 | 2.0% / 2.4% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 24.1 MiB | 26.0 MiB | 1.079 | 1.7% / 3.8% | MEM+ |
| quad/dsg/equi-exp/b0/d7/L10 | 37.5 MiB | 42.2 MiB | 1.124 | 3.0% / 3.9% | MEM+ |
| quad/dsg/equi-exp/b0/d10/L7 | 37.3 MiB | 41.9 MiB | 1.122 | 2.4% / 2.9% | MEM+ |
| quad/dsg/equi-exp/b1/d15/L7 | 233.1 MiB | 151.8 MiB | 0.651 | 2.3% / 2.4% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 327.6 MiB | 217.4 MiB | 0.664 | 1.2% / 0.5% | MEM- |
| quad/dsg/cc-uq/b0/d5/L12 | 23.9 MiB | 25.8 MiB | 1.078 | 2.9% / 3.8% | MEM+ |
| quad/dsg/cc-uq/b0/d7/L10 | 37.2 MiB | 43.6 MiB | 1.172 | 3.1% / 3.9% | MEM+ |
| quad/dsg/cc-uq/b0/d10/L7 | 37.3 MiB | 42.4 MiB | 1.138 | 3.2% / 4.6% | MEM+ |

Peak memory (common cases): geomean B/A = 0.981; largest B/A: `quad/dsg/cc-uq/b0/d7/L10` 37.2 MiB -> 43.6 MiB (1.172x); 6 MEM+, 2 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.9%, 0.5%, 0.5%, 0.4%, 0.4%
Round durations (s): 91, 90, 90, 90, 90 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 452s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
