A/B `micro`: A=`WP-04-before` B=`WP-04-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.91ms | 14.04ms | 0.987 | [0.975, 0.999] | SLOWER |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.64ms | 7.01ms | 1.090 | [1.086, 1.094] | faster |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.20ms | 62.22ms | 1.072 | [1.057, 1.087] | faster |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.67us | 116.42us | 1.067 | [1.051, 1.083] | faster |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 114.10ms | 108.06ms | 1.058 | [1.048, 1.067] | faster |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.91ms | 19.83ms | 1.001 | [0.986, 1.017] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.27ms | 9.27ms | 1.019 | [0.956, 1.086] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.01ms | 52.88ms | 1.040 | [1.027, 1.052] | faster |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.97ms | 85.42ms | 1.064 | [1.050, 1.078] | faster |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 282.34ms | 261.95ms | 1.078 | [1.074, 1.082] | faster |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 222.98ms | 202.37ms | 1.100 | [1.092, 1.109] | faster |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 191.55ms | 92.86ms | 1.797 | [1.426, 2.265] | faster |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 86.10ms | 66.31ms | 1.321 | [1.245, 1.402] | faster |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 85.87ms | 66.24ms | 1.295 | [1.291, 1.300] | faster |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 30.89ms | 29.91ms | 1.033 | [1.028, 1.039] | faster |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b0/d10/L7/m1 | call | 145.25ms | 135.34ms | 1.071 | [1.065, 1.077] | faster |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m1 | call | 152.19ms | 144.95ms | 1.050 | [1.046, 1.054] | faster |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b0/d10/L7/m4 | call | 581.78ms | 541.28ms | 1.052 | [0.987, 1.121] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m4 | call | 308.72ms | 300.63ms | 1.027 | [1.023, 1.031] | faster |

Sum of medians (call, common cases): A = 2.465s, B = 2.201s, saved = 264.12ms (1.120x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.3%, 0.3%, 0.2%
Round durations (s): 29, 29, 29, 29, 29 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 147s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
