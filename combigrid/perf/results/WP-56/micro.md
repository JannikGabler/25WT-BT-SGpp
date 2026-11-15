A/B `micro`: A=`WP-56-before` B=`WP-56-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/cc/b0/d2/L16 | call | 5.91ms | 5.91ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/quad/operator/cc/b0/d2/L16 | call | 122.38ms | 112.13ms | 1.100 | [1.079, 1.121] | faster |
| micro/quad/operator/equi/b0/d2/L16 | call | 101.70ms | 100.57ms | 1.011 | [1.007, 1.015] | faster |
| micro/quadrule/cc_weights/n33 | call | 1.93us | 955.6ns | 2.023 | [2.010, 2.035] | faster |
| micro/quadrule/cc_weights/n1025 | call | 49.12us | 27.72us | 1.770 | [1.761, 1.779] | faster |
| micro/quadrule/cc_weights/n32769 | call | 3.18ms | 1.41ms | 2.262 | [2.254, 2.271] | faster |
| micro/quadrule/cc_weights/n131073 | call | 13.90ms | 6.27ms | 2.220 | [2.202, 2.238] | faster |
| micro/quadrule/cc_weights/n1048577 | call | 145.35ms | 69.67ms | 2.089 | [2.053, 2.126] | faster |

Sum of medians (call, common cases): A = 392.48ms, B = 295.99ms, saved = 96.49ms (1.326x)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.5%, 0.3%, 0.4%, 0.2%
Round durations (s): 5, 5, 5, 5, 5 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 25s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
