A/B `e2e`: A=`WP-61-before` B=`WP-61-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 313.54ms | 316.41ms | 0.974 | [0.872, 1.086] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 274.39ms | 277.68ms | 0.968 | [0.866, 1.081] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 37.25ms | 38.78ms | 0.980 | [0.880, 1.091] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 248.40ms | 247.76ms | 1.003 | [0.992, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 246.16ms | 246.18ms | 1.002 | [0.990, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.67ms | 1.51ms | 1.226 | [0.907, 1.658] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 496.52ms | 497.55ms | 0.998 | [0.988, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 494.91ms | 496.11ms | 0.998 | [0.987, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.49ms | 1.34ms | 1.174 | [0.781, 1.766] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 903.80ms | 903.42ms | 1.001 | [0.994, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 901.52ms | 901.06ms | 1.001 | [0.994, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.19ms | 2.11ms | 1.035 | [0.645, 1.663] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 241.37ms | 241.11ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 239.17ms | 239.01ms | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.54ms | 1.44ms | 1.066 | [0.881, 1.289] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 491.29ms | 490.66ms | 1.004 | [0.991, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 481.92ms | 481.51ms | 1.001 | [0.987, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.71ms | 6.31ms | 1.164 | [0.876, 1.547] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 213.07ms | 212.89ms | 1.002 | [0.990, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.31ms | 201.42ms | 1.001 | [0.990, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.48ms | 8.17ms | 1.042 | [0.771, 1.408] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.397s | 2.397s | 0.998 | [0.990, 1.006] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.395s | 2.395s | 0.998 | [0.990, 1.006] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.49ms | 1.43ms | 1.059 | [0.942, 1.189] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 444.05ms | 461.29ms | 0.964 | [0.900, 1.032] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 372.37ms | 380.60ms | 0.959 | [0.899, 1.023] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 80.27ms | 70.79ms | 1.057 | [0.846, 1.320] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 75.25ms | 75.36ms | 1.000 | [0.986, 1.014] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 73.94ms | 74.07ms | 1.000 | [0.987, 1.012] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.29ms | 1.26ms | 1.013 | [0.962, 1.067] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 321.55ms | 322.21ms | 1.000 | [0.992, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 319.56ms | 320.63ms | 0.998 | [0.992, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.61ms | 1.63ms | 1.162 | [0.732, 1.846] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 688.16ms | 688.81ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 686.50ms | 687.31ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.55ms | 1.47ms | 1.063 | [0.954, 1.184] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.310s | 1.306s | 1.001 | [0.995, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.308s | 1.304s | 1.001 | [0.996, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 3.59ms | 2.30ms | 1.108 | [0.755, 1.625] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 689.23ms | 685.22ms | 1.002 | [0.950, 1.057] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 686.02ms | 682.01ms | 1.002 | [0.950, 1.057] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.69ms | 2.66ms | 1.008 | [0.996, 1.020] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 372.71ms | 378.30ms | 0.988 | [0.972, 1.004] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 364.97ms | 370.54ms | 0.988 | [0.965, 1.012] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.18ms | 7.16ms | 1.001 | [0.975, 1.028] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.58ms | 26.85ms | 0.989 | [0.965, 1.014] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.02ms | 24.31ms | 0.988 | [0.939, 1.039] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.39ms | 2.38ms | 1.001 | [0.991, 1.012] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.04ms | 3.05ms | 0.958 | [0.839, 1.094] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.12ms | 2.13ms | 0.940 | [0.795, 1.112] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 767.83us | 765.94us | 1.006 | [0.899, 1.125] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | 1.538s | 1.538s | 1.000 | [0.993, 1.008] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | operator | 1.535s | 1.535s | 1.000 | [0.993, 1.007] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | generation | 3.41ms | 2.04ms | 1.384 | [0.626, 3.058] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | 1.037s | 1.037s | 1.000 | [0.986, 1.015] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | operator | 1.034s | 1.034s | 1.000 | [0.986, 1.014] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | generation | 3.20ms | 2.11ms | 1.155 | [0.552, 2.417] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | 2.155s | 2.157s | 0.999 | [0.995, 1.003] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | operator | 2.154s | 2.156s | 0.999 | [0.995, 1.003] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.14ms | 1.15ms | 0.985 | [0.933, 1.039] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | 2.476s | 2.477s | 1.000 | [0.996, 1.003] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | operator | 2.475s | 2.476s | 1.000 | [0.996, 1.003] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | generation | 1.15ms | 1.14ms | 1.002 | [0.974, 1.031] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | 1.153s | 1.153s | 1.000 | [0.980, 1.021] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | operator | 1.150s | 1.151s | 1.000 | [0.980, 1.021] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | generation | 1.87ms | 1.76ms | 1.090 | [1.011, 1.176] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | 657.87ms | 676.00ms | 0.982 | [0.954, 1.012] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | operator | 654.33ms | 672.47ms | 0.983 | [0.954, 1.012] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.36ms | 3.41ms | 0.973 | [0.921, 1.027] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | 576.73ms | 577.02ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | operator | 572.66ms | 572.41ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | generation | 2.59ms | 2.22ms | 1.043 | [0.569, 1.910] | ~ (noise) |

Sum of medians (total, common cases, group suite): A = 9.236s, B = 9.254s, saved = -17857281.0ns (0.998x)
Sum of medians (total, common cases, group expensive/): A = 9.018s, B = 9.038s, saved = -19782441.0ns (0.998x)
Sum of medians (total, common cases, group expensive/ cached): A = 576.73ms, B = 577.02ms, saved = -291307.0ns (0.999x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 4.15e-12 -> B 4.36e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 119.3 MiB | 119.0 MiB | 0.998 | 17.6% / 11.9% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.8 MiB | 12.8 MiB | 1.000 | 3.8% / 3.7% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.8 MiB | 12.9 MiB | 1.005 | 2.3% / 2.2% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 13.1 MiB | 13.1 MiB | 1.002 | 1.9% / 1.5% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.4 MiB | 13.4 MiB | 0.995 | 2.1% / 1.6% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.6 MiB | 18.2 MiB | 0.977 | 0.7% / 1.1% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.5 MiB | 20.0 MiB | 0.977 | 0.9% / 1.1% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 13.0 MiB | 12.9 MiB | 0.995 | 1.4% / 2.5% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 190.6 MiB | 189.1 MiB | 0.992 | 7.2% / 3.1% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 15.2 MiB | 15.5 MiB | 1.018 | 5.8% / 3.5% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 13.0 MiB | 12.9 MiB | 0.995 | 1.9% / 1.4% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 13.1 MiB | 1.012 | 1.6% / 4.1% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.1 MiB | 13.1 MiB | 1.001 | 3.7% / 1.0% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.4 MiB | 13.5 MiB | 1.003 | 1.3% / 1.8% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 15.1 MiB | 15.1 MiB | 0.997 | 0.8% / 2.4% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.3 MiB | 13.4 MiB | 1.005 | 1.6% / 1.6% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.7 MiB | 12.8 MiB | 1.010 | 1.4% / 2.5% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | 13.1 MiB | 13.1 MiB | 1.005 | 2.1% / 4.1% | ~ |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | 13.2 MiB | 13.3 MiB | 1.009 | 2.6% / 1.7% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 12.6 MiB | 12.7 MiB | 1.006 | 2.2% / 1.8% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 12.6 MiB | 12.6 MiB | 1.004 | 2.3% / 2.8% | ~ |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | 13.9 MiB | 13.7 MiB | 0.990 | 1.7% / 1.1% | ~ |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 13.3 MiB | 13.1 MiB | 0.989 | 0.8% / 2.6% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 21.0 MiB | 21.0 MiB | 1.002 | 1.6% / 1.6% | ~ |

Peak memory (common cases): geomean B/A = 0.999; largest B/A: `quad/sg/cc-uq/b0/d3/L14` 15.2 MiB -> 15.5 MiB (1.018x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.1%, 0.1%, 0.2%, 0.1%
Round durations (s): 80, 80, 80, 80, 80 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 401s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
