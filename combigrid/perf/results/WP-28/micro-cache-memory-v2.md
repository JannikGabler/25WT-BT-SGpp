A/B `micro`: A=`WP-28-before` B=`WP-28-v2`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/lookup/production/d2/n10000 | call | 507.26us | 514.38us | 0.988 | [0.985, 0.992] | SLOWER |
| micro/cache/insert/production/d2/n10000 | call | 1.05ms | 1.07ms | 0.979 | [0.972, 0.986] | SLOWER |
| micro/cache/lookup/production/d2/n1000000 | call | 7.94ms | 7.71ms | 1.009 | [0.960, 1.061] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 171.32ms | 168.82ms | 0.989 | [0.970, 1.008] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 770.02us | 770.04us | 0.994 | [0.967, 1.022] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 937.54us | 946.40us | 0.988 | [0.976, 1.000] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 16.26ms | 16.45ms | 0.999 | [0.955, 1.045] | ~ (noise) |
| micro/cache/insert/production/d10/n1000000 | call | 253.93ms | 256.64ms | 0.986 | [0.972, 0.999] | SLOWER |
| micro/cache/lookup/production/d20/n10000 | call | 1.15ms | 1.15ms | 1.005 | [0.985, 1.025] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 1.04ms | 1.07ms | 0.972 | [0.963, 0.981] | SLOWER |
| micro/cache/lookup/production/d20/n1000000 | call | 21.78ms | 22.06ms | 0.989 | [0.945, 1.037] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 336.16ms | 341.18ms | 0.984 | [0.977, 0.992] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 18.46ms | 18.92ms | 0.972 | [0.968, 0.977] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 14.15ms | 14.16ms | 1.001 | [0.989, 1.013] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 53.34ms | 53.74ms | 0.992 | [0.989, 0.996] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 14.15ms | 14.14ms | 1.001 | [0.990, 1.013] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 35.78ms | 37.51ms | 0.953 | [0.942, 0.965] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c0 | call | 21.20ms | 20.65ms | 1.022 | [1.004, 1.040] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 114.17ms | 116.33ms | 0.983 | [0.979, 0.986] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c1000 | call | 21.02ms | 20.83ms | 1.010 | [0.988, 1.032] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 17.25ms | 17.20ms | 1.003 | [0.997, 1.009] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 10.12ms | 9.97ms | 1.015 | [1.004, 1.026] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 76.25ms | 76.71ms | 0.995 | [0.991, 0.998] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c1000 | call | 10.33ms | 10.90ms | 0.949 | [0.939, 0.960] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 34.84ms | 34.40ms | 1.015 | [1.006, 1.024] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 20.85ms | 20.78ms | 0.991 | [0.943, 1.041] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 121.46ms | 122.62ms | 0.990 | [0.986, 0.993] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 21.16ms | 21.21ms | 0.991 | [0.971, 1.012] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 20.62ms | 20.72ms | 0.996 | [0.992, 0.999] | SLOWER |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c0 | call | 9.32ms | 9.22ms | 1.006 | [0.995, 1.017] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 117.73ms | 118.51ms | 0.995 | [0.991, 0.998] | SLOWER |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c1000 | call | 9.23ms | 9.31ms | 0.983 | [0.961, 1.005] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 42.11ms | 41.92ms | 1.006 | [0.997, 1.015] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 39.79ms | 39.75ms | 1.000 | [0.992, 1.008] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 53.50ms | 53.61ms | 0.997 | [0.987, 1.007] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 39.68ms | 39.50ms | 1.005 | [0.996, 1.014] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 63.62ms | 62.75ms | 1.015 | [1.008, 1.023] | faster |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c0 | call | 37.16ms | 37.39ms | 0.991 | [0.985, 0.997] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 138.18ms | 137.59ms | 1.004 | [1.000, 1.008] | faster |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c1000 | call | 37.29ms | 37.52ms | 0.998 | [0.974, 1.024] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit1GiB | call | 2.558s | 2.631s | 0.999 | [0.927, 1.076] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit16MiB | call | 1.594s | 1.585s | 1.002 | [0.988, 1.016] | ~ (noise) |

Sum of medians (call, common cases): A = 6.178s, B = 6.253s, saved = -74719604.0ns (0.988x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.3%
Round durations (s): 96, 96, 95, 95, 96 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 478s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
