A/B `micro`: A=`WP-27-before` B=`WP-27-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/lookup/production/d2/n10000 | call | 580.23us | 516.72us | 1.127 | [1.118, 1.135] | faster |
| micro/cache/insert/production/d2/n10000 | call | 1.05ms | 1.05ms | 1.005 | [0.998, 1.011] | ~ (noise) |
| micro/cache/lookup/production/d2/n1000000 | call | 7.92ms | 7.58ms | 1.093 | [0.935, 1.277] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 161.29ms | 163.34ms | 0.989 | [0.936, 1.046] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 777.32us | 773.23us | 1.006 | [0.997, 1.015] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 938.60us | 955.85us | 0.980 | [0.971, 0.989] | SLOWER |
| micro/cache/lookup/production/d10/n1000000 | call | 17.04ms | 16.95ms | 0.994 | [0.968, 1.021] | ~ (noise) |
| micro/cache/insert/production/d10/n1000000 | call | 245.58ms | 247.74ms | 0.983 | [0.962, 1.005] | ~ (noise) |
| micro/cache/lookup/production/d20/n10000 | call | 1.16ms | 1.15ms | 1.009 | [0.998, 1.019] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 1.04ms | 1.05ms | 0.989 | [0.984, 0.995] | SLOWER |
| micro/cache/lookup/production/d20/n1000000 | call | 21.70ms | 21.96ms | 0.988 | [0.935, 1.043] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 337.46ms | 338.24ms | 0.997 | [0.991, 1.003] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c0 | call | 5.72ms | 5.69ms | 1.002 | [0.977, 1.028] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c1000 | call | 197.19ms | 197.66ms | 0.999 | [0.992, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 19.01ms | 18.83ms | 1.010 | [1.005, 1.015] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 14.14ms | 14.05ms | 1.009 | [1.002, 1.016] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 53.46ms | 53.53ms | 0.997 | [0.992, 1.001] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 14.09ms | 13.99ms | 1.010 | [1.001, 1.020] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c0 | call | 5.46ms | 5.49ms | 0.979 | [0.922, 1.038] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c1000 | call | 154.74ms | 154.87ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 36.91ms | 38.61ms | 0.957 | [0.953, 0.962] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c0 | call | 20.51ms | 20.66ms | 0.990 | [0.958, 1.023] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 115.85ms | 114.42ms | 1.013 | [1.008, 1.017] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c1000 | call | 20.50ms | 20.56ms | 1.008 | [0.968, 1.050] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c0 | call | 2.69ms | 2.76ms | 0.974 | [0.969, 0.979] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c1000 | call | 151.46ms | 151.61ms | 0.987 | [0.951, 1.024] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 17.12ms | 16.66ms | 1.024 | [1.015, 1.033] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 10.65ms | 10.06ms | 1.050 | [1.043, 1.056] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 76.34ms | 75.70ms | 1.007 | [1.003, 1.012] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c1000 | call | 10.52ms | 10.22ms | 1.025 | [1.021, 1.029] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c0 | call | 5.58ms | 5.60ms | 0.994 | [0.987, 1.001] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c1000 | call | 225.45ms | 225.34ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 34.07ms | 34.81ms | 0.978 | [0.968, 0.988] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 20.85ms | 21.16ms | 0.979 | [0.960, 0.998] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 122.39ms | 122.84ms | 0.994 | [0.990, 0.999] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 20.70ms | 21.04ms | 0.982 | [0.964, 1.001] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c0 | call | 3.74ms | 3.74ms | 1.002 | [0.995, 1.009] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c1000 | call | 100.22ms | 100.53ms | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 20.71ms | 20.11ms | 1.033 | [1.025, 1.042] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c0 | call | 9.27ms | 9.30ms | 0.986 | [0.950, 1.023] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 118.53ms | 118.67ms | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c1000 | call | 9.30ms | 9.26ms | 1.005 | [0.993, 1.016] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c0 | call | 49.15ms | 49.08ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c1000 | call | 482.88ms | 481.50ms | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 42.15ms | 41.90ms | 1.008 | [0.998, 1.017] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 39.47ms | 39.54ms | 0.999 | [0.982, 1.016] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 53.83ms | 53.74ms | 1.000 | [0.993, 1.007] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 39.78ms | 39.54ms | 1.006 | [1.001, 1.011] | faster |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c0 | call | 61.19ms | 60.95ms | 1.007 | [0.989, 1.025] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c1000 | call | 389.81ms | 390.02ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 62.07ms | 62.07ms | 1.001 | [0.992, 1.011] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c0 | call | 37.02ms | 37.12ms | 0.996 | [0.982, 1.010] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 136.49ms | 136.25ms | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c1000 | call | 37.12ms | 37.01ms | 1.001 | [0.991, 1.011] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_cold_production/c0 | call | 37.93ms | 38.24ms | 0.993 | [0.983, 1.003] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_warm_production/c0 | call | 21.90ms | 21.74ms | 1.009 | [1.000, 1.017] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_production/c0 | call | 208.50ms | 209.34ms | 0.999 | [0.992, 1.006] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_warm_production/c0 | call | 109.02ms | 108.19ms | 1.006 | [0.994, 1.018] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_cold_production/c1000 | call | 73.09ms | 74.05ms | 0.990 | [0.987, 0.994] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_warm_production/c1000 | call | 21.83ms | 21.80ms | 0.993 | [0.969, 1.018] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_production/c1000 | call | 245.07ms | 242.30ms | 1.009 | [0.994, 1.024] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_warm_production/c1000 | call | 109.58ms | 108.26ms | 1.013 | [0.996, 1.030] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_cold_production/c0 | call | 111.20ms | 111.12ms | 1.007 | [0.995, 1.018] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_warm_production/c0 | call | 63.73ms | 63.28ms | 1.018 | [0.987, 1.050] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_production/c0 | call | 467.96ms | 470.80ms | 0.999 | [0.969, 1.029] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_warm_production/c0 | call | 101.47ms | 101.17ms | 1.002 | [0.979, 1.026] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_cold_production/c1000 | call | 186.73ms | 187.48ms | 0.993 | [0.990, 0.997] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_warm_production/c1000 | call | 64.35ms | 62.18ms | 1.030 | [1.020, 1.039] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_production/c1000 | call | 548.85ms | 547.23ms | 1.003 | [0.994, 1.011] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_warm_production/c1000 | call | 101.18ms | 99.95ms | 1.003 | [0.951, 1.058] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_cold_production/c0 | call | 47.33ms | 48.45ms | 0.977 | [0.973, 0.980] | SLOWER |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_warm_production/c0 | call | 44.81ms | 44.55ms | 1.006 | [1.002, 1.011] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c0 | call | 201.96ms | 200.92ms | 1.005 | [0.999, 1.010] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_warm_production/c0 | call | 206.90ms | 205.42ms | 1.006 | [0.999, 1.013] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_cold_production/c1000 | call | 59.40ms | 59.84ms | 0.994 | [0.989, 0.999] | SLOWER |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_warm_production/c1000 | call | 44.18ms | 44.72ms | 0.990 | [0.978, 1.002] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c1000 | call | 213.68ms | 213.31ms | 1.000 | [0.991, 1.010] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_warm_production/c1000 | call | 207.34ms | 205.17ms | 0.999 | [0.967, 1.033] | ~ (noise) |

Sum of medians (call, common cases): A = 7.343s, B = 7.337s, saved = 5.37ms (1.001x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.2%, 0.2%, 0.2%
Round durations (s): 124, 124, 124, 124, 124 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 620s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
