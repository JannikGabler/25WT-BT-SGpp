A/B `micro`: A=`WP-06-before` B=`WP-06-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 5.99ms | 6.03ms | 0.948 | [0.849, 1.059] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 3.03ms | 3.00ms | 1.003 | [0.980, 1.027] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 14.05ms | 14.01ms | 1.003 | [0.991, 1.015] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.11ms | 24.91ms | 1.008 | [1.001, 1.016] | faster |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 14.04ms | 14.03ms | 1.003 | [0.997, 1.009] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.30ms | 108.95ms | 1.009 | [0.988, 1.031] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.51ms | 52.77ms | 1.011 | [0.969, 1.055] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 911.68us | 1.229 | [1.213, 1.245] | faster |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 515.81us | 243.94us | 2.124 | [2.079, 2.169] | faster |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 46.67ms | 39.27ms | 1.215 | [1.140, 1.295] | faster |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.36ms | 1.01ms | 1.341 | [1.330, 1.352] | faster |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 121.14us | 69.67us | 1.734 | [1.711, 1.757] | faster |
| micro/interp/sg/cc/b0/d8/L2 | call | 155.21ms | 126.65ms | 1.225 | [1.204, 1.246] | faster |
| micro/interp/sg/cc/b0/d5/L5 | call | 74.01ms | 61.65ms | 1.201 | [1.196, 1.205] | faster |
| micro/interp/sg/cc/b0/d3/L8 | call | 14.75ms | 12.33ms | 1.197 | [1.192, 1.201] | faster |
| micro/interp/sg/cc/b0/d2/L12 | call | 16.63ms | 13.05ms | 1.300 | [1.227, 1.376] | faster |
| micro/interp/sg/equi/b0/d3/L5 | call | 3.26ms | 1.11ms | 2.943 | [2.918, 2.968] | faster |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 41.48ms | 18.55ms | 2.237 | [2.228, 2.245] | faster |
| micro/interp/dsg/cc/b0/d8/L2 | call | 35.04ms | 35.10ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/interp/dsg/cc/b0/d5/L5 | call | 10.87ms | 10.86ms | 1.004 | [0.995, 1.013] | ~ (noise) |
| micro/interp/dsg/cc/b0/d3/L8 | call | 3.14ms | 3.15ms | 0.998 | [0.994, 1.001] | ~ (noise) |
| micro/interp/dsg/cc/b0/d2/L12 | call | 5.30ms | 5.30ms | 1.004 | [0.995, 1.014] | ~ (noise) |
| micro/interp/dsg/equi/b0/d3/L5 | call | 416.77us | 418.25us | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/interp/sg_vec/cc/b0/d5/L5/m4 | call | 261.67ms | 245.86ms | 1.064 | [1.060, 1.069] | faster |
| micro/interp/sg_vec/cc/b0/d2/L12/m4 | call | 55.31ms | 52.66ms | 1.051 | [1.045, 1.056] | faster |
| micro/sol/interp_tg/cc/b0/d10/L2 | call | 41.18ms | 41.35ms | 0.993 | [0.982, 1.004] | ~ (noise) |
| micro/sol/interp_tg/cc/b0/d5/L6 | call | 995.59us | 999.10us | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/sol/interp_tg/cheb1/b0/d5/L4 | call | 488.87us | 480.27us | 1.018 | [1.008, 1.029] | faster |
| micro/sol/interp_tg/cc/b0/d2/L12 | call | 1.05ms | 1.05ms | 1.003 | [0.994, 1.012] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d10/L2 | call | 38.81ms | 38.88ms | 1.001 | [0.987, 1.015] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d5/L6 | call | 888.11us | 887.19us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/sol/interp_tg_integrand/cheb1/b0/d5/L4 | call | 227.25us | 227.20us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/sol/interp_tg_integrand/cc/b0/d2/L12 | call | 853.71us | 854.93us | 0.994 | [0.983, 1.006] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m1 | call | 9.60ms | 9.57ms | 0.994 | [0.967, 1.022] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m1 | call | 37.42ms | 37.76ms | 0.991 | [0.987, 0.995] | SLOWER |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m1 | call | 271.15ms | 272.84ms | 0.994 | [0.987, 1.000] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m4 | call | 24.82ms | 24.72ms | 1.012 | [0.991, 1.033] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m4 | call | 77.80ms | 77.75ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m4 | call | 594.34ms | 594.89ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d2/L12/m1 | call | 1.39ms | 1.01ms | 1.370 | [1.365, 1.375] | faster |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m1 | call | 1.69ms | 1.12ms | 1.498 | [1.484, 1.513] | faster |
| micro/interp/global_tg_scalar_x_m/cc/b0/d5/L6/m1 | call | 1.12ms | 908.66us | 1.230 | [1.226, 1.235] | faster |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m1 | call | 1.27ms | 963.03us | 1.322 | [1.300, 1.345] | faster |
| micro/interp/global_tg_scalar_x_m/cheb1/b0/d5/L4/m1 | call | 516.71us | 243.04us | 2.130 | [2.120, 2.141] | faster |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m1 | call | 522.18us | 257.02us | 2.045 | [2.008, 2.082] | faster |
| micro/interp/global_tg_scalar_x_m/cc/b0/d10/L2/m1 | call | 48.29ms | 40.84ms | 1.190 | [1.156, 1.225] | faster |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m1 | call | 50.10ms | 42.24ms | 1.186 | [1.180, 1.192] | faster |
| micro/interp/global_tg_scalar_x_m/cc/b0/d2/L12/m4 | call | 5.54ms | 4.06ms | 1.377 | [1.342, 1.413] | faster |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m4 | call | 4.49ms | 3.73ms | 1.205 | [1.199, 1.210] | faster |
| micro/interp/global_tg_scalar_x_m/cc/b0/d5/L6/m4 | call | 4.48ms | 3.64ms | 1.229 | [1.224, 1.235] | faster |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m4 | call | 3.92ms | 3.64ms | 1.079 | [1.075, 1.083] | faster |
| micro/interp/global_tg_scalar_x_m/cheb1/b0/d5/L4/m4 | call | 2.06ms | 971.67us | 2.122 | [2.114, 2.130] | faster |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m4 | call | 1.21ms | 941.53us | 1.284 | [1.269, 1.300] | faster |
| micro/interp/global_tg_scalar_x_m/cc/b0/d10/L2/m4 | call | 193.37ms | 162.56ms | 1.189 | [1.184, 1.193] | faster |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m4 | call | 154.27ms | 145.30ms | 1.062 | [1.058, 1.066] | faster |

Sum of medians (call, common cases): A = 2.525s, B = 2.367s, saved = 158.17ms (1.067x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 43, 43, 42, 43, 43 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 213s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
