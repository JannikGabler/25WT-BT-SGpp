A/B `e2e`: A=`rp-3` B=`rp-4`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 409.54ms | 441.00ms | 0.923 | [0.827, 1.029] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 330.02ms | 363.72ms | 0.900 | [0.806, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 30.87ms | 32.46ms | 0.972 | [0.873, 1.083] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 254.51ms | 264.37ms | 0.986 | [0.926, 1.051] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 250.22ms | 261.86ms | 0.984 | [0.922, 1.051] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.04ms | 2.00ms | 1.142 | [0.744, 1.755] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 502.75ms | 514.55ms | 0.984 | [0.962, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 500.25ms | 511.94ms | 0.984 | [0.962, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.78ms | 1.82ms | 0.944 | [0.892, 0.999] | SLOWER |
| quad/sg/equi-exp/b0/d10/L4 | total | 935.37ms | 909.70ms | 1.029 | [1.023, 1.035] | faster |
| quad/sg/equi-exp/b0/d10/L4 | operator | 931.43ms | 907.21ms | 1.029 | [1.022, 1.036] | faster |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.10ms | 2.11ms | 0.978 | [0.506, 1.891] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 303.35ms | 297.13ms | 1.014 | [0.991, 1.038] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 294.76ms | 289.23ms | 1.014 | [0.992, 1.037] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 5.26ms | 5.11ms | 1.009 | [0.827, 1.230] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 523.43ms | 518.35ms | 1.015 | [0.989, 1.042] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 498.91ms | 495.35ms | 1.015 | [0.989, 1.042] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.80ms | 15.29ms | 1.036 | [0.924, 1.160] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 239.28ms | 230.59ms | 1.029 | [0.995, 1.064] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 208.38ms | 203.37ms | 1.034 | [1.015, 1.053] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 20.04ms | 18.50ms | 0.964 | [0.713, 1.302] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.412s | 2.404s | 0.999 | [0.985, 1.012] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.410s | 2.401s | 0.999 | [0.985, 1.012] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.86ms | 1.88ms | 0.971 | [0.874, 1.079] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.220s | 1.286s | 0.954 | [0.912, 0.998] | SLOWER |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.130s | 1.198s | 0.954 | [0.904, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 81.82ms | 79.36ms | 0.996 | [0.834, 1.189] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 87.26ms | 86.99ms | 1.037 | [0.950, 1.133] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 84.46ms | 83.93ms | 1.038 | [0.947, 1.138] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.05ms | 2.05ms | 0.973 | [0.862, 1.099] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 325.91ms | 323.16ms | 1.008 | [1.004, 1.011] | faster |
| quad/sg/cc-uq/b0/d5/L10 | operator | 323.11ms | 320.46ms | 1.008 | [1.004, 1.012] | faster |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.30ms | 2.30ms | 0.993 | [0.895, 1.102] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 701.61ms | 693.57ms | 0.999 | [0.972, 1.027] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 698.99ms | 691.05ms | 0.999 | [0.972, 1.027] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.00ms | 1.95ms | 0.998 | [0.926, 1.076] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.311s | 1.318s | 0.985 | [0.951, 1.020] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.308s | 1.314s | 0.985 | [0.951, 1.020] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.48ms | 2.46ms | 0.980 | [0.624, 1.541] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 93.16ms | 96.95ms | 0.962 | [0.950, 0.973] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 91.81ms | 95.62ms | 0.961 | [0.949, 0.973] | SLOWER |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.05ms | 977.07us | 1.037 | [0.848, 1.269] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 202.04ms | 210.24ms | 0.961 | [0.956, 0.966] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 199.55ms | 207.96ms | 0.960 | [0.956, 0.963] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.24ms | 1.03ms | 1.152 | [0.893, 1.487] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.135s | 1.174s | 0.966 | [0.961, 0.972] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.127s | 1.166s | 0.966 | [0.961, 0.972] | SLOWER |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.05ms | 3.01ms | 1.056 | [0.929, 1.201] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.401s | 5.359s | 1.001 | [0.981, 1.021] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.393s | 5.350s | 1.001 | [0.981, 1.021] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.41ms | 3.47ms | 0.944 | [0.711, 1.254] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.219s | 1.222s | 0.998 | [0.991, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.148s | 1.152s | 0.997 | [0.991, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 42.04ms | 42.39ms | 1.000 | [0.857, 1.167] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.226s | 1.214s | 1.007 | [1.001, 1.014] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.124s | 1.113s | 1.007 | [0.999, 1.016] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 60.73ms | 60.01ms | 1.006 | [0.952, 1.064] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 247.92ms | 249.59ms | 0.995 | [0.991, 1.000] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 244.25ms | 245.45ms | 0.997 | [0.993, 1.000] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.33ms | 2.40ms | 0.945 | [0.795, 1.123] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.439s | 1.442s | 0.996 | [0.991, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.430s | 1.432s | 0.996 | [0.991, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.42ms | 4.93ms | 0.998 | [0.850, 1.170] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.780s | 6.849s | 0.990 | [0.985, 0.996] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.771s | 6.840s | 0.990 | [0.986, 0.995] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.56ms | 4.58ms | 1.020 | [0.667, 1.559] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 735.68ms | 778.62ms | 0.931 | [0.867, 0.999] | SLOWER |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 731.94ms | 774.97ms | 0.931 | [0.867, 0.999] | SLOWER |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.05ms | 2.97ms | 1.021 | [0.972, 1.073] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 483.14ms | 477.94ms | 0.993 | [0.918, 1.074] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 474.64ms | 469.47ms | 0.993 | [0.916, 1.076] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.50ms | 7.48ms | 1.001 | [0.989, 1.014] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 27.48ms | 26.79ms | 1.014 | [0.971, 1.059] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.77ms | 24.10ms | 1.016 | [0.965, 1.068] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.44ms | 2.42ms | 1.003 | [0.994, 1.012] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.17ms | 3.28ms | 0.964 | [0.840, 1.107] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.13ms | 2.12ms | 0.979 | [0.855, 1.122] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 832.22us | 905.07us | 0.932 | [0.529, 1.641] | ~ (noise) |

Sum of medians (total, common cases): A = 28.219s, B = 28.391s, saved = -172035429.0ns (0.994x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 3.04e-12 -> B 3.63e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.5 MiB | 620.1 MiB | 1.001 | 0.0% / 0.1% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.4 MiB | 15.8 MiB | 1.030 | 3.7% / 2.2% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.5 MiB | 15.0 MiB | 1.038 | 2.3% / 4.1% | MEM+ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 13.0 MiB | 1.015 | 2.6% / 3.0% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.6 MiB | 26.2 MiB | 1.024 | 1.7% / 6.8% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 52.8 MiB | 53.9 MiB | 1.020 | 2.1% / 5.4% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 59.3 MiB | 58.5 MiB | 0.986 | 12.4% / 7.1% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.3 MiB | 14.7 MiB | 1.028 | 4.1% / 1.9% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1083.4 MiB | 1075.2 MiB | 0.992 | 2.6% / 3.0% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 41.2 MiB | 40.8 MiB | 0.988 | 4.3% / 7.2% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.6 MiB | 17.7 MiB | 1.004 | 5.2% / 1.9% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 15.0 MiB | 15.2 MiB | 1.012 | 4.0% / 3.4% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 13.1 MiB | 1.016 | 2.1% / 3.0% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 62.5 MiB | 63.2 MiB | 1.010 | 2.1% / 1.7% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 25.8 MiB | 25.8 MiB | 1.000 | 3.0% / 4.0% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 42.7 MiB | 41.8 MiB | 0.979 | 7.1% / 2.1% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 41.5 MiB | 41.9 MiB | 1.009 | 1.5% / 2.9% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 150.5 MiB | 149.9 MiB | 0.996 | 0.9% / 0.8% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.1 MiB | 217.6 MiB | 1.002 | 0.2% / 0.7% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 25.5 MiB | 26.0 MiB | 1.019 | 2.1% / 2.1% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 41.7 MiB | 42.2 MiB | 1.011 | 3.7% / 4.2% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 43.1 MiB | 43.3 MiB | 1.005 | 3.4% / 5.9% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.3 MiB | 16.4 MiB | 1.009 | 2.0% / 1.1% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.1 MiB | 17.8 MiB | 1.040 | 5.8% / 3.0% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.7 MiB | 13.1 MiB | 1.033 | 0.9% / 1.1% | MEM+ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.1 MiB | 12.5 MiB | 1.033 | 2.2% / 2.6% | MEM+ |

Peak memory (common cases): geomean B/A = 1.012; largest B/A: `mivec/sg/cc-exp/b1/d10/S12/n1000` 17.1 MiB -> 17.8 MiB (1.040x); 3 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.1 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 119, 119, 119, 119, 119 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 596s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
