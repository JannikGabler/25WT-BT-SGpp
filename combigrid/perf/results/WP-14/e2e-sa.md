A/B `e2e`: A=`WP-14-before` B=`WP-14-sa`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 393.31ms | 395.75ms | 1.014 | [0.909, 1.132] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 329.55ms | 327.54ms | 1.014 | [0.908, 1.133] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 30.14ms | 30.52ms | 0.981 | [0.881, 1.093] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 250.27ms | 252.78ms | 0.987 | [0.975, 0.998] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | operator | 247.74ms | 250.33ms | 0.987 | [0.976, 0.998] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.01ms | 2.03ms | 0.980 | [0.859, 1.119] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 500.53ms | 502.34ms | 0.997 | [0.987, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 498.04ms | 500.01ms | 0.997 | [0.987, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.79ms | 1.81ms | 0.996 | [0.939, 1.057] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 908.85ms | 936.25ms | 0.969 | [0.963, 0.974] | SLOWER |
| quad/sg/equi-exp/b0/d10/L4 | operator | 906.34ms | 933.93ms | 0.970 | [0.964, 0.976] | SLOWER |
| quad/sg/equi-exp/b0/d10/L4 | generation | 1.97ms | 2.31ms | 0.689 | [0.400, 1.189] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 298.00ms | 303.76ms | 0.980 | [0.973, 0.987] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | operator | 289.46ms | 295.53ms | 0.979 | [0.971, 0.987] | SLOWER |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.21ms | 5.22ms | 1.004 | [0.831, 1.215] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 520.92ms | 526.96ms | 0.987 | [0.957, 1.019] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 496.88ms | 499.39ms | 0.986 | [0.957, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.57ms | 15.53ms | 1.056 | [0.826, 1.350] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 236.87ms | 240.67ms | 0.987 | [0.951, 1.024] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 206.26ms | 208.74ms | 0.974 | [0.936, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.49ms | 18.13ms | 1.093 | [0.809, 1.477] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.412s | 2.418s | 0.996 | [0.988, 1.004] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.409s | 2.415s | 0.996 | [0.988, 1.004] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.88ms | 1.91ms | 1.005 | [0.905, 1.117] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.190s | 1.278s | 0.958 | [0.908, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.108s | 1.179s | 0.963 | [0.916, 1.012] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 79.25ms | 79.72ms | 0.963 | [0.821, 1.130] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 86.86ms | 87.95ms | 0.975 | [0.950, 1.000] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.36ms | 85.13ms | 0.979 | [0.959, 1.000] | SLOWER |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.07ms | 2.11ms | 0.866 | [0.558, 1.344] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 328.37ms | 326.21ms | 1.012 | [0.998, 1.026] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 325.63ms | 323.73ms | 1.011 | [0.998, 1.023] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.36ms | 2.28ms | 1.158 | [0.968, 1.384] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 702.38ms | 701.24ms | 1.002 | [0.997, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 699.65ms | 698.91ms | 1.003 | [0.998, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.99ms | 1.97ms | 0.881 | [0.529, 1.467] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.319s | 1.310s | 1.005 | [0.998, 1.012] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.317s | 1.306s | 1.005 | [0.999, 1.012] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.14ms | 2.39ms | 0.810 | [0.571, 1.149] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 97.98ms | 96.52ms | 1.018 | [1.001, 1.035] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 96.34ms | 95.23ms | 1.015 | [0.996, 1.035] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.32ms | 1.02ms | 1.279 | [0.955, 1.713] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 210.30ms | 213.48ms | 1.007 | [0.959, 1.057] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 207.84ms | 210.76ms | 1.004 | [0.964, 1.046] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.36ms | 1.18ms | 1.328 | [0.581, 3.035] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.187s | 1.219s | 0.975 | [0.972, 0.979] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.180s | 1.211s | 0.975 | [0.972, 0.979] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.05ms | 3.04ms | 0.986 | [0.945, 1.028] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.538s | 5.667s | 0.977 | [0.971, 0.982] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.522s | 5.661s | 0.976 | [0.971, 0.982] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.45ms | 4.26ms | 1.492 | [0.851, 2.615] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.903s | 4.094s | 0.957 | [0.950, 0.964] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.843s | 4.029s | 0.956 | [0.949, 0.963] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 35.77ms | 35.68ms | 1.006 | [0.862, 1.174] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.686s | 7.026s | 0.951 | [0.947, 0.954] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.596s | 6.935s | 0.951 | [0.947, 0.954] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 50.68ms | 51.49ms | 0.952 | [0.841, 1.077] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 257.78ms | 257.94ms | 0.990 | [0.962, 1.020] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 253.86ms | 254.45ms | 0.990 | [0.964, 1.017] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.36ms | 2.29ms | 1.012 | [0.911, 1.124] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.490s | 1.514s | 0.982 | [0.977, 0.988] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.480s | 1.505s | 0.983 | [0.978, 0.987] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.81ms | 4.52ms | 0.939 | [0.728, 1.212] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.993s | 7.027s | 0.993 | [0.987, 0.999] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.983s | 7.016s | 0.993 | [0.987, 0.999] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 3.67ms | 4.85ms | 0.795 | [0.520, 1.215] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 187.35ms | 176.28ms | 1.059 | [1.016, 1.104] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 186.98ms | 175.87ms | 1.059 | [1.016, 1.104] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 213.78us | 216.65us | 1.100 | [0.768, 1.576] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 266.86ms | 245.69ms | 1.076 | [1.029, 1.125] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 266.72ms | 245.53ms | 1.076 | [1.029, 1.125] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 72.36us | 73.05us | 0.990 | [0.919, 1.066] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.626s | 1.450s | 1.154 | [1.036, 1.285] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.625s | 1.450s | 1.154 | [1.036, 1.285] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 114.96us | 95.40us | 1.973 | [0.490, 7.948] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.428s | 1.272s | 1.144 | [1.082, 1.210] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.428s | 1.272s | 1.144 | [1.083, 1.209] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 68.16us | 61.74us | 1.818 | [0.481, 6.874] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.164s | 2.008s | 1.083 | [1.070, 1.096] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.164s | 2.008s | 1.083 | [1.070, 1.096] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.57us | 34.43us | 1.024 | [0.968, 1.083] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 183.55ms | 183.34ms | 0.993 | [0.943, 1.046] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 183.47ms | 183.26ms | 0.992 | [0.943, 1.042] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 28.01us | 26.61us | 2.310 | [0.240, 22.221] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 356.35ms | 337.91ms | 1.044 | [1.013, 1.075] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 356.23ms | 337.80ms | 1.044 | [1.014, 1.075] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 36.25us | 35.27us | 0.938 | [0.729, 1.208] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 52.55ms | 51.13ms | 0.922 | [0.755, 1.125] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 52.47ms | 51.07ms | 0.928 | [0.773, 1.114] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 20.74us | 23.61us | 0.360 | [0.031, 4.226] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 111.96ms | 113.65ms | 0.958 | [0.839, 1.094] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 105.71ms | 107.07ms | 0.955 | [0.831, 1.099] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.72ms | 1.66ms | 1.044 | [0.960, 1.135] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 485.35ms | 439.39ms | 1.111 | [1.083, 1.140] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 481.62ms | 436.56ms | 1.110 | [1.081, 1.140] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.39ms | 2.18ms | 1.284 | [0.848, 1.944] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.693s | 1.519s | 1.121 | [0.971, 1.295] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.691s | 1.516s | 1.121 | [0.971, 1.295] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.21ms | 2.10ms | 1.047 | [0.692, 1.585] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.696s | 2.458s | 1.100 | [1.078, 1.123] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.693s | 2.456s | 1.100 | [1.078, 1.123] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.75ms | 1.71ms | 1.027 | [0.950, 1.109] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 736.24ms | 756.46ms | 0.982 | [0.931, 1.035] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 730.95ms | 750.70ms | 0.981 | [0.930, 1.035] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.65ms | 5.06ms | 1.028 | [0.745, 1.419] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.788s | 1.997s | 0.891 | [0.873, 0.910] | SLOWER |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 474.86ms | 475.11ms | 0.999 | [0.966, 1.034] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.316s | 1.516s | 0.858 | [0.838, 0.878] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.710s | 2.102s | 0.821 | [0.804, 0.838] | SLOWER |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.09ms | 24.45ms | 0.979 | [0.902, 1.063] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.684s | 2.076s | 0.819 | [0.802, 0.836] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.888s | 2.072s | 0.909 | [0.898, 0.919] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.07ms | 2.07ms | 0.994 | [0.918, 1.077] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.886s | 2.070s | 0.908 | [0.898, 0.919] | SLOWER |

Sum of medians (total, common cases): A = 52.884s, B = 53.577s, saved = -692266473.0ns (0.987x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d15/L7`: A 1.05e-09 -> B 1.45e-09 (the binding check is `refcheck`)
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.7%, 0.4%, 0.2%, 0.2%
Round durations (s): 222, 223, 222, 221, 221 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1108s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
