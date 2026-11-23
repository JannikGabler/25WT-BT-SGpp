A/B `micro`: A=`WP-61-before` B=`WP-61-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.93ms | 5.93ms | 0.997 | [0.986, 1.008] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.97ms | 2.97ms | 1.001 | [0.997, 1.006] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.03ms | 14.06ms | 0.998 | [0.986, 1.009] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 5.93ms | 5.95ms | 0.995 | [0.978, 1.012] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.01ms | 14.04ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 107.59ms | 107.19ms | 1.007 | [0.992, 1.023] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 44.53ms | 44.44ms | 1.018 | [0.966, 1.073] | ~ (noise) |
| micro/quad/operator/cc/b0/d2/L16 | call | 112.32ms | 112.43ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/quad/operator/equi/b0/d2/L16 | call | 100.86ms | 100.85ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/operator/cc/b1/d10/L5 | call | 44.31ms | 44.19ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/quad/operator/equi/b1/d20/L4 | call | 106.99ms | 107.00ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d2/L16 | call | 157.13ms | 156.84ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/quad/operator/cheb1/b0/d3/L8 | call | 3.42ms | 3.42ms | 0.999 | [0.993, 1.006] | ~ (noise) |
| micro/quad/operator/cc/b0/d3/L8 | call | 3.32ms | 3.33ms | 0.997 | [0.991, 1.003] | ~ (noise) |
| micro/quad/operator/cheb1/b1/d5/L6 | call | 4.79ms | 4.80ms | 0.998 | [0.987, 1.009] | ~ (noise) |
| micro/quad/operator/cc/b1/d5/L6 | call | 4.76ms | 4.76ms | 0.999 | [0.990, 1.007] | ~ (noise) |

Sum of medians (call, common cases): A = 732.90ms, B = 732.20ms, saved = 698.54us (1.001x)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.3%, 0.3%, 0.1%, 0.2%
Round durations (s): 10, 10, 10, 10, 10 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 52s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
