A/B `micro`: A=`WP-26-after` B=`WP-26-after-r`, 16 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/lookup/production/d2/n10000 | call | 104.03us | 103.13us | 1.091 | [0.855, 1.391] | ~ (noise) |
| micro/cache/insert/production/d2/n10000 | call | 259.42us | 260.87us | 0.973 | [0.901, 1.050] | ~ (noise) |
| micro/cache/lookup/production/d2/n1000000 | call | 1.03ms | 1.03ms | 0.996 | [0.950, 1.045] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 40.63ms | 40.40ms | 1.013 | [0.991, 1.036] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 115.07us | 119.92us | 0.917 | [0.242, 3.473] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 271.72us | 272.37us | 1.061 | [0.140, 8.061] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 1.45ms | 1.47ms | 0.995 | [0.951, 1.040] | ~ (noise) |
| micro/cache/insert/production/d10/n1000000 | call | 91.86ms | 90.93ms | 1.008 | [0.985, 1.032] | ~ (noise) |
| micro/cache/lookup/production/d20/n10000 | call | 135.96us | 136.97us | 0.748 | [0.277, 2.017] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 287.47us | 292.67us | 0.727 | [0.324, 1.629] | ~ (noise) |
| micro/cache/lookup/production/d20/n1000000 | call | 1.98ms | 1.89ms | 1.017 | [0.925, 1.119] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 128.43ms | 129.02ms | 0.988 | [0.943, 1.036] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 4.23ms | 4.24ms | 1.001 | [0.984, 1.018] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 2.52ms | 2.54ms | 0.982 | [0.900, 1.071] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 6.75ms | 6.78ms | 0.998 | [0.984, 1.012] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 2.54ms | 2.56ms | 0.842 | [0.537, 1.321] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 8.41ms | 8.46ms | 0.989 | [0.915, 1.069] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c0 | call | 2.45ms | 2.46ms | 0.991 | [0.973, 1.009] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 12.75ms | 12.63ms | 1.024 | [0.968, 1.083] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c1000 | call | 2.45ms | 2.48ms | 0.956 | [0.862, 1.061] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 5.56ms | 5.62ms | 0.989 | [0.957, 1.022] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 2.54ms | 2.54ms | 1.100 | [0.843, 1.434] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 13.98ms | 13.95ms | 1.006 | [0.980, 1.034] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c1000 | call | 2.52ms | 2.55ms | 0.985 | [0.971, 0.998] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 8.21ms | 8.23ms | 1.006 | [0.968, 1.046] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 2.96ms | 2.99ms | 1.007 | [0.938, 1.081] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 12.78ms | 12.89ms | 1.004 | [0.969, 1.040] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 2.98ms | 3.04ms | 0.995 | [0.952, 1.041] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 5.22ms | 5.20ms | 0.969 | [0.917, 1.024] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c0 | call | 1.66ms | 1.67ms | 0.989 | [0.977, 1.000] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 12.86ms | 12.88ms | 0.989 | [0.959, 1.020] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c1000 | call | 1.68ms | 1.68ms | 1.078 | [0.939, 1.236] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 6.87ms | 6.80ms | 0.976 | [0.870, 1.094] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 5.93ms | 5.94ms | 1.013 | [0.973, 1.055] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 7.89ms | 7.90ms | 0.995 | [0.899, 1.101] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 5.93ms | 5.98ms | 1.023 | [0.949, 1.103] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 11.11ms | 11.41ms | 0.961 | [0.919, 1.005] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c0 | call | 4.87ms | 4.90ms | 0.980 | [0.886, 1.085] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 18.05ms | 18.02ms | 0.999 | [0.981, 1.017] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c1000 | call | 4.88ms | 4.89ms | 0.999 | [0.991, 1.007] | ~ (noise) |

Sum of medians (call, common cases): A = 447.13ms, B = 447.15ms, saved = -11414.7ns (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.1%, 0.1%, 0.5%, 0.6%
Round durations (s): 37, 38, 38, 37, 38 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 188s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
