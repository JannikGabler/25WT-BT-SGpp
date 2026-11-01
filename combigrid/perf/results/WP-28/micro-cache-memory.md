A/B `micro`: A=`WP-28-before` B=`WP-28-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/lookup/production/d2/n10000 | call | 508.30us | 526.28us | 0.968 | [0.957, 0.979] | SLOWER |
| micro/cache/insert/production/d2/n10000 | call | 1.05ms | 1.08ms | 0.968 | [0.961, 0.974] | SLOWER |
| micro/cache/lookup/production/d2/n1000000 | call | 7.87ms | 7.74ms | 1.009 | [0.985, 1.033] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 171.82ms | 170.87ms | 1.010 | [0.976, 1.045] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 766.12us | 762.33us | 1.002 | [0.997, 1.006] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 935.13us | 965.17us | 0.974 | [0.961, 0.988] | SLOWER |
| micro/cache/lookup/production/d10/n1000000 | call | 16.59ms | 16.05ms | 1.035 | [0.985, 1.087] | ~ (noise) |
| micro/cache/insert/production/d10/n1000000 | call | 245.81ms | 254.34ms | 0.970 | [0.960, 0.980] | SLOWER |
| micro/cache/lookup/production/d20/n10000 | call | 1.15ms | 1.14ms | 0.978 | [0.900, 1.063] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 1.03ms | 1.06ms | 0.971 | [0.962, 0.981] | SLOWER |
| micro/cache/lookup/production/d20/n1000000 | call | 21.80ms | 22.11ms | 0.989 | [0.967, 1.012] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 335.03ms | 350.29ms | 0.959 | [0.945, 0.974] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 18.39ms | 19.32ms | 0.953 | [0.948, 0.958] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 14.14ms | 14.42ms | 0.980 | [0.969, 0.991] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 53.32ms | 53.92ms | 0.989 | [0.985, 0.992] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 14.03ms | 14.43ms | 0.975 | [0.965, 0.986] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 35.62ms | 37.41ms | 0.952 | [0.943, 0.961] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c0 | call | 21.31ms | 20.78ms | 1.016 | [0.979, 1.054] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 113.89ms | 116.39ms | 0.979 | [0.973, 0.986] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c1000 | call | 21.29ms | 20.83ms | 1.016 | [0.976, 1.057] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 17.19ms | 17.42ms | 0.987 | [0.978, 0.996] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 10.16ms | 10.15ms | 0.995 | [0.990, 1.000] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 76.05ms | 76.82ms | 0.988 | [0.982, 0.994] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c1000 | call | 10.29ms | 11.19ms | 0.925 | [0.920, 0.931] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 34.84ms | 34.55ms | 1.011 | [0.997, 1.024] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 20.97ms | 20.81ms | 1.006 | [0.977, 1.035] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 121.25ms | 123.04ms | 0.986 | [0.982, 0.989] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 20.93ms | 20.85ms | 1.011 | [0.998, 1.025] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 20.60ms | 21.12ms | 0.975 | [0.967, 0.983] | SLOWER |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c0 | call | 9.26ms | 9.34ms | 0.996 | [0.984, 1.008] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 117.62ms | 118.82ms | 0.990 | [0.986, 0.994] | SLOWER |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c1000 | call | 9.28ms | 9.37ms | 0.985 | [0.960, 1.010] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 42.41ms | 42.69ms | 0.991 | [0.974, 1.008] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 39.87ms | 40.54ms | 0.984 | [0.974, 0.994] | SLOWER |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 53.55ms | 54.54ms | 0.984 | [0.975, 0.992] | SLOWER |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 39.83ms | 40.44ms | 0.985 | [0.975, 0.995] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 63.91ms | 62.62ms | 1.026 | [1.010, 1.042] | faster |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c0 | call | 37.20ms | 37.44ms | 0.997 | [0.990, 1.004] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 137.87ms | 137.55ms | 1.004 | [0.993, 1.015] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c1000 | call | 37.22ms | 37.46ms | 0.995 | [0.982, 1.008] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit1GiB | call | 2.660s | 2.584s | 1.019 | [0.960, 1.081] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit16MiB | call | 1.588s | 1.602s | 0.990 | [0.983, 0.996] | SLOWER |

Sum of medians (call, common cases): A = 6.265s, B = 6.238s, saved = 26.76ms (1.004x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 96, 96, 96, 96, 96 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 479s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
