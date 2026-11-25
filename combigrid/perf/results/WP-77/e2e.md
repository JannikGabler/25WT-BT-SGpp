A/B `e2e`: A=`WP-77-before` B=`WP-77-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 295.98ms | 277.77ms | 1.117 | [1.001, 1.247] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 256.37ms | 239.82ms | 1.123 | [1.005, 1.255] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 39.96ms | 37.46ms | 1.029 | [0.924, 1.145] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 247.60ms | 246.31ms | 1.005 | [0.993, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 245.92ms | 244.65ms | 1.003 | [0.991, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.01ms | 1.55ms | 1.249 | [0.807, 1.934] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 499.21ms | 495.67ms | 1.006 | [0.995, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 495.39ms | 494.15ms | 1.004 | [0.993, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.48ms | 1.60ms | 1.394 | [0.589, 3.297] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 903.86ms | 897.91ms | 1.007 | [1.001, 1.013] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 901.96ms | 895.69ms | 1.007 | [1.001, 1.014] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.00ms | 2.06ms | 0.941 | [0.487, 1.819] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 241.29ms | 241.18ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 239.16ms | 238.95ms | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.43ms | 1.41ms | 0.989 | [0.818, 1.197] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 491.71ms | 489.92ms | 1.002 | [0.990, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 482.40ms | 481.08ms | 1.002 | [0.988, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.35ms | 6.05ms | 1.059 | [0.945, 1.186] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 216.60ms | 212.93ms | 1.015 | [1.002, 1.028] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 204.97ms | 201.56ms | 1.015 | [1.009, 1.020] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.14ms | 7.95ms | 1.027 | [0.760, 1.388] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.398s | 2.385s | 1.006 | [0.997, 1.014] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.395s | 2.384s | 1.005 | [0.997, 1.014] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.68ms | 1.55ms | 1.315 | [0.756, 2.289] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 454.91ms | 432.70ms | 1.000 | [0.891, 1.123] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 381.58ms | 352.36ms | 1.013 | [0.896, 1.145] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 80.60ms | 80.91ms | 0.967 | [0.836, 1.117] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 75.31ms | 72.00ms | 1.050 | [1.035, 1.066] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.03ms | 70.72ms | 1.051 | [1.035, 1.067] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.26ms | 1.26ms | 1.002 | [0.939, 1.068] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 320.91ms | 320.29ms | 1.001 | [0.996, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 319.18ms | 318.32ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.64ms | 1.66ms | 0.820 | [0.518, 1.299] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 687.10ms | 686.69ms | 0.998 | [0.984, 1.012] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 685.57ms | 685.14ms | 0.998 | [0.984, 1.012] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.41ms | 1.45ms | 0.977 | [0.906, 1.053] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.312s | 1.296s | 1.013 | [1.007, 1.018] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.309s | 1.293s | 1.012 | [1.007, 1.017] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 3.89ms | 1.93ms | 1.293 | [0.792, 2.109] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.78ms | 84.48ms | 1.108 | [1.102, 1.114] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 90.72ms | 81.00ms | 1.118 | [1.111, 1.125] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 709.51us | 1.07ms | 0.555 | [0.257, 1.198] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 202.73ms | 197.34ms | 1.032 | [1.021, 1.043] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 201.89ms | 196.39ms | 1.033 | [1.022, 1.043] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 465.77us | 545.95us | 0.859 | [0.666, 1.109] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.148s | 1.137s | 1.010 | [1.006, 1.013] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.147s | 1.136s | 1.010 | [1.006, 1.014] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 180.33us | 537.43us | 0.357 | [0.201, 0.637] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.408s | 5.375s | 1.006 | [1.002, 1.010] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.407s | 5.374s | 1.006 | [1.002, 1.010] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 214.62us | 331.05us | 0.647 | [0.399, 1.049] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.165s | 1.167s | 0.998 | [0.994, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.151s | 1.151s | 1.000 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 4.09ms | 5.99ms | 0.695 | [0.596, 0.812] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.137s | 1.140s | 0.997 | [0.994, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.110s | 1.110s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 11.03ms | 13.86ms | 0.797 | [0.774, 0.821] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | total | 247.42ms | 239.89ms | 1.032 | [1.029, 1.036] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 246.38ms | 238.77ms | 1.032 | [1.028, 1.036] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 645.82us | 736.82us | 0.884 | [0.822, 0.950] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.452s | 1.435s | 1.012 | [1.006, 1.019] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.451s | 1.434s | 1.012 | [1.006, 1.019] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 223.00us | 337.40us | 0.603 | [0.456, 0.797] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.870s | 6.822s | 1.007 | [1.001, 1.013] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.869s | 6.821s | 1.007 | [1.003, 1.012] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 302.27us | 338.87us | 0.787 | [0.354, 1.750] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 128.06ms | 84.12ms | 1.518 | [1.490, 1.547] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 127.78ms | 83.86ms | 1.520 | [1.492, 1.548] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 222.79us | 220.28us | 0.996 | [0.912, 1.087] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 185.52ms | 167.73ms | 1.103 | [1.081, 1.126] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 185.38ms | 167.59ms | 1.103 | [1.081, 1.126] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 88.91us | 92.26us | 0.969 | [0.900, 1.043] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.137s | 1.121s | 1.016 | [1.006, 1.027] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.137s | 1.121s | 1.016 | [1.006, 1.027] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 65.79us | 69.49us | 0.986 | [0.786, 1.236] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.010s | 988.02ms | 1.018 | [0.999, 1.037] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.010s | 987.92ms | 1.018 | [0.999, 1.037] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 57.76us | 73.05us | 0.840 | [0.708, 0.997] | SLOWER |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.614s | 1.616s | 0.987 | [0.966, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.613s | 1.615s | 0.987 | [0.966, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 52.03us | 53.29us | 0.906 | [0.740, 1.109] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.05ms | 23.04ms | 1.473 | [1.444, 1.503] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 33.96ms | 22.97ms | 1.475 | [1.445, 1.505] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 41.91us | 35.69us | 1.186 | [0.829, 1.698] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 117.05ms | 109.15ms | 1.070 | [1.056, 1.084] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 116.98ms | 109.06ms | 1.070 | [1.056, 1.085] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 30.45us | 45.81us | 0.684 | [0.531, 0.880] | SLOWER |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.94ms | 8.31ms | 1.439 | [1.349, 1.536] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.87ms | 8.24ms | 1.443 | [1.352, 1.541] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 32.69us | 28.08us | 1.050 | [0.839, 1.313] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 101.47ms | 102.52ms | 0.990 | [0.953, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 94.61ms | 95.69ms | 0.989 | [0.949, 1.031] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.26ms | 2.23ms | 1.000 | [0.903, 1.107] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 366.25ms | 367.15ms | 0.990 | [0.964, 1.017] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 365.68ms | 366.62ms | 0.990 | [0.964, 1.017] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 80.60us | 93.80us | 0.869 | [0.764, 0.988] | SLOWER |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.195s | 1.196s | 1.003 | [0.980, 1.026] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.192s | 1.194s | 1.002 | [0.980, 1.025] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.34ms | 2.08ms | 1.322 | [0.874, 2.001] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.846s | 1.846s | 0.997 | [0.977, 1.017] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.844s | 1.844s | 0.997 | [0.977, 1.017] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.59ms | 1.51ms | 1.005 | [0.930, 1.085] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 689.36ms | 665.06ms | 1.037 | [0.983, 1.094] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 686.18ms | 661.88ms | 1.037 | [0.983, 1.094] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.66ms | 2.64ms | 1.003 | [0.988, 1.019] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 379.39ms | 323.48ms | 1.161 | [1.137, 1.186] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 371.64ms | 315.74ms | 1.165 | [1.138, 1.193] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.12ms | 7.20ms | 0.991 | [0.979, 1.003] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.62ms | 19.69ms | 1.356 | [1.313, 1.399] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.08ms | 17.15ms | 1.409 | [1.339, 1.482] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.37ms | 2.38ms | 0.999 | [0.988, 1.009] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.15ms | 2.14ms | 1.347 | [1.125, 1.614] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.13ms | 1.20ms | 1.763 | [1.628, 1.909] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 763.50us | 785.50us | 0.932 | [0.647, 1.341] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | 1.536s | 1.533s | 1.002 | [0.994, 1.009] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | operator | 1.534s | 1.531s | 1.002 | [0.994, 1.009] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | generation | 2.24ms | 2.06ms | 1.082 | [0.664, 1.762] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | total | 570.83ms | 570.03ms | 1.000 | [0.994, 1.005] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | operator | 568.58ms | 567.96ms | 1.001 | [0.998, 1.005] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | generation | 2.10ms | 2.05ms | 0.764 | [0.420, 1.390] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | 1.037s | 1.036s | 1.001 | [0.987, 1.016] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | operator | 1.034s | 1.032s | 1.002 | [0.988, 1.016] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | generation | 2.11ms | 2.16ms | 0.877 | [0.572, 1.344] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | total | 521.05ms | 520.30ms | 1.001 | [0.977, 1.026] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | operator | 518.56ms | 518.09ms | 1.001 | [0.981, 1.022] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | generation | 2.02ms | 2.05ms | 0.949 | [0.425, 2.117] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | 2.155s | 1.562s | 1.377 | [1.372, 1.382] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | operator | 2.154s | 1.561s | 1.377 | [1.372, 1.382] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.17ms | 1.15ms | 1.003 | [0.980, 1.027] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | total | 881.26ms | 551.72ms | 1.596 | [1.590, 1.602] | faster |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | operator | 879.94ms | 550.41ms | 1.597 | [1.591, 1.603] | faster |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.13ms | 1.14ms | 0.992 | [0.958, 1.027] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | total | 1.073s | 1.028s | 1.032 | [0.997, 1.067] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | operator | 1.073s | 1.028s | 1.032 | [0.997, 1.067] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | generation | 63.41us | 78.01us | 0.838 | [0.712, 0.987] | SLOWER |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | total | 502.91ms | 503.41ms | 0.988 | [0.959, 1.018] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | operator | 502.83ms | 503.33ms | 0.988 | [0.959, 1.018] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | generation | 46.16us | 50.49us | 0.909 | [0.835, 0.989] | SLOWER |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | total | 824.97ms | 828.54ms | 0.997 | [0.970, 1.025] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | operator | 824.91ms | 828.46ms | 0.997 | [0.970, 1.025] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | generation | 38.41us | 52.31us | 0.739 | [0.551, 0.992] | SLOWER |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | total | 345.29ms | 345.32ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | operator | 345.22ms | 345.26ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | generation | 36.18us | 38.69us | 0.935 | [0.871, 1.004] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | 2.477s | 1.845s | 1.342 | [1.337, 1.347] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | operator | 2.475s | 1.843s | 1.342 | [1.337, 1.347] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | generation | 1.16ms | 1.15ms | 1.003 | [0.994, 1.012] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | total | 398.19ms | 331.67ms | 1.193 | [1.175, 1.211] | faster |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | operator | 398.13ms | 331.61ms | 1.193 | [1.175, 1.211] | faster |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | generation | 38.69us | 39.46us | 0.964 | [0.796, 1.167] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | total | 1.289s | 1.289s | 1.001 | [0.970, 1.034] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | operator | 1.289s | 1.289s | 1.001 | [0.970, 1.034] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | generation | 50.84us | 56.71us | 0.893 | [0.769, 1.036] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | 1.153s | 1.153s | 1.000 | [0.979, 1.020] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | operator | 1.150s | 1.150s | 1.000 | [0.979, 1.020] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | generation | 1.79ms | 1.73ms | 1.038 | [1.010, 1.067] | faster |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | total | 350.10ms | 350.10ms | 0.997 | [0.989, 1.006] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | operator | 348.81ms | 348.75ms | 0.998 | [0.992, 1.004] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | generation | 335.72us | 401.45us | 0.871 | [0.820, 0.925] | SLOWER |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | 662.42ms | 495.20ms | 1.347 | [1.312, 1.383] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | operator | 658.86ms | 491.65ms | 1.350 | [1.314, 1.386] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.36ms | 3.36ms | 1.003 | [0.983, 1.024] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | total | 334.04ms | 314.71ms | 1.061 | [1.057, 1.065] | faster |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | operator | 330.46ms | 311.07ms | 1.062 | [1.058, 1.066] | faster |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.22ms | 3.29ms | 0.980 | [0.974, 0.987] | SLOWER |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | total | 257.89ms | 252.28ms | 1.021 | [0.837, 1.246] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | operator | 255.75ms | 250.26ms | 1.022 | [0.843, 1.241] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | generation | 2.08ms | 1.98ms | 0.940 | [0.598, 1.478] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | total | 98.79ms | 98.93ms | 1.005 | [0.949, 1.064] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | operator | 96.75ms | 96.82ms | 1.009 | [0.958, 1.063] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | generation | 1.97ms | 2.02ms | 0.880 | [0.558, 1.389] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | total | 1.082s | 1.071s | 1.016 | [0.900, 1.147] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | operator | 1.080s | 1.069s | 1.015 | [0.899, 1.147] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | generation | 2.19ms | 2.02ms | 1.175 | [0.612, 2.258] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | total | 207.74ms | 208.24ms | 1.000 | [0.990, 1.011] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | operator | 205.73ms | 205.85ms | 1.000 | [0.993, 1.007] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | generation | 2.05ms | 1.99ms | 1.030 | [0.344, 3.079] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | 576.49ms | 622.42ms | 0.926 | [0.923, 0.930] | SLOWER |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | operator | 572.86ms | 618.07ms | 0.926 | [0.922, 0.929] | SLOWER |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | generation | 2.09ms | 2.08ms | 0.989 | [0.603, 1.620] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | total | 70.48ms | 72.05ms | 0.978 | [0.972, 0.984] | SLOWER |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | operator | 70.38ms | 71.95ms | 0.978 | [0.972, 0.985] | SLOWER |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | generation | 42.88us | 47.63us | 0.915 | [0.862, 0.972] | SLOWER |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | total | 244.38ms | 243.66ms | 1.002 | [0.913, 1.098] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | operator | 244.24ms | 243.56ms | 1.001 | [0.913, 1.098] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | generation | 37.22us | 38.62us | 0.969 | [0.833, 1.125] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | total | 152.69ms | 151.52ms | 1.008 | [0.980, 1.037] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | operator | 144.45ms | 143.18ms | 1.006 | [0.983, 1.030] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | generation | 57.34us | 63.21us | 0.916 | [0.880, 0.952] | SLOWER |

Sum of medians (total, common cases, group suite): A = 34.713s, B = 34.291s, saved = 421.70ms (1.012x)
Sum of medians (total, common cases, group expensive/): A = 17.756s, B = 15.888s, saved = 1.868s (1.118x)
Sum of medians (total, common cases, group expensive/ cached): A = 1.044s, B = 1.090s, saved = -45607917.0ns (0.958x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d2/L20`: A 7.23e-14 -> B 7.23e-14 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 111.8 MiB | 112.9 MiB | 1.010 | 22.4% / 19.6% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.8 MiB | 12.8 MiB | 1.001 | 2.4% / 3.6% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.8 MiB | 12.8 MiB | 0.996 | 1.8% / 1.6% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.9 MiB | 1.005 | 1.2% / 1.7% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.4 MiB | 13.4 MiB | 1.001 | 1.1% / 2.5% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.3 MiB | 18.3 MiB | 1.001 | 2.6% / 1.0% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.2 MiB | 20.2 MiB | 1.002 | 0.4% / 0.7% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 13.0 MiB | 12.9 MiB | 0.992 | 1.4% / 2.2% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 188.9 MiB | 190.1 MiB | 1.006 | 9.0% / 5.1% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 15.2 MiB | 15.4 MiB | 1.010 | 0.9% / 5.0% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 12.9 MiB | 13.0 MiB | 1.002 | 0.6% / 2.2% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 13.0 MiB | 1.007 | 3.1% / 1.7% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 13.1 MiB | 1.007 | 2.3% / 1.4% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 43.6 MiB | 45.7 MiB | 1.047 | 3.0% / 2.3% | MEM+ |
| quad/dsg/equi-exp/b0/d5/L12 | 18.4 MiB | 18.7 MiB | 1.017 | 2.9% / 3.4% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 28.9 MiB | 29.0 MiB | 1.002 | 4.1% / 1.9% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 30.0 MiB | 27.4 MiB | 0.912 | 8.2% / 8.5% | MEM- |
| quad/dsg/equi-exp/b1/d15/L7 | 51.7 MiB | 54.3 MiB | 1.051 | 0.4% / 0.3% | MEM+ |
| quad/dsg/equi-exp/b1/d20/L6 | 83.0 MiB | 86.4 MiB | 1.041 | 0.1% / 0.5% | MEM+ |
| quad/dsg/cc-uq/b0/d5/L12 | 18.4 MiB | 18.8 MiB | 1.021 | 1.0% / 3.4% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 28.9 MiB | 29.1 MiB | 1.008 | 6.7% / 7.7% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 30.0 MiB | 28.8 MiB | 0.959 | 3.7% / 2.7% | MEM- |
| interp/sg/cc/b0/d2/L12/p64 | 14.4 MiB | 14.5 MiB | 1.004 | 2.5% / 3.0% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.4 MiB | 12.4 MiB | 0.999 | 1.0% / 3.5% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.2 MiB | 12.3 MiB | 1.012 | 3.3% / 2.0% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.3 MiB | 12.3 MiB | 1.004 | 1.6% / 2.4% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.4 MiB | 1.007 | 1.2% / 2.2% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.3 MiB | 12.4 MiB | 1.002 | 3.2% / 1.7% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.3 MiB | 12.4 MiB | 1.003 | 3.2% / 1.1% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.2 MiB | 0.994 | 3.1% / 2.7% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 20.8 MiB | 20.2 MiB | 0.969 | 7.0% / 8.6% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.8 MiB | 12.9 MiB | 1.006 | 1.4% / 0.7% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.8 MiB | 1.003 | 2.7% / 1.4% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.7 MiB | 12.8 MiB | 1.008 | 2.3% / 1.0% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.3 MiB | 13.5 MiB | 1.011 | 0.9% / 1.4% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 14.9 MiB | 14.9 MiB | 0.999 | 1.0% / 2.9% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.3 MiB | 13.3 MiB | 1.001 | 2.1% / 2.2% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.6 MiB | 12.7 MiB | 1.006 | 1.6% / 1.5% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | 12.9 MiB | 13.1 MiB | 1.017 | 1.7% / 1.4% | ~ |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | 13.2 MiB | 13.2 MiB | 1.000 | 1.3% / 1.1% | ~ |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | 13.0 MiB | 13.1 MiB | 1.008 | 1.9% / 0.8% | ~ |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | 13.2 MiB | 13.3 MiB | 1.007 | 1.0% / 1.5% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 12.5 MiB | 12.5 MiB | 1.003 | 2.1% / 2.1% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | 12.7 MiB | 12.8 MiB | 1.009 | 2.0% / 1.3% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | 12.2 MiB | 12.4 MiB | 1.014 | 2.4% / 1.3% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | 12.4 MiB | 12.4 MiB | 1.001 | 0.9% / 1.9% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | 12.1 MiB | 12.2 MiB | 1.009 | 0.6% / 1.9% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | 12.3 MiB | 12.4 MiB | 1.002 | 3.8% / 2.6% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 12.6 MiB | 12.7 MiB | 1.004 | 0.9% / 2.2% | ~ |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | 12.3 MiB | 12.4 MiB | 1.006 | 2.9% / 2.1% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | 12.1 MiB | 12.1 MiB | 0.997 | 1.5% / 2.9% | ~ |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | 13.8 MiB | 13.9 MiB | 1.003 | 0.6% / 0.9% | ~ |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | 15.9 MiB | 16.2 MiB | 1.018 | 1.6% / 0.6% | ~ |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 13.1 MiB | 13.2 MiB | 1.005 | 1.9% / 1.3% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | 13.5 MiB | 13.6 MiB | 1.008 | 2.0% / 1.3% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | 12.7 MiB | 12.8 MiB | 1.006 | 3.6% / 1.2% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | 13.0 MiB | 13.1 MiB | 1.010 | 2.0% / 1.0% | ~ |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | 12.6 MiB | 12.6 MiB | 0.999 | 3.5% / 1.1% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | 14.2 MiB | 14.1 MiB | 0.998 | 5.8% / 4.3% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 20.7 MiB | 20.8 MiB | 1.006 | 1.3% / 1.8% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | 13.5 MiB | 13.5 MiB | 0.995 | 3.6% / 4.5% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | 13.4 MiB | 13.3 MiB | 0.998 | 1.4% / 2.6% | ~ |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | 59.5 MiB | 59.5 MiB | 1.000 | 0.7% / 0.4% | ~ |

Peak memory (common cases): geomean B/A = 1.004; largest B/A: `quad/dsg/equi-exp/b1/d15/L7` 51.7 MiB -> 54.3 MiB (1.051x); 3 MEM+, 2 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.1%, 0.1%, 0.1%, 0.2%
Round durations (s): 223, 223, 223, 223, 223 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1113s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
