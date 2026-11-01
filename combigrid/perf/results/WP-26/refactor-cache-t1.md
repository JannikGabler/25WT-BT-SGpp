A/B `micro`: A=`WP-26-after` B=`WP-26-after-r`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/lookup/production/d2/n10000 | call | 505.20us | 508.85us | 0.985 | [0.961, 1.010] | ~ (noise) |
| micro/cache/insert/production/d2/n10000 | call | 1.05ms | 1.05ms | 0.996 | [0.989, 1.003] | ~ (noise) |
| micro/cache/lookup/production/d2/n1000000 | call | 7.90ms | 7.73ms | 1.028 | [0.948, 1.115] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 161.58ms | 170.32ms | 0.958 | [0.921, 0.996] | SLOWER |
| micro/cache/lookup/production/d10/n10000 | call | 751.01us | 770.08us | 0.975 | [0.971, 0.979] | SLOWER |
| micro/cache/insert/production/d10/n10000 | call | 929.87us | 933.18us | 0.996 | [0.989, 1.004] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 16.46ms | 16.92ms | 0.981 | [0.945, 1.018] | ~ (noise) |
| micro/cache/insert/production/d10/n1000000 | call | 247.36ms | 248.27ms | 0.991 | [0.973, 1.011] | ~ (noise) |
| micro/cache/lookup/production/d20/n10000 | call | 1.15ms | 1.16ms | 1.021 | [0.945, 1.104] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 1.05ms | 1.04ms | 1.033 | [0.948, 1.125] | ~ (noise) |
| micro/cache/lookup/production/d20/n1000000 | call | 21.90ms | 21.79ms | 1.004 | [0.984, 1.024] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 333.16ms | 334.98ms | 0.994 | [0.986, 1.001] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 18.42ms | 18.43ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 13.68ms | 14.11ms | 0.966 | [0.959, 0.974] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 53.27ms | 53.26ms | 0.999 | [0.991, 1.008] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 13.75ms | 14.11ms | 0.972 | [0.958, 0.987] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 37.24ms | 35.75ms | 1.038 | [1.026, 1.051] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c0 | call | 20.57ms | 20.70ms | 1.001 | [0.974, 1.029] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 116.87ms | 114.35ms | 1.023 | [1.019, 1.027] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c1000 | call | 20.63ms | 21.04ms | 0.976 | [0.941, 1.012] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 17.03ms | 17.18ms | 0.990 | [0.983, 0.997] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 10.00ms | 10.10ms | 0.994 | [0.986, 1.002] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 76.15ms | 76.20ms | 0.999 | [0.992, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c1000 | call | 10.01ms | 10.37ms | 0.970 | [0.965, 0.974] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 34.59ms | 34.52ms | 0.996 | [0.985, 1.008] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 20.10ms | 20.79ms | 0.971 | [0.952, 0.989] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 121.27ms | 121.65ms | 0.998 | [0.994, 1.001] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 20.09ms | 20.79ms | 0.965 | [0.949, 0.982] | SLOWER |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 20.36ms | 20.82ms | 0.977 | [0.971, 0.984] | SLOWER |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c0 | call | 9.15ms | 9.27ms | 0.989 | [0.980, 0.997] | SLOWER |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 118.04ms | 117.67ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c1000 | call | 9.13ms | 9.33ms | 0.974 | [0.959, 0.990] | SLOWER |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 41.93ms | 42.13ms | 0.994 | [0.987, 1.001] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 39.64ms | 39.61ms | 1.002 | [0.990, 1.014] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 53.85ms | 53.79ms | 0.999 | [0.987, 1.012] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 39.80ms | 39.63ms | 1.007 | [0.996, 1.017] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 62.68ms | 63.19ms | 0.991 | [0.981, 1.001] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c0 | call | 37.00ms | 37.21ms | 0.995 | [0.992, 0.999] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 137.10ms | 137.58ms | 0.996 | [0.992, 1.000] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c1000 | call | 37.05ms | 37.18ms | 1.000 | [0.983, 1.018] | ~ (noise) |

Sum of medians (call, common cases): A = 2.003s, B = 2.016s, saved = -13055200.7ns (0.994x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 53, 53, 53, 53, 53 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 267s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
