A/B `e2e`: A=`WP-20-before` B=`WP-20-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 393.34ms | 406.66ms | 0.988 | [0.885, 1.102] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 322.82ms | 333.42ms | 0.992 | [0.888, 1.108] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 29.45ms | 30.10ms | 0.985 | [0.885, 1.097] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 252.66ms | 252.43ms | 0.990 | [0.956, 1.025] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 249.89ms | 250.19ms | 0.988 | [0.955, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.98ms | 1.86ms | 1.222 | [0.705, 2.119] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 500.92ms | 502.55ms | 0.999 | [0.989, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 498.69ms | 500.18ms | 0.999 | [0.989, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.79ms | 1.71ms | 1.033 | [0.928, 1.148] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 936.09ms | 936.71ms | 1.007 | [0.990, 1.024] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 933.79ms | 933.06ms | 1.007 | [0.991, 1.023] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.10ms | 2.16ms | 0.951 | [0.429, 2.107] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 302.71ms | 309.76ms | 0.985 | [0.966, 1.004] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 294.57ms | 301.28ms | 0.986 | [0.969, 1.003] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 4.24ms | 5.23ms | 0.937 | [0.740, 1.188] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 522.72ms | 520.76ms | 1.002 | [0.990, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 498.07ms | 496.71ms | 1.002 | [0.988, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.65ms | 15.35ms | 1.021 | [0.912, 1.144] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 234.97ms | 239.26ms | 1.010 | [0.955, 1.069] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 208.39ms | 208.56ms | 1.016 | [0.967, 1.068] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 18.37ms | 20.26ms | 0.978 | [0.654, 1.463] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.411s | 2.413s | 0.997 | [0.989, 1.006] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.408s | 2.410s | 0.997 | [0.989, 1.006] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.82ms | 1.86ms | 0.954 | [0.859, 1.060] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.191s | 1.204s | 1.004 | [0.949, 1.063] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.110s | 1.117s | 1.001 | [0.944, 1.060] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 78.97ms | 75.39ms | 1.045 | [0.877, 1.246] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.34ms | 87.02ms | 1.004 | [0.990, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.87ms | 84.44ms | 1.004 | [0.992, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.04ms | 2.06ms | 1.062 | [0.916, 1.230] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 327.31ms | 327.05ms | 1.001 | [0.989, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 324.35ms | 324.37ms | 1.001 | [0.989, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.32ms | 2.17ms | 1.058 | [0.988, 1.134] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 701.71ms | 702.67ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 699.14ms | 699.99ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.95ms | 1.94ms | 1.009 | [0.936, 1.088] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.315s | 1.310s | 1.004 | [0.993, 1.015] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.312s | 1.308s | 1.004 | [0.994, 1.014] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.27ms | 2.21ms | 1.152 | [0.800, 1.660] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 95.93ms | 95.49ms | 1.002 | [0.990, 1.013] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 94.56ms | 94.15ms | 1.002 | [0.990, 1.014] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 999.28us | 1.03ms | 0.967 | [0.790, 1.183] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 213.21ms | 209.48ms | 1.019 | [1.014, 1.024] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 210.72ms | 206.88ms | 1.019 | [1.015, 1.022] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.42ms | 1.30ms | 1.087 | [0.820, 1.442] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.217s | 1.196s | 1.019 | [1.014, 1.024] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.210s | 1.188s | 1.019 | [1.014, 1.024] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.15ms | 2.99ms | 1.115 | [0.854, 1.456] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.663s | 5.573s | 1.017 | [1.013, 1.021] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.656s | 5.565s | 1.017 | [1.013, 1.021] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.73ms | 3.36ms | 1.014 | [0.685, 1.501] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 4.085s | 3.826s | 1.068 | [1.064, 1.072] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 4.026s | 3.761s | 1.070 | [1.066, 1.074] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 35.25ms | 35.26ms | 0.951 | [0.815, 1.111] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 7.024s | 6.521s | 1.076 | [1.072, 1.080] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.934s | 6.432s | 1.077 | [1.073, 1.081] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 50.24ms | 49.78ms | 1.010 | [0.980, 1.040] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 257.98ms | 254.06ms | 1.014 | [1.011, 1.018] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 254.14ms | 250.54ms | 1.014 | [1.010, 1.018] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.58ms | 2.28ms | 1.104 | [0.977, 1.246] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.509s | 1.488s | 1.013 | [1.009, 1.017] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.499s | 1.479s | 1.013 | [1.010, 1.017] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.97ms | 3.97ms | 1.042 | [0.896, 1.212] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 7.012s | 6.936s | 1.012 | [1.006, 1.018] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 7.002s | 6.926s | 1.012 | [1.007, 1.016] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.07ms | 3.69ms | 1.051 | [0.687, 1.606] | ~ (noise) |

Sum of medians (total, common cases): A = 36.254s, B = 35.309s, saved = 944.22ms (1.027x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 2.14e-13 -> B 4.30e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.8 MiB | 619.8 MiB | 1.000 | 0.0% / 0.1% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.6 MiB | 15.6 MiB | 0.999 | 3.2% / 5.6% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.6 MiB | 14.1 MiB | 0.964 | 3.9% / 2.5% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.8 MiB | 1.002 | 2.1% / 3.4% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.7 MiB | 25.9 MiB | 1.010 | 2.4% / 3.7% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.6 MiB | 53.2 MiB | 0.992 | 2.9% / 3.0% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 58.6 MiB | 59.9 MiB | 1.021 | 12.5% / 12.8% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.6 MiB | 14.4 MiB | 0.990 | 2.3% / 3.4% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1060.7 MiB | 1066.5 MiB | 1.005 | 4.8% / 3.3% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.3 MiB | 41.3 MiB | 1.026 | 5.4% / 6.7% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.7 MiB | 17.3 MiB | 0.979 | 2.5% / 2.2% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.8 MiB | 14.8 MiB | 0.998 | 1.9% / 1.6% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.8 MiB | 13.0 MiB | 1.012 | 1.4% / 3.3% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.5 MiB | 62.9 MiB | 1.006 | 1.4% / 2.4% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 24.2 MiB | 24.7 MiB | 1.020 | 6.5% / 4.8% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 37.1 MiB | 37.1 MiB | 1.001 | 4.0% / 2.5% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 36.9 MiB | 37.0 MiB | 1.004 | 4.1% / 6.2% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 231.3 MiB | 232.9 MiB | 1.007 | 1.2% / 2.6% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 326.4 MiB | 329.3 MiB | 1.009 | 0.9% / 0.9% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 23.9 MiB | 23.9 MiB | 0.997 | 4.2% / 3.4% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 37.7 MiB | 37.4 MiB | 0.992 | 2.9% / 2.7% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 37.2 MiB | 36.8 MiB | 0.990 | 3.1% / 4.3% | ~ |

Peak memory (common cases): geomean B/A = 1.001; largest B/A: `quad/sg/cc-uq/b0/d3/L14` 40.3 MiB -> 41.3 MiB (1.026x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.0 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 148, 148, 148, 148, 148 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 739s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
