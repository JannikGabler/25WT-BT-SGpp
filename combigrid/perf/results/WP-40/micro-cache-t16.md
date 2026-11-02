A/B `micro`: A=`WP-40-before` B=`WP-40-after`, 16 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c0 | call | 655.35us | 642.57us | 1.062 | [0.985, 1.145] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c100 | call | 1.85ms | 1.84ms | 1.009 | [1.001, 1.016] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c1000 | call | 14.91ms | 14.93ms | 0.998 | [0.990, 1.007] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c0 | call | 588.85us | 589.94us | 0.995 | [0.985, 1.006] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c100 | call | 1.34ms | 1.37ms | 0.947 | [0.890, 1.007] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c1000 | call | 10.76ms | 10.75ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c0 | call | 507.86us | 460.78us | 1.104 | [1.078, 1.130] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c100 | call | 2.10ms | 2.08ms | 0.996 | [0.964, 1.029] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c1000 | call | 18.73ms | 18.68ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c0 | call | 605.82us | 603.86us | 1.006 | [0.993, 1.018] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c100 | call | 1.90ms | 1.89ms | 1.004 | [0.920, 1.096] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c1000 | call | 16.05ms | 16.04ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c0 | call | 586.74us | 576.25us | 1.019 | [1.008, 1.030] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c100 | call | 1.28ms | 1.27ms | 1.009 | [1.001, 1.017] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c1000 | call | 8.92ms | 8.92ms | 0.996 | [0.983, 1.010] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c0 | call | 5.23ms | 5.23ms | 1.004 | [0.989, 1.019] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c100 | call | 7.81ms | 7.82ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c1000 | call | 37.49ms | 37.44ms | 1.001 | [0.991, 1.011] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c0 | call | 6.85ms | 6.80ms | 1.023 | [0.993, 1.054] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c100 | call | 8.37ms | 8.38ms | 1.018 | [0.980, 1.058] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c1000 | call | 30.03ms | 30.02ms | 0.999 | [0.983, 1.014] | ~ (noise) |
| micro/cache/lookup/production/d2/n10000 | call | 103.38us | 106.28us | 0.970 | [0.921, 1.021] | ~ (noise) |
| micro/cache/insert/production/d2/n10000 | call | 261.46us | 266.60us | 0.918 | [0.805, 1.048] | ~ (noise) |
| micro/cache/lookup/production/d2/n1000000 | call | 1.01ms | 1.06ms | 0.972 | [0.914, 1.033] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 40.31ms | 40.63ms | 0.989 | [0.968, 1.011] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 126.88us | 466.89us | 0.504 | [0.174, 1.456] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 279.54us | 285.73us | 0.639 | [0.184, 2.219] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 1.49ms | 1.42ms | 1.078 | [1.012, 1.148] | faster |
| micro/cache/insert/production/d10/n1000000 | call | 90.75ms | 90.62ms | 1.000 | [0.971, 1.029] | ~ (noise) |
| micro/cache/lookup/production/d20/n10000 | call | 134.83us | 136.42us | 0.674 | [0.240, 1.890] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 295.62us | 290.64us | 1.436 | [0.537, 3.838] | ~ (noise) |
| micro/cache/lookup/production/d20/n1000000 | call | 1.90ms | 1.89ms | 1.020 | [0.951, 1.092] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 128.03ms | 131.29ms | 0.973 | [0.951, 0.995] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 4.28ms | 4.27ms | 1.021 | [0.960, 1.086] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 2.54ms | 2.54ms | 1.006 | [0.986, 1.026] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 6.80ms | 6.81ms | 0.997 | [0.991, 1.002] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 2.56ms | 2.55ms | 1.013 | [0.962, 1.067] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 8.40ms | 8.62ms | 0.972 | [0.913, 1.036] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c0 | call | 2.47ms | 2.46ms | 1.007 | [0.994, 1.019] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 12.67ms | 12.71ms | 0.996 | [0.974, 1.017] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c1000 | call | 2.45ms | 2.47ms | 0.979 | [0.953, 1.007] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 5.61ms | 5.63ms | 1.000 | [0.971, 1.029] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 2.55ms | 2.53ms | 1.008 | [0.998, 1.017] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 14.02ms | 14.06ms | 1.001 | [0.994, 1.007] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c1000 | call | 2.54ms | 2.53ms | 1.026 | [0.966, 1.089] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 8.10ms | 8.22ms | 1.004 | [0.964, 1.046] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 3.01ms | 2.96ms | 1.004 | [0.984, 1.024] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 12.87ms | 12.87ms | 0.974 | [0.898, 1.057] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 2.98ms | 2.99ms | 0.997 | [0.973, 1.022] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 5.29ms | 5.29ms | 1.007 | [0.979, 1.036] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c0 | call | 1.67ms | 1.68ms | 0.986 | [0.952, 1.021] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 12.98ms | 12.95ms | 1.004 | [0.985, 1.024] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c1000 | call | 1.69ms | 1.67ms | 1.014 | [0.979, 1.051] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 6.79ms | 6.81ms | 0.998 | [0.978, 1.019] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 5.94ms | 5.98ms | 0.995 | [0.988, 1.002] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 7.87ms | 7.91ms | 0.985 | [0.964, 1.006] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 5.93ms | 5.95ms | 1.027 | [0.962, 1.095] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 11.31ms | 11.37ms | 0.983 | [0.968, 0.998] | SLOWER |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c0 | call | 4.90ms | 4.92ms | 0.995 | [0.974, 1.016] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 18.19ms | 18.22ms | 0.976 | [0.911, 1.046] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c1000 | call | 4.91ms | 4.90ms | 1.005 | [0.976, 1.034] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit1GiB | call | 364.08ms | 360.81ms | 1.006 | [0.999, 1.013] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit16MiB | call | 233.36ms | 235.38ms | 0.997 | [0.985, 1.010] | ~ (noise) |
| micro/cache/disk/write/memory/d5/n100000 | call | 4.04ms | 4.15ms | 0.947 | [0.853, 1.052] | ~ (noise) |
| micro/cache/disk/write/persistent/d5/n100000 | call | 60.58ms | 60.61ms | 0.995 | [0.980, 1.010] | ~ (noise) |
| micro/cache/disk/write/temporary0/d5/n100000 | call | 351.14ms | 352.42ms | 1.000 | [0.992, 1.007] | ~ (noise) |
| micro/cache/disk/lookup/memory/d5/n100000 | call | 1.22ms | 1.23ms | 0.988 | [0.960, 1.018] | ~ (noise) |
| micro/cache/disk/lookup/temporary0/d5/n100000 | call | 10.12ms | 10.20ms | 0.959 | [0.899, 1.023] | ~ (noise) |
| micro/cache/disk/load/persistent/d5/n100000 | call | 15.17ms | 15.17ms | 1.004 | [0.985, 1.023] | ~ (noise) |
| micro/cache/disk/append/persistent/d5/n100000/e1000000 | call | 142.10ms | 138.98ms | 1.031 | [1.015, 1.048] | faster |
| micro/cache/disk/write/memory/d20/n100000 | call | 8.79ms | 8.77ms | 1.001 | [0.956, 1.048] | ~ (noise) |
| micro/cache/disk/write/persistent/d20/n100000 | call | 94.55ms | 95.47ms | 0.994 | [0.990, 0.998] | SLOWER |
| micro/cache/disk/write/temporary0/d20/n100000 | call | 473.60ms | 474.19ms | 0.997 | [0.988, 1.005] | ~ (noise) |
| micro/cache/disk/lookup/memory/d20/n100000 | call | 2.10ms | 2.28ms | 0.952 | [0.835, 1.086] | ~ (noise) |
| micro/cache/disk/lookup/temporary0/d20/n100000 | call | 10.87ms | 10.88ms | 0.996 | [0.976, 1.016] | ~ (noise) |
| micro/cache/disk/load/persistent/d20/n100000 | call | 25.79ms | 25.46ms | 1.017 | [1.001, 1.033] | faster |
| micro/cache/disk/append/persistent/d20/n100000/e1000000 | call | 307.93ms | 302.89ms | 1.016 | [1.011, 1.021] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_cold_production/c0 | call | 25.88ms | 25.99ms | 0.995 | [0.909, 1.089] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_warm_production/c0 | call | 9.08ms | 9.08ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_production/c0 | call | 99.49ms | 100.14ms | 0.998 | [0.983, 1.013] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_warm_production/c0 | call | 21.27ms | 21.42ms | 0.991 | [0.977, 1.006] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_cold_production/c1000 | call | 28.41ms | 28.21ms | 1.019 | [0.970, 1.070] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_warm_production/c1000 | call | 9.05ms | 9.06ms | 0.997 | [0.991, 1.002] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_production/c1000 | call | 102.05ms | 100.86ms | 1.011 | [1.001, 1.021] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_warm_production/c1000 | call | 21.40ms | 21.41ms | 0.981 | [0.928, 1.038] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_cold_production/c0 | call | 79.74ms | 80.55ms | 0.990 | [0.974, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_warm_production/c0 | call | 39.39ms | 38.79ms | 1.009 | [0.964, 1.055] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_production/c0 | call | 384.20ms | 384.42ms | 0.997 | [0.987, 1.007] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_warm_production/c0 | call | 16.98ms | 16.89ms | 0.997 | [0.951, 1.046] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_cold_production/c1000 | call | 82.31ms | 83.56ms | 0.987 | [0.975, 0.998] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_warm_production/c1000 | call | 38.98ms | 38.78ms | 1.001 | [0.979, 1.024] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_production/c1000 | call | 387.10ms | 386.39ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_warm_production/c1000 | call | 16.88ms | 16.93ms | 0.996 | [0.977, 1.016] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_cold_production/c0 | call | 13.67ms | 13.57ms | 1.013 | [0.919, 1.117] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_warm_production/c0 | call | 8.22ms | 8.25ms | 0.996 | [0.987, 1.005] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c0 | call | 49.98ms | 50.14ms | 0.998 | [0.984, 1.013] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_warm_production/c0 | call | 44.13ms | 44.13ms | 1.017 | [0.956, 1.083] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_cold_production/c1000 | call | 14.61ms | 14.63ms | 0.997 | [0.989, 1.005] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_warm_production/c1000 | call | 8.28ms | 8.29ms | 0.997 | [0.975, 1.018] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c1000 | call | 51.28ms | 51.44ms | 0.999 | [0.979, 1.019] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_warm_production/c1000 | call | 44.59ms | 44.13ms | 1.013 | [0.990, 1.036] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/bypass_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/bypass_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/bypass_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/bypass_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/bypass_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/bypass_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_sharded_spin/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_sharded_spin/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/bypass_sharded_spin/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_boost_cfs/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_boost_cfs/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_boost_cfs/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_oldproto/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_oldproto/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_oldproto/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_perthread/c0 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_perthread/c100 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_perthread/c1000 | - | present | (not in B) | - | - | removed case |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_perthread/c1000 | - | present | (not in B) | - | - | removed case |

Sum of medians (call, common cases): A = 4.325s, B = 4.323s, saved = 2.39ms (1.001x)
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.1%, 0.1%, 0.1%, 0.1%
Round durations (s): 118, 118, 117, 119, 117 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 589s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
