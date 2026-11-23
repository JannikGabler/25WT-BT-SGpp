A/B `e2e`: A=`WP-61-before` B=`WP-61-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 315.69ms | 333.11ms | 0.992 | [0.889, 1.107] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 279.01ms | 295.46ms | 0.981 | [0.878, 1.096] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 39.31ms | 37.03ms | 1.022 | [0.918, 1.138] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 247.59ms | 248.10ms | 0.999 | [0.987, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 246.26ms | 246.48ms | 0.999 | [0.988, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.50ms | 1.53ms | 0.954 | [0.822, 1.107] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 496.18ms | 496.78ms | 1.016 | [0.949, 1.087] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 494.60ms | 494.97ms | 1.019 | [0.955, 1.087] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.46ms | 2.84ms | 0.564 | [0.251, 1.269] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 903.72ms | 903.62ms | 0.998 | [0.978, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 901.71ms | 901.52ms | 0.998 | [0.979, 1.017] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 1.82ms | 2.05ms | 1.027 | [0.630, 1.674] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 240.88ms | 240.96ms | 1.000 | [0.995, 1.004] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 238.60ms | 238.81ms | 0.999 | [0.991, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.55ms | 1.42ms | 1.081 | [0.894, 1.307] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 490.86ms | 490.72ms | 0.998 | [0.985, 1.010] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 481.37ms | 481.53ms | 0.997 | [0.983, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.71ms | 6.49ms | 1.034 | [0.923, 1.159] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 213.10ms | 213.02ms | 1.001 | [0.988, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.26ms | 201.45ms | 0.999 | [0.994, 1.005] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.51ms | 8.12ms | 1.045 | [0.773, 1.412] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.397s | 2.397s | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.394s | 2.395s | 1.001 | [0.992, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.70ms | 1.45ms | 1.232 | [0.474, 3.204] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 446.36ms | 441.54ms | 1.018 | [0.964, 1.075] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 376.48ms | 370.18ms | 1.033 | [0.968, 1.102] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 69.84ms | 71.03ms | 0.996 | [0.862, 1.151] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 75.71ms | 75.19ms | 1.005 | [0.980, 1.030] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.40ms | 73.89ms | 1.005 | [0.980, 1.030] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.28ms | 1.26ms | 1.000 | [0.954, 1.048] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 321.30ms | 322.88ms | 0.998 | [0.988, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 319.48ms | 321.07ms | 0.999 | [0.988, 1.010] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.70ms | 1.60ms | 0.923 | [0.597, 1.426] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 689.13ms | 690.93ms | 0.998 | [0.993, 1.002] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 687.54ms | 689.24ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.53ms | 1.55ms | 0.826 | [0.516, 1.324] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.305s | 1.308s | 0.999 | [0.993, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.303s | 1.306s | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 3.74ms | 2.01ms | 1.540 | [0.858, 2.763] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 690.09ms | 687.21ms | 0.999 | [0.947, 1.053] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 686.89ms | 684.03ms | 0.999 | [0.946, 1.053] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.68ms | 2.65ms | 1.011 | [0.999, 1.024] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 376.35ms | 375.21ms | 1.002 | [0.985, 1.020] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 368.59ms | 367.57ms | 1.002 | [0.979, 1.026] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.21ms | 7.12ms | 1.003 | [0.975, 1.032] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.90ms | 26.48ms | 1.271 | [0.671, 2.404] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.89ms | 23.91ms | 1.007 | [0.957, 1.059] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.40ms | 2.38ms | 1.927 | [0.321, 11.572] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.05ms | 3.03ms | 1.030 | [0.977, 1.085] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.11ms | 2.12ms | 0.999 | [0.922, 1.081] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 761.34us | 754.35us | 1.111 | [0.944, 1.309] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | 1.536s | 1.536s | 1.001 | [0.993, 1.008] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | operator | 1.534s | 1.534s | 1.000 | [0.993, 1.008] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | generation | 2.18ms | 2.16ms | 1.294 | [0.769, 2.175] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | 1.037s | 1.037s | 1.000 | [0.985, 1.014] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | operator | 1.035s | 1.034s | 1.001 | [0.987, 1.015] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | generation | 2.18ms | 2.27ms | 0.767 | [0.468, 1.258] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | 2.156s | 2.156s | 1.000 | [0.997, 1.004] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | operator | 2.155s | 2.154s | 1.000 | [0.997, 1.004] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.16ms | 1.14ms | 1.010 | [0.988, 1.032] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | 2.477s | 2.476s | 1.000 | [0.996, 1.004] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | operator | 2.475s | 2.475s | 1.000 | [0.996, 1.004] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | generation | 1.15ms | 1.15ms | 1.009 | [0.979, 1.041] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | 1.152s | 1.153s | 1.000 | [0.979, 1.021] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | operator | 1.150s | 1.150s | 1.000 | [0.979, 1.021] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | generation | 1.87ms | 1.76ms | 1.058 | [1.029, 1.087] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | 674.64ms | 672.99ms | 0.990 | [0.967, 1.014] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | operator | 671.07ms | 669.27ms | 0.992 | [0.968, 1.016] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.38ms | 3.54ms | 0.775 | [0.519, 1.156] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | 576.70ms | 577.59ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | operator | 572.45ms | 572.66ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | generation | 2.08ms | 4.07ms | 0.834 | [0.368, 1.891] | ~ (noise) |

Sum of medians (total, common cases, group suite): A = 9.239s, B = 9.253s, saved = -14432420.0ns (0.998x)
Sum of medians (total, common cases, group expensive/): A = 9.034s, B = 9.031s, saved = 3.40ms (1.000x)
Sum of medians (total, common cases, group expensive/ cached): A = 576.70ms, B = 577.59ms, saved = -894872.0ns (0.998x)
Result sanity (rel. error vs exact): worst growth in `mivec/sg/cc-exp/b1/d5/S16/n1000`: A 1.27e-13 -> B 1.34e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 110.5 MiB | 113.0 MiB | 1.022 | 18.3% / 24.9% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 13.0 MiB | 13.0 MiB | 0.998 | 1.5% / 2.8% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.8 MiB | 12.9 MiB | 1.009 | 2.8% / 1.2% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 13.1 MiB | 13.2 MiB | 1.007 | 2.3% / 1.6% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.5 MiB | 13.4 MiB | 0.988 | 1.9% / 2.5% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.5 MiB | 18.1 MiB | 0.983 | 2.3% / 1.0% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.5 MiB | 20.0 MiB | 0.975 | 0.7% / 0.9% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.9 MiB | 12.9 MiB | 0.996 | 1.6% / 3.1% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 189.4 MiB | 196.4 MiB | 1.037 | 5.2% / 7.8% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 15.2 MiB | 15.1 MiB | 0.996 | 4.5% / 3.9% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 13.1 MiB | 12.8 MiB | 0.978 | 2.1% / 1.4% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 13.1 MiB | 1.014 | 2.1% / 1.9% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 13.2 MiB | 13.0 MiB | 0.986 | 3.0% / 2.1% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.5 MiB | 13.5 MiB | 1.003 | 1.3% / 1.1% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 15.1 MiB | 15.0 MiB | 0.995 | 2.5% / 2.8% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.3 MiB | 13.4 MiB | 1.010 | 2.4% / 1.3% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.8 MiB | 12.8 MiB | 1.002 | 2.4% / 2.2% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | 13.0 MiB | 13.0 MiB | 1.003 | 1.1% / 2.8% | ~ |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | 13.1 MiB | 13.1 MiB | 1.001 | 1.5% / 1.8% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 12.5 MiB | 12.7 MiB | 1.013 | 2.7% / 1.4% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 12.5 MiB | 12.7 MiB | 1.010 | 3.0% / 1.7% | ~ |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | 13.9 MiB | 13.7 MiB | 0.991 | 0.8% / 0.7% | ~ |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 13.2 MiB | 13.3 MiB | 1.006 | 1.3% / 2.5% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 20.9 MiB | 21.0 MiB | 1.004 | 1.0% / 8.6% | ~ |

Peak memory (common cases): geomean B/A = 1.001; largest B/A: `quad/sg/cc-uq/b0/d2/L20` 189.4 MiB -> 196.4 MiB (1.037x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.7 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.1%, 0.2%, 0.2%
Round durations (s): 80, 80, 80, 80, 80 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 401s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
