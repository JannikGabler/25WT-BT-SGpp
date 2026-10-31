A/B `micro`: A=`WP-26-after` B=`WP-26-v1`, 16 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/lookup/production/d2/n10000 | call | 133.29us | 102.94us | 1.297 | [1.270, 1.324] | faster |
| micro/cache/insert/production/d2/n10000 | call | 259.40us | 264.56us | 0.964 | [0.924, 1.005] | ~ (noise) |
| micro/cache/lookup/production/d2/n1000000 | call | 1.26ms | 1.05ms | 1.192 | [1.094, 1.298] | faster |
| micro/cache/insert/production/d2/n1000000 | call | 40.41ms | 40.78ms | 0.997 | [0.978, 1.015] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 143.69us | 115.81us | 1.666 | [0.504, 5.506] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 284.16us | 275.73us | 1.856 | [0.483, 7.131] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 1.70ms | 1.42ms | 1.201 | [1.136, 1.269] | faster |
| micro/cache/insert/production/d10/n1000000 | call | 91.51ms | 92.53ms | 0.991 | [0.971, 1.012] | ~ (noise) |
| micro/cache/lookup/production/d20/n10000 | call | 165.86us | 136.34us | 0.725 | [0.296, 1.774] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 288.73us | 292.12us | 0.987 | [0.962, 1.013] | ~ (noise) |
| micro/cache/lookup/production/d20/n1000000 | call | 2.20ms | 1.93ms | 1.122 | [1.062, 1.185] | faster |
| micro/cache/insert/production/d20/n1000000 | call | 131.46ms | 129.60ms | 1.014 | [0.976, 1.054] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 4.50ms | 4.24ms | 1.069 | [1.037, 1.102] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 3.07ms | 2.54ms | 1.221 | [1.185, 1.259] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 7.01ms | 6.78ms | 1.037 | [1.013, 1.061] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 3.08ms | 2.54ms | 1.210 | [1.179, 1.242] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 8.30ms | 8.20ms | 1.017 | [0.984, 1.050] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 3.67ms | 2.97ms | 1.225 | [1.194, 1.257] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 12.94ms | 12.80ms | 1.012 | [0.975, 1.050] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 3.71ms | 3.01ms | 1.243 | [1.164, 1.327] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 7.89ms | 6.81ms | 1.156 | [1.102, 1.213] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 7.05ms | 5.95ms | 1.184 | [1.168, 1.201] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 9.00ms | 7.88ms | 1.138 | [1.118, 1.158] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 7.15ms | 6.01ms | 1.168 | [1.137, 1.201] | faster |

Sum of medians (call, common cases): A = 347.18ms, B = 338.22ms, saved = 8.95ms (1.026x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.1%, 0.1%, 0.1%
Round durations (s): 24, 24, 24, 24, 25 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 122s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
