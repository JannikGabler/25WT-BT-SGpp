A/B `micro`: A=`WP-52-before` B=`WP-52-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.99ms | 5.99ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.00ms | 2.98ms | 1.005 | [0.982, 1.028] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.98ms | 13.90ms | 1.006 | [0.994, 1.018] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 24.88ms | 24.96ms | 0.994 | [0.979, 1.008] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.03ms | 13.93ms | 1.007 | [1.003, 1.011] | faster |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 108.95ms | 109.13ms | 1.001 | [0.991, 1.012] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.89ms | 52.88ms | 0.989 | [0.967, 1.013] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.66ms | 7.64ms | 1.000 | [0.995, 1.006] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 66.06ms | 66.06ms | 1.001 | [0.992, 1.009] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.51us | 123.64us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 113.14ms | 112.23ms | 1.007 | [0.996, 1.018] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 20.01ms | 20.02ms | 1.006 | [0.996, 1.016] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.13ms | 9.13ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 56.06ms | 55.60ms | 1.017 | [0.983, 1.051] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.86ms | 91.83ms | 0.992 | [0.979, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 282.94ms | 282.92ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 222.19ms | 222.24ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 144.98ms | 133.48ms | 1.078 | [0.917, 1.267] | ~ (noise) |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 85.84ms | 85.89ms | 0.980 | [0.886, 1.084] | ~ (noise) |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 85.78ms | 85.92ms | 0.989 | [0.952, 1.027] | ~ (noise) |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 31.57ms | 31.81ms | 0.971 | [0.906, 1.040] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 911.37us | 910.38us | 1.003 | [0.990, 1.016] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 243.48us | 243.09us | 1.001 | [0.980, 1.023] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 39.38ms | 40.04ms | 1.026 | [0.910, 1.158] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.01ms | 1.01ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 69.45us | 69.65us | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 126.53ms | 128.62ms | 0.983 | [0.980, 0.987] | SLOWER |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.70ms | 61.59ms | 1.000 | [0.993, 1.007] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.30ms | 12.33ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 13.04ms | 13.03ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.52ms | 18.55ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.09ms | 35.04ms | 1.000 | [0.994, 1.006] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.87ms | 10.83ms | 1.007 | [0.995, 1.018] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.15ms | 3.14ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.30ms | 5.30ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 416.83us | 417.11us | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m4 | call | 24.82ms | 24.74ms | 1.002 | [0.997, 1.008] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m4 | call | 77.77ms | 77.55ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m4 | call | 310.84ms | 310.21ms | 1.007 | [0.990, 1.023] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b1/d20/L6/m4 | call | 547.62us | 547.13us | 1.005 | [0.996, 1.015] | ~ (noise) |
| micro/dsg/process_delta_tgs_vec/equi/b1/d20/L6/s256/m4 | call | 191.35ms | 191.49ms | 0.997 | [0.986, 1.008] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m4 | call | 3.74ms | 3.73ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m4 | call | 3.64ms | 3.63ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m4 | call | 941.27us | 941.99us | 1.101 | [0.839, 1.445] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m4 | call | 145.63ms | 145.60ms | 1.004 | [0.992, 1.016] | ~ (noise) |

Sum of medians (call, common cases): A = 2.529s, B = 2.519s, saved = 9.64ms (1.004x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.7%, 0.9%, 0.8%
Round durations (s): 44, 44, 44, 43, 43 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 218s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
