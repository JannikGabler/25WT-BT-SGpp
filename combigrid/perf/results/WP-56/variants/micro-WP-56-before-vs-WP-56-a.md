A/B `micro`: A=`WP-56-before` B=`WP-56-a`, 1 thread(s), 3 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quadrule/cc_weights/n33 | call | 1.92us | 964.4ns | 1.996 | [1.972, 2.020] | faster |
| micro/quadrule/cc_weights/n1025 | call | 49.16us | 27.76us | 1.768 | [1.742, 1.794] | faster |
| micro/quadrule/cc_weights/n32769 | call | 3.18ms | 1.40ms | 2.262 | [2.231, 2.292] | faster |
| micro/quadrule/cc_weights/n131073 | call | 13.82ms | 6.27ms | 2.203 | [2.187, 2.220] | faster |
| micro/quadrule/cc_weights/n1048577 | call | 144.40ms | 69.27ms | 2.085 | [2.070, 2.101] | faster |

Sum of medians (call, common cases): A = 161.44ms, B = 76.97ms, saved = 84.47ms (2.098x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.3%
Round durations (s): 3, 3, 3 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 8s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
