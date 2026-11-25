A/B `e2e`: A=`WP-77-before` B=`WP-77-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 309.05ms | 279.43ms | 1.124 | [1.007, 1.254] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 269.15ms | 242.08ms | 1.132 | [1.013, 1.265] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 39.15ms | 37.32ms | 1.028 | [0.924, 1.145] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 248.09ms | 246.26ms | 1.010 | [0.999, 1.022] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 246.19ms | 244.52ms | 1.008 | [0.997, 1.020] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.58ms | 1.64ms | 1.285 | [0.827, 1.996] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 496.86ms | 495.53ms | 1.002 | [0.987, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 495.25ms | 493.78ms | 1.001 | [0.986, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.47ms | 1.43ms | 1.060 | [0.524, 2.147] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 911.87ms | 897.23ms | 1.014 | [1.007, 1.020] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 910.23ms | 895.27ms | 1.015 | [1.008, 1.021] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.00ms | 2.00ms | 0.813 | [0.415, 1.592] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 241.66ms | 241.32ms | 1.002 | [0.993, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 239.56ms | 239.05ms | 1.002 | [0.993, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.44ms | 1.44ms | 0.996 | [0.824, 1.204] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 491.83ms | 490.14ms | 1.006 | [0.994, 1.019] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 482.58ms | 481.08ms | 1.006 | [0.992, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.34ms | 6.23ms | 1.027 | [0.916, 1.150] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 215.95ms | 212.68ms | 1.018 | [1.005, 1.031] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 204.36ms | 201.12ms | 1.019 | [1.013, 1.025] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.13ms | 8.08ms | 1.008 | [0.746, 1.362] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.395s | 2.381s | 1.009 | [1.000, 1.018] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.394s | 2.379s | 1.009 | [1.000, 1.018] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.41ms | 1.51ms | 0.952 | [0.857, 1.058] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 466.78ms | 469.34ms | 0.993 | [0.904, 1.090] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 377.23ms | 382.08ms | 0.996 | [0.880, 1.127] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 80.99ms | 80.68ms | 1.009 | [0.873, 1.166] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 75.37ms | 71.99ms | 1.049 | [1.034, 1.064] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.08ms | 70.69ms | 1.049 | [1.037, 1.063] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.26ms | 1.28ms | 0.983 | [0.960, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 321.65ms | 319.10ms | 1.004 | [0.991, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 319.28ms | 317.05ms | 1.004 | [0.991, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.69ms | 1.70ms | 0.979 | [0.850, 1.128] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 688.07ms | 684.76ms | 1.005 | [1.000, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 686.59ms | 682.66ms | 1.005 | [1.000, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.49ms | 1.50ms | 0.979 | [0.809, 1.187] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.308s | 1.299s | 1.008 | [1.002, 1.014] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.304s | 1.295s | 1.007 | [1.001, 1.014] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.34ms | 2.13ms | 1.103 | [0.684, 1.777] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.30ms | 84.76ms | 1.102 | [1.090, 1.115] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 90.38ms | 81.29ms | 1.111 | [1.106, 1.115] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 489.31us | 1.04ms | 0.740 | [0.366, 1.498] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 202.74ms | 196.83ms | 1.029 | [1.024, 1.034] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 201.91ms | 195.93ms | 1.029 | [1.025, 1.033] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 484.00us | 557.96us | 0.892 | [0.692, 1.151] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.149s | 1.134s | 1.013 | [1.009, 1.017] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.148s | 1.134s | 1.013 | [1.009, 1.017] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 179.49us | 293.19us | 0.516 | [0.366, 0.727] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.405s | 5.370s | 1.003 | [0.995, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.404s | 5.369s | 1.003 | [0.995, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 242.70us | 959.47us | 0.274 | [0.109, 0.686] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.163s | 1.175s | 0.989 | [0.985, 0.993] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.149s | 1.158s | 0.991 | [0.987, 0.996] | SLOWER |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 4.06ms | 6.56ms | 0.616 | [0.528, 0.719] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.136s | 1.162s | 0.977 | [0.972, 0.983] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.109s | 1.130s | 0.981 | [0.976, 0.985] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 11.15ms | 14.66ms | 0.747 | [0.718, 0.776] | SLOWER |
| quad/dsg/cc-uq/b0/d5/L12 | total | 247.48ms | 239.82ms | 1.032 | [1.028, 1.036] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 246.26ms | 238.67ms | 1.032 | [1.028, 1.036] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 655.39us | 721.32us | 0.921 | [0.686, 1.237] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.451s | 1.436s | 1.008 | [0.994, 1.023] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.450s | 1.435s | 1.008 | [0.994, 1.023] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 220.91us | 423.24us | 0.485 | [0.337, 0.697] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.875s | 6.815s | 1.009 | [1.003, 1.015] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.873s | 6.813s | 1.009 | [1.004, 1.013] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 301.71us | 521.22us | 0.512 | [0.262, 1.001] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | total | 128.66ms | 84.29ms | 1.469 | [1.355, 1.593] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 128.41ms | 84.03ms | 1.471 | [1.357, 1.595] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 213.85us | 217.28us | 0.987 | [0.904, 1.078] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 185.74ms | 167.21ms | 1.110 | [1.088, 1.133] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 185.60ms | 167.05ms | 1.110 | [1.088, 1.133] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 87.93us | 91.56us | 0.959 | [0.891, 1.033] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.134s | 1.121s | 1.015 | [1.007, 1.023] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.134s | 1.121s | 1.015 | [1.007, 1.023] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 66.77us | 70.68us | 0.892 | [0.711, 1.118] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.008s | 985.91ms | 1.021 | [1.008, 1.034] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.008s | 985.81ms | 1.021 | [1.008, 1.034] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 61.11us | 63.83us | 0.896 | [0.791, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.603s | 1.618s | 0.996 | [0.987, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.603s | 1.618s | 0.996 | [0.987, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 52.52us | 50.63us | 1.025 | [0.930, 1.130] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.01ms | 22.90ms | 1.485 | [1.473, 1.498] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 33.93ms | 22.82ms | 1.487 | [1.474, 1.500] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 43.02us | 35.55us | 1.255 | [0.877, 1.796] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 117.13ms | 109.09ms | 1.073 | [1.059, 1.087] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 117.06ms | 109.01ms | 1.073 | [1.059, 1.087] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 30.73us | 40.58us | 0.811 | [0.630, 1.044] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.87ms | 8.21ms | 1.401 | [1.216, 1.613] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.78ms | 8.14ms | 1.404 | [1.219, 1.617] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 33.59us | 28.15us | 1.182 | [0.884, 1.580] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 101.36ms | 101.49ms | 0.998 | [0.961, 1.037] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 94.49ms | 94.81ms | 0.996 | [0.955, 1.038] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.43ms | 2.27ms | 1.111 | [1.000, 1.233] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 363.56ms | 361.60ms | 1.012 | [0.983, 1.042] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 363.03ms | 361.07ms | 1.012 | [0.983, 1.042] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 81.30us | 88.70us | 0.914 | [0.804, 1.039] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.193s | 1.203s | 0.990 | [0.968, 1.013] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.189s | 1.200s | 0.990 | [0.968, 1.012] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 3.74ms | 2.14ms | 1.231 | [0.721, 2.101] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.837s | 1.850s | 1.000 | [0.979, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.835s | 1.848s | 1.000 | [0.979, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.45ms | 1.49ms | 0.860 | [0.579, 1.277] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 692.95ms | 663.63ms | 1.043 | [0.989, 1.100] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 689.76ms | 660.43ms | 1.043 | [0.989, 1.101] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.67ms | 2.65ms | 1.012 | [0.987, 1.038] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 376.10ms | 321.88ms | 1.175 | [1.156, 1.194] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 368.46ms | 314.17ms | 1.179 | [1.152, 1.208] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.09ms | 7.14ms | 0.990 | [0.978, 1.003] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.26ms | 19.64ms | 1.338 | [1.318, 1.360] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.72ms | 17.10ms | 1.388 | [1.319, 1.460] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.39ms | 2.38ms | 1.008 | [0.997, 1.020] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.07ms | 2.13ms | 1.475 | [1.214, 1.792] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.15ms | 1.22ms | 1.717 | [1.576, 1.870] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 756.79us | 763.57us | 1.138 | [0.649, 1.993] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | 1.535s | 1.531s | 1.003 | [0.995, 1.010] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | operator | 1.533s | 1.529s | 1.003 | [0.995, 1.010] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | generation | 1.96ms | 2.13ms | 1.044 | [0.656, 1.659] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | total | 570.72ms | 569.32ms | 1.003 | [0.997, 1.008] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | operator | 568.46ms | 567.10ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | generation | 2.14ms | 2.15ms | 1.138 | [0.805, 1.610] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | 1.037s | 1.037s | 1.001 | [0.986, 1.016] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | operator | 1.035s | 1.033s | 1.002 | [0.988, 1.016] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | generation | 2.10ms | 3.31ms | 0.695 | [0.408, 1.184] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | total | 520.51ms | 520.00ms | 1.000 | [0.976, 1.025] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | operator | 518.45ms | 517.45ms | 1.002 | [0.981, 1.023] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | generation | 1.90ms | 2.07ms | 0.765 | [0.343, 1.707] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | 2.158s | 1.564s | 1.380 | [1.375, 1.385] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | operator | 2.157s | 1.563s | 1.380 | [1.375, 1.385] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.16ms | 1.15ms | 1.001 | [0.984, 1.017] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | total | 881.21ms | 551.26ms | 1.598 | [1.592, 1.604] | faster |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | operator | 879.89ms | 549.94ms | 1.600 | [1.594, 1.606] | faster |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.12ms | 1.13ms | 0.987 | [0.953, 1.022] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | total | 1.073s | 1.028s | 1.044 | [1.029, 1.058] | faster |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | operator | 1.073s | 1.027s | 1.044 | [1.029, 1.058] | faster |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | generation | 62.44us | 73.96us | 0.916 | [0.779, 1.077] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | total | 502.88ms | 502.86ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | operator | 502.80ms | 502.78ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | generation | 44.98us | 49.24us | 0.922 | [0.829, 1.025] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | total | 828.58ms | 827.38ms | 0.996 | [0.969, 1.024] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | operator | 828.51ms | 827.30ms | 0.996 | [0.969, 1.024] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | generation | 36.67us | 55.59us | 0.692 | [0.608, 0.787] | SLOWER |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | total | 345.20ms | 345.47ms | 0.998 | [0.986, 1.011] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | operator | 345.14ms | 345.41ms | 0.998 | [0.986, 1.011] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | generation | 34.85us | 38.20us | 0.892 | [0.686, 1.160] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | 2.476s | 1.846s | 1.342 | [1.337, 1.347] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | operator | 2.475s | 1.844s | 1.342 | [1.337, 1.347] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | generation | 1.16ms | 1.15ms | 1.005 | [0.997, 1.014] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | total | 398.09ms | 331.87ms | 1.199 | [1.190, 1.208] | faster |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | operator | 398.03ms | 331.82ms | 1.199 | [1.190, 1.208] | faster |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | generation | 37.57us | 37.57us | 0.988 | [0.816, 1.197] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | total | 1.289s | 1.288s | 1.002 | [0.970, 1.035] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | operator | 1.289s | 1.288s | 1.002 | [0.970, 1.035] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | generation | 59.16us | 55.38us | 1.041 | [0.911, 1.190] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | 1.153s | 1.153s | 0.999 | [0.979, 1.020] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | operator | 1.150s | 1.151s | 1.000 | [0.979, 1.020] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | generation | 1.80ms | 1.81ms | 0.998 | [0.971, 1.025] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | total | 349.91ms | 351.54ms | 0.995 | [0.987, 1.004] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | operator | 348.63ms | 350.17ms | 0.996 | [0.992, 0.999] | SLOWER |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | generation | 334.96us | 438.88us | 0.766 | [0.721, 0.813] | SLOWER |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | 664.36ms | 494.50ms | 1.343 | [1.311, 1.375] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | operator | 660.82ms | 490.98ms | 1.345 | [1.314, 1.378] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.35ms | 3.36ms | 0.979 | [0.920, 1.041] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | total | 333.69ms | 314.12ms | 1.061 | [1.057, 1.065] | faster |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | operator | 330.08ms | 310.52ms | 1.062 | [1.058, 1.066] | faster |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.22ms | 3.26ms | 0.990 | [0.983, 0.998] | SLOWER |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | total | 257.77ms | 252.69ms | 1.020 | [0.836, 1.245] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | operator | 255.52ms | 250.42ms | 1.020 | [0.841, 1.238] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | generation | 2.13ms | 2.10ms | 1.019 | [0.619, 1.677] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | total | 98.70ms | 98.91ms | 0.975 | [0.915, 1.039] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | operator | 96.77ms | 96.80ms | 0.976 | [0.914, 1.042] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | generation | 1.94ms | 2.04ms | 0.942 | [0.858, 1.034] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | total | 1.072s | 1.071s | 1.003 | [0.889, 1.133] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | operator | 1.070s | 1.069s | 1.003 | [0.888, 1.134] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | generation | 3.48ms | 2.07ms | 1.093 | [0.413, 2.892] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | total | 208.49ms | 208.27ms | 1.001 | [0.989, 1.013] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | operator | 205.92ms | 205.72ms | 1.002 | [0.995, 1.008] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | generation | 2.10ms | 2.13ms | 0.969 | [0.324, 2.897] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | 577.19ms | 620.79ms | 0.929 | [0.926, 0.933] | SLOWER |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | operator | 572.62ms | 617.56ms | 0.927 | [0.924, 0.931] | SLOWER |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | generation | 2.14ms | 1.93ms | 1.420 | [0.895, 2.253] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | total | 70.77ms | 72.15ms | 0.982 | [0.972, 0.992] | SLOWER |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | operator | 70.68ms | 72.01ms | 0.982 | [0.972, 0.992] | SLOWER |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | generation | 42.81us | 46.23us | 0.978 | [0.796, 1.202] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | total | 243.39ms | 243.41ms | 0.998 | [0.910, 1.095] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | operator | 243.25ms | 243.31ms | 0.998 | [0.910, 1.095] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | generation | 35.76us | 37.57us | 0.922 | [0.793, 1.071] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | total | 154.38ms | 153.19ms | 1.011 | [0.982, 1.040] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | operator | 145.38ms | 144.38ms | 1.010 | [0.980, 1.041] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | generation | 57.34us | 62.93us | 0.915 | [0.880, 0.951] | SLOWER |

Sum of medians (total, common cases, group suite): A = 34.708s, B = 34.343s, saved = 365.31ms (1.011x)
Sum of medians (total, common cases, group expensive/): A = 17.754s, B = 15.886s, saved = 1.868s (1.118x)
Sum of medians (total, common cases, group expensive/ cached): A = 1.046s, B = 1.090s, saved = -43799292.0ns (0.960x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d2/L20`: A 7.23e-14 -> B 7.23e-14 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 115.3 MiB | 119.4 MiB | 1.036 | 24.8% / 19.7% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.8 MiB | 12.9 MiB | 1.011 | 1.7% / 2.4% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.8 MiB | 13.0 MiB | 1.014 | 2.1% / 1.7% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.9 MiB | 13.0 MiB | 1.004 | 2.8% / 2.1% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.3 MiB | 13.4 MiB | 1.005 | 0.9% / 1.4% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.3 MiB | 18.4 MiB | 1.005 | 2.2% / 1.5% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.3 MiB | 20.3 MiB | 1.001 | 1.6% / 1.2% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.9 MiB | 12.9 MiB | 0.997 | 2.9% / 4.7% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 191.9 MiB | 201.1 MiB | 1.048 | 6.1% / 10.1% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 15.1 MiB | 15.2 MiB | 1.006 | 5.6% / 6.1% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 12.9 MiB | 13.0 MiB | 1.006 | 0.8% / 2.2% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 12.9 MiB | 1.004 | 1.6% / 2.9% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 13.1 MiB | 1.004 | 2.3% / 3.7% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 43.6 MiB | 45.3 MiB | 1.040 | 1.4% / 1.7% | MEM+ |
| quad/dsg/equi-exp/b0/d5/L12 | 18.2 MiB | 19.3 MiB | 1.062 | 2.6% / 2.3% | MEM+ |
| quad/dsg/equi-exp/b0/d7/L10 | 29.3 MiB | 28.8 MiB | 0.983 | 5.1% / 2.1% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 30.0 MiB | 31.0 MiB | 1.034 | 4.5% / 4.3% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 51.7 MiB | 54.4 MiB | 1.053 | 0.2% / 0.4% | MEM+ |
| quad/dsg/equi-exp/b1/d20/L6 | 82.9 MiB | 86.4 MiB | 1.043 | 0.3% / 0.2% | MEM+ |
| quad/dsg/cc-uq/b0/d5/L12 | 18.4 MiB | 19.3 MiB | 1.048 | 0.9% / 1.8% | MEM+ |
| quad/dsg/cc-uq/b0/d7/L10 | 28.7 MiB | 29.1 MiB | 1.014 | 5.5% / 3.2% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 29.8 MiB | 31.3 MiB | 1.049 | 2.0% / 6.8% | ~ |
| interp/sg/cc/b0/d2/L12/p64 | 14.6 MiB | 14.5 MiB | 0.998 | 2.5% / 1.4% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.3 MiB | 12.4 MiB | 1.010 | 1.2% / 1.0% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.3 MiB | 12.3 MiB | 1.002 | 1.2% / 1.6% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.3 MiB | 12.3 MiB | 1.000 | 2.5% / 0.7% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.2 MiB | 12.4 MiB | 1.014 | 1.4% / 2.9% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.3 MiB | 12.4 MiB | 1.006 | 2.7% / 1.8% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.4 MiB | 12.3 MiB | 0.990 | 2.4% / 4.7% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.5 MiB | 1.018 | 1.8% / 1.2% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.9 MiB | 20.5 MiB | 1.031 | 8.0% / 5.9% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.8 MiB | 12.9 MiB | 1.007 | 1.2% / 2.8% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.6 MiB | 12.8 MiB | 1.014 | 1.1% / 2.9% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.6 MiB | 12.8 MiB | 1.009 | 3.2% / 1.7% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.3 MiB | 13.5 MiB | 1.010 | 0.9% / 0.2% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 15.1 MiB | 14.9 MiB | 0.993 | 1.4% / 1.4% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.3 MiB | 13.3 MiB | 1.004 | 1.3% / 1.6% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.7 MiB | 12.7 MiB | 1.002 | 1.1% / 1.3% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | 13.0 MiB | 13.0 MiB | 1.003 | 2.5% / 3.0% | ~ |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | 13.3 MiB | 13.2 MiB | 0.994 | 1.3% / 3.3% | ~ |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | 13.1 MiB | 13.0 MiB | 0.996 | 1.7% / 0.8% | ~ |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | 13.2 MiB | 13.3 MiB | 1.005 | 1.9% / 1.6% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 12.5 MiB | 12.6 MiB | 1.005 | 1.8% / 1.7% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | 12.7 MiB | 12.8 MiB | 1.006 | 1.2% / 1.0% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | 12.2 MiB | 12.3 MiB | 1.011 | 2.5% / 2.0% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | 12.4 MiB | 12.5 MiB | 1.006 | 1.9% / 0.7% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | 12.1 MiB | 12.2 MiB | 1.006 | 1.3% / 2.5% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | 12.3 MiB | 12.4 MiB | 1.002 | 1.3% / 1.9% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 12.6 MiB | 12.7 MiB | 1.008 | 2.2% / 0.8% | ~ |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | 12.2 MiB | 12.4 MiB | 1.011 | 1.9% / 2.0% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | 12.1 MiB | 12.2 MiB | 1.003 | 1.1% / 1.2% | ~ |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | 13.8 MiB | 13.8 MiB | 0.998 | 1.3% / 0.7% | ~ |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | 16.0 MiB | 16.1 MiB | 1.007 | 1.0% / 0.8% | ~ |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 13.1 MiB | 13.2 MiB | 1.005 | 2.1% / 0.9% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | 13.5 MiB | 13.5 MiB | 0.996 | 0.9% / 1.6% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | 12.7 MiB | 12.9 MiB | 1.014 | 1.6% / 1.7% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | 13.0 MiB | 13.1 MiB | 1.013 | 2.9% / 0.6% | ~ |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | 12.7 MiB | 12.8 MiB | 1.013 | 0.9% / 4.5% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | 14.3 MiB | 14.0 MiB | 0.981 | 2.4% / 2.3% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 20.7 MiB | 21.1 MiB | 1.022 | 2.1% / 4.9% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | 13.6 MiB | 13.7 MiB | 1.014 | 8.7% / 4.0% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | 13.3 MiB | 13.3 MiB | 0.998 | 1.5% / 2.5% | ~ |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | 59.5 MiB | 59.3 MiB | 0.998 | 0.6% / 0.6% | ~ |

Peak memory (common cases): geomean B/A = 1.011; largest B/A: `quad/dsg/equi-exp/b0/d5/L12` 18.2 MiB -> 19.3 MiB (1.062x); 5 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.1%, 0.1%, 0.1%, 0.1%
Round durations (s): 223, 222, 223, 223, 223 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1113s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
