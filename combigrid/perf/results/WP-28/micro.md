A/B `micro`: A=`WP-28-before` B=`WP-28-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/quad/process_tg/equi/b0/d2/L16 | call | 6.18ms | 6.20ms | 0.961 | [0.894, 1.033] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d5/L10 | call | 2.99ms | 2.98ms | 1.003 | [0.987, 1.020] | ~ (noise) |
| micro/quad/process_tg/equi/b0/d10/L4 | call | 13.91ms | 13.90ms | 0.997 | [0.985, 1.009] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d2/L16 | call | 25.08ms | 25.14ms | 1.003 | [0.991, 1.015] | ~ (noise) |
| micro/quad/process_tg/cc/b0/d10/L4 | call | 13.91ms | 13.94ms | 0.998 | [0.992, 1.003] | ~ (noise) |
| micro/quad/process_all_tgs/equi/b1/d20/L4 | call | 109.25ms | 108.46ms | 1.007 | [1.002, 1.011] | faster |
| micro/quad/process_all_tgs/cc/b1/d10/L5 | call | 52.70ms | 52.65ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d5/L10 | call | 3.47ms | 3.41ms | 1.011 | [0.973, 1.051] | ~ (noise) |
| micro/sol/quad_tg/equi/b0/d10/L4 | call | 15.47ms | 15.51ms | 1.027 | [0.943, 1.118] | ~ (noise) |
| micro/tg/iterate_mutable/equi/b0/d10/L4 | call | 7.45ms | 7.48ms | 0.986 | [0.945, 1.030] | ~ (noise) |
| micro/sourcefunc/eval_normalized_in_place/d10 | call | 35.12us | 35.55us | 0.982 | [0.967, 0.998] | SLOWER |
| micro/quadrule/cc_weights/n33 | call | 2.10us | 2.11us | 0.997 | [0.981, 1.012] | ~ (noise) |
| micro/quadrule/cc_weights/n1025 | call | 50.99us | 51.33us | 0.991 | [0.986, 0.997] | SLOWER |
| micro/quadrule/cc_weights/n32769 | call | 4.35ms | 4.34ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d5/L12 | call | 7.72ms | 7.65ms | 1.020 | [0.990, 1.051] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b0/d10/L7 | call | 65.97ms | 66.20ms | 1.009 | [0.972, 1.047] | ~ (noise) |
| micro/dsg/process_delta_tg/equi/b1/d20/L6 | call | 123.00us | 123.35us | 1.015 | [0.969, 1.062] | ~ (noise) |
| micro/dsg/prepare/equi/b1/d20/L6 | call | 112.74ms | 112.95ms | 0.997 | [0.989, 1.006] | ~ (noise) |
| micro/dsg/sub_tries/equi/b1/d20/L6/s256 | call | 19.84ms | 19.86ms | 1.000 | [0.991, 1.009] | ~ (noise) |
| micro/dsg/sub_tries/equi/b0/d10/L7/s16 | call | 9.14ms | 9.12ms | 0.999 | [0.993, 1.006] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d20/L6/s256 | call | 55.07ms | 54.89ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b1/d15/L7/s128 | call | 90.76ms | 90.86ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| micro/dsg/process_delta_tgs/equi/b0/d10/L6/s64 | call | 283.46ms | 282.33ms | 0.997 | [0.980, 1.016] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d5/L6 | call | 1.12ms | 1.12ms | 0.999 | [0.986, 1.012] | ~ (noise) |
| micro/interp/global_tg/cheb1/b0/d5/L4 | call | 504.82us | 510.22us | 0.988 | [0.967, 1.009] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d10/L2 | call | 46.64ms | 46.64ms | 1.000 | [0.986, 1.014] | ~ (noise) |
| micro/interp/global_tg/cc/b0/d2/L12 | call | 1.36ms | 1.36ms | 0.998 | [0.995, 1.002] | ~ (noise) |
| micro/interp/global_tg/equi/b0/d3/L5 | call | 118.42us | 120.48us | 0.983 | [0.970, 0.996] | SLOWER |
| micro/interp/barycentric_1d/generic/n9 | call | 833.6ns | 839.0ns | 0.995 | [0.982, 1.008] | ~ (noise) |
| micro/interp/barycentric_1d/generic/n65 | call | 37.45us | 37.40us | 1.004 | [0.995, 1.013] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n9 | call | 31.9ns | 31.8ns | 1.007 | [0.998, 1.017] | ~ (noise) |
| micro/interp/barycentric_1d/opt_cc/n65 | call | 196.7ns | 197.0ns | 0.998 | [0.994, 1.001] | ~ (noise) |
| micro/interplin/tg/equi/b1/d8/L5 | call | 42.03us | 41.85us | 1.012 | [0.990, 1.035] | ~ (noise) |
| micro/interplin/tg/equi/b1/d10/L5 | call | 234.91us | 233.07us | 1.023 | [0.954, 1.098] | ~ (noise) |
| micro/interplin/tg/equi/b1/d2/L16 | call | 206.8ns | 205.4ns | 0.998 | [0.977, 1.018] | ~ (noise) |
| micro/gen/complete_mis_with_coeffs/d20/L6 | call | 45.19ms | 45.14ms | 0.998 | [0.991, 1.005] | ~ (noise) |
| micro/gen/sparse_grid/cc/b1/d20/L5 | call | 90.05ms | 90.32ms | 0.996 | [0.981, 1.011] | ~ (noise) |
| micro/mivec/downwards_closure/d10/S10/n300 | call | 325.24us | 321.39us | 1.016 | [1.008, 1.024] | faster |
| micro/mivec/downwards_closure/d20/S6/n100 | call | 11.04us | 11.17us | 0.989 | [0.978, 1.000] | ~ (noise) |
| micro/mivec/downwards_closure/d5/S16/n1000 | call | 470.38us | 464.14us | 1.015 | [0.979, 1.051] | ~ (noise) |
| micro/mivec/downwards_closure/d10/S12/n1000 | call | 1.53ms | 1.52ms | 0.998 | [0.991, 1.005] | ~ (noise) |
| micro/mivec/downwards_closure/d15/S10/n300 | call | 564.27us | 559.82us | 1.009 | [1.003, 1.015] | faster |
| micro/mivec/downwards_closure/d20/S7/n300 | call | 110.69us | 110.87us | 0.999 | [0.990, 1.007] | ~ (noise) |
| micro/mivec/pareto_maxima/d10/S12/n1000 | call | 6.00ms | 6.00ms | 1.008 | [0.964, 1.053] | ~ (noise) |
| micro/mivec/pareto_maxima/d20/S7/n300 | call | 91.19us | 90.91us | 1.017 | [0.976, 1.059] | ~ (noise) |
| micro/mivec/ct_coeffs/d10/S12/n1000 | call | 15.05ms | 15.06ms | 0.998 | [0.992, 1.005] | ~ (noise) |
| micro/vec/quad/scalar/equi/b0/d2/L16/m1 | call | 6.03ms | 6.01ms | 0.993 | [0.919, 1.072] | ~ (noise) |
| micro/vec/quad/tvt_scalar/equi/b0/d2/L16/m1 | call | 5.99ms | 5.91ms | 1.002 | [0.974, 1.031] | ~ (noise) |
| micro/vec/quad/rt_nested/equi/b0/d2/L16/m1 | call | 8.87ms | 8.87ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/vec/quad/batch_nested/equi/b0/d2/L16/m1 | call | 8.53ms | 8.53ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/vec/quad/rt_direct/equi/b0/d2/L16/m1 | call | 8.87ms | 8.65ms | 1.022 | [1.011, 1.034] | faster |
| micro/vec/quad/ct_direct/equi/b0/d2/L16/m1 | call | 7.16ms | 7.18ms | 0.999 | [0.986, 1.012] | ~ (noise) |
| micro/vec/quad/rowbuf_direct/equi/b0/d2/L16/m1 | call | 9.94ms | 9.96ms | 0.996 | [0.990, 1.003] | ~ (noise) |
| micro/vec/quad/rowbufk_direct/equi/b0/d2/L16/m1 | call | 10.57ms | 10.57ms | 0.996 | [0.983, 1.010] | ~ (noise) |
| micro/vec/quad/batch_direct/equi/b0/d2/L16/m1 | call | 8.42ms | 8.36ms | 1.000 | [0.978, 1.022] | ~ (noise) |
| micro/vec/quad/scalar_x_m/equi/b0/d2/L16/m1 | call | 6.58ms | 6.63ms | 0.995 | [0.988, 1.003] | ~ (noise) |
| micro/vec/quad/rt/equi/b0/d2/L16/m1 | call | 9.54ms | 9.65ms | 0.983 | [0.963, 1.004] | ~ (noise) |
| micro/vec/quad/ct/equi/b0/d2/L16/m1 | call | 8.30ms | 8.34ms | 0.996 | [0.991, 1.001] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b0/d2/L16/m1 | call | 10.55ms | 10.67ms | 0.977 | [0.942, 1.013] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b0/d2/L16/m1 | call | 11.72ms | 11.55ms | 1.022 | [0.995, 1.051] | ~ (noise) |
| micro/vec/quad/batch/equi/b0/d2/L16/m1 | call | 9.22ms | 9.23ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/vec/quad/scalar_x_m/equi/b0/d2/L16/m2 | call | 13.13ms | 13.25ms | 0.991 | [0.986, 0.997] | SLOWER |
| micro/vec/quad/rt/equi/b0/d2/L16/m2 | call | 16.27ms | 16.35ms | 1.020 | [0.951, 1.095] | ~ (noise) |
| micro/vec/quad/ct/equi/b0/d2/L16/m2 | call | 14.28ms | 14.29ms | 0.994 | [0.975, 1.013] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b0/d2/L16/m2 | call | 15.72ms | 15.67ms | 1.003 | [0.994, 1.011] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b0/d2/L16/m2 | call | 16.93ms | 17.03ms | 1.000 | [0.952, 1.049] | ~ (noise) |
| micro/vec/quad/batch/equi/b0/d2/L16/m2 | call | 15.26ms | 15.35ms | 0.994 | [0.990, 0.998] | SLOWER |
| micro/vec/quad/scalar_x_m/equi/b0/d2/L16/m4 | call | 26.30ms | 26.72ms | 1.018 | [0.888, 1.167] | ~ (noise) |
| micro/vec/quad/rt/equi/b0/d2/L16/m4 | call | 26.49ms | 26.28ms | 1.021 | [0.991, 1.052] | ~ (noise) |
| micro/vec/quad/ct/equi/b0/d2/L16/m4 | call | 25.17ms | 25.09ms | 1.002 | [0.996, 1.007] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b0/d2/L16/m4 | call | 24.70ms | 24.52ms | 1.027 | [0.962, 1.096] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b0/d2/L16/m4 | call | 26.87ms | 26.86ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/vec/quad/batch/equi/b0/d2/L16/m4 | call | 26.17ms | 26.10ms | 1.017 | [0.972, 1.065] | ~ (noise) |
| micro/vec/quad/scalar_x_m/equi/b0/d2/L16/m8 | call | 52.63ms | 53.15ms | 0.996 | [0.981, 1.011] | ~ (noise) |
| micro/vec/quad/rt/equi/b0/d2/L16/m8 | call | 45.02ms | 44.91ms | 1.007 | [0.995, 1.018] | ~ (noise) |
| micro/vec/quad/ct/equi/b0/d2/L16/m8 | call | 43.76ms | 43.62ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b0/d2/L16/m8 | call | 43.97ms | 43.90ms | 1.001 | [0.995, 1.008] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b0/d2/L16/m8 | call | 47.28ms | 47.37ms | 0.996 | [0.953, 1.042] | ~ (noise) |
| micro/vec/quad/batch/equi/b0/d2/L16/m8 | call | 47.00ms | 45.89ms | 1.024 | [1.020, 1.027] | faster |
| micro/vec/quad/scalar_x_m/equi/b0/d2/L16/m32 | call | 210.02ms | 212.07ms | 0.998 | [0.975, 1.022] | ~ (noise) |
| micro/vec/quad/rt/equi/b0/d2/L16/m32 | call | 160.46ms | 159.35ms | 1.004 | [0.995, 1.014] | ~ (noise) |
| micro/vec/quad/ct/equi/b0/d2/L16/m32 | call | 158.70ms | 158.34ms | 0.996 | [0.975, 1.018] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b0/d2/L16/m32 | call | 159.20ms | 158.45ms | 1.004 | [1.000, 1.008] | faster |
| micro/vec/quad/rowbufk/equi/b0/d2/L16/m32 | call | 171.29ms | 172.13ms | 1.000 | [0.984, 1.016] | ~ (noise) |
| micro/vec/quad/batch/equi/b0/d2/L16/m32 | call | 182.29ms | 175.86ms | 1.037 | [1.033, 1.040] | faster |
| micro/vec/quad/scalar/equi/b0/d5/L10/m1 | call | 3.00ms | 3.02ms | 0.991 | [0.984, 0.998] | SLOWER |
| micro/vec/quad/tvt_scalar/equi/b0/d5/L10/m1 | call | 3.03ms | 3.03ms | 1.002 | [0.997, 1.008] | ~ (noise) |
| micro/vec/quad/rt_nested/equi/b0/d5/L10/m1 | call | 4.23ms | 4.22ms | 1.018 | [0.916, 1.132] | ~ (noise) |
| micro/vec/quad/batch_nested/equi/b0/d5/L10/m1 | call | 4.16ms | 4.13ms | 1.007 | [1.000, 1.014] | ~ (noise) |
| micro/vec/quad/rt_direct/equi/b0/d5/L10/m1 | call | 4.13ms | 4.14ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/vec/quad/ct_direct/equi/b0/d5/L10/m1 | call | 3.49ms | 3.51ms | 0.987 | [0.975, 1.000] | SLOWER |
| micro/vec/quad/rowbuf_direct/equi/b0/d5/L10/m1 | call | 4.63ms | 4.64ms | 0.999 | [0.995, 1.004] | ~ (noise) |
| micro/vec/quad/rowbufk_direct/equi/b0/d5/L10/m1 | call | 5.01ms | 5.00ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| micro/vec/quad/batch_direct/equi/b0/d5/L10/m1 | call | 4.10ms | 4.04ms | 1.016 | [1.012, 1.020] | faster |
| micro/vec/quad/scalar/equi/b0/d10/L4/m1 | call | 13.94ms | 14.00ms | 0.996 | [0.992, 0.999] | SLOWER |
| micro/vec/quad/tvt_scalar/equi/b0/d10/L4/m1 | call | 14.09ms | 14.25ms | 0.993 | [0.980, 1.006] | ~ (noise) |
| micro/vec/quad/rt_nested/equi/b0/d10/L4/m1 | call | 19.28ms | 19.33ms | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/vec/quad/batch_nested/equi/b0/d10/L4/m1 | call | 18.63ms | 18.46ms | 1.011 | [1.004, 1.018] | faster |
| micro/vec/quad/rt_direct/equi/b0/d10/L4/m1 | call | 18.80ms | 19.02ms | 0.989 | [0.983, 0.994] | SLOWER |
| micro/vec/quad/ct_direct/equi/b0/d10/L4/m1 | call | 15.79ms | 15.79ms | 0.996 | [0.985, 1.008] | ~ (noise) |
| micro/vec/quad/rowbuf_direct/equi/b0/d10/L4/m1 | call | 20.07ms | 20.07ms | 1.007 | [0.988, 1.026] | ~ (noise) |
| micro/vec/quad/rowbufk_direct/equi/b0/d10/L4/m1 | call | 21.55ms | 21.46ms | 1.010 | [0.970, 1.053] | ~ (noise) |
| micro/vec/quad/batch_direct/equi/b0/d10/L4/m1 | call | 18.44ms | 18.04ms | 1.022 | [1.017, 1.028] | faster |
| micro/vec/quad/scalar_x_m/equi/b0/d10/L4/m1 | call | 34.19ms | 34.23ms | 0.998 | [0.994, 1.001] | ~ (noise) |
| micro/vec/quad/rt/equi/b0/d10/L4/m1 | call | 37.33ms | 37.43ms | 0.997 | [0.993, 1.002] | ~ (noise) |
| micro/vec/quad/ct/equi/b0/d10/L4/m1 | call | 35.33ms | 35.43ms | 1.010 | [0.973, 1.048] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b0/d10/L4/m1 | call | 37.96ms | 37.88ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b0/d10/L4/m1 | call | 40.86ms | 41.01ms | 0.999 | [0.992, 1.006] | ~ (noise) |
| micro/vec/quad/batch/equi/b0/d10/L4/m1 | call | 37.59ms | 37.63ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/vec/quad/scalar_x_m/equi/b0/d10/L4/m2 | call | 68.45ms | 68.37ms | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/vec/quad/rt/equi/b0/d10/L4/m2 | call | 49.45ms | 49.36ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/vec/quad/ct/equi/b0/d10/L4/m2 | call | 47.41ms | 47.35ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b0/d10/L4/m2 | call | 46.17ms | 46.27ms | 0.998 | [0.994, 1.001] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b0/d10/L4/m2 | call | 48.60ms | 48.77ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/vec/quad/batch/equi/b0/d10/L4/m2 | call | 46.29ms | 45.96ms | 1.008 | [1.004, 1.012] | faster |
| micro/vec/quad/scalar_x_m/equi/b0/d10/L4/m4 | call | 136.79ms | 136.78ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/vec/quad/rt/equi/b0/d10/L4/m4 | call | 80.18ms | 80.47ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/vec/quad/ct/equi/b0/d10/L4/m4 | call | 78.34ms | 78.35ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b0/d10/L4/m4 | call | 77.30ms | 77.08ms | 1.007 | [0.991, 1.025] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b0/d10/L4/m4 | call | 81.19ms | 81.35ms | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/vec/quad/batch/equi/b0/d10/L4/m4 | call | 78.55ms | 78.02ms | 1.008 | [1.002, 1.015] | faster |
| micro/vec/quad/scalar_x_m/equi/b0/d10/L4/m8 | call | 273.43ms | 273.52ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/vec/quad/rt/equi/b0/d10/L4/m8 | call | 127.42ms | 128.65ms | 0.955 | [0.863, 1.057] | ~ (noise) |
| micro/vec/quad/ct/equi/b0/d10/L4/m8 | call | 124.39ms | 125.69ms | 0.990 | [0.986, 0.993] | SLOWER |
| micro/vec/quad/rowbuf/equi/b0/d10/L4/m8 | call | 123.95ms | 124.83ms | 0.995 | [0.980, 1.009] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b0/d10/L4/m8 | call | 131.01ms | 132.07ms | 0.993 | [0.989, 0.996] | SLOWER |
| micro/vec/quad/batch/equi/b0/d10/L4/m8 | call | 128.61ms | 128.63ms | 0.995 | [0.983, 1.007] | ~ (noise) |
| micro/vec/quad/scalar_x_m/equi/b0/d10/L4/m32 | call | 1.094s | 1.094s | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/vec/quad/rt/equi/b0/d10/L4/m32 | call | 426.13ms | 427.26ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/vec/quad/ct/equi/b0/d10/L4/m32 | call | 427.80ms | 428.33ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b0/d10/L4/m32 | call | 423.16ms | 423.78ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b0/d10/L4/m32 | call | 445.55ms | 448.81ms | 0.993 | [0.989, 0.997] | SLOWER |
| micro/vec/quad/batch/equi/b0/d10/L4/m32 | call | 449.49ms | 447.40ms | 1.004 | [1.001, 1.008] | faster |
| micro/vec/quad/scalar/cc/b0/d10/L4/m1 | call | 13.96ms | 14.04ms | 0.995 | [0.991, 0.999] | SLOWER |
| micro/vec/quad/tvt_scalar/cc/b0/d10/L4/m1 | call | 14.05ms | 14.21ms | 0.987 | [0.979, 0.996] | SLOWER |
| micro/vec/quad/rt_nested/cc/b0/d10/L4/m1 | call | 19.29ms | 19.25ms | 1.000 | [0.993, 1.006] | ~ (noise) |
| micro/vec/quad/batch_nested/cc/b0/d10/L4/m1 | call | 18.60ms | 18.47ms | 1.003 | [0.960, 1.047] | ~ (noise) |
| micro/vec/quad/rt_direct/cc/b0/d10/L4/m1 | call | 18.83ms | 18.82ms | 0.995 | [0.982, 1.009] | ~ (noise) |
| micro/vec/quad/ct_direct/cc/b0/d10/L4/m1 | call | 15.78ms | 15.82ms | 0.993 | [0.980, 1.007] | ~ (noise) |
| micro/vec/quad/rowbuf_direct/cc/b0/d10/L4/m1 | call | 20.14ms | 20.09ms | 1.004 | [0.996, 1.013] | ~ (noise) |
| micro/vec/quad/rowbufk_direct/cc/b0/d10/L4/m1 | call | 21.53ms | 21.39ms | 0.996 | [0.958, 1.036] | ~ (noise) |
| micro/vec/quad/batch_direct/cc/b0/d10/L4/m1 | call | 18.42ms | 18.05ms | 1.021 | [1.017, 1.025] | faster |
| micro/vec/quad/scalar/equi/b1/d20/L4/all/m1 | call | 111.47ms | 111.66ms | 0.996 | [0.987, 1.005] | ~ (noise) |
| micro/vec/quad/tvt_scalar/equi/b1/d20/L4/all/m1 | call | 108.94ms | 108.96ms | 0.987 | [0.959, 1.015] | ~ (noise) |
| micro/vec/quad/rt_nested/equi/b1/d20/L4/all/m1 | call | 145.56ms | 150.64ms | 0.969 | [0.929, 1.010] | ~ (noise) |
| micro/vec/quad/batch_nested/equi/b1/d20/L4/all/m1 | call | 131.22ms | 130.43ms | 1.006 | [1.002, 1.010] | faster |
| micro/vec/quad/rt_direct/equi/b1/d20/L4/all/m1 | call | 142.99ms | 142.84ms | 0.998 | [0.992, 1.005] | ~ (noise) |
| micro/vec/quad/ct_direct/equi/b1/d20/L4/all/m1 | call | 123.68ms | 123.53ms | 1.002 | [0.995, 1.008] | ~ (noise) |
| micro/vec/quad/rowbuf_direct/equi/b1/d20/L4/all/m1 | call | 143.42ms | 147.34ms | 0.976 | [0.966, 0.987] | SLOWER |
| micro/vec/quad/rowbufk_direct/equi/b1/d20/L4/all/m1 | call | 138.88ms | 139.83ms | 0.990 | [0.978, 1.002] | ~ (noise) |
| micro/vec/quad/batch_direct/equi/b1/d20/L4/all/m1 | call | 129.14ms | 128.36ms | 1.005 | [1.000, 1.010] | faster |
| micro/vec/quad/scalar_x_m/equi/b1/d20/L4/all/m1 | call | 253.98ms | 254.08ms | 0.995 | [0.981, 1.009] | ~ (noise) |
| micro/vec/quad/rt/equi/b1/d20/L4/all/m1 | call | 273.91ms | 273.19ms | 1.003 | [0.999, 1.006] | ~ (noise) |
| micro/vec/quad/ct/equi/b1/d20/L4/all/m1 | call | 262.97ms | 262.39ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b1/d20/L4/all/m1 | call | 270.39ms | 270.70ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b1/d20/L4/all/m1 | call | 280.90ms | 279.93ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/vec/quad/batch/equi/b1/d20/L4/all/m1 | call | 268.87ms | 268.29ms | 1.001 | [0.993, 1.008] | ~ (noise) |
| micro/vec/quad/scalar_x_m/equi/b1/d20/L4/all/m2 | call | 508.18ms | 507.87ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/vec/quad/rt/equi/b1/d20/L4/all/m2 | call | 353.87ms | 352.93ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/vec/quad/ct/equi/b1/d20/L4/all/m2 | call | 342.22ms | 341.96ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b1/d20/L4/all/m2 | call | 324.36ms | 323.53ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b1/d20/L4/all/m2 | call | 335.04ms | 334.89ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/vec/quad/batch/equi/b1/d20/L4/all/m2 | call | 319.73ms | 320.33ms | 0.998 | [0.994, 1.001] | ~ (noise) |
| micro/vec/quad/scalar_x_m/equi/b1/d20/L4/all/m4 | call | 1.017s | 1.017s | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/vec/quad/rt/equi/b1/d20/L4/all/m4 | call | 609.55ms | 609.76ms | 0.997 | [0.986, 1.007] | ~ (noise) |
| micro/vec/quad/ct/equi/b1/d20/L4/all/m4 | call | 601.83ms | 601.12ms | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b1/d20/L4/all/m4 | call | 579.37ms | 578.43ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b1/d20/L4/all/m4 | call | 595.72ms | 594.99ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/vec/quad/batch/equi/b1/d20/L4/all/m4 | call | 589.12ms | 585.70ms | 1.005 | [1.001, 1.008] | faster |
| micro/vec/quad/scalar_x_m/equi/b1/d20/L4/all/m8 | call | 2.033s | 2.032s | 1.011 | [0.980, 1.043] | ~ (noise) |
| micro/vec/quad/rt/equi/b1/d20/L4/all/m8 | call | 987.15ms | 990.07ms | 0.997 | [0.993, 1.000] | ~ (noise) |
| micro/vec/quad/ct/equi/b1/d20/L4/all/m8 | call | 975.63ms | 979.76ms | 0.990 | [0.974, 1.007] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b1/d20/L4/all/m8 | call | 955.38ms | 957.22ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b1/d20/L4/all/m8 | call | 983.94ms | 986.02ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/vec/quad/batch/equi/b1/d20/L4/all/m8 | call | 982.55ms | 980.07ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/vec/quad/scalar_x_m/equi/b1/d20/L4/all/m32 | call | 8.139s | 8.131s | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/vec/quad/rt/equi/b1/d20/L4/all/m32 | call | 3.413s | 3.412s | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/vec/quad/ct/equi/b1/d20/L4/all/m32 | call | 3.426s | 3.425s | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/vec/quad/rowbuf/equi/b1/d20/L4/all/m32 | call | 3.397s | 3.392s | 1.003 | [0.997, 1.010] | ~ (noise) |
| micro/vec/quad/rowbufk/equi/b1/d20/L4/all/m32 | call | 3.498s | 3.504s | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/vec/quad/batch/equi/b1/d20/L4/all/m32 | call | 3.544s | 3.541s | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/vec/dsg/scalar/equi/b0/d10/L7/m1 | call | 67.91ms | 68.66ms | 0.982 | [0.957, 1.007] | ~ (noise) |
| micro/vec/dsg/rt_direct/equi/b0/d10/L7/m1 | call | 82.83ms | 81.93ms | 1.013 | [1.002, 1.024] | faster |
| micro/vec/dsg/batch_direct/equi/b0/d10/L7/m1 | call | 81.71ms | 81.29ms | 1.005 | [1.001, 1.008] | faster |
| micro/vec/dsg/scalar_x_m/equi/b0/d10/L7/m1 | call | 145.76ms | 145.76ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/vec/dsg/rt/equi/b0/d10/L7/m1 | call | 154.49ms | 154.34ms | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/vec/dsg/batch/equi/b0/d10/L7/m1 | call | 156.05ms | 156.87ms | 0.995 | [0.992, 0.999] | SLOWER |
| micro/vec/dsg/scalar_x_m/equi/b0/d10/L7/m2 | call | 291.27ms | 291.76ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/vec/dsg/rt/equi/b0/d10/L7/m2 | call | 201.20ms | 201.11ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/vec/dsg/batch/equi/b0/d10/L7/m2 | call | 190.96ms | 191.28ms | 0.987 | [0.950, 1.026] | ~ (noise) |
| micro/vec/dsg/scalar_x_m/equi/b0/d10/L7/m4 | call | 582.88ms | 583.02ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/vec/dsg/rt/equi/b0/d10/L7/m4 | call | 321.65ms | 321.73ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/vec/dsg/batch/equi/b0/d10/L7/m4 | call | 312.61ms | 312.32ms | 1.005 | [0.993, 1.018] | ~ (noise) |
| micro/vec/dsg/scalar_x_m/equi/b0/d10/L7/m8 | call | 1.166s | 1.167s | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/vec/dsg/rt/equi/b0/d10/L7/m8 | call | 501.90ms | 505.71ms | 0.992 | [0.989, 0.996] | SLOWER |
| micro/vec/dsg/batch/equi/b0/d10/L7/m8 | call | 515.33ms | 514.44ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| micro/vec/dsg/scalar_x_m/equi/b0/d10/L7/m32 | call | 4.663s | 4.668s | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/vec/dsg/rt/equi/b0/d10/L7/m32 | call | 1.659s | 1.660s | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/vec/dsg/batch/equi/b0/d10/L7/m32 | call | 1.771s | 1.749s | 1.036 | [0.969, 1.108] | ~ (noise) |
| micro/vec/dsg/scalar/equi/b1/d20/L6/m1 | call | 126.06us | 126.29us | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/vec/dsg/rt_direct/equi/b1/d20/L6/m1 | call | 160.60us | 160.29us | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/vec/dsg/batch_direct/equi/b1/d20/L6/m1 | call | 144.50us | 144.76us | 0.996 | [0.993, 1.000] | ~ (noise) |
| micro/vec/dsg/scalar_x_m/equi/b1/d20/L6/m1 | call | 204.45us | 204.51us | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/vec/dsg/rt/equi/b1/d20/L6/m1 | call | 225.53us | 223.17us | 1.016 | [0.993, 1.040] | ~ (noise) |
| micro/vec/dsg/batch/equi/b1/d20/L6/m1 | call | 223.50us | 223.14us | 0.997 | [0.983, 1.012] | ~ (noise) |
| micro/vec/dsg/scalar_x_m/equi/b1/d20/L6/m2 | call | 408.52us | 409.35us | 0.988 | [0.967, 1.010] | ~ (noise) |
| micro/vec/dsg/rt/equi/b1/d20/L6/m2 | call | 313.78us | 313.65us | 0.989 | [0.952, 1.028] | ~ (noise) |
| micro/vec/dsg/batch/equi/b1/d20/L6/m2 | call | 296.98us | 294.85us | 1.009 | [1.000, 1.018] | faster |
| micro/vec/dsg/scalar_x_m/equi/b1/d20/L6/m4 | call | 818.34us | 819.44us | 0.995 | [0.983, 1.006] | ~ (noise) |
| micro/vec/dsg/rt/equi/b1/d20/L6/m4 | call | 570.68us | 569.54us | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/vec/dsg/batch/equi/b1/d20/L6/m4 | call | 550.47us | 548.59us | 1.006 | [1.001, 1.011] | faster |
| micro/vec/dsg/scalar_x_m/equi/b1/d20/L6/m8 | call | 1.63ms | 1.64ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/vec/dsg/rt/equi/b1/d20/L6/m8 | call | 950.33us | 949.83us | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/vec/dsg/batch/equi/b1/d20/L6/m8 | call | 955.57us | 951.42us | 1.004 | [1.000, 1.008] | faster |
| micro/vec/dsg/scalar_x_m/equi/b1/d20/L6/m32 | call | 6.55ms | 6.55ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/vec/dsg/rt/equi/b1/d20/L6/m32 | call | 3.43ms | 3.43ms | 0.996 | [0.980, 1.012] | ~ (noise) |
| micro/vec/dsg/batch/equi/b1/d20/L6/m32 | call | 3.57ms | 3.57ms | 1.001 | [0.996, 1.005] | ~ (noise) |
| micro/vec/interp/scalar/cc/b0/d2/L12/m1 | call | 1.52ms | 1.52ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/vec/interp/inner_direct/cc/b0/d2/L12/m1 | call | 1.81ms | 1.81ms | 0.991 | [0.973, 1.010] | ~ (noise) |
| micro/vec/interp/innerk_direct/cc/b0/d2/L12/m1 | call | 1.78ms | 1.78ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/vec/interp/outer_direct/cc/b0/d2/L12/m1 | call | 1.62ms | 1.62ms | 1.004 | [1.001, 1.008] | faster |
| micro/vec/interp/scalar_x_m/cc/b0/d2/L12/m1 | call | 1.55ms | 1.55ms | 1.019 | [0.956, 1.086] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d2/L12/m1 | call | 1.88ms | 1.85ms | 1.015 | [0.997, 1.034] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d2/L12/m1 | call | 1.83ms | 1.83ms | 0.996 | [0.991, 1.002] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d2/L12/m1 | call | 1.68ms | 1.69ms | 1.000 | [0.972, 1.029] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d2/L12/m2 | call | 3.10ms | 3.09ms | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d2/L12/m2 | call | 2.81ms | 2.81ms | 1.004 | [0.987, 1.021] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d2/L12/m2 | call | 2.80ms | 2.80ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d2/L12/m2 | call | 2.81ms | 2.79ms | 1.009 | [0.999, 1.021] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d2/L12/m4 | call | 6.19ms | 6.19ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d2/L12/m4 | call | 4.53ms | 4.52ms | 1.002 | [0.997, 1.008] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d2/L12/m4 | call | 4.57ms | 4.58ms | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d2/L12/m4 | call | 4.78ms | 4.76ms | 1.006 | [0.996, 1.015] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d2/L12/m8 | call | 12.41ms | 12.39ms | 1.001 | [0.998, 1.005] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d2/L12/m8 | call | 8.60ms | 8.62ms | 0.997 | [0.992, 1.002] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d2/L12/m8 | call | 8.84ms | 8.86ms | 0.997 | [0.994, 1.001] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d2/L12/m8 | call | 9.17ms | 9.14ms | 1.007 | [0.996, 1.018] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d2/L12/m32 | call | 50.77ms | 50.12ms | 1.015 | [1.004, 1.027] | faster |
| micro/vec/interp/inner/cc/b0/d2/L12/m32 | call | 32.34ms | 32.36ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d2/L12/m32 | call | 33.69ms | 33.68ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d2/L12/m32 | call | 36.13ms | 36.02ms | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/vec/interp/scalar/cc/b0/d5/L6/m1 | call | 1.14ms | 1.14ms | 0.996 | [0.992, 0.999] | SLOWER |
| micro/vec/interp/inner_direct/cc/b0/d5/L6/m1 | call | 1.22ms | 1.24ms | 0.992 | [0.978, 1.006] | ~ (noise) |
| micro/vec/interp/innerk_direct/cc/b0/d5/L6/m1 | call | 1.21ms | 1.22ms | 0.993 | [0.989, 0.997] | SLOWER |
| micro/vec/interp/outer_direct/cc/b0/d5/L6/m1 | call | 1.20ms | 1.20ms | 1.068 | [0.889, 1.283] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d5/L6/m1 | call | 1.15ms | 1.15ms | 1.000 | [0.994, 1.007] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d5/L6/m1 | call | 1.26ms | 1.26ms | 1.017 | [0.990, 1.044] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d5/L6/m1 | call | 1.25ms | 1.26ms | 0.992 | [0.986, 0.998] | SLOWER |
| micro/vec/interp/outer/cc/b0/d5/L6/m1 | call | 1.23ms | 1.23ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d5/L6/m2 | call | 2.29ms | 2.29ms | 0.996 | [0.985, 1.007] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d5/L6/m2 | call | 2.17ms | 2.17ms | 1.000 | [0.993, 1.006] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d5/L6/m2 | call | 2.16ms | 2.18ms | 0.995 | [0.989, 1.000] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d5/L6/m2 | call | 2.24ms | 2.24ms | 1.008 | [0.978, 1.039] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d5/L6/m4 | call | 4.58ms | 4.59ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d5/L6/m4 | call | 3.94ms | 3.95ms | 0.996 | [0.988, 1.004] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d5/L6/m4 | call | 3.96ms | 3.97ms | 1.002 | [0.986, 1.020] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d5/L6/m4 | call | 4.23ms | 4.25ms | 0.996 | [0.992, 1.000] | SLOWER |
| micro/vec/interp/scalar_x_m/cc/b0/d5/L6/m8 | call | 9.19ms | 9.22ms | 0.996 | [0.993, 1.000] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d5/L6/m8 | call | 7.64ms | 7.63ms | 1.002 | [0.993, 1.012] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d5/L6/m8 | call | 7.76ms | 7.77ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d5/L6/m8 | call | 8.40ms | 8.39ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d5/L6/m32 | call | 37.26ms | 37.26ms | 1.004 | [0.985, 1.023] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d5/L6/m32 | call | 29.37ms | 29.38ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d5/L6/m32 | call | 29.94ms | 30.01ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d5/L6/m32 | call | 33.07ms | 33.05ms | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/vec/interp/scalar/cheb1/b0/d5/L4/m1 | call | 504.35us | 511.41us | 0.989 | [0.982, 0.997] | SLOWER |
| micro/vec/interp/inner_direct/cheb1/b0/d5/L4/m1 | call | 536.22us | 544.38us | 0.979 | [0.952, 1.007] | ~ (noise) |
| micro/vec/interp/innerk_direct/cheb1/b0/d5/L4/m1 | call | 518.49us | 522.28us | 0.994 | [0.983, 1.004] | ~ (noise) |
| micro/vec/interp/outer_direct/cheb1/b0/d5/L4/m1 | call | 519.58us | 525.19us | 1.000 | [0.967, 1.034] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cheb1/b0/d5/L4/m1 | call | 508.31us | 510.48us | 0.994 | [0.975, 1.012] | ~ (noise) |
| micro/vec/interp/inner/cheb1/b0/d5/L4/m1 | call | 540.69us | 552.88us | 0.934 | [0.808, 1.079] | ~ (noise) |
| micro/vec/interp/innerk/cheb1/b0/d5/L4/m1 | call | 528.89us | 530.30us | 1.001 | [0.994, 1.008] | ~ (noise) |
| micro/vec/interp/outer/cheb1/b0/d5/L4/m1 | call | 528.73us | 536.37us | 0.965 | [0.925, 1.008] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cheb1/b0/d5/L4/m2 | call | 1.02ms | 1.02ms | 1.002 | [0.995, 1.009] | ~ (noise) |
| micro/vec/interp/inner/cheb1/b0/d5/L4/m2 | call | 779.71us | 776.04us | 1.002 | [0.997, 1.006] | ~ (noise) |
| micro/vec/interp/innerk/cheb1/b0/d5/L4/m2 | call | 758.09us | 757.44us | 1.001 | [0.993, 1.009] | ~ (noise) |
| micro/vec/interp/outer/cheb1/b0/d5/L4/m2 | call | 1.02ms | 1.03ms | 1.006 | [0.976, 1.036] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cheb1/b0/d5/L4/m4 | call | 2.02ms | 2.03ms | 0.995 | [0.988, 1.001] | ~ (noise) |
| micro/vec/interp/inner/cheb1/b0/d5/L4/m4 | call | 1.24ms | 1.24ms | 0.994 | [0.974, 1.014] | ~ (noise) |
| micro/vec/interp/innerk/cheb1/b0/d5/L4/m4 | call | 1.22ms | 1.22ms | 1.004 | [1.001, 1.008] | faster |
| micro/vec/interp/outer/cheb1/b0/d5/L4/m4 | call | 1.97ms | 1.97ms | 1.003 | [0.984, 1.023] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cheb1/b0/d5/L4/m8 | call | 4.05ms | 4.07ms | 0.996 | [0.992, 1.000] | SLOWER |
| micro/vec/interp/inner/cheb1/b0/d5/L4/m8 | call | 2.20ms | 2.19ms | 1.004 | [0.999, 1.010] | ~ (noise) |
| micro/vec/interp/innerk/cheb1/b0/d5/L4/m8 | call | 2.21ms | 2.21ms | 0.998 | [0.992, 1.005] | ~ (noise) |
| micro/vec/interp/outer/cheb1/b0/d5/L4/m8 | call | 3.88ms | 3.89ms | 0.997 | [0.990, 1.004] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cheb1/b0/d5/L4/m32 | call | 16.23ms | 16.34ms | 0.995 | [0.983, 1.007] | ~ (noise) |
| micro/vec/interp/inner/cheb1/b0/d5/L4/m32 | call | 7.84ms | 7.83ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/vec/interp/innerk/cheb1/b0/d5/L4/m32 | call | 7.93ms | 7.96ms | 0.999 | [0.988, 1.010] | ~ (noise) |
| micro/vec/interp/outer/cheb1/b0/d5/L4/m32 | call | 15.36ms | 15.42ms | 0.998 | [0.991, 1.004] | ~ (noise) |
| micro/vec/interp/scalar/cc/b0/d10/L2/m1 | call | 46.26ms | 46.41ms | 0.996 | [0.986, 1.006] | ~ (noise) |
| micro/vec/interp/inner_direct/cc/b0/d10/L2/m1 | call | 47.85ms | 47.78ms | 1.018 | [0.969, 1.070] | ~ (noise) |
| micro/vec/interp/innerk_direct/cc/b0/d10/L2/m1 | call | 47.91ms | 47.88ms | 1.000 | [0.995, 1.004] | ~ (noise) |
| micro/vec/interp/outer_direct/cc/b0/d10/L2/m1 | call | 47.78ms | 47.56ms | 1.004 | [0.999, 1.009] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d10/L2/m1 | call | 48.36ms | 48.25ms | 1.003 | [0.998, 1.009] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d10/L2/m1 | call | 49.98ms | 49.94ms | 1.009 | [0.987, 1.032] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d10/L2/m1 | call | 49.60ms | 50.01ms | 0.996 | [0.985, 1.006] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d10/L2/m1 | call | 49.89ms | 49.60ms | 1.002 | [0.991, 1.014] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d10/L2/m2 | call | 96.30ms | 96.52ms | 0.997 | [0.974, 1.019] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d10/L2/m2 | call | 87.85ms | 87.57ms | 1.002 | [0.991, 1.014] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d10/L2/m2 | call | 87.73ms | 87.94ms | 0.948 | [0.813, 1.105] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d10/L2/m2 | call | 89.29ms | 89.44ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d10/L2/m4 | call | 193.30ms | 193.24ms | 0.999 | [0.985, 1.013] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d10/L2/m4 | call | 152.95ms | 153.10ms | 0.995 | [0.984, 1.005] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d10/L2/m4 | call | 153.57ms | 153.58ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d10/L2/m4 | call | 158.53ms | 158.59ms | 1.004 | [0.990, 1.018] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d10/L2/m8 | call | 387.56ms | 390.79ms | 0.993 | [0.977, 1.009] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d10/L2/m8 | call | 288.34ms | 288.25ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d10/L2/m8 | call | 290.10ms | 290.05ms | 0.998 | [0.994, 1.003] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d10/L2/m8 | call | 301.55ms | 301.64ms | 1.000 | [0.995, 1.004] | ~ (noise) |
| micro/vec/interp/scalar_x_m/cc/b0/d10/L2/m32 | call | 1.581s | 1.580s | 0.997 | [0.988, 1.006] | ~ (noise) |
| micro/vec/interp/inner/cc/b0/d10/L2/m32 | call | 1.098s | 1.094s | 1.012 | [0.987, 1.038] | ~ (noise) |
| micro/vec/interp/innerk/cc/b0/d10/L2/m32 | call | 1.103s | 1.103s | 0.999 | [0.993, 1.005] | ~ (noise) |
| micro/vec/interp/outer/cc/b0/d10/L2/m32 | call | 1.158s | 1.159s | 1.001 | [0.994, 1.008] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d2/L16/m1 | call | 6.31ms | 6.31ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m1 | call | 9.56ms | 9.55ms | 0.989 | [0.961, 1.018] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d10/L4/m1 | call | 33.78ms | 33.77ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m1 | call | 37.36ms | 37.36ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/quad/process_all_tgs_scalar_x_m/equi/b1/d20/L4/m1 | call | 253.09ms | 253.12ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m1 | call | 271.50ms | 270.82ms | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d2/L16/m2 | call | 12.64ms | 12.66ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m2 | call | 15.67ms | 15.66ms | 0.987 | [0.946, 1.030] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d10/L4/m2 | call | 67.61ms | 67.48ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m2 | call | 48.73ms | 48.67ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/quad/process_all_tgs_scalar_x_m/equi/b1/d20/L4/m2 | call | 506.59ms | 506.16ms | 1.003 | [0.996, 1.011] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m2 | call | 348.53ms | 348.38ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d2/L16/m4 | call | 25.30ms | 25.36ms | 1.005 | [0.989, 1.021] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m4 | call | 24.91ms | 24.77ms | 1.007 | [1.004, 1.011] | faster |
| micro/quad/process_tg_scalar_x_m/equi/b0/d10/L4/m4 | call | 135.03ms | 135.00ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m4 | call | 77.86ms | 77.83ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs_scalar_x_m/equi/b1/d20/L4/m4 | call | 1.013s | 1.013s | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m4 | call | 592.64ms | 593.24ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d2/L16/m8 | call | 50.61ms | 50.64ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m8 | call | 42.54ms | 42.68ms | 0.998 | [0.994, 1.001] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d10/L4/m8 | call | 270.20ms | 270.29ms | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m8 | call | 121.03ms | 120.76ms | 1.000 | [0.986, 1.014] | ~ (noise) |
| micro/quad/process_all_tgs_scalar_x_m/equi/b1/d20/L4/m8 | call | 2.026s | 2.025s | 0.995 | [0.980, 1.011] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m8 | call | 942.70ms | 942.65ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d2/L16/m32 | call | 201.95ms | 202.55ms | 0.996 | [0.991, 1.001] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d2/L16/m32 | call | 159.95ms | 160.25ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| micro/quad/process_tg_scalar_x_m/equi/b0/d10/L4/m32 | call | 1.081s | 1.081s | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/quad/process_tg_vec/equi/b0/d10/L4/m32 | call | 403.49ms | 403.10ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/quad/process_all_tgs_scalar_x_m/equi/b1/d20/L4/m32 | call | 8.107s | 8.101s | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/quad/process_all_tgs_vec/equi/b1/d20/L4/m32 | call | 3.273s | 3.273s | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b0/d10/L7/m1 | call | 145.34ms | 145.66ms | 0.996 | [0.988, 1.003] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m1 | call | 152.37ms | 152.25ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b1/d20/L6/m1 | call | 202.83us | 203.50us | 0.996 | [0.991, 1.000] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b1/d20/L6/m1 | call | 219.08us | 218.97us | 1.002 | [0.997, 1.006] | ~ (noise) |
| micro/dsg/process_delta_tgs_scalar_x_m/equi/b1/d20/L6/s256/m1 | call | 95.08ms | 95.23ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tgs_vec/equi/b1/d20/L6/s256/m1 | call | 98.88ms | 99.05ms | 0.998 | [0.982, 1.015] | ~ (noise) |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b0/d10/L7/m2 | call | 290.55ms | 290.52ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m2 | call | 172.70ms | 173.30ms | 1.000 | [0.966, 1.034] | ~ (noise) |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b1/d20/L6/m2 | call | 404.71us | 406.79us | 0.996 | [0.993, 1.000] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b1/d20/L6/m2 | call | 286.94us | 287.63us | 1.002 | [0.980, 1.023] | ~ (noise) |
| micro/dsg/process_delta_tgs_scalar_x_m/equi/b1/d20/L6/s256/m2 | call | 190.00ms | 190.39ms | 0.998 | [0.990, 1.006] | ~ (noise) |
| micro/dsg/process_delta_tgs_vec/equi/b1/d20/L6/s256/m2 | call | 111.39ms | 111.35ms | 0.995 | [0.979, 1.012] | ~ (noise) |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b0/d10/L7/m4 | call | 580.89ms | 580.92ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m4 | call | 309.52ms | 308.73ms | 0.987 | [0.949, 1.026] | ~ (noise) |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b1/d20/L6/m4 | call | 812.24us | 814.23us | 1.089 | [0.852, 1.393] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b1/d20/L6/m4 | call | 548.09us | 548.74us | 1.001 | [0.981, 1.022] | ~ (noise) |
| micro/dsg/process_delta_tgs_scalar_x_m/equi/b1/d20/L6/s256/m4 | call | 379.84ms | 381.02ms | 1.002 | [0.992, 1.012] | ~ (noise) |
| micro/dsg/process_delta_tgs_vec/equi/b1/d20/L6/s256/m4 | call | 190.61ms | 191.71ms | 0.995 | [0.992, 0.999] | SLOWER |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b0/d10/L7/m8 | call | 1.162s | 1.162s | 0.999 | [0.996, 1.003] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m8 | call | 475.31ms | 477.77ms | 0.993 | [0.987, 0.998] | SLOWER |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b1/d20/L6/m8 | call | 1.62ms | 1.63ms | 0.996 | [0.992, 0.999] | SLOWER |
| micro/dsg/process_delta_tg_vec/equi/b1/d20/L6/m8 | call | 909.64us | 908.86us | 1.000 | [0.964, 1.038] | ~ (noise) |
| micro/dsg/process_delta_tgs_scalar_x_m/equi/b1/d20/L6/s256/m8 | call | 760.37ms | 762.23ms | 0.999 | [0.986, 1.012] | ~ (noise) |
| micro/dsg/process_delta_tgs_vec/equi/b1/d20/L6/s256/m8 | call | 289.31ms | 289.89ms | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b0/d10/L7/m32 | call | 4.646s | 4.648s | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/dsg/process_delta_tg_vec/equi/b0/d10/L7/m32 | call | 1.560s | 1.567s | 0.998 | [0.984, 1.012] | ~ (noise) |
| micro/dsg/process_delta_tg_scalar_x_m/equi/b1/d20/L6/m32 | call | 6.46ms | 6.50ms | 0.995 | [0.991, 0.999] | SLOWER |
| micro/dsg/process_delta_tg_vec/equi/b1/d20/L6/m32 | call | 3.32ms | 3.33ms | 0.998 | [0.988, 1.008] | ~ (noise) |
| micro/dsg/process_delta_tgs_scalar_x_m/equi/b1/d20/L6/s256/m32 | call | 3.050s | 3.059s | 0.994 | [0.983, 1.005] | ~ (noise) |
| micro/dsg/process_delta_tgs_vec/equi/b1/d20/L6/s256/m32 | call | 962.10ms | 959.24ms | 1.005 | [0.996, 1.014] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d2/L12/m1 | call | 1.40ms | 1.40ms | 0.998 | [0.994, 1.003] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m1 | call | 1.69ms | 1.67ms | 1.024 | [0.982, 1.067] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d5/L6/m1 | call | 1.12ms | 1.12ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m1 | call | 1.27ms | 1.28ms | 0.994 | [0.986, 1.003] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cheb1/b0/d5/L4/m1 | call | 508.25us | 508.67us | 1.005 | [0.985, 1.026] | ~ (noise) |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m1 | call | 517.84us | 516.37us | 1.007 | [0.995, 1.019] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d10/L2/m1 | call | 48.40ms | 48.43ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m1 | call | 50.12ms | 50.30ms | 0.998 | [0.986, 1.011] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m1 | call | 208.9ns | 211.3ns | 0.987 | [0.983, 0.991] | SLOWER |
| micro/interplin/tg_vec/equi/b1/d2/L16/m1 | call | 232.8ns | 233.2ns | 0.998 | [0.992, 1.004] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m1 | call | 42.53us | 42.47us | 1.001 | [0.997, 1.005] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m1 | call | 43.32us | 43.22us | 1.003 | [0.993, 1.013] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m1 | call | 236.33us | 234.50us | 1.008 | [0.978, 1.039] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m1 | call | 243.98us | 242.73us | 1.018 | [0.961, 1.079] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d2/L12/m2 | call | 2.79ms | 2.80ms | 1.004 | [0.984, 1.025] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m2 | call | 2.60ms | 2.62ms | 0.993 | [0.990, 0.997] | SLOWER |
| micro/interp/global_tg_scalar_x_m/cc/b0/d5/L6/m2 | call | 2.24ms | 2.24ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m2 | call | 2.17ms | 2.18ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cheb1/b0/d5/L4/m2 | call | 1.01ms | 1.02ms | 0.996 | [0.981, 1.010] | ~ (noise) |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m2 | call | 743.95us | 746.18us | 0.999 | [0.983, 1.015] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d10/L2/m2 | call | 95.47ms | 96.75ms | 0.997 | [0.981, 1.013] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m2 | call | 89.16ms | 89.19ms | 1.000 | [0.996, 1.005] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m2 | call | 416.1ns | 420.7ns | 0.989 | [0.984, 0.994] | SLOWER |
| micro/interplin/tg_vec/equi/b1/d2/L16/m2 | call | 368.0ns | 368.3ns | 0.998 | [0.993, 1.003] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m2 | call | 85.10us | 84.79us | 1.003 | [1.000, 1.007] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m2 | call | 78.52us | 78.40us | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m2 | call | 472.11us | 476.79us | 0.987 | [0.957, 1.018] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m2 | call | 468.17us | 463.25us | 1.015 | [0.995, 1.037] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d2/L12/m4 | call | 5.57ms | 5.60ms | 0.994 | [0.984, 1.004] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m4 | call | 4.49ms | 4.48ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d5/L6/m4 | call | 4.49ms | 4.49ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m4 | call | 3.93ms | 3.91ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cheb1/b0/d5/L4/m4 | call | 2.03ms | 2.03ms | 0.999 | [0.986, 1.012] | ~ (noise) |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m4 | call | 1.20ms | 1.21ms | 1.004 | [0.962, 1.047] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d10/L2/m4 | call | 193.63ms | 193.83ms | 0.999 | [0.982, 1.017] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m4 | call | 154.06ms | 154.00ms | 1.001 | [0.982, 1.021] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m4 | call | 854.0ns | 859.4ns | 0.995 | [0.991, 0.999] | SLOWER |
| micro/interplin/tg_vec/equi/b1/d2/L16/m4 | call | 648.0ns | 649.1ns | 1.001 | [0.996, 1.006] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m4 | call | 170.12us | 169.81us | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m4 | call | 151.10us | 150.20us | 1.004 | [0.996, 1.012] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m4 | call | 939.88us | 950.36us | 1.000 | [0.962, 1.039] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m4 | call | 861.71us | 862.60us | 1.000 | [0.990, 1.011] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d2/L12/m8 | call | 11.16ms | 11.15ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m8 | call | 8.33ms | 8.32ms | 1.000 | [0.995, 1.004] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d5/L6/m8 | call | 9.05ms | 9.06ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m8 | call | 7.64ms | 7.62ms | 1.006 | [0.987, 1.025] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cheb1/b0/d5/L4/m8 | call | 4.08ms | 4.08ms | 0.995 | [0.981, 1.010] | ~ (noise) |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m8 | call | 2.16ms | 2.16ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d10/L2/m8 | call | 392.88ms | 385.38ms | 1.012 | [0.999, 1.026] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m8 | call | 289.24ms | 288.96ms | 1.004 | [0.995, 1.013] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m8 | call | 1.73us | 1.74us | 0.994 | [0.987, 1.001] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d2/L16/m8 | call | 1.19us | 1.19us | 0.999 | [0.991, 1.006] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m8 | call | 344.50us | 342.42us | 1.007 | [1.000, 1.014] | faster |
| micro/interplin/tg_vec/equi/b1/d8/L5/m8 | call | 293.29us | 292.83us | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m8 | call | 2.05ms | 2.03ms | 1.012 | [0.972, 1.053] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m8 | call | 1.65ms | 1.66ms | 0.999 | [0.977, 1.022] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d2/L12/m32 | call | 45.41ms | 45.36ms | 1.007 | [0.985, 1.031] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d2/L12/m32 | call | 31.86ms | 31.76ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d5/L6/m32 | call | 36.53ms | 36.46ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d5/L6/m32 | call | 29.36ms | 29.28ms | 1.004 | [1.000, 1.008] | faster |
| micro/interp/global_tg_scalar_x_m/cheb1/b0/d5/L4/m32 | call | 16.34ms | 16.33ms | 1.003 | [0.985, 1.021] | ~ (noise) |
| micro/interp/global_tg_vec/cheb1/b0/d5/L4/m32 | call | 7.68ms | 7.69ms | 1.000 | [0.993, 1.008] | ~ (noise) |
| micro/interp/global_tg_scalar_x_m/cc/b0/d10/L2/m32 | call | 1.611s | 1.572s | 1.013 | [0.995, 1.031] | ~ (noise) |
| micro/interp/global_tg_vec/cc/b0/d10/L2/m32 | call | 1.056s | 1.057s | 0.999 | [0.995, 1.002] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d2/L16/m32 | call | 7.08us | 7.12us | 0.995 | [0.991, 1.000] | SLOWER |
| micro/interplin/tg_vec/equi/b1/d2/L16/m32 | call | 4.52us | 4.52us | 1.011 | [0.988, 1.034] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d8/L5/m32 | call | 1.39ms | 1.38ms | 1.008 | [0.993, 1.024] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d8/L5/m32 | call | 1.15ms | 1.15ms | 1.005 | [0.992, 1.018] | ~ (noise) |
| micro/interplin/tg_scalar_x_m/equi/b1/d10/L5/m32 | call | 8.48ms | 8.46ms | 0.998 | [0.978, 1.018] | ~ (noise) |
| micro/interplin/tg_vec/equi/b1/d10/L5/m32 | call | 6.08ms | 6.08ms | 1.001 | [0.994, 1.008] | ~ (noise) |

Sum of medians (call, common cases): A = 110.762s, B = 110.714s, saved = 47.76ms (1.000x)
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 1406, 1406, 1405, 1404, 1404 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 7025s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
