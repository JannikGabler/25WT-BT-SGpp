A/B `micro`: A=`WP-40-before` B=`WP-40-binA-libB`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c0 | call | 2.86ms | 2.85ms | 1.003 | [0.998, 1.007] | ~ (noise) |
| micro/cache/lookup/production/d2/n10000 | call | 516.46us | 515.41us | 1.019 | [0.968, 1.072] | ~ (noise) |
| micro/cache/lookup/production/d2/n1000000 | call | 7.14ms | 7.26ms | 1.001 | [0.944, 1.060] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 766.02us | 762.16us | 1.002 | [0.997, 1.007] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 16.03ms | 15.55ms | 1.040 | [1.008, 1.074] | faster |
| micro/cache/lookup/production/d20/n10000 | call | 1.15ms | 1.15ms | 1.002 | [0.985, 1.020] | ~ (noise) |
| micro/cache/lookup/production/d20/n1000000 | call | 22.26ms | 22.21ms | 0.995 | [0.979, 1.012] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 9.98ms | 9.93ms | 1.000 | [0.980, 1.020] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_cold_production/c0 | call | 109.09ms | 109.72ms | 0.999 | [0.981, 1.016] | ~ (noise) |

Sum of medians (call, common cases): A = 169.78ms, B = 169.95ms, saved = -172930.3ns (0.999x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 10, 10, 10, 10, 10 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 49s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
