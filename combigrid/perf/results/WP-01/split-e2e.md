A/B `e2e`: A=`WP-01-after` B=`WP-01-split`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 441.61ms | 430.59ms | 1.053 | [0.944, 1.175] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 371.10ms | 355.24ms | 1.043 | [0.934, 1.166] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.75ms | 29.48ms | 1.074 | [0.964, 1.195] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 271.16ms | 269.47ms | 1.007 | [0.995, 1.019] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 268.61ms | 266.93ms | 1.007 | [0.995, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.02ms | 1.92ms | 1.041 | [0.912, 1.189] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 519.75ms | 514.05ms | 1.017 | [0.996, 1.038] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 516.97ms | 511.52ms | 1.017 | [0.996, 1.038] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.88ms | 1.89ms | 0.992 | [0.937, 1.049] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 944.00ms | 938.84ms | 1.005 | [0.999, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 941.59ms | 936.33ms | 1.006 | [1.000, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.20ms | 2.23ms | 0.817 | [0.570, 1.173] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 307.55ms | 312.02ms | 0.996 | [0.986, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 298.95ms | 302.78ms | 0.998 | [0.987, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 4.99ms | 5.68ms | 0.893 | [0.738, 1.080] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 534.16ms | 532.48ms | 1.003 | [0.990, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 508.28ms | 504.63ms | 1.004 | [0.990, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 17.28ms | 16.68ms | 1.003 | [0.895, 1.123] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 241.68ms | 240.62ms | 1.011 | [0.998, 1.024] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 207.95ms | 207.59ms | 1.003 | [0.997, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 22.93ms | 21.77ms | 1.055 | [0.781, 1.426] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.422s | 2.424s | 0.997 | [0.989, 1.005] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.420s | 2.421s | 0.997 | [0.989, 1.005] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.91ms | 2.00ms | 0.977 | [0.879, 1.085] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.216s | 1.217s | 1.002 | [0.897, 1.120] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.135s | 1.132s | 0.999 | [0.889, 1.123] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 82.31ms | 82.72ms | 1.005 | [0.870, 1.162] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 90.21ms | 89.60ms | 1.002 | [0.988, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 87.37ms | 86.65ms | 1.006 | [0.990, 1.022] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.07ms | 2.21ms | 0.961 | [0.903, 1.023] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 343.55ms | 342.38ms | 1.010 | [0.983, 1.037] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 340.71ms | 339.54ms | 1.009 | [0.983, 1.037] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.33ms | 2.31ms | 1.038 | [0.939, 1.147] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 718.14ms | 712.15ms | 1.010 | [1.001, 1.019] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 715.46ms | 709.62ms | 1.010 | [1.001, 1.019] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.01ms | 2.03ms | 1.005 | [0.932, 1.084] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.318s | 1.309s | 1.010 | [1.002, 1.018] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.315s | 1.306s | 1.010 | [1.003, 1.018] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.32ms | 2.56ms | 0.875 | [0.567, 1.351] | ~ (noise) |

Sum of medians (total, common cases): A = 9.368s, B = 9.333s, saved = 35.36ms (1.004x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 3.00e-13 -> B 1.13e-12 (the binding check is `refcheck`)
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.3%, 0.3%, 0.3%, 0.3%
Round durations (s): 40, 40, 40, 40, 40 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 201s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
