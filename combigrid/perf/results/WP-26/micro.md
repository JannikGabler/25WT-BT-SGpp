A/B `micro`: A=`WP-26-before` B=`WP-26-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.32ms | 6.18ms | 1.046 | [0.983, 1.112] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.96ms | 3.00ms | 0.973 | [0.940, 1.008] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.97ms | 14.04ms | 0.992 | [0.981, 1.004] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.38ms | 25.14ms | 1.009 | [1.002, 1.016] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.93ms | 13.98ms | 0.996 | [0.990, 1.001] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 107.32ms | 108.32ms | 0.990 | [0.978, 1.003] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.27ms | 52.36ms | 1.076 | [0.873, 1.326] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.67ms | 7.69ms | 0.999 | [0.987, 1.011] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.35ms | 66.53ms | 1.003 | [0.982, 1.025] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.50us | 123.58us | 0.996 | [0.989, 1.003] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.05ms | 55.33ms | 0.997 | [0.989, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 91.00ms | 91.08ms | 1.000 | [0.993, 1.008] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 286.10ms | 285.34ms | 0.996 | [0.963, 1.031] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.13ms | 1.12ms | 1.009 | [0.996, 1.022] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 509.08us | 506.46us | 0.999 | [0.971, 1.027] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 47.19ms | 46.57ms | 1.019 | [1.005, 1.033] | faster |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.38ms | 1.37ms | 0.998 | [0.978, 1.018] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.94us | 118.12us | 1.008 | [1.000, 1.015] | faster |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.46us | 41.91us | 1.014 | [1.004, 1.024] | faster |
| micro/interplin/tg/equi/b1/d10/L5 | call | 245.65us | 231.23us | 1.062 | [1.015, 1.111] | faster |
| micro/interplin/tg/equi/b1/d2/L16 | call | 210.3ns | 208.1ns | 1.010 | [1.000, 1.021] | faster |

Sum of medians (call, common cases): A = 779.05ms, B = 779.09ms, saved = -35940.7ns (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 15, 15, 15, 15, 15 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 75s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
