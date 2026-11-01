A/B `micro`: A=`WP-26-after-r` B=`WP-26-final`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.18ms | 6.18ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.98ms | 3.06ms | 0.975 | [0.933, 1.018] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.88ms | 13.92ms | 0.996 | [0.984, 1.008] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.19ms | 25.22ms | 1.000 | [0.991, 1.008] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.90ms | 13.90ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.15ms | 109.09ms | 1.007 | [0.981, 1.035] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.57ms | 52.54ms | 1.000 | [0.993, 1.006] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 2.10us | 2.10us | 0.996 | [0.985, 1.007] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 51.05us | 50.94us | 1.004 | [0.996, 1.012] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 4.35ms | 4.36ms | 0.997 | [0.992, 1.003] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.67ms | 7.66ms | 0.994 | [0.970, 1.018] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.03ms | 66.05ms | 0.995 | [0.976, 1.014] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 122.88us | 122.78us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.15ms | 55.05ms | 1.011 | [0.979, 1.045] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.81ms | 91.23ms | 0.993 | [0.984, 1.002] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 282.94ms | 284.74ms | 0.988 | [0.967, 1.009] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.12ms | 0.999 | [0.986, 1.013] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 504.46us | 506.96us | 0.985 | [0.951, 1.021] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 46.67ms | 46.58ms | 1.004 | [0.994, 1.013] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.37ms | 1.36ms | 0.994 | [0.977, 1.012] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.11us | 118.49us | 0.989 | [0.974, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 41.88us | 41.97us | 1.001 | [0.989, 1.013] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 234.47us | 235.40us | 0.988 | [0.954, 1.024] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 208.1ns | 207.7ns | 0.994 | [0.971, 1.017] | ~ (noise) |

Sum of medians (call, common cases): A = 781.04ms, B = 783.13ms, saved = -2095295.5ns (0.997x)
Foreign CPU load per round (other processes, share of all CPUs): 0.7%, 0.6%, 0.3%, 0.2%, 0.3%
Round durations (s): 16, 16, 16, 16, 16 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 81s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
