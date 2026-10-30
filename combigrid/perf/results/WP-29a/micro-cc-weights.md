A/B `micro`: A=`WP-29a-before` B=`WP-29a-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quadrule/cc_weights/n33 | call | 2.09us | 2.10us | 0.995 | [0.989, 1.001] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 51.54us | 51.80us | 0.993 | [0.986, 1.000] | SLOWER |
| micro/quadrule/cc_weights/n32769 | call | 4.42ms | 4.42ms | 0.999 | [0.993, 1.005] | ~ (noise) |

Sum of medians (call, common cases): A = 4.47ms, B = 4.48ms, saved = -6690.3ns (0.999x)
Foreign CPU load per round (other processes, share of all CPUs): 0.8%, 0.5%, 0.2%, 0.3%, 0.5%
Round durations (s): 1, 1, 1, 1, 1 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 6s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
