A/B `micro`: A=`WP-18-before` B=`WP-18-after`, 1 thread(s), 8 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.37ms | 54.89ms | 1.012 | [1.001, 1.022] | faster |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.80ms | 90.90ms | 0.997 | [0.992, 1.002] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 283.04ms | 287.56ms | 0.982 | [0.974, 0.989] | SLOWER |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 222.57ms | 226.14ms | 0.985 | [0.982, 0.987] | SLOWER |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 182.63ms | 176.85ms | 0.990 | [0.831, 1.181] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 85.76ms | 109.31ms | 0.790 | [0.680, 0.919] | SLOWER |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 85.78ms | 109.31ms | 0.852 | [0.784, 0.927] | SLOWER |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 31.26ms | 31.14ms | 1.003 | [0.990, 1.016] | ~ (noise) |

Sum of medians (call, common cases): A = 1.037s, B = 1.086s, saved = -48900271.2ns (0.955x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 11, 10, 11, 11, 11, 11, 11, 11 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 86s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
