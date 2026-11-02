A/B `micro`: A=`WP-40-before` B=`WP-40-binA-libB`, 16 thread(s), 8 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/lookup/production/d2/n10000 | call | 104.99us | 106.76us | 0.737 | [0.374, 1.451] | ~ (noise) |
| micro/cache/insert/production/d2/n10000 | call | 263.12us | 268.78us | 0.951 | [0.859, 1.053] | ~ (noise) |
| micro/cache/lookup/production/d2/n1000000 | call | 1.04ms | 1.05ms | 1.069 | [0.885, 1.291] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 40.95ms | 40.59ms | 1.011 | [0.992, 1.031] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 201.70us | 125.73us | 1.271 | [0.594, 2.720] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 275.67us | 273.09us | 0.918 | [0.439, 1.919] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 1.44ms | 1.45ms | 0.992 | [0.964, 1.021] | ~ (noise) |
| micro/cache/insert/production/d10/n1000000 | call | 91.99ms | 90.82ms | 1.004 | [0.983, 1.026] | ~ (noise) |
| micro/cache/lookup/production/d20/n10000 | call | 137.77us | 138.84us | 0.876 | [0.619, 1.241] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 290.96us | 291.66us | 1.010 | [0.954, 1.070] | ~ (noise) |
| micro/cache/lookup/production/d20/n1000000 | call | 1.92ms | 1.94ms | 0.976 | [0.949, 1.005] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 127.82ms | 130.39ms | 0.980 | [0.965, 0.995] | SLOWER |

Sum of medians (call, common cases): A = 266.43ms, B = 267.45ms, saved = -1013156.3ns (0.996x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.1%, 0.2%, 0.2%, 0.3%
Round durations (s): 15, 14, 14, 14, 15, 15, 14, 15 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 116s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
