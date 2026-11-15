A/B `e2e`: A=`WP-56-before` B=`WP-56-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 324.64ms | 286.46ms | 1.148 | [1.029, 1.281] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 287.15ms | 245.68ms | 1.189 | [1.064, 1.328] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 37.21ms | 37.25ms | 0.942 | [0.814, 1.090] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 262.67ms | 261.80ms | 1.003 | [0.991, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 260.99ms | 260.36ms | 1.002 | [0.991, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.57ms | 1.42ms | 1.054 | [0.843, 1.317] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 513.49ms | 511.52ms | 1.018 | [0.976, 1.061] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 510.04ms | 509.65ms | 1.018 | [0.976, 1.061] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.36ms | 1.43ms | 0.993 | [0.480, 2.051] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 915.99ms | 911.92ms | 1.008 | [0.982, 1.035] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 913.99ms | 907.31ms | 1.009 | [0.982, 1.036] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.17ms | 2.29ms | 0.870 | [0.614, 1.233] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 291.55ms | 246.39ms | 1.188 | [1.171, 1.205] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 289.29ms | 244.17ms | 1.190 | [1.173, 1.207] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.53ms | 1.54ms | 0.974 | [0.806, 1.178] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 502.89ms | 490.29ms | 1.026 | [1.013, 1.039] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 493.59ms | 480.89ms | 1.026 | [1.012, 1.041] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.55ms | 6.71ms | 0.976 | [0.871, 1.094] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 213.57ms | 213.18ms | 0.999 | [0.976, 1.023] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.96ms | 201.43ms | 1.000 | [0.975, 1.026] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.38ms | 8.48ms | 0.984 | [0.728, 1.330] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.415s | 2.403s | 1.008 | [0.998, 1.018] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.414s | 2.402s | 1.007 | [0.998, 1.017] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.50ms | 1.47ms | 1.185 | [0.536, 2.622] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 776.44ms | 371.99ms | 2.122 | [2.001, 2.250] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 705.48ms | 296.95ms | 2.415 | [2.241, 2.602] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 69.99ms | 70.34ms | 0.968 | [0.838, 1.119] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 77.01ms | 75.70ms | 1.013 | [0.999, 1.028] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 75.71ms | 74.40ms | 1.013 | [1.000, 1.026] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.29ms | 1.28ms | 0.974 | [0.878, 1.081] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 319.79ms | 318.68ms | 1.003 | [0.999, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 317.81ms | 317.03ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.68ms | 1.64ms | 1.121 | [0.766, 1.640] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 690.30ms | 690.78ms | 0.989 | [0.950, 1.030] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 688.77ms | 689.24ms | 0.989 | [0.951, 1.030] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.41ms | 1.48ms | 0.972 | [0.890, 1.062] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.309s | 1.313s | 0.997 | [0.990, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.307s | 1.310s | 0.997 | [0.990, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.03ms | 2.22ms | 0.765 | [0.524, 1.116] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 95.91ms | 94.86ms | 1.009 | [0.999, 1.020] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 95.47ms | 94.30ms | 1.013 | [1.001, 1.025] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 533.38us | 1.01ms | 0.649 | [0.355, 1.187] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 211.58ms | 211.78ms | 1.000 | [0.991, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 210.99ms | 211.21ms | 1.000 | [0.991, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 488.68us | 462.49us | 1.023 | [0.793, 1.320] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.191s | 1.191s | 1.000 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.190s | 1.191s | 1.000 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 178.16us | 178.93us | 0.998 | [0.981, 1.015] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.436s | 5.436s | 1.002 | [0.996, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.435s | 5.436s | 1.002 | [0.996, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 464.51us | 228.80us | 1.154 | [0.396, 3.363] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.168s | 1.171s | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.159s | 1.161s | 0.999 | [0.993, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 4.03ms | 4.04ms | 0.922 | [0.737, 1.154] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.139s | 1.138s | 1.000 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.120s | 1.120s | 0.999 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 10.95ms | 10.82ms | 1.020 | [0.985, 1.056] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 247.90ms | 245.51ms | 1.011 | [1.007, 1.014] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 247.17ms | 244.79ms | 1.011 | [1.007, 1.015] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 635.83us | 648.26us | 0.996 | [0.927, 1.070] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.447s | 1.450s | 0.999 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.447s | 1.449s | 0.999 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 228.59us | 222.65us | 1.031 | [0.900, 1.181] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.888s | 6.895s | 1.001 | [0.995, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.887s | 6.894s | 1.001 | [0.996, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 462.00us | 623.47us | 0.809 | [0.390, 1.676] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 768.52ms | 718.19ms | 1.080 | [1.024, 1.139] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 765.29ms | 715.01ms | 1.080 | [1.024, 1.140] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.66ms | 2.65ms | 1.005 | [0.994, 1.017] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 485.05ms | 339.08ms | 1.454 | [1.386, 1.526] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 477.33ms | 331.36ms | 1.465 | [1.395, 1.538] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.13ms | 7.12ms | 1.004 | [0.883, 1.141] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.36ms | 26.19ms | 1.018 | [0.968, 1.072] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.79ms | 23.63ms | 1.020 | [0.965, 1.079] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.39ms | 2.38ms | 1.001 | [0.986, 1.015] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 2.91ms | 3.03ms | 1.041 | [0.815, 1.328] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 1.96ms | 2.11ms | 0.936 | [0.865, 1.014] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 759.38us | 766.43us | 1.240 | [0.659, 2.334] | ~ (noise) |

Sum of medians (total, common cases): A = 27.720s, B = 27.015s, saved = 704.35ms (1.026x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 1.03e-12 -> B 4.19e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 115.5 MiB | 112.4 MiB | 0.973 | 12.1% / 15.9% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.8 MiB | 12.7 MiB | 0.996 | 2.8% / 4.8% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.8 MiB | 12.8 MiB | 1.001 | 0.5% / 1.4% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.6 MiB | 13.0 MiB | 1.028 | 3.2% / 2.3% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.2 MiB | 13.1 MiB | 0.998 | 1.5% / 2.6% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.4 MiB | 18.3 MiB | 0.996 | 0.7% / 1.0% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.2 MiB | 20.1 MiB | 0.992 | 1.0% / 1.8% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.8 MiB | 12.8 MiB | 1.002 | 2.0% / 2.1% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 305.6 MiB | 203.1 MiB | 0.664 | 28.9% / 7.1% | MEM- |
| quad/sg/cc-uq/b0/d3/L14 | 17.2 MiB | 14.7 MiB | 0.852 | 4.0% / 8.5% | MEM- |
| quad/sg/cc-uq/b0/d5/L10 | 13.0 MiB | 12.8 MiB | 0.987 | 3.1% / 2.7% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 12.8 MiB | 0.993 | 1.8% / 1.3% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.8 MiB | 12.9 MiB | 1.005 | 2.9% / 2.3% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 43.9 MiB | 43.4 MiB | 0.989 | 0.6% / 3.2% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 18.2 MiB | 18.0 MiB | 0.989 | 5.2% / 2.5% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 28.7 MiB | 30.1 MiB | 1.049 | 10.6% / 4.6% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 29.7 MiB | 29.8 MiB | 1.003 | 2.4% / 3.9% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 51.7 MiB | 51.6 MiB | 0.999 | 0.4% / 0.3% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 82.8 MiB | 82.8 MiB | 0.999 | 0.4% / 0.2% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 18.6 MiB | 18.2 MiB | 0.982 | 2.5% / 2.8% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 29.3 MiB | 29.5 MiB | 1.006 | 6.7% / 10.0% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 30.3 MiB | 30.2 MiB | 0.994 | 4.6% / 2.6% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.6 MiB | 13.3 MiB | 0.977 | 1.2% / 2.2% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 15.0 MiB | 14.8 MiB | 0.987 | 3.6% / 1.6% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.2 MiB | 13.2 MiB | 1.003 | 1.5% / 1.1% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.6 MiB | 12.7 MiB | 1.010 | 0.9% / 1.7% | ~ |

Peak memory (common cases): geomean B/A = 0.977; largest B/A: `quad/dsg/equi-exp/b0/d7/L10` 28.7 MiB -> 30.1 MiB (1.049x); 0 MEM+, 2 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.5 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.1%, 0.2%, 0.2%, 0.2%
Round durations (s): 115, 115, 115, 115, 115 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 574s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
