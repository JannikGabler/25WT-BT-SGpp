A/B `e2e`: A=`rp-4` B=`rp-4-sor-a64`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 435.25ms | 436.46ms | 1.006 | [0.934, 1.083] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 364.28ms | 364.90ms | 1.000 | [0.928, 1.078] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.08ms | 30.97ms | 1.003 | [0.933, 1.078] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 264.95ms | 267.78ms | 0.989 | [0.981, 0.997] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | operator | 262.48ms | 265.35ms | 0.989 | [0.981, 0.996] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.01ms | 1.96ms | 1.036 | [0.948, 1.133] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 514.37ms | 520.26ms | 0.991 | [0.984, 0.998] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | operator | 511.89ms | 517.83ms | 0.991 | [0.984, 0.998] | SLOWER |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.82ms | 1.80ms | 0.989 | [0.941, 1.040] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 909.50ms | 917.56ms | 0.992 | [0.988, 0.996] | SLOWER |
| quad/sg/equi-exp/b0/d10/L4 | operator | 907.18ms | 913.72ms | 0.993 | [0.989, 0.998] | SLOWER |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.16ms | 3.50ms | 0.662 | [0.499, 0.877] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | total | 297.69ms | 304.21ms | 0.983 | [0.973, 0.992] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | operator | 289.10ms | 294.55ms | 0.985 | [0.974, 0.995] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | generation | 4.84ms | 5.37ms | 0.889 | [0.733, 1.077] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 520.31ms | 521.16ms | 1.000 | [0.992, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 496.08ms | 496.56ms | 0.999 | [0.990, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 16.04ms | 15.93ms | 1.038 | [0.912, 1.182] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 233.65ms | 234.03ms | 1.003 | [0.977, 1.030] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 205.01ms | 202.63ms | 1.014 | [1.000, 1.029] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 19.21ms | 20.75ms | 0.907 | [0.720, 1.142] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.404s | 2.407s | 0.997 | [0.992, 1.003] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.402s | 2.404s | 0.997 | [0.992, 1.003] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.85ms | 1.92ms | 0.966 | [0.900, 1.037] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.213s | 1.260s | 0.976 | [0.930, 1.025] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.122s | 1.170s | 0.973 | [0.928, 1.019] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 87.10ms | 87.31ms | 1.050 | [0.919, 1.200] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 86.94ms | 87.12ms | 0.996 | [0.987, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.06ms | 84.60ms | 0.998 | [0.988, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.07ms | 2.10ms | 0.990 | [0.944, 1.040] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 323.32ms | 323.83ms | 0.992 | [0.982, 1.002] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 320.45ms | 321.09ms | 0.992 | [0.982, 1.002] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.34ms | 2.34ms | 0.979 | [0.910, 1.053] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 694.23ms | 697.61ms | 0.995 | [0.992, 0.999] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | operator | 691.65ms | 695.01ms | 0.995 | [0.992, 0.999] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.95ms | 1.97ms | 0.994 | [0.945, 1.046] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.319s | 1.317s | 1.005 | [0.993, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.317s | 1.314s | 1.005 | [0.994, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.34ms | 2.31ms | 0.933 | [0.706, 1.232] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 130.00ms | 129.34ms | 1.010 | [0.987, 1.034] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 129.73ms | 129.08ms | 1.012 | [0.990, 1.035] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 205.40us | 208.13us | 0.770 | [0.419, 1.415] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 187.81ms | 186.88ms | 1.009 | [0.980, 1.040] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 187.67ms | 186.73ms | 1.009 | [0.980, 1.040] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.79us | 75.32us | 1.001 | [0.911, 1.099] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.125s | 1.129s | 0.998 | [0.994, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.124s | 1.129s | 0.998 | [0.994, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 154.84us | 155.12us | 0.902 | [0.763, 1.066] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.010s | 1.012s | 0.996 | [0.988, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.009s | 1.011s | 0.997 | [0.988, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 117.19us | 129.45us | 0.688 | [0.251, 1.886] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.623s | 1.620s | 0.997 | [0.978, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.623s | 1.620s | 0.997 | [0.978, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 56.82us | 55.98us | 1.060 | [0.786, 1.431] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.52ms | 34.42ms | 1.012 | [0.967, 1.060] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 34.44ms | 34.34ms | 1.012 | [0.967, 1.060] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 26.43us | 26.68us | 1.017 | [0.799, 1.294] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 116.62ms | 116.42ms | 1.001 | [0.992, 1.010] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 116.51ms | 116.30ms | 1.001 | [0.992, 1.010] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 38.66us | 47.67us | 0.879 | [0.739, 1.046] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.91ms | 11.93ms | 0.992 | [0.981, 1.003] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.84ms | 11.86ms | 0.992 | [0.981, 1.003] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 19.55us | 22.03us | 0.940 | [0.848, 1.041] | ~ (noise) |

Sum of medians (total, common cases): A = 13.455s, B = 13.534s, saved = -78862874.5ns (0.994x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 2.78e-12 -> B 8.11e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 620.1 MiB | 620.1 MiB | 1.000 | 0.0% / 0.1% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.9 MiB | 15.9 MiB | 0.999 | 4.8% / 4.3% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.8 MiB | 14.7 MiB | 0.994 | 2.1% / 2.0% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 13.1 MiB | 13.1 MiB | 1.003 | 2.8% / 3.1% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 26.2 MiB | 26.2 MiB | 1.001 | 3.5% / 5.3% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 53.9 MiB | 53.7 MiB | 0.997 | 6.9% / 10.1% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 58.6 MiB | 60.8 MiB | 1.037 | 13.4% / 13.7% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.9 MiB | 15.0 MiB | 1.008 | 1.7% / 3.6% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1084.6 MiB | 1068.7 MiB | 0.985 | 4.7% / 2.6% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.9 MiB | 40.8 MiB | 0.996 | 5.0% / 6.3% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.7 MiB | 17.8 MiB | 1.006 | 4.1% / 6.0% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 15.1 MiB | 15.2 MiB | 1.012 | 4.7% / 5.5% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.2 MiB | 13.3 MiB | 1.004 | 2.9% / 6.0% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 15.9 MiB | 15.8 MiB | 0.995 | 3.5% / 4.9% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.3 MiB | 12.3 MiB | 1.003 | 2.1% / 2.5% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.2 MiB | 12.3 MiB | 1.008 | 3.7% / 4.0% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.2 MiB | 12.2 MiB | 1.000 | 2.6% / 1.8% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.1 MiB | 12.2 MiB | 1.005 | 2.8% / 4.1% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.2 MiB | 12.3 MiB | 1.004 | 2.0% / 3.2% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.2 MiB | 12.2 MiB | 0.997 | 2.9% / 2.1% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.2 MiB | 0.993 | 2.0% / 4.3% | ~ |

Peak memory (common cases): geomean B/A = 1.002; largest B/A: `quad/sg/equi-exp/b1/d20/L5` 58.6 MiB -> 60.8 MiB (1.037x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.6 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 58, 58, 58, 59, 58, 58, 58, 58 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 467s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
