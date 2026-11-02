A/B `micro`: A=`WP-28-checkpoint` B=`WP-28-refactor`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.90ms | 52.82ms | 1.003 | [0.993, 1.012] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.47ms | 7.49ms | 0.998 | [0.988, 1.007] | ~ (noise) |
| micro/quadrule/cc_weights/n33 | call | 2.12us | 2.12us | 0.976 | [0.917, 1.039] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 51.19us | 51.69us | 0.991 | [0.986, 0.996] | SLOWER |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 323.82us | 324.30us | 1.000 | [0.990, 1.011] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 11.19us | 11.21us | 0.996 | [0.985, 1.007] | ~ (noise) |

Sum of medians (call, common cases): A = 60.76ms, B = 60.70ms, saved = 66.32us (1.001x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.1%, 0.2%, 0.3%
Round durations (s): 3, 3, 3, 3, 3 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 13s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
