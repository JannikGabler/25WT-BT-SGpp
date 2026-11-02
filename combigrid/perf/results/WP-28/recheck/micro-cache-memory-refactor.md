A/B `micro`: A=`WP-28-checkpoint` B=`WP-28-refactor`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/lookup/production/d2/n10000 | call | 510.91us | 516.10us | 0.991 | [0.977, 1.005] | ~ (noise) |
| micro/cache/insert/production/d2/n10000 | call | 1.05ms | 1.05ms | 0.997 | [0.990, 1.005] | ~ (noise) |
| micro/cache/lookup/production/d2/n1000000 | call | 7.81ms | 7.80ms | 1.029 | [0.962, 1.101] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 169.65ms | 159.72ms | 1.027 | [0.978, 1.079] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 765.05us | 763.10us | 1.003 | [0.998, 1.008] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 936.48us | 930.16us | 1.003 | [0.986, 1.020] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 16.39ms | 15.84ms | 1.029 | [0.979, 1.082] | ~ (noise) |
| micro/cache/insert/production/d10/n1000000 | call | 249.31ms | 249.20ms | 0.997 | [0.981, 1.013] | ~ (noise) |
| micro/cache/lookup/production/d20/n10000 | call | 1.15ms | 1.14ms | 1.004 | [0.993, 1.015] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 1.04ms | 1.04ms | 1.008 | [1.000, 1.016] | ~ (noise) |
| micro/cache/lookup/production/d20/n1000000 | call | 21.99ms | 22.30ms | 0.985 | [0.955, 1.016] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 335.88ms | 337.38ms | 0.987 | [0.973, 1.002] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 18.83ms | 18.79ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 14.09ms | 14.19ms | 0.997 | [0.989, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 53.53ms | 53.66ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 14.22ms | 14.06ms | 1.008 | [0.993, 1.023] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 37.42ms | 37.48ms | 0.994 | [0.984, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c0 | call | 20.66ms | 20.50ms | 1.004 | [0.978, 1.031] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 116.14ms | 116.01ms | 1.001 | [0.996, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c1000 | call | 20.52ms | 20.68ms | 0.986 | [0.941, 1.032] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 17.09ms | 17.13ms | 0.997 | [0.987, 1.007] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 9.92ms | 9.97ms | 0.996 | [0.977, 1.015] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 76.23ms | 76.45ms | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c1000 | call | 10.94ms | 10.89ms | 0.999 | [0.981, 1.017] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 34.27ms | 34.52ms | 0.994 | [0.977, 1.012] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 20.66ms | 21.10ms | 0.978 | [0.956, 1.000] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 122.42ms | 122.37ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 20.97ms | 20.81ms | 1.007 | [0.999, 1.016] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 20.82ms | 20.87ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c0 | call | 9.29ms | 9.35ms | 0.995 | [0.989, 1.001] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 118.12ms | 117.98ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c1000 | call | 9.29ms | 9.33ms | 0.993 | [0.976, 1.011] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 41.97ms | 42.09ms | 0.997 | [0.989, 1.004] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 39.87ms | 39.66ms | 1.003 | [0.987, 1.019] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 53.70ms | 53.68ms | 1.000 | [0.993, 1.008] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 39.64ms | 39.63ms | 0.998 | [0.986, 1.011] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 62.96ms | 62.15ms | 1.011 | [0.997, 1.025] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c0 | call | 37.22ms | 37.68ms | 0.993 | [0.973, 1.013] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 137.60ms | 137.16ms | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c1000 | call | 37.04ms | 37.79ms | 0.977 | [0.938, 1.017] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit1GiB | call | 2.645s | 2.499s | 1.035 | [0.977, 1.097] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit16MiB | call | 1.580s | 1.590s | 0.997 | [0.984, 1.010] | ~ (noise) |

Sum of medians (call, common cases): A = 6.247s, B = 6.102s, saved = 144.45ms (1.024x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 95, 95, 96, 95, 95 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 476s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
