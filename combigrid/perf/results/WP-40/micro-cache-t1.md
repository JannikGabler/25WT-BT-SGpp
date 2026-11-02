A/B `micro`: A=`WP-40-before` B=`WP-40-after`, 1 thread(s), 5 rounds (interleaved processes) x 2 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_micro.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c0 | call | 5.73ms | 5.71ms | 1.009 | [0.995, 1.024] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c100 | call | 16.44ms | 16.37ms | 1.005 | [1.001, 1.009] | faster |
| micro/cache/op/quad/sg/cc/b0/d5/L4/none/c1000 | call | 196.38ms | 196.49ms | 0.998 | [0.992, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c0 | call | 5.46ms | 5.46ms | 0.997 | [0.988, 1.006] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c100 | call | 12.24ms | 12.22ms | 1.002 | [0.997, 1.008] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/none/c1000 | call | 154.71ms | 154.75ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c0 | call | 2.86ms | 2.69ms | 1.061 | [1.054, 1.068] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c100 | call | 10.37ms | 10.29ms | 1.008 | [1.004, 1.012] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/none/c1000 | call | 151.52ms | 151.44ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c0 | call | 5.59ms | 5.58ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c100 | call | 16.39ms | 16.41ms | 1.001 | [0.996, 1.005] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/none/c1000 | call | 225.14ms | 225.80ms | 0.998 | [0.994, 1.001] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c0 | call | 3.76ms | 3.73ms | 0.984 | [0.919, 1.054] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c100 | call | 9.10ms | 9.05ms | 1.005 | [1.001, 1.009] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/none/c1000 | call | 99.73ms | 100.40ms | 0.997 | [0.991, 1.002] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c0 | call | 49.34ms | 49.15ms | 1.004 | [0.998, 1.010] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c100 | call | 80.89ms | 80.56ms | 1.004 | [1.000, 1.007] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/none/c1000 | call | 482.22ms | 480.80ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c0 | call | 61.03ms | 60.97ms | 1.006 | [0.994, 1.018] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c100 | call | 85.74ms | 85.31ms | 1.005 | [0.999, 1.010] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/none/c1000 | call | 390.27ms | 389.76ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| micro/cache/lookup/production/d2/n10000 | call | 516.31us | 579.44us | 0.891 | [0.882, 0.900] | SLOWER |
| micro/cache/insert/production/d2/n10000 | call | 1.05ms | 1.04ms | 1.006 | [0.999, 1.012] | ~ (noise) |
| micro/cache/lookup/production/d2/n1000000 | call | 7.59ms | 7.74ms | 0.983 | [0.855, 1.131] | ~ (noise) |
| micro/cache/insert/production/d2/n1000000 | call | 160.97ms | 160.93ms | 0.995 | [0.981, 1.009] | ~ (noise) |
| micro/cache/lookup/production/d10/n10000 | call | 764.53us | 775.10us | 0.989 | [0.974, 1.003] | ~ (noise) |
| micro/cache/insert/production/d10/n10000 | call | 930.66us | 932.66us | 0.996 | [0.983, 1.009] | ~ (noise) |
| micro/cache/lookup/production/d10/n1000000 | call | 15.86ms | 16.94ms | 0.934 | [0.901, 0.968] | SLOWER |
| micro/cache/insert/production/d10/n1000000 | call | 245.30ms | 244.32ms | 1.003 | [0.999, 1.007] | ~ (noise) |
| micro/cache/lookup/production/d20/n10000 | call | 1.16ms | 1.16ms | 1.000 | [0.978, 1.022] | ~ (noise) |
| micro/cache/insert/production/d20/n10000 | call | 1.04ms | 1.04ms | 1.001 | [0.993, 1.009] | ~ (noise) |
| micro/cache/lookup/production/d20/n1000000 | call | 22.24ms | 21.70ms | 1.021 | [0.997, 1.045] | ~ (noise) |
| micro/cache/insert/production/d20/n1000000 | call | 335.54ms | 336.57ms | 0.999 | [0.989, 1.009] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c0 | call | 18.81ms | 18.84ms | 0.997 | [0.985, 1.009] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c0 | call | 14.12ms | 14.12ms | 1.000 | [0.991, 1.009] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/always_production/c1000 | call | 53.39ms | 53.17ms | 1.004 | [0.997, 1.010] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/warm_production/c1000 | call | 14.15ms | 14.25ms | 0.994 | [0.984, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c0 | call | 37.16ms | 36.39ms | 1.023 | [1.012, 1.034] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c0 | call | 20.48ms | 20.53ms | 1.000 | [0.968, 1.032] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/always_production/c1000 | call | 115.46ms | 115.55ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/warm_production/c1000 | call | 20.70ms | 20.53ms | 1.011 | [1.001, 1.021] | faster |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c0 | call | 17.08ms | 17.04ms | 0.999 | [0.982, 1.016] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c0 | call | 10.14ms | 10.60ms | 0.960 | [0.940, 0.981] | SLOWER |
| micro/cache/op/quad/sg/equi/b0/d2/L11/always_production/c1000 | call | 76.29ms | 76.07ms | 1.002 | [0.999, 1.006] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b0/d2/L11/warm_production/c1000 | call | 10.96ms | 10.58ms | 1.031 | [1.019, 1.043] | faster |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c0 | call | 33.98ms | 33.91ms | 0.998 | [0.983, 1.012] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c0 | call | 20.82ms | 20.86ms | 0.994 | [0.979, 1.009] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/always_production/c1000 | call | 122.15ms | 122.04ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d10/L4/warm_production/c1000 | call | 20.92ms | 20.88ms | 1.044 | [0.924, 1.180] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c0 | call | 20.68ms | 20.49ms | 1.012 | [1.008, 1.016] | faster |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c0 | call | 9.33ms | 9.30ms | 1.006 | [0.989, 1.023] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/always_production/c1000 | call | 117.89ms | 118.22ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| micro/cache/op/quad/dsg/cc/b0/d5/L5/warm_production/c1000 | call | 9.30ms | 9.26ms | 1.006 | [1.000, 1.011] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c0 | call | 42.02ms | 42.06ms | 0.999 | [0.991, 1.006] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c0 | call | 39.93ms | 39.58ms | 1.009 | [1.001, 1.017] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/always_production/c1000 | call | 53.51ms | 53.87ms | 0.994 | [0.990, 0.997] | SLOWER |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/warm_production/c1000 | call | 39.90ms | 39.63ms | 0.998 | [0.988, 1.009] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c0 | call | 61.77ms | 61.90ms | 0.998 | [0.993, 1.004] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c0 | call | 37.47ms | 36.92ms | 1.000 | [0.949, 1.053] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/always_production/c1000 | call | 136.72ms | 136.30ms | 1.002 | [0.998, 1.007] | ~ (noise) |
| micro/cache/op/interplin/sg/equi/b1/d8/L3/p8/warm_production/c1000 | call | 37.12ms | 36.79ms | 1.009 | [1.006, 1.013] | faster |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit1GiB | call | 2.513s | 2.538s | 0.998 | [0.946, 1.052] | ~ (noise) |
| micro/cache/memory/production/quad/sg/equi/b0/d5/L8/limit16MiB | call | 1.580s | 1.582s | 0.994 | [0.977, 1.012] | ~ (noise) |
| micro/cache/disk/write/memory/d5/n100000 | call | 16.71ms | 16.32ms | 1.022 | [1.012, 1.033] | faster |
| micro/cache/disk/write/persistent/d5/n100000 | call | 72.74ms | 73.10ms | 0.996 | [0.974, 1.019] | ~ (noise) |
| micro/cache/disk/write/temporary0/d5/n100000 | call | 374.95ms | 375.27ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| micro/cache/disk/lookup/memory/d5/n100000 | call | 8.14ms | 8.21ms | 0.994 | [0.969, 1.020] | ~ (noise) |
| micro/cache/disk/lookup/temporary0/d5/n100000 | call | 57.64ms | 57.97ms | 0.995 | [0.988, 1.003] | ~ (noise) |
| micro/cache/disk/load/persistent/d5/n100000 | call | 14.97ms | 15.14ms | 0.990 | [0.981, 1.000] | SLOWER |
| micro/cache/disk/append/persistent/d5/n100000/e1000000 | call | 153.72ms | 152.31ms | 1.008 | [1.004, 1.012] | faster |
| micro/cache/disk/write/memory/d20/n100000 | call | 31.43ms | 31.48ms | 0.995 | [0.988, 1.001] | ~ (noise) |
| micro/cache/disk/write/persistent/d20/n100000 | call | 119.93ms | 119.33ms | 1.001 | [0.988, 1.015] | ~ (noise) |
| micro/cache/disk/write/temporary0/d20/n100000 | call | 503.88ms | 503.30ms | 1.000 | [0.987, 1.014] | ~ (noise) |
| micro/cache/disk/lookup/memory/d20/n100000 | call | 16.62ms | 16.72ms | 0.996 | [0.980, 1.012] | ~ (noise) |
| micro/cache/disk/lookup/temporary0/d20/n100000 | call | 73.32ms | 73.31ms | 0.999 | [0.980, 1.018] | ~ (noise) |
| micro/cache/disk/load/persistent/d20/n100000 | call | 25.92ms | 25.57ms | 1.014 | [0.983, 1.046] | ~ (noise) |
| micro/cache/disk/append/persistent/d20/n100000/e1000000 | call | 328.34ms | 327.74ms | 1.004 | [0.994, 1.013] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_cold_production/c0 | call | 38.00ms | 37.83ms | 1.004 | [1.000, 1.008] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_warm_production/c0 | call | 21.75ms | 21.79ms | 0.999 | [0.989, 1.010] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_production/c0 | call | 207.51ms | 209.23ms | 0.992 | [0.974, 1.010] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_warm_production/c0 | call | 109.93ms | 109.33ms | 1.015 | [0.979, 1.053] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_cold_production/c1000 | call | 73.32ms | 73.12ms | 1.005 | [0.997, 1.013] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/persistent_warm_production/c1000 | call | 21.78ms | 21.89ms | 0.993 | [0.990, 0.997] | SLOWER |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_production/c1000 | call | 242.84ms | 242.61ms | 0.999 | [0.992, 1.007] | ~ (noise) |
| micro/cache/op/quad/sg/cc/b0/d5/L4/temporary_spill_warm_production/c1000 | call | 108.51ms | 108.39ms | 1.002 | [0.994, 1.009] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_cold_production/c0 | call | 109.47ms | 111.29ms | 0.981 | [0.974, 0.988] | SLOWER |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_warm_production/c0 | call | 62.98ms | 63.01ms | 0.998 | [0.985, 1.011] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_production/c0 | call | 464.76ms | 475.57ms | 0.988 | [0.960, 1.017] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_warm_production/c0 | call | 99.77ms | 100.37ms | 0.997 | [0.977, 1.018] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_cold_production/c1000 | call | 188.34ms | 185.98ms | 1.013 | [1.009, 1.018] | faster |
| micro/cache/op/quad/sg/equi/b1/d20/L3/persistent_warm_production/c1000 | call | 63.43ms | 63.45ms | 1.000 | [0.987, 1.013] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_production/c1000 | call | 549.89ms | 544.39ms | 1.005 | [0.985, 1.026] | ~ (noise) |
| micro/cache/op/quad/sg/equi/b1/d20/L3/temporary_spill_warm_production/c1000 | call | 101.00ms | 99.11ms | 1.005 | [0.971, 1.040] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_cold_production/c0 | call | 47.16ms | 47.08ms | 0.999 | [0.991, 1.007] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_warm_production/c0 | call | 44.46ms | 44.34ms | 0.998 | [0.990, 1.006] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c0 | call | 202.47ms | 201.60ms | 1.001 | [0.987, 1.016] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_warm_production/c0 | call | 207.78ms | 207.71ms | 1.001 | [0.992, 1.010] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_cold_production/c1000 | call | 59.67ms | 59.08ms | 1.011 | [1.007, 1.015] | faster |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/persistent_warm_production/c1000 | call | 44.48ms | 44.17ms | 1.004 | [0.997, 1.011] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c1000 | call | 214.63ms | 215.05ms | 0.992 | [0.979, 1.005] | ~ (noise) |
| micro/cache/op/interp/sg/cc/b0/d5/L3/p8/temporary_spill_warm_production/c1000 | call | 207.98ms | 207.69ms | 0.997 | [0.984, 1.010] | ~ (noise) |
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

Sum of medians (call, common cases): A = 13.453s, B = 13.477s, saved = -23191826.4ns (0.998x)
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.1%, 0.2%, 0.2%
Round durations (s): 199, 198, 198, 198, 198 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 990s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
