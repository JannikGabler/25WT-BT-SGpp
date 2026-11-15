A/B `micro`: A=`WP-56-before` B=`WP-56-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/cc/b0/d2/L16 | call | 5.93ms | 5.92ms | 1.006 | [0.991, 1.021] | ~ (noise) |
| micro/quad/operator/cc/b0/d2/L16 | call | 122.50ms | 110.37ms | 1.082 | [1.010, 1.159] | faster |
| micro/quad/operator/equi/b0/d2/L16 | call | 101.75ms | 101.89ms | 0.979 | [0.902, 1.062] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 1.93us | 956.0ns | 2.015 | [1.999, 2.031] | faster |
| micro/quadrule/cc_weights/n1025 | call | 49.11us | 27.67us | 1.772 | [1.749, 1.796] | faster |
| micro/quadrule/cc_weights/n32769 | call | 3.18ms | 1.40ms | 2.269 | [2.251, 2.287] | faster |
| micro/quadrule/cc_weights/n131073 | call | 13.84ms | 6.26ms | 2.217 | [2.198, 2.235] | faster |
| micro/quadrule/cc_weights/n1048577 | call | 144.90ms | 69.63ms | 2.081 | [2.065, 2.096] | faster |

Sum of medians (call, common cases): A = 392.16ms, B = 295.50ms, saved = 96.65ms (1.327x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 5, 5, 5, 5, 5 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 25s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
