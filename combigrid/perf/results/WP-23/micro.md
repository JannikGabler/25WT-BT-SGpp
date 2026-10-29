A/B `micro`: A=`WP-23-before` B=`WP-23-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.30ms | 6.32ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.98ms | 2.97ms | 1.002 | [0.998, 1.007] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.85ms | 13.90ms | 0.998 | [0.986, 1.010] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.38ms | 26.09ms | 0.972 | [0.959, 0.987] | SLOWER |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.83ms | 13.88ms | 0.998 | [0.994, 1.003] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.46ms | 109.09ms | 1.004 | [0.997, 1.012] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.22ms | 52.20ms | 1.000 | [0.996, 1.005] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 8.38ms | 8.94ms | 0.937 | [0.928, 0.947] | SLOWER |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 37.54us | 37.50us | 0.999 | [0.982, 1.017] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 2.07us | 2.08us | 1.006 | [0.951, 1.065] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 50.90us | 51.33us | 0.967 | [0.902, 1.038] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 4.35ms | 4.48ms | 0.973 | [0.960, 0.986] | SLOWER |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.73ms | 7.73ms | 0.997 | [0.989, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.69ms | 66.36ms | 1.002 | [0.991, 1.013] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.53us | 123.42us | 0.999 | [0.992, 1.007] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 113.73ms | 113.14ms | 1.004 | [0.996, 1.013] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 54.93ms | 54.82ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.89ms | 91.34ms | 1.001 | [0.971, 1.031] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 282.88ms | 283.19ms | 0.986 | [0.933, 1.043] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.13ms | 1.12ms | 0.999 | [0.986, 1.013] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 516.57us | 507.14us | 1.092 | [0.896, 1.330] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.44us | 42.39us | 1.000 | [0.994, 1.005] | ~ (noise) |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 91.96ms | 90.85ms | 1.013 | [1.003, 1.022] | faster |

Sum of medians (call, common cases): A = 947.45ms, B = 947.19ms, saved = 263.78us (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.3%, 0.3%
Round durations (s): 17, 17, 17, 17, 17 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 86s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
