A/B `micro`: A=`WP-83-after` B=`WP-83-v2`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/operator/cc/b0/d2/L16 | call | 112.25ms | 111.99ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.63ms | 100.75ms | 0.994 | [0.980, 1.009] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.13ms | 44.12ms | 0.997 | [0.985, 1.009] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.85ms | 107.09ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 959.2ns | 967.0ns | 0.990 | [0.984, 0.996] | SLOWER |
| micro/quadrule/cc_weights/n1025 | call | 27.79us | 27.72us | 1.002 | [0.989, 1.014] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 1.40ms | 1.40ms | 0.998 | [0.991, 1.005] | ~ (noise) |
| micro/quadrule/cc_weights/n131073 | call | 6.28ms | 6.26ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| micro/quadrule/cc_weights/n1048577 | call | 68.75ms | 68.58ms | 1.004 | [0.999, 1.008] | ~ (noise) |
| micro/quadrule/cheb1_weights/n33 | call | 4.99us | 3.94us | 1.264 | [1.255, 1.273] | faster |
| micro/quadrule/fejer1_weights/n33 | call | 6.27us | 4.66us | 1.277 | [1.159, 1.407] | faster |
| micro/quadrule/cheb1_weights/n1025 | call | 134.43us | 114.43us | 1.173 | [1.167, 1.179] | faster |
| micro/quadrule/fejer1_weights/n1025 | call | 247.64us | 207.48us | 1.193 | [1.185, 1.200] | faster |
| micro/quadrule/cheb1_weights/n32769 | call | 7.92ms | 7.29ms | 1.087 | [1.083, 1.091] | faster |
| micro/quadrule/fejer1_weights/n32769 | call | 13.01ms | 11.69ms | 1.112 | [1.105, 1.118] | faster |
| micro/quadrule/cheb1_weights/n131073 | call | 32.45ms | 30.20ms | 1.080 | [1.075, 1.085] | faster |
| micro/quadrule/fejer1_weights/n131073 | call | 62.09ms | 54.33ms | 1.142 | [1.130, 1.154] | faster |
| micro/quadrule/cheb1_weights/n1048577 | call | 360.94ms | 333.19ms | 1.090 | [1.073, 1.107] | faster |
| micro/quadrule/fejer1_weights/n1048577 | call | 817.77ms | 712.00ms | 1.153 | [1.133, 1.173] | faster |
| micro/quadrule/fejer1_weights/n131071 | call | 30.92ms | 28.53ms | 1.084 | [1.080, 1.088] | faster |
| micro/quad/operator/cheb1/b0/d2/L16 | call | 161.68ms | 157.56ms | 1.027 | [1.013, 1.041] | faster |
| micro/quad/operator/cheb1/b0/d3/L8 | call | 3.43ms | 3.42ms | 1.005 | [1.001, 1.008] | faster |
| micro/quad/operator/cc/b0/d3/L8 | call | 3.31ms | 3.32ms | 0.998 | [0.992, 1.004] | ~ (noise) |
| micro/quad/operator/cheb1/b1/d5/L6 | call | 4.81ms | 4.80ms | 1.006 | [0.975, 1.039] | ~ (noise) |
| micro/quad/operator/cc/b1/d5/L6 | call | 4.76ms | 4.76ms | 1.001 | [0.987, 1.014] | ~ (noise) |

Sum of medians (call, common cases): A = 1.944s, B = 1.792s, saved = 152.17ms (1.085x)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.2%, 0.4%, 0.2%, 0.3%
Round durations (s): 22, 22, 22, 22, 22 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 110s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
