A/B `micro`: A=`WP-17-before` B=`WP-17-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.00ms | 7.58ms | 0.795 | [0.783, 0.807] | SLOWER |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.98ms | 3.15ms | 0.952 | [0.935, 0.970] | SLOWER |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.87ms | 14.91ms | 0.931 | [0.920, 0.942] | SLOWER |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.08ms | 26.69ms | 0.939 | [0.917, 0.961] | SLOWER |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.93ms | 14.97ms | 0.931 | [0.925, 0.938] | SLOWER |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.90ms | 107.43ms | 1.019 | [1.005, 1.033] | faster |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.40ms | 53.69ms | 0.972 | [0.955, 0.989] | SLOWER |
| micro/quad/row_loop/equi/b0/d10/L4 | call | 3.692s | 2.786s | 1.302 | [1.252, 1.353] | faster |
| micro/quad/row_loop/equi/b1/d20/L4 | call | 37.17ms | 24.84ms | 1.496 | [1.483, 1.508] | faster |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.46ms | 4.15ms | 0.893 | [0.734, 1.085] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.51ms | 19.60ms | 0.793 | [0.786, 0.799] | SLOWER |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 8.59ms | 8.27ms | 1.049 | [1.015, 1.084] | faster |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 35.02us | 38.47us | 0.909 | [0.905, 0.914] | SLOWER |
| micro/quadrule/cc_weights/n33 | call | 2.10us | 2.11us | 0.997 | [0.988, 1.007] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 51.38us | 51.36us | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quadrule/cc_weights/n32769 | call | 4.36ms | 4.40ms | 0.993 | [0.989, 0.997] | SLOWER |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.67ms | 8.19ms | 0.960 | [0.889, 1.037] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 68.21ms | 72.08ms | 1.069 | [0.734, 1.557] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.80us | 123.74us | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 113.87ms | 114.20ms | 1.002 | [0.990, 1.013] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.68ms | 19.81ms | 0.992 | [0.973, 1.012] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.18ms | 9.21ms | 0.996 | [0.984, 1.008] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.07ms | 54.82ms | 1.004 | [0.999, 1.010] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 91.06ms | 92.71ms | 0.987 | [0.974, 1.001] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 282.62ms | 308.42ms | 0.916 | [0.908, 0.925] | SLOWER |
| micro/dsg/process_delta_tgs/equi/b0/d10/L7/s256 | call | 223.03ms | 241.86ms | 0.920 | [0.911, 0.929] | SLOWER |
| micro/dsg/row_loop/equi/b0/d10/L7/s256 | call | 200.90ms | 110.34ms | 1.737 | [1.547, 1.951] | faster |
| micro/dsg/row_loop/equi/b0/d7/L10/s64 | call | 85.88ms | 74.39ms | 1.166 | [1.073, 1.267] | faster |
| micro/dsg/row_loop/cc/b0/d7/L10/s64 | call | 85.93ms | 74.65ms | 1.157 | [1.136, 1.178] | faster |
| micro/dsg/row_loop/equi/b1/d20/L6/s256 | call | 30.95ms | 29.48ms | 1.053 | [1.042, 1.065] | faster |
| micro/dsg/row_loop_opaque/equi/b0/d10/L7/s256 | call | 124.45ms | 125.80ms | 0.985 | [0.963, 1.008] | ~ (noise) |
| micro/dsg/row_loop_opaque/equi/b0/d7/L10/s64 | call | 87.15ms | 85.08ms | 1.015 | [0.985, 1.046] | ~ (noise) |
| micro/dsg/row_loop_opaque/equi/b1/d20/L6/s256 | call | 31.32ms | 31.42ms | 0.997 | [0.978, 1.015] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.16ms | 0.969 | [0.956, 0.982] | SLOWER |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 505.47us | 515.13us | 0.981 | [0.961, 1.002] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 46.81ms | 47.12ms | 0.993 | [0.985, 1.002] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.37ms | 1.43ms | 0.953 | [0.938, 0.969] | SLOWER |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.70us | 119.20us | 0.991 | [0.977, 1.006] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n9 | call | 828.1ns | 829.4ns | 0.995 | [0.988, 1.002] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.35us | 37.40us | 0.993 | [0.982, 1.005] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.9ns | 31.7ns | 1.007 | [0.998, 1.017] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.6ns | 197.0ns | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.04us | 42.31us | 0.993 | [0.988, 0.999] | SLOWER |
| micro/interplin/tg/equi/b1/d10/L5 | call | 231.89us | 234.73us | 0.996 | [0.933, 1.063] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 205.6ns | 215.0ns | 0.956 | [0.952, 0.960] | SLOWER |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 45.68ms | 45.47ms | 1.005 | [1.001, 1.009] | faster |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 90.89ms | 90.15ms | 1.009 | [1.000, 1.019] | ~ (noise) |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 321.68us | 321.33us | 1.001 | [0.992, 1.009] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 11.08us | 11.03us | 1.006 | [0.995, 1.018] | ~ (noise) |
| micro/mivec/downwards_closure/d5/S16/n1000 | call | 503.12us | 470.92us | 1.077 | [1.051, 1.104] | faster |
| micro/mivec/downwards_closure/d10/S12/n1000 | call | 1.55ms | 1.53ms | 1.013 | [1.006, 1.020] | faster |
| micro/mivec/downwards_closure/d15/S10/n300 | call | 571.04us | 566.97us | 1.005 | [0.998, 1.012] | ~ (noise) |
| micro/mivec/downwards_closure/d20/S7/n300 | call | 110.90us | 114.69us | 0.966 | [0.954, 0.978] | SLOWER |
| micro/mivec/pareto_maxima/d10/S12/n1000 | call | 6.00ms | 6.00ms | 1.011 | [0.978, 1.045] | ~ (noise) |
| micro/mivec/pareto_maxima/d20/S7/n300 | call | 94.42us | 91.21us | 1.035 | [1.031, 1.039] | faster |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.09ms | 16.06ms | 0.941 | [0.931, 0.952] | SLOWER |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m4 | call | 25.06ms | 23.31ms | 1.084 | [1.061, 1.107] | faster |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m4 | call | 77.82ms | 72.50ms | 1.073 | [1.069, 1.078] | faster |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m4 | call | 592.76ms | 566.51ms | 1.056 | [1.028, 1.085] | faster |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m4 | call | 308.85ms | 295.74ms | 1.050 | [1.039, 1.061] | faster |
| micro/dsg/process_delta_tg_vec/equi/b1/d20/L6/m4 | call | 547.96us | 531.69us | 1.028 | [1.024, 1.033] | faster |
| micro/dsg/process_delta_tgs_vec/equi/b1/d20/L6/s256/m4 | call | 191.21ms | 186.79ms | 1.026 | [1.017, 1.035] | faster |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m4 | call | 4.48ms | 4.46ms | 1.004 | [1.000, 1.008] | faster |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m4 | call | 3.93ms | 3.92ms | 1.002 | [0.994, 1.009] | ~ (noise) |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m4 | call | 1.21ms | 1.20ms | 0.989 | [0.949, 1.031] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m4 | call | 153.80ms | 154.37ms | 0.992 | [0.984, 1.000] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d2/L16/m4 | call | 652.7ns | 650.1ns | 1.004 | [1.000, 1.008] | faster |
| micro/interplin/tg_vec/equi/b1/d8/L5/m4 | call | 150.53us | 150.64us | 1.000 | [0.994, 1.006] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m4 | call | 859.36us | 860.69us | 0.998 | [0.994, 1.003] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c0 | call | 5.70ms | 5.34ms | 1.065 | [1.060, 1.071] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c100 | call | 16.36ms | 14.47ms | 1.130 | [1.125, 1.136] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c1000 | call | 197.54ms | 190.53ms | 1.035 | [1.029, 1.041] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 18.96ms | 18.50ms | 1.029 | [1.017, 1.040] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 14.03ms | 12.29ms | 1.146 | [1.133, 1.158] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/adaptive_production/c0 | call | 6.89ms | 6.53ms | 1.056 | [1.050, 1.062] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/adaptive_steady_production/c0 | call | 6.36ms | 6.01ms | 1.070 | [1.041, 1.100] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 53.53ms | 53.71ms | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 13.98ms | 12.36ms | 1.129 | [1.123, 1.135] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/adaptive_production/c1000 | call | 55.81ms | 53.68ms | 1.042 | [1.038, 1.045] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/adaptive_steady_production/c1000 | call | 15.24ms | 13.16ms | 1.155 | [1.133, 1.178] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c0 | call | 5.47ms | 5.04ms | 1.084 | [1.079, 1.090] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c100 | call | 12.24ms | 11.36ms | 1.078 | [1.074, 1.082] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c1000 | call | 154.15ms | 152.41ms | 1.012 | [1.008, 1.015] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 36.82ms | 40.69ms | 0.905 | [0.893, 0.916] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c0 | call | 20.56ms | 19.36ms | 1.061 | [1.016, 1.108] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/adaptive_production/c0 | call | 7.69ms | 7.02ms | 1.098 | [1.092, 1.103] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/adaptive_steady_production/c0 | call | 6.39ms | 5.74ms | 1.113 | [1.109, 1.117] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 115.83ms | 122.49ms | 0.947 | [0.942, 0.953] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c1000 | call | 20.79ms | 19.72ms | 1.037 | [1.001, 1.074] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/adaptive_production/c1000 | call | 118.45ms | 114.07ms | 1.037 | [1.028, 1.046] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/adaptive_steady_production/c1000 | call | 21.23ms | 20.63ms | 1.016 | [0.965, 1.071] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c0 | call | 2.69ms | 2.50ms | 1.077 | [1.073, 1.081] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c100 | call | 10.33ms | 10.51ms | 0.983 | [0.979, 0.986] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c1000 | call | 151.55ms | 152.84ms | 0.992 | [0.986, 0.998] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 16.80ms | 19.32ms | 0.866 | [0.863, 0.869] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 10.26ms | 10.26ms | 0.997 | [0.993, 1.000] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/adaptive_production/c0 | call | 3.88ms | 3.68ms | 1.072 | [1.019, 1.127] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/adaptive_steady_production/c0 | call | 3.30ms | 3.15ms | 1.048 | [1.044, 1.052] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 75.95ms | 75.58ms | 1.010 | [1.006, 1.013] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c1000 | call | 10.30ms | 10.56ms | 0.974 | [0.964, 0.983] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/adaptive_production/c1000 | call | 78.68ms | 76.33ms | 1.031 | [1.025, 1.036] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/adaptive_steady_production/c1000 | call | 11.54ms | 10.70ms | 1.078 | [1.066, 1.090] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c0 | call | 5.59ms | 5.17ms | 1.077 | [1.067, 1.087] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c100 | call | 16.44ms | 15.56ms | 1.056 | [1.052, 1.060] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c1000 | call | 225.56ms | 222.55ms | 1.012 | [1.008, 1.016] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 35.23ms | 35.71ms | 0.986 | [0.978, 0.994] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 21.19ms | 17.14ms | 1.242 | [1.201, 1.284] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/adaptive_production/c0 | call | 7.48ms | 6.74ms | 1.112 | [1.103, 1.122] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/adaptive_steady_production/c0 | call | 6.67ms | 5.90ms | 1.130 | [1.125, 1.135] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 122.53ms | 122.70ms | 0.999 | [0.993, 1.006] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 21.35ms | 17.00ms | 1.252 | [1.220, 1.286] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/adaptive_production/c1000 | call | 125.82ms | 119.31ms | 1.055 | [1.051, 1.059] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/adaptive_steady_production/c1000 | call | 23.03ms | 18.15ms | 1.290 | [1.238, 1.344] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c0 | call | 3.73ms | 3.77ms | 1.028 | [0.917, 1.152] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c100 | call | 9.07ms | 8.31ms | 1.091 | [1.087, 1.095] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c1000 | call | 100.05ms | 96.35ms | 1.039 | [1.034, 1.044] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 20.10ms | 23.74ms | 0.846 | [0.843, 0.849] | SLOWER |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c0 | call | 9.30ms | 8.26ms | 1.124 | [1.109, 1.138] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/adaptive_production/c0 | call | 5.15ms | 5.08ms | 1.011 | [1.004, 1.018] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/adaptive_steady_production/c0 | call | 4.13ms | 4.13ms | 0.998 | [0.982, 1.014] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 117.51ms | 124.80ms | 0.943 | [0.940, 0.947] | SLOWER |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c1000 | call | 9.22ms | 8.26ms | 1.116 | [1.106, 1.126] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/adaptive_production/c1000 | call | 116.00ms | 113.49ms | 1.021 | [1.017, 1.025] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/adaptive_steady_production/c1000 | call | 13.84ms | 11.72ms | 1.178 | [1.126, 1.231] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c0 | call | 49.15ms | 49.32ms | 0.996 | [0.992, 1.000] | SLOWER |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c100 | call | 80.47ms | 82.15ms | 0.978 | [0.972, 0.984] | SLOWER |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c1000 | call | 481.95ms | 483.79ms | 0.996 | [0.993, 1.000] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 42.08ms | 41.56ms | 1.014 | [1.006, 1.023] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 39.92ms | 39.59ms | 1.008 | [1.004, 1.012] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/adaptive_production/c0 | call | 43.41ms | 44.18ms | 0.981 | [0.967, 0.996] | SLOWER |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/adaptive_steady_production/c0 | call | 41.19ms | 41.42ms | 0.992 | [0.984, 1.001] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 53.67ms | 53.90ms | 0.997 | [0.986, 1.007] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 39.49ms | 39.47ms | 0.999 | [0.990, 1.009] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/adaptive_production/c1000 | call | 55.59ms | 55.83ms | 0.995 | [0.988, 1.002] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/adaptive_steady_production/c1000 | call | 40.99ms | 41.45ms | 0.985 | [0.976, 0.994] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c0 | call | 60.97ms | 61.70ms | 0.990 | [0.980, 1.000] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c100 | call | 85.27ms | 87.53ms | 0.973 | [0.969, 0.977] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c1000 | call | 388.93ms | 391.78ms | 0.993 | [0.989, 0.997] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 61.85ms | 64.73ms | 0.957 | [0.951, 0.963] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c0 | call | 37.25ms | 37.45ms | 0.991 | [0.959, 1.024] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/adaptive_production/c0 | call | 64.83ms | 66.99ms | 0.972 | [0.959, 0.986] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/adaptive_steady_production/c0 | call | 38.23ms | 39.51ms | 0.959 | [0.926, 0.994] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 137.12ms | 137.85ms | 0.994 | [0.990, 0.998] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c1000 | call | 36.91ms | 37.40ms | 0.982 | [0.961, 1.002] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/adaptive_production/c1000 | call | 139.44ms | 142.25ms | 0.980 | [0.977, 0.984] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/adaptive_steady_production/c1000 | call | 38.41ms | 39.74ms | 0.973 | [0.949, 0.998] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_cold_production/c0 | call | 38.29ms | 37.57ms | 1.019 | [1.010, 1.028] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_warm_production/c0 | call | 21.80ms | 19.19ms | 1.134 | [1.118, 1.149] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_production/c0 | call | 208.11ms | 208.75ms | 0.997 | [0.990, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_warm_production/c0 | call | 109.16ms | 109.20ms | 0.996 | [0.989, 1.003] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/adaptive_spill_production/c0 | call | 6.57ms | 6.21ms | 1.059 | [1.055, 1.063] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_cold_production/c1000 | call | 73.71ms | 74.49ms | 0.992 | [0.988, 0.995] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_warm_production/c1000 | call | 21.79ms | 19.24ms | 1.132 | [1.127, 1.137] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_production/c1000 | call | 243.50ms | 244.66ms | 0.994 | [0.988, 1.001] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_warm_production/c1000 | call | 108.58ms | 109.46ms | 0.994 | [0.990, 0.998] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/adaptive_spill_production/c1000 | call | 201.17ms | 197.34ms | 1.019 | [1.008, 1.030] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_cold_production/c0 | call | 112.02ms | 112.96ms | 0.989 | [0.985, 0.994] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_warm_production/c0 | call | 63.24ms | 60.83ms | 1.041 | [1.021, 1.061] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_production/c0 | call | 470.15ms | 469.77ms | 0.996 | [0.963, 1.030] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_warm_production/c0 | call | 100.17ms | 101.86ms | 0.990 | [0.960, 1.022] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/adaptive_spill_production/c0 | call | 11.61ms | 10.62ms | 1.126 | [1.037, 1.222] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_cold_production/c1000 | call | 186.93ms | 192.09ms | 0.971 | [0.964, 0.978] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_warm_production/c1000 | call | 63.08ms | 60.47ms | 1.045 | [1.027, 1.064] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_production/c1000 | call | 549.28ms | 548.34ms | 0.994 | [0.966, 1.022] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_warm_production/c1000 | call | 100.05ms | 100.02ms | 1.006 | [0.994, 1.019] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/adaptive_spill_production/c1000 | call | 291.07ms | 290.89ms | 1.003 | [0.990, 1.016] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_cold_production/c0 | call | 48.30ms | 47.15ms | 1.024 | [1.015, 1.033] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_warm_production/c0 | call | 44.63ms | 44.23ms | 1.009 | [1.004, 1.013] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c0 | call | 201.96ms | 202.26ms | 0.997 | [0.992, 1.003] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_warm_production/c0 | call | 206.44ms | 206.08ms | 0.996 | [0.981, 1.012] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/adaptive_spill_production/c0 | call | 69.87ms | 70.14ms | 0.999 | [0.991, 1.007] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_cold_production/c1000 | call | 59.78ms | 59.08ms | 1.013 | [1.006, 1.019] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_warm_production/c1000 | call | 44.60ms | 44.18ms | 1.008 | [1.000, 1.015] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c1000 | call | 212.44ms | 214.70ms | 0.989 | [0.981, 0.997] | SLOWER |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_warm_production/c1000 | call | 205.92ms | 206.56ms | 0.995 | [0.984, 1.007] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/adaptive_spill_production/c1000 | call | 223.90ms | 222.87ms | 1.005 | [1.000, 1.009] | faster |

Sum of medians (call, common cases): A = 15.748s, B = 14.695s, saved = 1.053s (1.072x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.1%
Round durations (s): 250, 250, 249, 250, 250 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 1249s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
