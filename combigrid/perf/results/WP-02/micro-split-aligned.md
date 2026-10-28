A/B `micro`: A=`WP-02-after` B=`WP-02-split-final`, 1 thread(s), 8 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.63ms | 7.65ms | 0.990 | [0.934, 1.050] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.18ms | 66.16ms | 1.001 | [0.995, 1.006] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.80us | 123.67us | 0.999 | [0.995, 1.004] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 113.62ms | 113.15ms | 1.006 | [1.000, 1.012] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.75ms | 19.92ms | 0.977 | [0.920, 1.037] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.08ms | 9.15ms | 0.993 | [0.989, 0.997] | SLOWER |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.67ms | 55.27ms | 1.010 | [0.998, 1.023] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 91.08ms | 91.00ms | 1.003 | [0.998, 1.008] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 284.42ms | 283.39ms | 1.008 | [0.995, 1.021] | ~ (noise) |

Sum of medians (call, common cases): A = 647.55ms, B = 645.82ms, saved = 1.73ms (1.003x)
Foreign CPU load per round (other processes, share of all CPUs): 0.6%, 0.5%, 0.3%, 0.3%, 0.3%, 0.6%, 0.4%, 0.3%
Round durations (s): 11, 11, 11, 11, 11, 11, 11, 11 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 90s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
