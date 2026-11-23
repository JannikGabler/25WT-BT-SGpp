A/B `e2e`: A=`WP-61-before` B=`WP-61-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 317.52ms | 308.78ms | 1.033 | [0.925, 1.152] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 279.00ms | 269.58ms | 1.040 | [0.931, 1.162] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 37.00ms | 39.00ms | 0.975 | [0.876, 1.085] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 247.87ms | 249.47ms | 0.994 | [0.983, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 246.43ms | 246.97ms | 0.995 | [0.984, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.41ms | 1.56ms | 0.845 | [0.630, 1.132] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 497.90ms | 497.50ms | 0.994 | [0.978, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 496.41ms | 496.01ms | 0.994 | [0.976, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.41ms | 1.37ms | 1.216 | [0.812, 1.821] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 903.22ms | 904.97ms | 0.999 | [0.993, 1.005] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 901.08ms | 902.79ms | 0.998 | [0.992, 1.005] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.07ms | 2.07ms | 1.127 | [0.573, 2.214] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 241.09ms | 241.31ms | 1.003 | [0.991, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 238.85ms | 239.20ms | 1.003 | [0.991, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.55ms | 1.42ms | 1.092 | [0.903, 1.321] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 491.69ms | 490.54ms | 1.004 | [0.992, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 482.33ms | 481.35ms | 1.004 | [0.990, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.67ms | 6.31ms | 1.046 | [0.934, 1.172] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 212.97ms | 213.02ms | 0.998 | [0.985, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.19ms | 201.55ms | 0.996 | [0.991, 1.002] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.50ms | 8.14ms | 1.045 | [0.773, 1.412] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.396s | 2.397s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.394s | 2.395s | 0.999 | [0.991, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.42ms | 1.22ms | 1.367 | [0.811, 2.305] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 433.65ms | 449.26ms | 1.000 | [0.929, 1.076] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 355.33ms | 371.89ms | 0.997 | [0.909, 1.094] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 70.30ms | 71.56ms | 0.998 | [0.864, 1.153] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 75.32ms | 75.12ms | 1.003 | [0.989, 1.017] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.05ms | 73.85ms | 1.002 | [0.990, 1.015] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.27ms | 1.27ms | 1.002 | [0.956, 1.051] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 322.79ms | 322.31ms | 1.002 | [0.996, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 320.48ms | 320.70ms | 0.999 | [0.994, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 3.11ms | 1.59ms | 1.415 | [0.808, 2.480] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 689.14ms | 689.46ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 686.40ms | 687.96ms | 0.999 | [0.994, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.56ms | 1.42ms | 1.233 | [0.749, 2.028] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.308s | 1.306s | 1.001 | [0.994, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.306s | 1.304s | 1.001 | [0.994, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.11ms | 2.24ms | 0.873 | [0.608, 1.252] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 689.99ms | 686.75ms | 1.017 | [0.964, 1.073] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 686.79ms | 683.57ms | 1.017 | [0.964, 1.073] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.67ms | 2.66ms | 1.001 | [0.987, 1.015] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 374.44ms | 379.36ms | 0.988 | [0.972, 1.004] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 366.69ms | 371.62ms | 0.988 | [0.965, 1.011] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.18ms | 7.12ms | 1.005 | [0.993, 1.018] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.64ms | 26.36ms | 0.997 | [0.979, 1.015] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.06ms | 23.73ms | 1.007 | [0.957, 1.059] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.40ms | 2.40ms | 0.923 | [0.735, 1.158] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.02ms | 3.16ms | 0.987 | [0.937, 1.040] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.11ms | 2.11ms | 0.984 | [0.909, 1.066] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 760.99us | 780.05us | 0.992 | [0.877, 1.122] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | 1.537s | 1.535s | 1.001 | [0.994, 1.009] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | operator | 1.534s | 1.533s | 1.001 | [0.994, 1.008] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | generation | 2.18ms | 2.00ms | 1.073 | [0.653, 1.763] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | 1.037s | 1.036s | 1.000 | [0.985, 1.014] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | operator | 1.034s | 1.034s | 1.000 | [0.986, 1.014] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | generation | 2.19ms | 2.20ms | 0.896 | [0.670, 1.197] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | 2.157s | 2.157s | 0.999 | [0.996, 1.003] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | operator | 2.155s | 2.155s | 0.999 | [0.996, 1.003] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.16ms | 1.15ms | 0.992 | [0.972, 1.014] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | 2.477s | 2.475s | 1.001 | [0.997, 1.004] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | operator | 2.476s | 2.474s | 1.001 | [0.997, 1.004] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | generation | 1.16ms | 1.14ms | 1.014 | [1.000, 1.028] | faster |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | 1.154s | 1.153s | 1.002 | [0.982, 1.023] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | operator | 1.151s | 1.151s | 1.002 | [0.982, 1.023] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | generation | 1.86ms | 1.75ms | 1.047 | [1.019, 1.076] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | 669.14ms | 675.25ms | 0.990 | [0.960, 1.020] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | operator | 665.54ms | 671.70ms | 0.990 | [0.961, 1.020] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.41ms | 3.36ms | 0.926 | [0.736, 1.165] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | 576.20ms | 577.86ms | 0.998 | [0.994, 1.002] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | operator | 572.79ms | 572.69ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | generation | 1.95ms | 3.71ms | 0.715 | [0.442, 1.155] | ~ (noise) |

Sum of medians (total, common cases, group suite): A = 9.231s, B = 9.241s, saved = -9342740.0ns (0.999x)
Sum of medians (total, common cases, group expensive/): A = 9.030s, B = 9.032s, saved = -2153758.0ns (1.000x)
Sum of medians (total, common cases, group expensive/ cached): A = 576.20ms, B = 577.86ms, saved = -1655931.0ns (0.997x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d10/L7`: A 1.23e-10 -> B 1.49e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 121.5 MiB | 122.3 MiB | 1.007 | 21.8% / 16.3% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.8 MiB | 12.9 MiB | 1.004 | 1.3% / 2.4% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.8 MiB | 13.0 MiB | 1.013 | 3.3% / 2.5% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 13.1 MiB | 12.9 MiB | 0.992 | 2.6% / 2.1% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.5 MiB | 13.3 MiB | 0.988 | 1.3% / 0.6% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.6 MiB | 18.1 MiB | 0.976 | 1.0% / 2.0% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.3 MiB | 20.0 MiB | 0.988 | 1.3% / 0.6% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 13.0 MiB | 12.9 MiB | 0.992 | 1.2% / 1.5% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 205.7 MiB | 189.9 MiB | 0.923 | 12.7% / 6.0% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 15.2 MiB | 15.3 MiB | 1.001 | 1.9% / 2.7% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 12.9 MiB | 12.9 MiB | 1.001 | 0.8% / 1.4% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 13.0 MiB | 13.0 MiB | 0.996 | 2.9% / 1.8% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.1 MiB | 13.1 MiB | 0.998 | 2.6% / 1.8% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.4 MiB | 13.5 MiB | 1.007 | 1.7% / 1.7% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 14.9 MiB | 15.1 MiB | 1.012 | 1.6% / 1.3% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.4 MiB | 13.3 MiB | 0.996 | 1.8% / 0.5% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.7 MiB | 12.8 MiB | 1.005 | 1.1% / 0.6% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | 13.0 MiB | 13.0 MiB | 1.001 | 2.5% / 3.0% | ~ |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | 13.1 MiB | 13.1 MiB | 1.002 | 1.9% / 0.9% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 12.7 MiB | 12.7 MiB | 1.001 | 1.5% / 2.3% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 12.7 MiB | 12.6 MiB | 0.997 | 1.8% / 2.5% | ~ |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | 13.9 MiB | 13.7 MiB | 0.988 | 1.5% / 1.7% | ~ |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 13.2 MiB | 13.2 MiB | 1.001 | 1.0% / 1.9% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 20.9 MiB | 20.8 MiB | 0.995 | 2.8% / 1.2% | ~ |

Peak memory (common cases): geomean B/A = 0.995; largest B/A: `quad/sg/equi-exp/b0/d7/L7` 12.8 MiB -> 13.0 MiB (1.013x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.1%, 0.1%
Round durations (s): 80, 80, 80, 80, 80 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 401s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
