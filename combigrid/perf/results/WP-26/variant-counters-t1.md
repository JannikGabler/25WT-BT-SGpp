A/B `micro`: A=`WP-26-after` B=`WP-26-v1`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/lookup/production/d2/n10000 | call | 510.47us | 506.55us | 1.005 | [0.987, 1.023] | ~ (noise) |
| micro/cache/insert/production/d2/n10000 | call | 1.04ms | 1.05ms | 0.992 | [0.987, 0.997] | SLOWER |
| micro/cache/lookup/production/d2/n1000000 | call | 7.87ms | 7.76ms | 1.020 | [0.892, 1.167] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 170.51ms | 161.94ms | 1.010 | [0.977, 1.044] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 762.18us | 757.63us | 1.005 | [1.002, 1.009] | faster |
| micro/cache/insert/production/d10/n10000 | call | 925.98us | 934.83us | 0.993 | [0.985, 1.002] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 16.74ms | 17.06ms | 0.959 | [0.886, 1.039] | ~ (noise) |
| micro/cache/insert/production/d10/n1000000 | call | 248.17ms | 249.56ms | 1.006 | [0.975, 1.038] | ~ (noise) |
| micro/cache/lookup/production/d20/n10000 | call | 1.15ms | 1.14ms | 1.003 | [0.993, 1.012] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 1.04ms | 1.04ms | 0.993 | [0.982, 1.004] | ~ (noise) |
| micro/cache/lookup/production/d20/n1000000 | call | 22.59ms | 21.67ms | 1.036 | [1.018, 1.054] | faster |
| micro/cache/insert/production/d20/n1000000 | call | 337.83ms | 336.04ms | 1.005 | [0.995, 1.015] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 18.53ms | 18.50ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 13.82ms | 13.79ms | 1.001 | [0.985, 1.016] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 53.43ms | 53.43ms | 1.000 | [0.996, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 13.86ms | 13.93ms | 0.995 | [0.987, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 34.75ms | 34.80ms | 0.998 | [0.985, 1.011] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 20.10ms | 20.16ms | 1.014 | [0.973, 1.057] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 121.21ms | 121.13ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 20.54ms | 20.08ms | 1.025 | [0.991, 1.059] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 42.28ms | 41.91ms | 1.007 | [0.998, 1.016] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 39.77ms | 39.60ms | 1.003 | [0.997, 1.010] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 53.64ms | 53.62ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 40.15ms | 39.82ms | 1.011 | [1.004, 1.019] | faster |

Sum of medians (call, common cases): A = 1.281s, B = 1.270s, saved = 10.99ms (1.009x)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 33, 33, 33, 33, 33 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 164s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
