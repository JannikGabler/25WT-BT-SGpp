A/B `micro`: A=`WP-02-before` B=`WP-02-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.90ms | 7.62ms | 1.037 | [1.033, 1.041] | faster |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 69.00ms | 66.04ms | 1.044 | [1.035, 1.053] | faster |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 2.70ms | 123.89us | 21.438 | [20.454, 22.468] | faster |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 332.75ms | 113.08ms | 2.930 | [2.889, 2.971] | faster |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 167.83ms | 19.93ms | 8.411 | [8.334, 8.490] | faster |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 22.93ms | 9.10ms | 2.514 | [2.491, 2.536] | faster |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 256.03ms | 55.05ms | 4.643 | [4.497, 4.794] | faster |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 277.65ms | 90.90ms | 3.057 | [3.037, 3.077] | faster |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 297.43ms | 284.01ms | 1.047 | [1.043, 1.051] | faster |

Sum of medians (call, common cases): A = 1.434s, B = 645.85ms, saved = 788.36ms (2.221x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.3%, 0.2%, 0.3%, 0.2%
Round durations (s): 15, 15, 15, 15, 15 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 75s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
