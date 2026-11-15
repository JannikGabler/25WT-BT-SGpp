A/B `e2e`: A=`WP-56-before` B=`WP-56-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 334.15ms | 352.14ms | 0.966 | [0.866, 1.078] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 297.28ms | 313.65ms | 0.956 | [0.856, 1.068] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 38.91ms | 38.46ms | 1.008 | [0.905, 1.122] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 261.91ms | 266.15ms | 0.987 | [0.976, 0.998] | SLOWER |
| quad/sg/equi-exp/b0/d5/L10 | operator | 260.44ms | 263.08ms | 0.990 | [0.979, 1.001] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.65ms | 3.07ms | 0.739 | [0.276, 1.982] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 513.64ms | 512.80ms | 1.005 | [0.994, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 512.19ms | 511.16ms | 1.005 | [0.994, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.44ms | 1.35ms | 1.031 | [0.890, 1.194] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 913.87ms | 914.29ms | 0.988 | [0.962, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 911.05ms | 912.26ms | 0.987 | [0.960, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.17ms | 1.80ms | 1.195 | [0.749, 1.905] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 300.12ms | 247.95ms | 1.215 | [1.170, 1.262] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 297.80ms | 245.72ms | 1.217 | [1.171, 1.264] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.56ms | 1.55ms | 1.012 | [0.837, 1.224] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 506.28ms | 500.81ms | 1.021 | [0.985, 1.058] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 496.90ms | 491.39ms | 1.023 | [0.989, 1.059] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.62ms | 6.70ms | 0.887 | [0.659, 1.196] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 213.20ms | 213.15ms | 1.000 | [0.988, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.49ms | 201.27ms | 1.000 | [0.995, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.39ms | 8.47ms | 0.992 | [0.734, 1.340] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.422s | 2.407s | 1.008 | [1.000, 1.016] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.420s | 2.406s | 1.008 | [1.000, 1.016] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.38ms | 1.44ms | 0.960 | [0.855, 1.079] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 791.50ms | 438.15ms | 1.843 | [1.690, 2.009] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 715.66ms | 355.83ms | 2.042 | [1.883, 2.215] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 78.05ms | 81.41ms | 0.970 | [0.840, 1.121] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 76.99ms | 76.34ms | 1.004 | [0.990, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 75.62ms | 74.99ms | 1.005 | [0.992, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.29ms | 1.29ms | 0.927 | [0.767, 1.120] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 319.84ms | 320.82ms | 1.000 | [0.946, 1.057] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 318.13ms | 319.05ms | 0.997 | [0.947, 1.050] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.65ms | 1.66ms | 1.410 | [0.765, 2.600] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 689.10ms | 693.09ms | 1.009 | [0.978, 1.041] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 686.58ms | 691.33ms | 1.008 | [0.978, 1.039] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.87ms | 1.44ms | 1.521 | [0.563, 4.107] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.310s | 1.323s | 0.995 | [0.980, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.308s | 1.321s | 0.994 | [0.981, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.32ms | 2.13ms | 1.193 | [0.545, 2.609] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | total | 96.00ms | 95.39ms | 1.007 | [1.001, 1.012] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 95.16ms | 94.96ms | 1.006 | [1.001, 1.010] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 565.50us | 400.33us | 1.264 | [0.432, 3.696] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 211.83ms | 217.02ms | 0.974 | [0.903, 1.050] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 211.24ms | 216.29ms | 0.974 | [0.904, 1.051] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 477.85us | 485.19us | 0.779 | [0.439, 1.381] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.191s | 1.190s | 0.996 | [0.982, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.190s | 1.189s | 0.996 | [0.982, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 179.49us | 178.09us | 1.157 | [0.772, 1.735] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.428s | 5.437s | 0.997 | [0.993, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.428s | 5.436s | 0.997 | [0.993, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 248.07us | 224.47us | 1.346 | [0.744, 2.433] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.173s | 1.169s | 1.002 | [0.995, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.162s | 1.160s | 1.002 | [0.995, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 4.05ms | 4.08ms | 1.020 | [0.697, 1.494] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.152s | 1.141s | 1.001 | [0.983, 1.020] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.133s | 1.122s | 1.001 | [0.982, 1.021] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 10.79ms | 10.77ms | 1.037 | [0.907, 1.186] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 247.90ms | 245.00ms | 1.012 | [1.008, 1.016] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 247.16ms | 244.29ms | 1.012 | [1.008, 1.016] | faster |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 637.79us | 645.75us | 1.009 | [0.938, 1.084] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.449s | 1.447s | 1.003 | [0.998, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.449s | 1.447s | 1.003 | [0.998, 1.009] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 225.03us | 222.72us | 1.042 | [0.910, 1.193] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.895s | 6.881s | 1.000 | [0.994, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.894s | 6.880s | 1.000 | [0.995, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 493.92us | 632.27us | 0.895 | [0.452, 1.771] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 775.71ms | 747.78ms | 1.041 | [0.987, 1.098] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 772.50ms | 744.60ms | 1.041 | [0.987, 1.098] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.66ms | 2.66ms | 0.997 | [0.986, 1.009] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 490.17ms | 376.13ms | 1.318 | [1.220, 1.424] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 482.47ms | 368.48ms | 1.325 | [1.224, 1.433] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.16ms | 7.13ms | 1.002 | [0.990, 1.014] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.32ms | 26.20ms | 0.996 | [0.947, 1.048] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.76ms | 23.66ms | 0.996 | [0.941, 1.053] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.38ms | 2.37ms | 1.002 | [0.989, 1.015] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 2.98ms | 3.05ms | 0.999 | [0.916, 1.089] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.03ms | 2.15ms | 0.949 | [0.877, 1.028] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 803.24us | 760.08us | 1.123 | [0.861, 1.464] | ~ (noise) |

Sum of medians (total, common cases): A = 27.793s, B = 27.241s, saved = 551.34ms (1.020x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d15/L6`: A 1.78e-09 -> B 2.62e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 111.0 MiB | 114.0 MiB | 1.027 | 6.1% / 9.5% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.6 MiB | 12.7 MiB | 1.013 | 4.2% / 3.0% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.5 MiB | 12.8 MiB | 1.020 | 5.6% / 3.7% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.9 MiB | 1.010 | 1.0% / 2.2% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.3 MiB | 13.3 MiB | 0.996 | 1.0% / 3.8% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.4 MiB | 18.4 MiB | 0.997 | 2.1% / 1.1% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.2 MiB | 20.3 MiB | 1.004 | 1.0% / 0.9% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.7 MiB | 12.7 MiB | 1.001 | 3.3% / 2.1% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 323.4 MiB | 194.6 MiB | 0.602 | 12.3% / 6.3% | MEM- |
| quad/sg/cc-uq/b0/d3/L14 | 17.1 MiB | 15.3 MiB | 0.897 | 6.0% / 7.1% | MEM- |
| quad/sg/cc-uq/b0/d5/L10 | 13.0 MiB | 13.0 MiB | 1.001 | 2.0% / 3.2% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 13.0 MiB | 1.003 | 3.3% / 2.3% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 12.9 MiB | 1.000 | 1.2% / 2.0% | ~ |
| quad/dsg/equi-exp/b0/d3/L16 | 44.0 MiB | 43.6 MiB | 0.992 | 3.8% / 3.5% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 18.0 MiB | 18.1 MiB | 1.006 | 4.8% / 2.6% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 29.2 MiB | 29.7 MiB | 1.015 | 6.1% / 4.8% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 29.4 MiB | 30.1 MiB | 1.025 | 5.9% / 4.6% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 51.7 MiB | 51.6 MiB | 0.997 | 0.3% / 0.5% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 82.8 MiB | 82.8 MiB | 0.999 | 0.1% / 0.2% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 18.3 MiB | 18.4 MiB | 1.006 | 1.8% / 2.8% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 29.2 MiB | 28.9 MiB | 0.991 | 5.7% / 2.5% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 30.0 MiB | 30.2 MiB | 1.005 | 2.2% / 2.5% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.6 MiB | 13.4 MiB | 0.986 | 0.8% / 2.6% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 14.9 MiB | 14.9 MiB | 1.002 | 1.6% / 2.2% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.2 MiB | 13.3 MiB | 1.004 | 1.6% / 1.0% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.5 MiB | 12.6 MiB | 1.007 | 1.3% / 1.6% | ~ |

Peak memory (common cases): geomean B/A = 0.981; largest B/A: `quad/sg/equi-exp/b0/d2/L20` 111.0 MiB -> 114.0 MiB (1.027x); 0 MEM+, 2 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.6%, 0.4%, 0.4%, 0.4%, 0.4%
Round durations (s): 116, 115, 115, 115, 115 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 577s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
