A/B `e2e`: A=`WP-56-lazy-dyn` B=`WP-56-eager-dyn`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 341.13ms | 335.86ms | 0.992 | [0.922, 1.068] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 302.93ms | 297.17ms | 0.997 | [0.925, 1.074] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 38.11ms | 38.28ms | 0.991 | [0.922, 1.065] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 264.53ms | 265.12ms | 0.997 | [0.989, 1.005] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 262.74ms | 263.08ms | 0.998 | [0.991, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.55ms | 1.61ms | 0.879 | [0.605, 1.276] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 514.27ms | 513.43ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 512.68ms | 511.50ms | 1.003 | [0.994, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.40ms | 1.50ms | 0.875 | [0.624, 1.226] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 910.00ms | 909.74ms | 0.999 | [0.990, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 907.28ms | 907.68ms | 0.999 | [0.990, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 1.93ms | 2.17ms | 0.928 | [0.613, 1.406] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 242.17ms | 242.60ms | 0.999 | [0.982, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 239.94ms | 240.09ms | 1.000 | [0.983, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.54ms | 1.57ms | 0.894 | [0.785, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 491.30ms | 490.90ms | 1.002 | [0.994, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 481.88ms | 481.62ms | 1.002 | [0.992, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.62ms | 6.64ms | 1.030 | [0.954, 1.112] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 213.19ms | 213.03ms | 1.004 | [0.996, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.44ms | 201.30ms | 1.005 | [0.995, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.45ms | 8.43ms | 1.000 | [0.816, 1.224] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.401s | 2.396s | 1.002 | [0.997, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.399s | 2.395s | 1.002 | [0.997, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.53ms | 1.52ms | 0.930 | [0.664, 1.303] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 494.73ms | 432.44ms | 1.130 | [1.058, 1.207] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 420.62ms | 360.83ms | 1.153 | [1.079, 1.231] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 70.12ms | 69.87ms | 0.949 | [0.861, 1.047] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 76.15ms | 76.35ms | 0.998 | [0.989, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.81ms | 74.99ms | 0.998 | [0.989, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.30ms | 1.28ms | 1.003 | [0.969, 1.038] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 320.32ms | 320.77ms | 0.998 | [0.991, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 318.59ms | 318.79ms | 0.998 | [0.992, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.65ms | 1.58ms | 0.910 | [0.684, 1.211] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 689.53ms | 694.14ms | 0.982 | [0.966, 0.999] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | operator | 687.93ms | 691.66ms | 0.983 | [0.966, 0.999] | SLOWER |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.43ms | 1.44ms | 0.910 | [0.728, 1.137] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.312s | 1.311s | 1.002 | [0.998, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.310s | 1.309s | 1.001 | [0.998, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.82ms | 1.92ms | 1.374 | [1.014, 1.862] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | total | 95.65ms | 94.61ms | 1.012 | [1.004, 1.020] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 94.59ms | 93.93ms | 1.008 | [0.999, 1.017] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 894.24us | 570.63us | 1.607 | [1.099, 2.349] | faster |
| quad/dsg/equi-exp/b0/d5/L12 | total | 211.74ms | 211.79ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 211.14ms | 211.21ms | 1.000 | [0.998, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 491.47us | 470.62us | 1.083 | [0.912, 1.285] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.191s | 1.190s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.190s | 1.190s | 0.999 | [0.995, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 176.49us | 179.18us | 1.119 | [0.777, 1.611] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.436s | 5.436s | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.435s | 5.435s | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 246.40us | 251.04us | 0.856 | [0.636, 1.151] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.170s | 1.169s | 1.000 | [0.998, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.161s | 1.160s | 1.000 | [0.998, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 4.01ms | 4.03ms | 0.985 | [0.887, 1.093] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.139s | 1.138s | 1.002 | [0.999, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.120s | 1.119s | 1.002 | [0.999, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 10.87ms | 10.94ms | 0.999 | [0.972, 1.027] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 245.55ms | 245.53ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.78ms | 244.78ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 626.23us | 641.87us | 0.988 | [0.942, 1.038] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.446s | 1.448s | 0.998 | [0.995, 1.000] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.445s | 1.448s | 0.998 | [0.995, 1.000] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 224.54us | 224.96us | 0.988 | [0.902, 1.083] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.890s | 6.888s | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.889s | 6.887s | 1.001 | [0.998, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 295.32us | 635.66us | 0.751 | [0.515, 1.095] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 742.45ms | 741.87ms | 1.002 | [0.967, 1.039] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 739.25ms | 738.63ms | 1.002 | [0.967, 1.039] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.67ms | 2.65ms | 0.999 | [0.988, 1.010] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 377.70ms | 375.52ms | 1.003 | [0.992, 1.014] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 369.80ms | 367.77ms | 1.002 | [0.987, 1.019] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.13ms | 7.14ms | 1.010 | [0.989, 1.031] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.51ms | 26.57ms | 0.999 | [0.969, 1.029] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.97ms | 24.02ms | 0.998 | [0.965, 1.033] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.38ms | 2.37ms | 1.001 | [0.995, 1.008] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.04ms | 3.05ms | 1.004 | [0.963, 1.046] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.12ms | 2.13ms | 0.997 | [0.945, 1.052] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 764.55us | 760.99us | 1.023 | [0.891, 1.174] | ~ (noise) |

Sum of medians (total, common cases): A = 27.243s, B = 27.171s, saved = 71.79ms (1.003x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d15/L6`: A 2.81e-10 -> B 4.03e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 117.7 MiB | 114.8 MiB | 0.976 | 23.0% / 27.4% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.7 MiB | 12.7 MiB | 0.997 | 2.5% / 3.9% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.8 MiB | 12.7 MiB | 0.993 | 3.5% / 3.6% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.8 MiB | 1.003 | 2.7% / 3.1% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.2 MiB | 13.2 MiB | 1.007 | 2.0% / 2.1% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.4 MiB | 18.2 MiB | 0.991 | 2.3% / 1.6% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.2 MiB | 20.2 MiB | 1.002 | 1.3% / 2.2% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.7 MiB | 12.8 MiB | 1.009 | 3.0% / 1.6% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 196.9 MiB | 197.6 MiB | 1.004 | 13.7% / 11.9% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 15.1 MiB | 14.8 MiB | 0.976 | 6.3% / 8.3% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 12.7 MiB | 12.8 MiB | 1.003 | 2.7% / 4.3% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.8 MiB | 12.8 MiB | 1.003 | 2.7% / 1.7% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 12.9 MiB | 1.002 | 2.3% / 3.9% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 43.6 MiB | 43.5 MiB | 0.998 | 2.7% / 3.1% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 18.0 MiB | 18.0 MiB | 1.000 | 4.5% / 5.1% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 28.9 MiB | 28.9 MiB | 1.000 | 4.6% / 7.7% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 29.6 MiB | 29.8 MiB | 1.006 | 6.0% / 3.0% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 51.5 MiB | 51.5 MiB | 0.999 | 0.5% / 0.8% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 82.8 MiB | 82.7 MiB | 0.998 | 0.4% / 0.5% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 18.3 MiB | 18.2 MiB | 0.994 | 2.7% / 2.1% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 29.3 MiB | 29.1 MiB | 0.993 | 8.1% / 5.5% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 29.8 MiB | 30.3 MiB | 1.015 | 6.2% / 4.1% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.3 MiB | 13.2 MiB | 0.994 | 1.6% / 2.3% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 14.8 MiB | 14.8 MiB | 0.998 | 1.6% / 1.5% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.2 MiB | 13.2 MiB | 1.002 | 1.9% / 2.8% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.5 MiB | 12.6 MiB | 1.003 | 1.8% / 2.8% | ~ |

Peak memory (common cases): geomean B/A = 0.999; largest B/A: `quad/dsg/cc-uq/b0/d10/L7` 29.8 MiB -> 30.3 MiB (1.015x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 9.9 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.1%, 0.2%
Round durations (s): 114, 114, 114, 114, 114, 114, 114, 114 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 913s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
