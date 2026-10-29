A/B `micro`: A=`WP-03-before` B=`WP-03-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/mivec/downwards_closure/d10/S10/n300 | call | 583.95ms | 323.24us | 1806.867 | [1800.150, 1813.610] | faster |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 17.63ms | 10.91us | 1614.594 | [1596.518, 1632.874] | faster |
| micro/mivec/downwards_closure/d5/S16/n1000 | call | 11.78ms | 487.55us | 24.222 | [23.921, 24.526] | faster |
| micro/mivec/downwards_closure/d10/S12/n1000 | call | 12.718s | 1.54ms | 8237.087 | [8206.466, 8267.823] | faster |
| micro/mivec/downwards_closure/d15/S10/n300 | call | 15.020s | 570.02us | 26356.261 | [26235.295, 26477.785] | faster |
| micro/mivec/downwards_closure/d20/S7/n300 | call | 15.847s | 110.56us | 143331.221 | [141933.852, 144742.347] | faster |
| micro/mivec/pareto_maxima/d10/S12/n1000 | call | 6.00ms | 6.00ms | 1.003 | [0.986, 1.021] | ~ (noise) |
| micro/mivec/pareto_maxima/d20/S7/n300 | call | 123.96us | 90.93us | 1.364 | [1.357, 1.371] | faster |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.97ms | 15.83ms | 1.022 | [0.983, 1.063] | ~ (noise) |

Sum of medians (call, common cases): A = 44.221s, B = 24.97ms, saved = 44.196s (1770.846x)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.4%, 0.3%, 0.2%, 0.2%
Round durations (s): 237, 237, 237, 237, 237 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1183s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
