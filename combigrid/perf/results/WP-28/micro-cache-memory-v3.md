A/B `micro`: A=`WP-28-before` B=`WP-28-v3`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/insert/production/d2/n10000 | call | 1.05ms | 1.05ms | 0.996 | [0.990, 1.002] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 171.62ms | 170.31ms | 1.014 | [0.981, 1.047] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 930.62us | 933.12us | 0.997 | [0.988, 1.006] | ~ (noise) |
| micro/cache/insert/production/d10/n1000000 | call | 252.68ms | 253.62ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 1.04ms | 1.04ms | 1.000 | [0.988, 1.013] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 335.85ms | 347.13ms | 0.975 | [0.961, 0.989] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 18.53ms | 19.00ms | 0.980 | [0.966, 0.995] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 53.30ms | 53.66ms | 0.994 | [0.990, 0.997] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 36.02ms | 37.52ms | 0.961 | [0.943, 0.978] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 113.92ms | 116.18ms | 0.981 | [0.977, 0.985] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 17.18ms | 17.17ms | 1.000 | [0.985, 1.016] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 76.21ms | 76.32ms | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 35.06ms | 34.34ms | 1.020 | [1.016, 1.024] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 122.25ms | 122.57ms | 0.999 | [0.978, 1.020] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 20.64ms | 20.89ms | 0.989 | [0.980, 0.997] | SLOWER |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 117.76ms | 118.14ms | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 42.03ms | 42.10ms | 0.998 | [0.991, 1.004] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 53.68ms | 53.95ms | 0.994 | [0.989, 0.999] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 64.09ms | 62.49ms | 1.028 | [1.012, 1.045] | faster |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 138.36ms | 137.82ms | 1.005 | [0.997, 1.014] | ~ (noise) |

Sum of medians (call, common cases): A = 1.672s, B = 1.686s, saved = -14039674.1ns (0.992x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.4%
Round durations (s): 29, 29, 29, 29, 29 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 144s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
