A/B `e2e`: A=`WP-24-before` B=`WP-24-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 402.26ms | 397.68ms | 1.018 | [0.912, 1.136] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 330.96ms | 329.98ms | 1.020 | [0.913, 1.140] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 31.27ms | 30.97ms | 1.032 | [0.927, 1.149] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 252.40ms | 249.42ms | 1.012 | [1.000, 1.023] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 249.76ms | 247.22ms | 1.011 | [1.000, 1.023] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.96ms | 1.95ms | 1.049 | [0.919, 1.197] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 501.42ms | 501.41ms | 0.999 | [0.989, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 498.88ms | 498.85ms | 1.000 | [0.990, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.81ms | 1.83ms | 0.853 | [0.518, 1.403] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 935.75ms | 924.11ms | 1.011 | [1.005, 1.017] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 933.75ms | 921.66ms | 1.012 | [1.005, 1.018] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.19ms | 2.20ms | 0.891 | [0.496, 1.600] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 310.95ms | 307.49ms | 1.013 | [0.987, 1.040] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 302.28ms | 299.11ms | 1.015 | [0.987, 1.043] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.19ms | 5.35ms | 0.949 | [0.785, 1.148] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 520.76ms | 521.26ms | 1.000 | [0.988, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 496.90ms | 496.14ms | 1.001 | [0.987, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.42ms | 15.24ms | 0.989 | [0.824, 1.188] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 233.93ms | 233.75ms | 1.002 | [0.971, 1.034] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 208.32ms | 206.21ms | 1.012 | [1.007, 1.018] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 17.45ms | 19.14ms | 0.934 | [0.667, 1.308] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.415s | 2.405s | 1.011 | [0.998, 1.023] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.412s | 2.402s | 1.011 | [0.998, 1.023] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.86ms | 1.88ms | 0.990 | [0.891, 1.100] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.299s | 1.199s | 1.059 | [0.996, 1.125] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.207s | 1.129s | 1.053 | [0.988, 1.122] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 83.05ms | 77.78ms | 1.123 | [0.957, 1.318] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.38ms | 86.91ms | 1.003 | [0.989, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.41ms | 83.96ms | 1.003 | [0.991, 1.016] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.06ms | 2.25ms | 0.963 | [0.880, 1.055] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 325.81ms | 322.22ms | 1.011 | [1.003, 1.019] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 322.78ms | 319.34ms | 1.011 | [1.003, 1.019] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.30ms | 2.21ms | 1.017 | [0.949, 1.089] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 700.59ms | 693.09ms | 1.002 | [0.981, 1.023] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 698.07ms | 690.59ms | 1.002 | [0.981, 1.023] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.93ms | 2.01ms | 0.972 | [0.883, 1.070] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.315s | 1.309s | 0.994 | [0.968, 1.022] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.312s | 1.307s | 0.994 | [0.968, 1.021] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 4.34ms | 2.35ms | 1.113 | [0.487, 2.540] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 177.08ms | 176.67ms | 1.024 | [0.983, 1.067] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 176.67ms | 176.26ms | 1.024 | [0.983, 1.067] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 208.06us | 211.34us | 0.995 | [0.911, 1.087] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 245.03ms | 246.14ms | 0.991 | [0.957, 1.027] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 244.88ms | 245.86ms | 0.991 | [0.957, 1.027] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.61us | 74.17us | 0.819 | [0.453, 1.481] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.446s | 1.452s | 0.998 | [0.993, 1.003] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.446s | 1.451s | 0.998 | [0.993, 1.003] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 118.24us | 107.55us | 1.100 | [0.877, 1.379] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.258s | 1.263s | 0.995 | [0.982, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.258s | 1.263s | 0.995 | [0.982, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 65.44us | 64.88us | 1.002 | [0.949, 1.057] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.000s | 2.003s | 0.999 | [0.988, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.000s | 2.003s | 0.999 | [0.988, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.85us | 33.87us | 1.020 | [0.947, 1.099] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.23ms | 181.46ms | 1.002 | [0.993, 1.011] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.13ms | 181.39ms | 1.002 | [0.993, 1.011] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 28.22us | 27.59us | 1.053 | [0.736, 1.507] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 341.98ms | 335.68ms | 1.021 | [1.008, 1.035] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 341.86ms | 335.58ms | 1.021 | [1.008, 1.035] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 36.53us | 34.57us | 1.069 | [0.831, 1.376] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 50.95ms | 50.87ms | 1.043 | [0.926, 1.175] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 50.88ms | 50.81ms | 1.043 | [0.926, 1.175] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 20.53us | 19.98us | 1.026 | [0.875, 1.204] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 739.50ms | 738.83ms | 1.003 | [0.950, 1.058] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 735.75ms | 735.06ms | 1.003 | [0.951, 1.058] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.03ms | 3.08ms | 0.956 | [0.854, 1.070] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 483.95ms | 478.26ms | 1.029 | [0.948, 1.117] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 475.52ms | 469.45ms | 1.030 | [0.947, 1.119] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.51ms | 7.62ms | 0.996 | [0.975, 1.018] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.22ms | 28.12ms | 0.960 | [0.911, 1.011] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.47ms | 25.09ms | 0.957 | [0.904, 1.012] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.45ms | 2.45ms | 0.988 | [0.944, 1.034] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.26ms | 3.15ms | 1.089 | [0.982, 1.208] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.23ms | 2.14ms | 1.090 | [0.936, 1.268] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 882.86us | 794.51us | 1.099 | [1.013, 1.193] | faster |

Sum of medians (total, common cases): A = 16.254s, B = 16.107s, saved = 146.95ms (1.009x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 2.85e-12 -> B 6.59e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.8 MiB | 619.9 MiB | 1.000 | 0.1% / 0.0% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.5 MiB | 15.3 MiB | 0.988 | 4.8% / 4.0% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.4 MiB | 14.4 MiB | 1.002 | 3.2% / 3.3% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.7 MiB | 12.8 MiB | 1.007 | 2.3% / 1.9% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.7 MiB | 25.6 MiB | 0.995 | 1.9% / 1.6% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 52.7 MiB | 52.4 MiB | 0.994 | 6.3% / 8.4% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 56.3 MiB | 58.1 MiB | 1.031 | 8.7% / 12.1% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.4 MiB | 14.7 MiB | 1.017 | 2.3% / 2.1% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1074.5 MiB | 1065.8 MiB | 0.992 | 2.3% / 3.8% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.8 MiB | 41.1 MiB | 1.009 | 4.8% / 6.0% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.4 MiB | 17.4 MiB | 1.001 | 3.1% / 3.6% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.7 MiB | 15.0 MiB | 1.022 | 5.8% / 4.2% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 12.9 MiB | 0.996 | 4.6% / 2.4% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.9 MiB | 14.8 MiB | 0.996 | 3.0% / 3.4% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.0 MiB | 12.1 MiB | 1.011 | 0.9% / 1.2% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.0 MiB | 12.0 MiB | 1.004 | 2.0% / 1.6% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.9 MiB | 13.1 MiB | 1.018 | 2.2% / 2.8% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 25.2 MiB | 24.4 MiB | 0.967 | 7.4% / 4.2% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 11.8 MiB | 0.995 | 1.8% / 3.8% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.8 MiB | 12.0 MiB | 1.012 | 2.1% / 1.9% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.8 MiB | 11.9 MiB | 1.004 | 1.6% / 1.7% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.3 MiB | 16.3 MiB | 1.000 | 3.7% / 1.8% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.2 MiB | 17.1 MiB | 0.992 | 4.3% / 5.3% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.8 MiB | 12.8 MiB | 0.998 | 1.0% / 2.1% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.1 MiB | 0.996 | 2.0% / 1.5% | ~ |

Peak memory (common cases): geomean B/A = 1.002; largest B/A: `quad/sg/equi-exp/b1/d20/L5` 56.3 MiB -> 58.1 MiB (1.031x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 70, 70, 70, 70, 70 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 351s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
