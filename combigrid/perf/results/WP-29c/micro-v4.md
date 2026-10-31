A/B `micro`: A=`WP-29c-before` B=`WP-29c-v4`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.13ms | 1.12ms | 1.000 | [0.987, 1.013] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 515.06us | 507.00us | 1.016 | [0.994, 1.037] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 47.07ms | 47.30ms | 0.993 | [0.985, 1.001] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.39ms | 1.39ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.73us | 118.37us | 1.002 | [0.998, 1.006] | ~ (noise) |

Sum of medians (call, common cases): A = 50.21ms, B = 50.43ms, saved = -220982.3ns (0.996x)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.6%, 0.3%, 0.3%, 0.3%
Round durations (s): 2, 2, 2, 2, 2 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 12s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
