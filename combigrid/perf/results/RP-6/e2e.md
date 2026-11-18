A/B `e2e`: A=`baseline` B=`rp-6`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 729.78ms | 318.51ms | 2.252 | [2.018, 2.513] | faster |
| quad/sg/equi-exp/b0/d2/L20 | operator | 690.61ms | 280.15ms | 2.426 | [2.171, 2.710] | faster |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.95ms | 38.33ms | 0.860 | [0.772, 0.957] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | total | 704.81ms | 248.19ms | 2.837 | [2.804, 2.870] | faster |
| quad/sg/equi-exp/b0/d5/L10 | operator | 702.15ms | 246.18ms | 2.852 | [2.820, 2.885] | faster |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.12ms | 1.90ms | 1.029 | [0.656, 1.613] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 1.639s | 497.70ms | 3.294 | [3.260, 3.328] | faster |
| quad/sg/equi-exp/b0/d7/L7 | operator | 1.637s | 496.15ms | 3.304 | [3.270, 3.339] | faster |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.97ms | 1.55ms | 0.949 | [0.517, 1.744] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 3.304s | 903.87ms | 3.663 | [3.638, 3.689] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 3.301s | 901.84ms | 3.669 | [3.643, 3.695] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.28ms | 2.08ms | 1.141 | [0.959, 1.359] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 1.110s | 241.36ms | 4.589 | [4.432, 4.752] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 1.100s | 239.01ms | 4.594 | [4.433, 4.761] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.73ms | 1.56ms | 3.556 | [2.940, 4.300] | faster |
| quad/sg/equi-exp/b1/d15/L6 | total | 2.367s | 490.66ms | 4.826 | [4.765, 4.887] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 2.342s | 481.27ms | 4.867 | [4.799, 4.936] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 17.15ms | 6.69ms | 2.550 | [2.276, 2.856] | faster |
| quad/sg/equi-exp/b1/d20/L5 | total | 1.114s | 213.28ms | 5.159 | [4.974, 5.351] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 1.082s | 201.37ms | 5.324 | [5.161, 5.493] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 22.00ms | 8.56ms | 2.239 | [1.657, 3.026] | faster |
| quad/sg/equi-sin/b0/d7/L7 | total | 3.523s | 2.398s | 1.469 | [1.457, 1.481] | faster |
| quad/sg/equi-sin/b0/d7/L7 | operator | 3.521s | 2.395s | 1.469 | [1.457, 1.481] | faster |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.92ms | 1.44ms | 1.157 | [0.822, 1.630] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.417s | 432.30ms | 3.264 | [3.114, 3.420] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.320s | 357.93ms | 3.681 | [3.571, 3.795] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 86.52ms | 70.14ms | 1.128 | [0.911, 1.396] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 179.05ms | 75.77ms | 2.393 | [2.278, 2.514] | faster |
| quad/sg/cc-uq/b0/d3/L14 | operator | 176.88ms | 74.51ms | 2.408 | [2.292, 2.529] | faster |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.06ms | 1.26ms | 1.611 | [1.518, 1.709] | faster |
| quad/sg/cc-uq/b0/d5/L10 | total | 779.08ms | 321.52ms | 2.426 | [2.415, 2.438] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 776.16ms | 319.77ms | 2.430 | [2.419, 2.442] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.37ms | 1.69ms | 1.391 | [1.298, 1.490] | faster |
| quad/sg/cc-uq/b0/d7/L7 | total | 1.813s | 687.25ms | 2.634 | [2.609, 2.659] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 1.810s | 685.62ms | 2.636 | [2.611, 2.662] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.09ms | 1.53ms | 1.353 | [1.255, 1.459] | faster |
| quad/sg/cc-uq/b0/d10/L4 | total | 3.621s | 1.306s | 2.772 | [2.746, 2.799] | faster |
| quad/sg/cc-uq/b0/d10/L4 | operator | 3.616s | 1.302s | 2.775 | [2.748, 2.802] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.52ms | 2.04ms | 1.349 | [0.906, 2.008] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 106.78ms | 94.26ms | 1.138 | [1.117, 1.159] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 105.19ms | 91.06ms | 1.162 | [1.139, 1.185] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.20ms | 844.72us | 1.675 | [0.942, 2.976] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 223.57ms | 202.95ms | 1.103 | [1.097, 1.108] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 220.68ms | 202.14ms | 1.093 | [1.089, 1.097] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.66ms | 475.55us | 3.398 | [2.634, 4.384] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.230s | 1.145s | 1.076 | [1.071, 1.080] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.223s | 1.144s | 1.069 | [1.065, 1.074] | faster |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.70ms | 181.38us | 20.081 | [19.220, 20.981] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.664s | 5.406s | 1.046 | [1.038, 1.054] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.654s | 5.404s | 1.045 | [1.037, 1.052] | faster |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 4.88ms | 468.00us | 12.778 | [8.325, 19.613] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | total | 3.975s | 1.162s | 3.417 | [3.405, 3.430] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 3.902s | 1.148s | 3.394 | [3.382, 3.407] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.26ms | 4.09ms | 10.970 | [9.398, 12.804] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.764s | 1.135s | 5.956 | [5.933, 5.978] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.654s | 1.107s | 6.007 | [5.985, 6.030] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 73.67ms | 11.33ms | 6.056 | [5.158, 7.110] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | total | 269.94ms | 247.43ms | 1.092 | [1.084, 1.100] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 265.79ms | 246.38ms | 1.080 | [1.071, 1.088] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.60ms | 633.95us | 4.054 | [3.580, 4.590] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.518s | 1.450s | 1.048 | [1.044, 1.052] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.508s | 1.449s | 1.041 | [1.037, 1.045] | faster |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 5.00ms | 221.88us | 22.061 | [19.264, 25.266] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.993s | 6.859s | 1.021 | [1.010, 1.032] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.984s | 6.858s | 1.020 | [1.009, 1.031] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.24ms | 651.12us | 9.853 | [5.363, 18.103] | faster |
| interp/sg/cc/b0/d2/L12/p64 | total | 235.31ms | 128.30ms | 1.808 | [1.718, 1.903] | faster |
| interp/sg/cc/b0/d2/L12/p64 | operator | 234.89ms | 128.05ms | 1.809 | [1.719, 1.904] | faster |
| interp/sg/cc/b0/d2/L12/p64 | generation | 214.34us | 217.97us | 0.988 | [0.905, 1.079] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 308.71ms | 185.80ms | 1.670 | [1.637, 1.705] | faster |
| interp/sg/cc/b0/d3/L9/p64 | operator | 308.55ms | 185.66ms | 1.671 | [1.637, 1.705] | faster |
| interp/sg/cc/b0/d3/L9/p64 | generation | 78.15us | 88.63us | 0.886 | [0.823, 0.954] | SLOWER |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.739s | 1.134s | 1.535 | [1.522, 1.547] | faster |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.738s | 1.134s | 1.534 | [1.522, 1.547] | faster |
| interp/sg/cc/b0/d5/L6/p64 | generation | 137.80us | 71.45us | 1.882 | [1.500, 2.360] | faster |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.469s | 1.011s | 1.453 | [1.435, 1.472] | faster |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.468s | 1.011s | 1.453 | [1.435, 1.472] | faster |
| interp/sg/cc/b0/d7/L4/p16 | generation | 79.20us | 72.08us | 1.094 | [0.949, 1.261] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.239s | 1.609s | 1.391 | [1.375, 1.407] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.239s | 1.609s | 1.391 | [1.375, 1.407] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 39.18us | 38.69us | 2.369 | [0.533, 10.529] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 187.16ms | 34.24ms | 5.470 | [5.401, 5.540] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 187.06ms | 34.16ms | 5.481 | [5.412, 5.551] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.01us | 41.35us | 0.705 | [0.493, 1.009] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 368.84ms | 118.54ms | 3.120 | [3.076, 3.164] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 368.72ms | 118.44ms | 3.121 | [3.077, 3.166] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 40.44us | 40.37us | 0.972 | [0.756, 1.251] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.22ms | 11.82ms | 4.342 | [3.864, 4.878] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.15ms | 11.75ms | 4.362 | [3.880, 4.904] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 22.84us | 33.38us | 0.710 | [0.604, 0.834] | SLOWER |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 111.89ms | 111.79ms | 0.999 | [0.962, 1.037] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 105.64ms | 105.10ms | 1.003 | [0.962, 1.045] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.75ms | 2.32ms | 0.765 | [0.703, 0.831] | SLOWER |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 496.99ms | 360.65ms | 1.403 | [1.349, 1.459] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 493.04ms | 360.12ms | 1.396 | [1.341, 1.453] | faster |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.23ms | 79.69us | 30.648 | [22.802, 41.192] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.671s | 1.205s | 1.382 | [1.351, 1.414] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.668s | 1.203s | 1.383 | [1.353, 1.414] | faster |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.26ms | 2.19ms | 0.872 | [0.563, 1.349] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.687s | 1.859s | 1.469 | [1.379, 1.565] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.685s | 1.857s | 1.470 | [1.380, 1.565] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.78ms | 1.63ms | 1.025 | [0.763, 1.378] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 2.265s | 691.08ms | 3.265 | [3.095, 3.444] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 2.259s | 687.90ms | 3.272 | [3.102, 3.452] | faster |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 4.67ms | 2.66ms | 1.755 | [1.731, 1.779] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 3.221s | 372.59ms | 8.650 | [8.510, 8.792] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 1.850s | 364.75ms | 5.079 | [4.961, 5.201] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.370s | 7.13ms | 191.677 | [188.566, 194.840] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 1.888s | 26.28ms | 71.010 | [69.316, 72.746] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 100.36ms | 23.72ms | 4.275 | [3.991, 4.579] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 1.784s | 2.39ms | 708.077 | [610.254, 821.581] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.902s | 3.03ms | 626.633 | [622.300, 630.996] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 10.38ms | 2.12ms | 4.875 | [4.502, 5.279] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.891s | 764.27us | 2467.398 | [2408.271, 2527.976] | faster |
| interp/dsg/cc/b0/d2/L12/p64 | total | (not in A) | 52.05ms | - | - | new case |
| interp/dsg/cc/b0/d3/L9/p64 | total | (not in A) | 43.09ms | - | - | new case |
| interp/dsg/cc/b0/d5/L6/p64 | total | (not in A) | 175.80ms | - | - | new case |
| interp/dsg/cc/b0/d7/L4/p16 | total | (not in A) | 152.05ms | - | - | new case |
| interp/dsg/cc/b0/d10/L2/p8 | total | (not in A) | 398.38ms | - | - | new case |
| interp/dsg/equi/b0/d3/L5/p64 | total | (not in A) | 6.52ms | - | - | new case |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | (not in A) | 155.09ms | - | - | new case |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | (not in A) | 121.12ms | - | - | new case |
| interplin/dsg/equi/b1/d8/L5/p256 | total | (not in A) | 237.54ms | - | - | new case |
| interplin/dsg/equi/b1/d10/L5/p32 | total | (not in A) | 271.22ms | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | (not in A) | 1.535s | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | total | (not in A) | 570.88ms | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | (not in A) | 1.037s | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | total | (not in A) | 520.38ms | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | (not in A) | 2.156s | - | - | new case |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | total | (not in A) | 881.11ms | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | total | (not in A) | 1.072s | - | - | new case |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | total | (not in A) | 503.22ms | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | total | (not in A) | 824.67ms | - | - | new case |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | total | (not in A) | 345.48ms | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | (not in A) | 2.476s | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | total | (not in A) | 398.37ms | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | total | (not in A) | 1.292s | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | (not in A) | 1.153s | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | total | (not in A) | 350.19ms | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | (not in A) | 664.62ms | - | - | new case |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | total | (not in A) | 333.17ms | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | total | (not in A) | 258.65ms | - | - | new case |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | total | (not in A) | 99.01ms | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | total | (not in A) | 1.068s | - | - | new case |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | total | (not in A) | 208.55ms | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | (not in A) | 576.04ms | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | total | (not in A) | 70.68ms | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | total | (not in A) | 243.39ms | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | total | (not in A) | 153.81ms | - | - | new case |

