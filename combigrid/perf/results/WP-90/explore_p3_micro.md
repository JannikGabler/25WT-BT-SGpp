A/B `micro`: A=`WP-90-p5` B=`WP-90-p3`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 106.61ms | 106.52ms | 0.993 | [0.961, 1.026] | ~ (noise) |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 44.22ms | 43.92ms | 1.005 | [0.996, 1.013] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.77ms | 7.70ms | 1.002 | [0.981, 1.024] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 36.32us | 36.35us | 0.999 | [0.994, 1.005] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 177.81us | 178.03us | 1.001 | [0.992, 1.011] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 202.4ns | 200.9ns | 1.006 | [0.997, 1.014] | ~ (noise) |
| micro/interplin/tg/equi/b1/d5/L8 | call | 2.99us | 2.98us | 1.001 | [0.997, 1.006] | ~ (noise) |
| micro/interplin/tg/equi/b1/d16/L2 | call | 23.47ms | 23.65ms | 0.994 | [0.986, 1.002] | ~ (noise) |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 45.31ms | 45.47ms | 0.995 | [0.989, 1.001] | ~ (noise) |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 11.78ms | 11.14ms | 1.057 | [1.047, 1.067] | faster |
| micro/gen/sg/cc/b1/d20/L5 | call | 11.76ms | 11.13ms | 1.056 | [1.049, 1.063] | faster |
| micro/gen/sg/cc/b0/d2/L20 | call | 137.00ms | 90.01ms | 1.523 | [1.516, 1.529] | faster |
| micro/gen/sg/equi/b0/d5/L10 | call | 116.10us | 121.01us | 0.959 | [0.943, 0.974] | SLOWER |
| micro/gen/dsg/equi/b1/d20/L6 | call | 18.70ms | 15.81ms | 1.182 | [1.175, 1.189] | faster |
| micro/gen/dsg/equi/b1/d15/L7 | call | 10.90ms | 9.16ms | 1.190 | [1.177, 1.204] | faster |
| micro/gen/dsg/equi/b0/d3/L16 | call | 1.78ms | 928.00us | 1.944 | [1.880, 2.011] | faster |
| micro/gen/sg/equi/b1/d10/L7 | call | 2.40ms | 2.29ms | 1.045 | [1.036, 1.054] | faster |

Sum of medians (call, common cases): A = 422.03ms, B = 368.08ms, saved = 53.95ms (1.147x)
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.3%, 0.3%, 0.3%, 0.2%
Round durations (s): 10, 10, 10, 10, 10 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 49s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
