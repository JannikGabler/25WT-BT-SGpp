A/B `micro`: A=`WP-25-before` B=`WP-25-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.16ms | 6.12ms | 1.004 | [1.000, 1.008] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.01ms | 3.05ms | 0.987 | [0.981, 0.994] | SLOWER |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.94ms | 14.18ms | 0.988 | [0.945, 1.033] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 24.98ms | 24.88ms | 1.000 | [0.979, 1.020] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.98ms | 14.27ms | 0.978 | [0.969, 0.988] | SLOWER |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 110.80ms | 112.49ms | 0.985 | [0.980, 0.990] | SLOWER |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.37ms | 54.08ms | 0.970 | [0.959, 0.981] | SLOWER |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 38.13us | 38.55us | 0.974 | [0.934, 1.016] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 56.02ms | 56.32ms | 0.993 | [0.982, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 91.08ms | 93.31ms | 0.976 | [0.967, 0.984] | SLOWER |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 285.39ms | 292.14ms | 0.978 | [0.971, 0.985] | SLOWER |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.13ms | 1.13ms | 0.998 | [0.985, 1.011] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 517.93us | 514.18us | 1.005 | [0.984, 1.026] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.72us | 42.68us | 0.998 | [0.993, 1.003] | ~ (noise) |

Sum of medians (call, common cases): A = 659.46ms, B = 672.58ms, saved = -13120145.6ns (0.980x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 11, 11, 11, 11, 11 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 53s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
