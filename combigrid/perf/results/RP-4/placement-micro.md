A/B `micro`: A=`rp-4` B=`rp-4-sor-a64`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.02ms | 5.98ms | 1.007 | [0.986, 1.028] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.98ms | 2.98ms | 0.997 | [0.979, 1.015] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.89ms | 14.05ms | 0.987 | [0.975, 0.999] | SLOWER |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 24.92ms | 24.74ms | 0.984 | [0.921, 1.051] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.93ms | 14.05ms | 0.993 | [0.987, 0.999] | SLOWER |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.37ms | 108.93ms | 1.004 | [0.998, 1.009] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.64ms | 52.59ms | 1.004 | [0.991, 1.017] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 908.87us | 909.88us | 1.007 | [0.987, 1.027] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 243.50us | 243.22us | 1.001 | [0.980, 1.023] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 40.01ms | 39.96ms | 1.000 | [0.995, 1.006] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.02ms | 1.02ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 69.66us | 69.48us | 1.002 | [0.979, 1.025] | ~ (noise) |
| micro/interp/sg/cc/b0/d8/L2 | call | 128.49ms | 128.85ms | 0.995 | [0.985, 1.005] | ~ (noise) |
| micro/interp/sg/cc/b0/d5/L5 | call | 61.66ms | 61.72ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interp/sg/cc/b0/d3/L8 | call | 12.33ms | 12.31ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| micro/interp/sg/cc/b0/d2/L12 | call | 13.04ms | 13.05ms | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/interp/sg/equi/b0/d3/L5 | call | 1.11ms | 1.11ms | 1.000 | [0.993, 1.008] | ~ (noise) |
| micro/interp/sg/cheb1/b0/d5/L4 | call | 18.54ms | 18.54ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/sg_vec/cc/b0/d5/L5/m4 | call | 245.39ms | 245.53ms | 1.000 | [0.993, 1.007] | ~ (noise) |
| micro/interp/sg_vec/cc/b0/d2/L12/m4 | call | 52.71ms | 52.70ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m1 | call | 37.79ms | 37.77ms | 1.019 | [0.933, 1.112] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m4 | call | 77.58ms | 77.79ms | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m1 | call | 963.89us | 962.32us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m1 | call | 42.54ms | 42.19ms | 1.009 | [0.999, 1.019] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m4 | call | 3.64ms | 3.64ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m4 | call | 145.44ms | 145.35ms | 1.003 | [0.992, 1.013] | ~ (noise) |

Sum of medians (call, common cases): A = 1.107s, B = 1.107s, saved = 188.36us (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.3%, 0.2%, 0.3%, 0.2%
Round durations (s): 17, 17, 17, 17, 17 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 86s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
