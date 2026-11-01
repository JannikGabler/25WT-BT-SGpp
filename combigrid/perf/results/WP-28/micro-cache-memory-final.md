A/B `micro`: A=`WP-28-before` B=`WP-28-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/lookup/production/d2/n10000 | call | 511.35us | 520.56us | 0.981 | [0.963, 0.999] | SLOWER |
| micro/cache/insert/production/d2/n10000 | call | 1.05ms | 1.05ms | 1.003 | [0.980, 1.026] | ~ (noise) |
| micro/cache/lookup/production/d2/n1000000 | call | 7.82ms | 7.42ms | 1.026 | [0.945, 1.114] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 162.61ms | 167.42ms | 0.990 | [0.959, 1.021] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 770.86us | 765.35us | 1.007 | [0.997, 1.018] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 933.69us | 940.39us | 0.996 | [0.989, 1.002] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 17.07ms | 16.23ms | 1.063 | [0.983, 1.149] | ~ (noise) |
| micro/cache/insert/production/d10/n1000000 | call | 251.79ms | 247.32ms | 1.014 | [0.971, 1.059] | ~ (noise) |
| micro/cache/lookup/production/d20/n10000 | call | 1.15ms | 1.16ms | 0.993 | [0.980, 1.007] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 1.04ms | 1.04ms | 0.997 | [0.988, 1.006] | ~ (noise) |
| micro/cache/lookup/production/d20/n1000000 | call | 21.88ms | 22.36ms | 0.987 | [0.934, 1.043] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 333.82ms | 345.73ms | 0.977 | [0.958, 0.996] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 18.66ms | 18.87ms | 0.988 | [0.971, 1.006] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 14.20ms | 14.13ms | 1.013 | [0.975, 1.052] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 53.37ms | 53.71ms | 0.993 | [0.989, 0.998] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 14.17ms | 14.20ms | 0.996 | [0.988, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 35.74ms | 37.64ms | 0.956 | [0.945, 0.967] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c0 | call | 21.44ms | 20.64ms | 1.033 | [0.999, 1.069] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 114.15ms | 115.95ms | 0.983 | [0.979, 0.988] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c1000 | call | 20.93ms | 20.46ms | 1.036 | [1.003, 1.070] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 17.25ms | 17.13ms | 1.010 | [1.000, 1.020] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 10.12ms | 10.00ms | 1.012 | [1.003, 1.021] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 76.29ms | 76.38ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c1000 | call | 10.31ms | 10.87ms | 0.947 | [0.940, 0.955] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 35.06ms | 34.21ms | 1.016 | [0.994, 1.037] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 20.89ms | 20.98ms | 0.995 | [0.932, 1.063] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 121.55ms | 122.46ms | 0.993 | [0.989, 0.997] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 20.87ms | 21.20ms | 0.988 | [0.975, 1.002] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 20.79ms | 20.70ms | 1.006 | [0.983, 1.029] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c0 | call | 9.31ms | 9.30ms | 1.004 | [0.984, 1.024] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 117.78ms | 117.86ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c1000 | call | 9.32ms | 9.33ms | 1.003 | [0.987, 1.019] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 42.06ms | 41.99ms | 1.001 | [0.994, 1.009] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 39.62ms | 39.73ms | 0.997 | [0.988, 1.005] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 53.49ms | 53.65ms | 0.998 | [0.988, 1.007] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 39.85ms | 39.53ms | 1.007 | [1.003, 1.010] | faster |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 63.52ms | 62.58ms | 1.016 | [0.998, 1.035] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c0 | call | 37.07ms | 37.25ms | 0.993 | [0.935, 1.055] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 137.89ms | 137.71ms | 1.003 | [0.996, 1.009] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c1000 | call | 37.48ms | 38.72ms | 0.973 | [0.937, 1.011] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit1GiB | call | 2.556s | 2.534s | 1.025 | [0.988, 1.063] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit16MiB | call | 1.594s | 1.601s | 0.998 | [0.988, 1.009] | ~ (noise) |

Sum of medians (call, common cases): A = 6.164s, B = 6.164s, saved = -153157.4ns (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 1.4%, 0.5%, 0.3%, 0.3%, 0.2%
Round durations (s): 96, 95, 96, 95, 95 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 477s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
