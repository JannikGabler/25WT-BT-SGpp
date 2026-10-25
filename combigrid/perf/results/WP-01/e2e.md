A/B `e2e`: A=`WP-01-before` B=`WP-01-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 735.81ms | 439.83ms | 1.677 | [1.503, 1.871] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 689.45ms | 368.21ms | 1.913 | [1.712, 2.137] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.68ms | 32.46ms | 1.000 | [0.898, 1.114] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 705.46ms | 271.45ms | 2.612 | [2.577, 2.647] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 702.49ms | 268.89ms | 2.627 | [2.591, 2.663] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.21ms | 2.01ms | 1.038 | [0.886, 1.215] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 1.628s | 515.71ms | 3.166 | [3.133, 3.199] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 1.625s | 513.18ms | 3.178 | [3.146, 3.212] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.91ms | 1.93ms | 0.871 | [0.599, 1.267] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 3.313s | 947.06ms | 3.481 | [3.410, 3.553] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 3.308s | 944.39ms | 3.486 | [3.417, 3.557] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.66ms | 2.20ms | 1.462 | [1.007, 2.123] | faster |
| quad/sg/equi-exp/b1/d10/L7 | total | 1.108s | 307.42ms | 3.608 | [3.479, 3.743] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 1.099s | 298.41ms | 3.685 | [3.557, 3.818] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.79ms | 5.78ms | 1.009 | [0.834, 1.220] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 2.372s | 533.87ms | 4.440 | [4.384, 4.496] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 2.347s | 505.39ms | 4.619 | [4.554, 4.685] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 17.20ms | 16.78ms | 1.017 | [0.862, 1.198] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 1.114s | 240.04ms | 4.662 | [4.592, 4.734] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 1.081s | 207.96ms | 5.212 | [5.181, 5.244] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 22.44ms | 21.85ms | 1.043 | [0.772, 1.409] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 3.517s | 2.426s | 1.450 | [1.438, 1.462] | faster |
| quad/sg/equi-sin/b0/d7/L7 | operator | 3.514s | 2.423s | 1.451 | [1.439, 1.463] | faster |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.97ms | 1.92ms | 1.010 | [0.909, 1.122] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.410s | 1.262s | 1.146 | [1.042, 1.260] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.324s | 1.177s | 1.153 | [1.042, 1.277] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 77.66ms | 81.75ms | 0.986 | [0.853, 1.139] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 179.39ms | 89.75ms | 1.992 | [1.948, 2.036] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 177.21ms | 86.76ms | 2.025 | [1.983, 2.067] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.06ms | 2.06ms | 1.027 | [0.963, 1.094] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 780.10ms | 344.20ms | 2.259 | [2.226, 2.292] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 777.06ms | 341.41ms | 2.271 | [2.241, 2.300] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.56ms | 2.26ms | 0.964 | [0.647, 1.436] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 1.818s | 715.71ms | 2.544 | [2.525, 2.563] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 1.815s | 712.87ms | 2.549 | [2.530, 2.569] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.11ms | 2.07ms | 1.026 | [0.931, 1.131] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 3.623s | 1.320s | 2.746 | [2.730, 2.762] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 3.620s | 1.315s | 2.751 | [2.736, 2.765] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.75ms | 2.37ms | 0.934 | [0.495, 1.762] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 2.220s | 816.47ms | 2.727 | [2.585, 2.877] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 2.215s | 810.94ms | 2.739 | [2.596, 2.890] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.72ms | 4.78ms | 0.989 | [0.969, 1.008] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 3.247s | 1.862s | 1.736 | [1.682, 1.792] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 1.870s | 478.05ms | 3.766 | [3.464, 4.095] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.381s | 1.378s | 1.002 | [0.989, 1.014] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.894s | 1.814s | 1.043 | [1.036, 1.050] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 108.88ms | 24.15ms | 4.358 | [4.142, 4.584] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.785s | 1.790s | 0.998 | [0.992, 1.004] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.906s | 1.896s | 1.004 | [1.000, 1.009] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 9.99ms | 2.08ms | 4.911 | [4.535, 5.318] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.896s | 1.894s | 1.000 | [0.996, 1.004] | ~ (noise) |

Sum of medians (total, common cases): A = 31.571s, B = 15.802s, saved = 15.768s (1.998x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 4.73e-13 -> B 8.87e-13 (the binding check is `refcheck`)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 98, 98, 98, 98, 98 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 491s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
