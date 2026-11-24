A/B `micro`: A=`WP-48-before` B=`WP-48-after`, 16 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/operator/equi/b0/d3/L12 | call | 4.94ms | 4.82ms | 0.984 | [0.928, 1.045] | ~ (noise) |
| micro/dsg/operator/equi/b0/d5/L8 | call | 5.91ms | 7.94ms | 0.806 | [0.550, 1.181] | ~ (noise) |
| micro/dsg/operator/equi/b1/d15/L4 | call | 2.19ms | 2.21ms | 0.775 | [0.557, 1.079] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 13.58ms | 13.59ms | 0.995 | [0.921, 1.075] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 5.78ms | 5.79ms | 0.990 | [0.617, 1.591] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 1.29ms | 1.29ms | 0.863 | [0.428, 1.741] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 2.00ms | 2.67ms | 0.954 | [0.603, 1.510] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 187.91us | 2.06ms | 0.599 | [0.159, 2.255] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 2.51ms | 1.86ms | 1.401 | [0.639, 3.068] | ~ (noise) |
| micro/interp/sg/cc/b0/d1/L6 | call | 4.39ms | 2.21ms | 1.195 | [0.814, 1.753] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L4 | call | 2.18ms | 1.97ms | 1.008 | [0.878, 1.157] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L3 | call | 2.95ms | 432.59us | 1.762 | [0.572, 5.421] | ~ (noise) |
| micro/interp/sg_vec/cc/b0/d5/L5/m4 | call | 22.33ms | 22.35ms | 1.004 | [0.991, 1.017] | ~ (noise) |
| micro/interp/sg_vec/cc/b0/d2/L12/m4 | call | 8.02ms | 7.72ms | 1.088 | [0.988, 1.198] | ~ (noise) |
| micro/interplin/sg/equi/b1/d10/L5 | call | 59.37ms | 58.75ms | 1.021 | [1.003, 1.038] | faster |
| micro/interplin/sg/equi/b1/d8/L5 | call | 6.00ms | 5.79ms | 1.027 | [0.850, 1.241] | ~ (noise) |
| micro/interplin/sg/equi/b1/d5/L8 | call | 2.29ms | 2.12ms | 1.106 | [0.904, 1.353] | ~ (noise) |
| micro/interplin/sg/equi/b1/d2/L16 | call | 979.70us | 1.07ms | 1.088 | [0.779, 1.520] | ~ (noise) |
| micro/interplin/sg/equi/b1/d1/L10 | call | 2.32ms | 1.75ms | 1.242 | [1.089, 1.416] | faster |
| micro/interplin/sg/equi/b1/d2/L8 | call | 1.99ms | 2.25ms | 0.976 | [0.857, 1.112] | ~ (noise) |
| micro/interplin/sg/equi/b1/d3/L6 | call | 2.05ms | 2.02ms | 0.962 | [0.800, 1.156] | ~ (noise) |
| micro/interplin/sg/equi/b1/d3/L10 | call | 2.25ms | 1.99ms | 1.126 | [0.975, 1.301] | ~ (noise) |
| micro/interplin/sg/equi/b1/d4/L6 | call | 2.01ms | 267.92us | 1.465 | [0.463, 4.640] | ~ (noise) |
| micro/interplin/sg_vec/equi/b1/d8/L5/m4 | call | 18.75ms | 18.57ms | 0.997 | [0.976, 1.018] | ~ (noise) |
| micro/interplin/sg_vec/equi/b1/d2/L16/m4 | call | 223.31us | 248.66us | 0.754 | [0.228, 2.499] | ~ (noise) |

Sum of medians (call, common cases): A = 176.50ms, B = 171.74ms, saved = 4.76ms (1.028x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.1%, 0.3%, 0.3%
Round durations (s): 9, 9, 9, 10, 9 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 46s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
