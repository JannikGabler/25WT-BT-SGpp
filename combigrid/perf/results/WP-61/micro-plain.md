A/B `micro`: A=`WP-61-before` B=`WP-61-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.92ms | 5.92ms | 1.002 | [0.996, 1.008] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.97ms | 2.97ms | 0.999 | [0.993, 1.006] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.00ms | 14.04ms | 0.994 | [0.982, 1.006] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 5.93ms | 5.93ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.97ms | 14.02ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 107.12ms | 106.93ms | 1.001 | [0.993, 1.009] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 44.42ms | 44.40ms | 0.994 | [0.969, 1.020] | ~ (noise) |
| micro/quad/operator/cc/b0/d2/L16 | call | 112.11ms | 112.08ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.54ms | 100.72ms | 0.991 | [0.969, 1.014] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.15ms | 44.12ms | 1.011 | [0.980, 1.042] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.80ms | 106.64ms | 1.003 | [0.998, 1.009] | ~ (noise) |
| micro/quad/ordered_combination/equi/b1/d15/L6 | call | 102.26us | 102.21us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/ordered_combination/equi/b1/d20/L5 | call | 99.74us | 99.88us | 0.998 | [0.991, 1.005] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d2/L16 | call | 156.63ms | 156.75ms | 0.999 | [0.995, 1.004] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d3/L8 | call | 3.40ms | 3.41ms | 1.000 | [0.994, 1.007] | ~ (noise) |
| micro/quad/operator/cc/b0/d3/L8 | call | 3.33ms | 3.31ms | 1.004 | [0.997, 1.010] | ~ (noise) |
| micro/quad/operator/cheb1/b1/d5/L6 | call | 4.77ms | 4.77ms | 1.000 | [0.987, 1.014] | ~ (noise) |
| micro/quad/operator/cc/b1/d5/L6 | call | 4.76ms | 4.77ms | 1.002 | [0.996, 1.008] | ~ (noise) |

Sum of medians (call, common cases): A = 731.02ms, B = 730.98ms, saved = 39.39us (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.3%, 0.2%, 0.2%
Round durations (s): 11, 11, 11, 11, 11 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 56s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