Sum of medians (total, common cases): A = 69.888s, B = 34.699s, saved = 35.189s (2.014x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d15/L6`: A 1.56e-09 -> B 7.83e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 363.9 MiB | 113.7 MiB | 0.312 | 0.2% / 12.8% | MEM- |
| quad/sg/equi-exp/b0/d5/L10 | 15.4 MiB | 12.9 MiB | 0.837 | 3.6% / 2.1% | MEM- |
| quad/sg/equi-exp/b0/d7/L7 | 14.4 MiB | 13.0 MiB | 0.905 | 2.8% / 1.7% | MEM- |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 13.0 MiB | 1.012 | 2.8% / 2.3% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.6 MiB | 13.4 MiB | 0.523 | 2.9% / 3.8% | MEM- |
| quad/sg/equi-exp/b1/d15/L6 | 53.4 MiB | 18.5 MiB | 0.347 | 5.2% / 1.5% | MEM- |
| quad/sg/equi-exp/b1/d20/L5 | 60.3 MiB | 20.4 MiB | 0.339 | 13.7% / 0.6% | MEM- |
| quad/sg/equi-sin/b0/d7/L7 | 14.7 MiB | 12.9 MiB | 0.879 | 4.8% / 1.7% | MEM- |
| quad/sg/cc-uq/b0/d2/L20 | 839.1 MiB | 191.1 MiB | 0.228 | 15.3% / 9.8% | MEM- |
| quad/sg/cc-uq/b0/d3/L14 | 37.6 MiB | 14.9 MiB | 0.396 | 3.8% / 11.1% | MEM- |
| quad/sg/cc-uq/b0/d5/L10 | 17.1 MiB | 12.9 MiB | 0.755 | 2.9% / 1.2% | MEM- |
| quad/sg/cc-uq/b0/d7/L7 | 14.7 MiB | 13.0 MiB | 0.884 | 3.7% / 1.4% | MEM- |
| quad/sg/cc-uq/b0/d10/L4 | 13.0 MiB | 13.0 MiB | 0.998 | 4.0% / 2.5% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.1 MiB | 44.3 MiB | 0.712 | 3.2% / 2.2% | MEM- |
| quad/dsg/equi-exp/b0/d5/L12 | 24.6 MiB | 18.1 MiB | 0.738 | 2.5% / 2.2% | MEM- |
| quad/dsg/equi-exp/b0/d7/L10 | 37.2 MiB | 29.8 MiB | 0.802 | 2.8% / 10.4% | MEM- |
| quad/dsg/equi-exp/b0/d10/L7 | 36.9 MiB | 30.2 MiB | 0.820 | 2.3% / 4.2% | MEM- |
| quad/dsg/equi-exp/b1/d15/L7 | 233.0 MiB | 51.8 MiB | 0.222 | 1.0% / 0.5% | MEM- |
| quad/dsg/equi-exp/b1/d20/L6 | 332.2 MiB | 82.9 MiB | 0.249 | 3.7% / 0.1% | MEM- |
| quad/dsg/cc-uq/b0/d5/L12 | 24.0 MiB | 18.5 MiB | 0.771 | 2.8% / 1.0% | MEM- |
| quad/dsg/cc-uq/b0/d7/L10 | 37.4 MiB | 29.6 MiB | 0.792 | 1.0% / 8.6% | MEM- |
| quad/dsg/cc-uq/b0/d10/L7 | 37.7 MiB | 29.6 MiB | 0.786 | 4.3% / 2.5% | MEM- |
| interp/sg/cc/b0/d2/L12/p64 | 14.9 MiB | 14.7 MiB | 0.986 | 4.1% / 4.4% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.1 MiB | 12.3 MiB | 1.022 | 1.8% / 1.0% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.3 MiB | 1.015 | 1.9% / 1.3% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.7 MiB | 12.2 MiB | 0.961 | 4.0% / 1.9% | MEM- |
| interp/sg/cc/b0/d10/L2/p8 | 24.6 MiB | 12.1 MiB | 0.494 | 8.1% / 2.0% | MEM- |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 12.4 MiB | 1.039 | 1.6% / 3.0% | MEM+ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.9 MiB | 12.4 MiB | 1.038 | 1.1% / 0.9% | MEM+ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 12.3 MiB | 1.038 | 2.9% / 2.6% | MEM+ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.3 MiB | 20.5 MiB | 1.063 | 3.4% / 7.9% | MEM+ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.1 MiB | 12.9 MiB | 0.986 | 4.4% / 2.0% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.9 MiB | 12.8 MiB | 0.990 | 2.1% / 0.8% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.3 MiB | 12.8 MiB | 0.958 | 4.6% / 3.1% | MEM- |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.1 MiB | 13.5 MiB | 0.839 | 4.3% / 1.7% | MEM- |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.4 MiB | 15.0 MiB | 0.862 | 2.7% / 1.7% | MEM- |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.7 MiB | 13.4 MiB | 1.054 | 1.4% / 2.8% | MEM+ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.7 MiB | 1.046 | 3.0% / 0.5% | MEM+ |
| interp/dsg/cc/b0/d2/L12/p64 | (not in A) | 15.4 MiB | - | - | new case |
| interp/dsg/cc/b0/d3/L9/p64 | (not in A) | 12.7 MiB | - | - | new case |
| interp/dsg/cc/b0/d5/L6/p64 | (not in A) | 12.6 MiB | - | - | new case |
| interp/dsg/cc/b0/d7/L4/p16 | (not in A) | 12.5 MiB | - | - | new case |
| interp/dsg/cc/b0/d10/L2/p8 | (not in A) | 12.4 MiB | - | - | new case |
| interp/dsg/equi/b0/d3/L5/p64 | (not in A) | 12.4 MiB | - | - | new case |
| interplin/dsg/equi/b1/d2/L16/p16384 | (not in A) | 21.2 MiB | - | - | new case |
| interplin/dsg/equi/b1/d5/L8/p1024 | (not in A) | 13.3 MiB | - | - | new case |
| interplin/dsg/equi/b1/d8/L5/p256 | (not in A) | 12.4 MiB | - | - | new case |
| interplin/dsg/equi/b1/d10/L5/p32 | (not in A) | 16.0 MiB | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | (not in A) | 13.0 MiB | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | (not in A) | 13.3 MiB | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | (not in A) | 13.1 MiB | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | (not in A) | 13.2 MiB | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | (not in A) | 12.5 MiB | - | - | new case |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | (not in A) | 12.6 MiB | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | (not in A) | 12.2 MiB | - | - | new case |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | (not in A) | 12.6 MiB | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | (not in A) | 12.3 MiB | - | - | new case |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | (not in A) | 12.4 MiB | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | (not in A) | 12.6 MiB | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | (not in A) | 12.3 MiB | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | (not in A) | 12.2 MiB | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | (not in A) | 13.8 MiB | - | - | new case |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | (not in A) | 16.0 MiB | - | - | new case |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | (not in A) | 13.2 MiB | - | - | new case |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | (not in A) | 13.6 MiB | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | (not in A) | 12.8 MiB | - | - | new case |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | (not in A) | 13.0 MiB | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | (not in A) | 12.7 MiB | - | - | new case |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | (not in A) | 14.1 MiB | - | - | new case |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | (not in A) | 20.9 MiB | - | - | new case |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | (not in A) | 13.5 MiB | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | (not in A) | 13.4 MiB | - | - | new case |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | (not in A) | 59.5 MiB | - | - | new case |

Peak memory (common cases): geomean B/A = 0.716; largest B/A: `interplin/sg/equi/b1/d2/L16/p16384` 19.3 MiB -> 20.5 MiB (1.063x); 6 MEM+, 25 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.1%, 0.1%, 0.1%, 0.1%
Round durations (s): 262, 262, 262, 262, 263 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1311s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
