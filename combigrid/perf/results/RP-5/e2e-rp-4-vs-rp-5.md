A/B `e2e`: A=`rp-4` B=`rp-5`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 435.72ms | 334.46ms | 1.310 | [1.174, 1.462] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 358.86ms | 295.70ms | 1.230 | [1.101, 1.374] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 29.57ms | 38.15ms | 0.787 | [0.707, 0.876] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | total | 264.44ms | 262.05ms | 1.011 | [1.000, 1.023] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 261.91ms | 260.38ms | 1.006 | [0.995, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.06ms | 1.49ms | 1.562 | [1.157, 2.108] | faster |
| quad/sg/equi-exp/b0/d7/L7 | total | 516.55ms | 512.93ms | 0.991 | [0.947, 1.037] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 513.55ms | 511.31ms | 0.989 | [0.945, 1.035] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.85ms | 1.37ms | 1.514 | [1.125, 2.038] | faster |
| quad/sg/equi-exp/b0/d10/L4 | total | 909.47ms | 914.12ms | 0.993 | [0.981, 1.005] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 907.34ms | 912.20ms | 0.993 | [0.980, 1.005] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.13ms | 1.96ms | 1.062 | [0.892, 1.265] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 299.98ms | 292.10ms | 1.018 | [1.010, 1.027] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 292.55ms | 289.78ms | 0.998 | [0.988, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.14ms | 1.51ms | 3.306 | [2.734, 3.998] | faster |
| quad/sg/equi-exp/b1/d15/L6 | total | 522.35ms | 502.61ms | 1.037 | [1.024, 1.050] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 496.34ms | 493.05ms | 1.006 | [0.992, 1.020] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.85ms | 6.53ms | 2.439 | [2.162, 2.751] | faster |
| quad/sg/equi-exp/b1/d20/L5 | total | 232.84ms | 212.64ms | 1.099 | [1.057, 1.142] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 202.73ms | 201.15ms | 1.018 | [0.990, 1.047] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.19ms | 8.38ms | 2.281 | [1.688, 3.083] | faster |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.404s | 2.405s | 0.999 | [0.991, 1.007] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.401s | 2.403s | 0.999 | [0.991, 1.007] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.91ms | 1.35ms | 1.469 | [1.177, 1.832] | faster |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.206s | 782.12ms | 1.560 | [1.470, 1.655] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.122s | 711.49ms | 1.599 | [1.485, 1.723] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 77.93ms | 70.59ms | 1.090 | [0.944, 1.260] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 86.90ms | 76.39ms | 1.133 | [1.116, 1.150] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.39ms | 75.14ms | 1.118 | [1.103, 1.132] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.05ms | 1.16ms | 1.848 | [1.614, 2.115] | faster |
| quad/sg/cc-uq/b0/d5/L10 | total | 322.67ms | 319.49ms | 1.011 | [1.005, 1.017] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 319.88ms | 317.70ms | 1.007 | [1.002, 1.013] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.28ms | 1.67ms | 1.368 | [1.277, 1.465] | faster |
| quad/sg/cc-uq/b0/d7/L7 | total | 693.32ms | 690.60ms | 1.003 | [0.996, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 690.71ms | 689.12ms | 1.001 | [0.995, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.98ms | 1.56ms | 1.326 | [1.201, 1.463] | faster |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.319s | 1.312s | 1.013 | [0.991, 1.035] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.317s | 1.309s | 1.013 | [0.991, 1.035] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.22ms | 2.09ms | 1.091 | [0.821, 1.449] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 96.34ms | 94.68ms | 1.011 | [1.002, 1.020] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 94.95ms | 94.04ms | 1.003 | [0.995, 1.012] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.09ms | 605.45us | 2.023 | [1.201, 3.407] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | total | 210.93ms | 211.45ms | 1.001 | [0.991, 1.011] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 208.19ms | 210.88ms | 0.991 | [0.982, 1.001] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.21ms | 472.75us | 2.538 | [1.967, 3.274] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.174s | 1.190s | 0.986 | [0.983, 0.990] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.166s | 1.189s | 0.980 | [0.977, 0.984] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.04ms | 178.51us | 17.159 | [16.142, 18.240] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.363s | 5.440s | 0.989 | [0.982, 0.996] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.354s | 5.439s | 0.988 | [0.981, 0.994] | SLOWER |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.19ms | 464.72us | 8.156 | [5.211, 12.766] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.224s | 1.168s | 1.049 | [1.041, 1.057] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.152s | 1.159s | 0.995 | [0.989, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.30ms | 4.08ms | 10.545 | [9.035, 12.308] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.216s | 1.138s | 1.067 | [1.063, 1.071] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.112s | 1.119s | 0.993 | [0.989, 0.997] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 63.22ms | 10.87ms | 5.825 | [5.654, 6.002] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 249.29ms | 248.06ms | 1.007 | [0.994, 1.019] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 245.41ms | 247.31ms | 0.994 | [0.982, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.23ms | 620.40us | 3.729 | [3.354, 4.145] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.443s | 1.446s | 0.997 | [0.994, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.432s | 1.446s | 0.991 | [0.987, 0.994] | SLOWER |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.95ms | 224.75us | 20.311 | [13.769, 29.962] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.840s | 6.897s | 0.979 | [0.953, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.831s | 6.896s | 0.978 | [0.952, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.64ms | 311.14us | 12.992 | [8.499, 19.858] | faster |
| interp/sg/cc/b0/d2/L12/p64 | total | 128.93ms | 127.67ms | 1.000 | [0.970, 1.030] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 128.67ms | 127.37ms | 1.000 | [0.970, 1.030] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 207.57us | 212.04us | 0.959 | [0.878, 1.047] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 186.38ms | 186.51ms | 0.998 | [0.978, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 186.24ms | 186.36ms | 0.998 | [0.978, 1.018] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.33us | 91.28us | 0.832 | [0.707, 0.979] | SLOWER |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.130s | 1.133s | 0.996 | [0.982, 1.010] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.130s | 1.133s | 0.996 | [0.982, 1.010] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 148.69us | 74.45us | 2.011 | [1.463, 2.766] | faster |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.008s | 1.005s | 1.003 | [0.991, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.008s | 1.005s | 1.003 | [0.991, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 89.12us | 63.41us | 1.382 | [1.104, 1.730] | faster |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.624s | 1.606s | 1.012 | [0.996, 1.029] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.624s | 1.606s | 1.012 | [0.996, 1.029] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 52.17us | 51.96us | 1.004 | [0.867, 1.163] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.30ms | 34.09ms | 1.008 | [0.999, 1.017] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 34.23ms | 34.00ms | 1.008 | [0.999, 1.017] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 26.40us | 34.08us | 0.789 | [0.551, 1.129] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 116.23ms | 116.43ms | 0.997 | [0.984, 1.010] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 116.12ms | 116.35ms | 0.997 | [0.984, 1.010] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 38.06us | 36.67us | 1.085 | [0.843, 1.397] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.89ms | 11.97ms | 0.926 | [0.738, 1.161] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.82ms | 11.90ms | 0.925 | [0.737, 1.162] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.79us | 31.92us | 0.688 | [0.550, 0.861] | SLOWER |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.45ms | 52.10ms | 1.005 | [0.993, 1.016] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 52.15ms | 51.73ms | 1.006 | [0.995, 1.018] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 134.93us | 232.22us | 0.593 | [0.567, 0.620] | SLOWER |
| interp/dsg/cc/b0/d3/L9/p64 | total | 43.10ms | 43.22ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 42.89ms | 43.06ms | 0.996 | [0.991, 1.000] | SLOWER |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 64.88us | 60.62us | 1.072 | [1.042, 1.103] | faster |
| interp/dsg/cc/b0/d5/L6/p64 | total | 170.24ms | 173.81ms | 0.982 | [0.967, 0.998] | SLOWER |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 169.99ms | 173.71ms | 0.981 | [0.966, 0.997] | SLOWER |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 76.69us | 49.87us | 1.615 | [1.360, 1.918] | faster |
| interp/dsg/cc/b0/d7/L4/p16 | total | 149.06ms | 151.07ms | 0.987 | [0.984, 0.991] | SLOWER |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 148.89ms | 150.99ms | 0.987 | [0.983, 0.990] | SLOWER |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 58.67us | 43.72us | 1.336 | [1.261, 1.416] | faster |
| interp/dsg/cc/b0/d10/L2/p8 | total | 392.37ms | 398.29ms | 0.975 | [0.950, 1.000] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 392.29ms | 398.23ms | 0.975 | [0.950, 1.000] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 32.82us | 33.80us | 0.973 | [0.905, 1.047] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.49ms | 6.52ms | 0.995 | [0.986, 1.003] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.42ms | 6.45ms | 0.993 | [0.985, 1.001] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 20.88us | 24.72us | 0.865 | [0.786, 0.951] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 108.57ms | 111.45ms | 0.979 | [0.943, 1.017] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 102.36ms | 104.75ms | 0.984 | [0.945, 1.026] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.68ms | 2.29ms | 0.726 | [0.667, 0.789] | SLOWER |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 358.31ms | 361.21ms | 0.988 | [0.963, 1.014] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 355.47ms | 360.67ms | 0.982 | [0.956, 1.008] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.13ms | 76.55us | 27.644 | [24.317, 31.427] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.198s | 1.212s | 0.994 | [0.961, 1.027] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.196s | 1.210s | 0.993 | [0.960, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.18ms | 2.11ms | 1.048 | [0.602, 1.824] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.856s | 1.842s | 1.007 | [0.987, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.853s | 1.841s | 1.007 | [0.987, 1.028] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.74ms | 1.42ms | 1.202 | [1.113, 1.299] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 185.45ms | 178.09ms | 1.063 | [0.983, 1.150] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 180.33ms | 172.89ms | 1.067 | [0.984, 1.157] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.16ms | 1.23ms | 0.927 | [0.838, 1.025] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 185.00ms | 179.45ms | 1.033 | [1.024, 1.042] | faster |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 182.11ms | 178.91ms | 1.019 | [1.010, 1.027] | faster |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 2.11ms | 82.20us | 28.199 | [17.878, 44.480] | faster |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 371.78ms | 369.56ms | 1.004 | [1.000, 1.008] | faster |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 368.67ms | 366.96ms | 1.004 | [1.000, 1.008] | faster |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.06ms | 2.06ms | 0.938 | [0.548, 1.604] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 433.72ms | 432.65ms | 1.004 | [0.992, 1.016] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 431.49ms | 430.79ms | 1.003 | [0.992, 1.015] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.48ms | 1.56ms | 0.933 | [0.687, 1.269] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 787.26ms | 767.47ms | 1.024 | [0.971, 1.081] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 783.54ms | 764.29ms | 1.024 | [0.970, 1.080] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.07ms | 2.67ms | 1.140 | [1.111, 1.171] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 524.47ms | 523.00ms | 1.006 | [0.895, 1.130] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 516.04ms | 515.30ms | 1.004 | [0.892, 1.130] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.62ms | 7.17ms | 1.053 | [1.037, 1.069] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.55ms | 26.13ms | 1.023 | [0.979, 1.069] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.85ms | 23.58ms | 1.019 | [0.969, 1.072] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.43ms | 2.37ms | 1.023 | [1.014, 1.031] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.16ms | 2.87ms | 0.900 | [0.620, 1.307] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.07ms | 1.97ms | 0.812 | [0.514, 1.285] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 871.82us | 754.07us | 1.088 | [0.876, 1.351] | ~ (noise) |

Sum of medians (total, common cases): A = 38.122s, B = 37.501s, saved = 620.91ms (1.017x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b0/d5/L10`: A 7.13e-14 -> B 7.17e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 620.1 MiB | 114.8 MiB | 0.185 | 0.1% / 19.1% | MEM- |
| quad/sg/equi-exp/b0/d5/L10 | 16.0 MiB | 12.6 MiB | 0.790 | 3.1% / 5.4% | MEM- |
| quad/sg/equi-exp/b0/d7/L7 | 14.7 MiB | 12.6 MiB | 0.860 | 6.4% / 1.8% | MEM- |
| quad/sg/equi-exp/b0/d10/L4 | 13.1 MiB | 12.9 MiB | 0.987 | 3.1% / 2.4% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 26.1 MiB | 13.3 MiB | 0.509 | 2.4% / 1.4% | MEM- |
| quad/sg/equi-exp/b1/d15/L6 | 53.6 MiB | 18.4 MiB | 0.344 | 7.5% / 1.4% | MEM- |
| quad/sg/equi-exp/b1/d20/L5 | 60.0 MiB | 20.2 MiB | 0.337 | 12.8% / 0.9% | MEM- |
| quad/sg/equi-sin/b0/d7/L7 | 14.7 MiB | 12.8 MiB | 0.867 | 1.1% / 4.1% | MEM- |
| quad/sg/cc-uq/b0/d2/L20 | 1074.3 MiB | 285.9 MiB | 0.266 | 4.8% / 19.2% | MEM- |
| quad/sg/cc-uq/b0/d3/L14 | 40.7 MiB | 17.4 MiB | 0.428 | 1.8% / 4.9% | MEM- |
| quad/sg/cc-uq/b0/d5/L10 | 17.8 MiB | 13.1 MiB | 0.735 | 2.9% / 1.5% | MEM- |
| quad/sg/cc-uq/b0/d7/L7 | 15.2 MiB | 13.0 MiB | 0.855 | 3.6% / 1.2% | MEM- |
| quad/sg/cc-uq/b0/d10/L4 | 13.1 MiB | 12.9 MiB | 0.983 | 2.1% / 2.5% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.9 MiB | 43.8 MiB | 0.697 | 1.6% / 1.9% | MEM- |
| quad/dsg/equi-exp/b0/d5/L12 | 26.3 MiB | 17.9 MiB | 0.680 | 2.2% / 2.8% | MEM- |
| quad/dsg/equi-exp/b0/d7/L10 | 42.2 MiB | 28.3 MiB | 0.672 | 5.3% / 4.6% | MEM- |
| quad/dsg/equi-exp/b0/d10/L7 | 42.1 MiB | 29.8 MiB | 0.707 | 2.0% / 2.3% | MEM- |
| quad/dsg/equi-exp/b1/d15/L7 | 151.0 MiB | 51.7 MiB | 0.342 | 1.8% / 0.5% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 217.7 MiB | 82.9 MiB | 0.381 | 0.1% / 0.2% | MEM- |
| quad/dsg/cc-uq/b0/d5/L12 | 26.2 MiB | 18.1 MiB | 0.692 | 2.1% / 3.1% | MEM- |
| quad/dsg/cc-uq/b0/d7/L10 | 42.4 MiB | 28.6 MiB | 0.674 | 8.1% / 1.8% | MEM- |
| quad/dsg/cc-uq/b0/d10/L7 | 43.4 MiB | 29.9 MiB | 0.689 | 2.9% / 5.4% | MEM- |
| interp/sg/cc/b0/d2/L12/p64 | 15.9 MiB | 14.6 MiB | 0.917 | 3.7% / 2.4% | MEM- |
| interp/sg/cc/b0/d3/L9/p64 | 12.2 MiB | 12.4 MiB | 1.021 | 3.5% / 2.1% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.3 MiB | 12.3 MiB | 1.003 | 2.8% / 2.3% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.2 MiB | 12.2 MiB | 1.001 | 1.8% / 2.3% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.1 MiB | 12.2 MiB | 1.014 | 2.3% / 1.8% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.2 MiB | 12.3 MiB | 1.010 | 3.0% / 3.0% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.2 MiB | 12.2 MiB | 1.001 | 1.3% / 1.5% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.3 MiB | 1.003 | 1.1% / 1.2% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 16.2 MiB | 15.2 MiB | 0.940 | 3.7% / 3.9% | MEM- |
| interp/dsg/cc/b0/d3/L9/p64 | 12.6 MiB | 12.6 MiB | 0.996 | 2.7% / 1.6% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.5 MiB | 12.3 MiB | 0.986 | 3.8% / 1.9% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.4 MiB | 12.4 MiB | 0.994 | 2.5% / 1.6% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.3 MiB | 1.001 | 4.1% / 2.2% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.1 MiB | 12.3 MiB | 1.019 | 2.1% / 1.2% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.8 MiB | 20.9 MiB | 1.052 | 2.1% / 9.9% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.3 MiB | 12.9 MiB | 0.969 | 1.5% / 2.0% | MEM- |
| interplin/sg/equi/b1/d8/L5/p256 | 12.9 MiB | 12.7 MiB | 0.983 | 3.1% / 2.5% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.5 MiB | 12.7 MiB | 0.940 | 7.3% / 2.5% | MEM- |
| interplin/dsg/equi/b1/d2/L16/p16384 | 20.6 MiB | 21.5 MiB | 1.048 | 1.9% / 2.3% | MEM+ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.0 MiB | 13.0 MiB | 1.002 | 1.5% / 4.3% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 13.0 MiB | 12.7 MiB | 0.977 | 5.4% / 3.8% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 17.1 MiB | 15.3 MiB | 0.894 | 2.2% / 3.1% | MEM- |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.2 MiB | 13.5 MiB | 0.836 | 4.3% / 1.6% | MEM- |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.7 MiB | 15.0 MiB | 0.847 | 2.8% / 3.4% | MEM- |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.1 MiB | 13.2 MiB | 1.012 | 2.6% / 1.1% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.4 MiB | 12.7 MiB | 1.023 | 3.0% / 1.8% | ~ |

Peak memory (common cases): geomean B/A = 0.764; largest B/A: `interplin/sg/equi/b1/d2/L16/p16384` 19.8 MiB -> 20.9 MiB (1.052x); 1 MEM+, 27 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.7%, 0.2%, 0.1%, 0.1%, 0.1%
Round durations (s): 163, 162, 162, 162, 162 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 811s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
