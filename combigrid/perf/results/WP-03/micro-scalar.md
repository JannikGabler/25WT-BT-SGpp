A/B `micro`: A=`WP-03-before` B=`WP-03-scalar`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/mivec/downwards_closure/d10/S10/n300 | call | 583.69ms | 373.25ms | 1.562 | [1.557, 1.568] | faster |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.63ms | 11.10ms | 1.587 | [1.569, 1.605] | faster |
| micro/mivec/downwards_closure/d5/S16/n1000 | call | 11.75ms | 9.89ms | 1.189 | [1.184, 1.194] | faster |
| micro/mivec/pareto_maxima/d10/S12/n1000 | call | 6.00ms | 6.00ms | 0.999 | [0.960, 1.040] | ~ (noise) |
| micro/mivec/pareto_maxima/d20/S7/n300 | call | 124.31us | 94.70us | 1.314 | [1.307, 1.320] | faster |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.91ms | 15.97ms | 0.997 | [0.990, 1.005] | ~ (noise) |

Sum of medians (call, common cases): A = 635.10ms, B = 416.31ms, saved = 218.79ms (1.526x)
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.3%, 0.2%, 0.2%, 0.4%
Round durations (s): 28, 28, 28, 28, 28 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 140s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
