A/B `micro`: A=`WP-40-binA-libB` B=`WP-40-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c0 | call | 2.86ms | 2.69ms | 1.058 | [1.045, 1.072] | faster |
| micro/cache/lookup/production/d2/n10000 | call | 514.91us | 583.63us | 0.886 | [0.875, 0.897] | SLOWER |
| micro/cache/lookup/production/d2/n1000000 | call | 7.01ms | 7.12ms | 0.973 | [0.883, 1.072] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 768.41us | 772.44us | 0.993 | [0.979, 1.007] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 16.20ms | 17.09ms | 0.942 | [0.927, 0.956] | SLOWER |
| micro/cache/lookup/production/d20/n10000 | call | 1.15ms | 1.16ms | 0.992 | [0.987, 0.996] | SLOWER |
| micro/cache/lookup/production/d20/n1000000 | call | 22.14ms | 22.14ms | 0.990 | [0.947, 1.036] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 9.96ms | 10.51ms | 0.949 | [0.945, 0.953] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_cold_production/c0 | call | 110.43ms | 111.73ms | 0.988 | [0.974, 1.001] | ~ (noise) |

Sum of medians (call, common cases): A = 171.04ms, B = 173.79ms, saved = -2751499.4ns (0.984x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 10, 10, 10, 10, 10 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 49s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
