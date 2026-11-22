A/B `e2e`: A=`WP-83-before` B=`WP-83-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 315.60ms | 334.69ms | 0.947 | [0.849, 1.057] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 278.98ms | 295.97ms | 0.943 | [0.844, 1.054] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 38.39ms | 37.23ms | 1.017 | [0.913, 1.132] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 247.66ms | 247.61ms | 0.996 | [0.981, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 245.95ms | 246.17ms | 0.996 | [0.980, 1.011] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.50ms | 1.46ms | 1.040 | [0.911, 1.187] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 496.66ms | 496.84ms | 0.996 | [0.985, 1.007] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 495.02ms | 495.38ms | 0.998 | [0.987, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.35ms | 1.60ms | 0.766 | [0.270, 2.172] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 904.08ms | 903.00ms | 0.993 | [0.971, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 901.27ms | 901.00ms | 0.992 | [0.969, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.32ms | 1.93ms | 1.395 | [0.892, 2.180] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 241.93ms | 240.84ms | 1.013 | [0.994, 1.032] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 239.68ms | 238.58ms | 1.013 | [0.994, 1.032] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.55ms | 1.54ms | 1.005 | [0.831, 1.215] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 491.29ms | 490.72ms | 1.001 | [0.988, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 482.03ms | 481.42ms | 1.001 | [0.987, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.58ms | 6.62ms | 0.995 | [0.889, 1.115] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 212.97ms | 213.34ms | 0.999 | [0.986, 1.012] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.21ms | 201.62ms | 0.999 | [0.994, 1.005] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.44ms | 8.41ms | 1.002 | [0.741, 1.353] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.399s | 2.398s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.396s | 2.395s | 1.001 | [0.992, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.43ms | 1.54ms | 0.772 | [0.329, 1.812] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 433.90ms | 446.84ms | 0.988 | [0.923, 1.057] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 358.31ms | 366.54ms | 1.010 | [0.928, 1.099] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 70.96ms | 80.14ms | 0.953 | [0.824, 1.101] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 75.43ms | 75.28ms | 1.006 | [0.991, 1.021] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.18ms | 73.93ms | 1.006 | [0.990, 1.022] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.26ms | 1.27ms | 0.981 | [0.937, 1.028] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 321.91ms | 321.66ms | 0.995 | [0.978, 1.013] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 319.09ms | 319.95ms | 0.993 | [0.977, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 1.68ms | 1.61ms | 1.355 | [0.798, 2.300] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 686.19ms | 688.52ms | 0.997 | [0.986, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 684.74ms | 687.20ms | 0.997 | [0.986, 1.008] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.32ms | 1.42ms | 0.936 | [0.748, 1.172] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.306s | 1.307s | 0.998 | [0.992, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.304s | 1.305s | 0.998 | [0.993, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 1.99ms | 2.25ms | 0.704 | [0.508, 0.976] | SLOWER |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 691.85ms | 688.42ms | 1.006 | [0.954, 1.061] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 688.64ms | 685.24ms | 1.006 | [0.953, 1.061] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.67ms | 2.67ms | 1.001 | [0.989, 1.012] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 373.22ms | 383.01ms | 0.987 | [0.959, 1.015] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 365.52ms | 374.95ms | 0.987 | [0.958, 1.016] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.14ms | 7.15ms | 0.990 | [0.968, 1.014] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.87ms | 26.89ms | 0.991 | [0.962, 1.022] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.30ms | 24.32ms | 0.991 | [0.942, 1.042] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.38ms | 2.39ms | 0.998 | [0.991, 1.004] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.03ms | 3.03ms | 0.936 | [0.774, 1.131] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.12ms | 2.12ms | 1.000 | [0.924, 1.083] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 758.48us | 757.85us | 0.828 | [0.479, 1.432] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | 1.536s | 1.537s | 0.999 | [0.992, 1.007] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | operator | 1.534s | 1.534s | 1.000 | [0.992, 1.007] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | generation | 2.09ms | 2.15ms | 0.781 | [0.503, 1.212] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | total | 570.68ms | 570.87ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | operator | 568.70ms | 568.63ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | generation | 2.12ms | 2.11ms | 0.892 | [0.536, 1.485] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | 1.036s | 1.037s | 1.000 | [0.985, 1.014] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | operator | 1.034s | 1.034s | 1.000 | [0.986, 1.014] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | generation | 2.08ms | 2.10ms | 0.994 | [0.835, 1.184] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | total | 521.21ms | 520.90ms | 1.000 | [0.976, 1.025] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | operator | 518.60ms | 518.74ms | 1.000 | [0.980, 1.021] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | generation | 1.99ms | 1.96ms | 1.026 | [0.460, 2.289] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | 2.156s | 2.156s | 1.000 | [0.996, 1.004] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | operator | 2.155s | 2.154s | 1.000 | [0.996, 1.004] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.16ms | 1.15ms | 0.997 | [0.974, 1.021] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | total | 881.11ms | 881.30ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | operator | 879.78ms | 879.97ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.12ms | 1.12ms | 1.001 | [0.966, 1.036] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | 2.476s | 2.476s | 1.000 | [0.996, 1.003] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | operator | 2.474s | 2.475s | 1.000 | [0.996, 1.003] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | generation | 1.15ms | 1.15ms | 1.001 | [0.979, 1.023] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | 1.153s | 1.153s | 1.000 | [0.980, 1.021] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | operator | 1.151s | 1.150s | 1.000 | [0.980, 1.021] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | generation | 1.85ms | 1.85ms | 0.995 | [0.968, 1.022] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | total | 349.66ms | 349.93ms | 0.999 | [0.990, 1.007] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | operator | 348.38ms | 348.47ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | generation | 344.60us | 314.84us | 1.067 | [0.969, 1.175] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | 664.03ms | 668.54ms | 0.993 | [0.969, 1.017] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | operator | 660.50ms | 664.98ms | 0.993 | [0.969, 1.017] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.35ms | 3.38ms | 0.995 | [0.975, 1.016] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | total | 332.60ms | 333.19ms | 1.001 | [0.996, 1.006] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | operator | 329.00ms | 329.60ms | 1.001 | [0.995, 1.007] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.25ms | 3.24ms | 1.010 | [0.991, 1.029] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | 576.49ms | 577.37ms | 0.978 | [0.924, 1.036] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | operator | 572.45ms | 572.27ms | 0.981 | [0.928, 1.036] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | generation | 2.13ms | 3.62ms | 0.677 | [0.266, 1.722] | ~ (noise) |

Sum of medians (total, common cases, group suite): A = 9.227s, B = 9.266s, saved = -38430853.0ns (0.996x)
Sum of medians (total, common cases, group expensive/): A = 11.676s, B = 11.683s, saved = -6561210.0ns (0.999x)
Sum of medians (total, common cases, group expensive/ cached): A = 576.49ms, B = 577.37ms, saved = -884924.0ns (0.998x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d15/L6`: A 3.14e-09 -> B 8.27e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 113.0 MiB | 109.3 MiB | 0.967 | 6.9% / 19.4% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.7 MiB | 12.7 MiB | 1.003 | 4.0% / 3.2% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.9 MiB | 12.7 MiB | 0.990 | 2.8% / 2.9% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.9 MiB | 1.012 | 2.3% / 1.9% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.3 MiB | 13.3 MiB | 1.001 | 2.4% / 1.9% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.4 MiB | 18.6 MiB | 1.008 | 1.3% / 0.7% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.3 MiB | 20.4 MiB | 1.002 | 1.1% / 1.1% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.9 MiB | 12.9 MiB | 1.000 | 2.2% / 1.5% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 195.4 MiB | 190.6 MiB | 0.975 | 5.4% / 4.1% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 14.9 MiB | 15.1 MiB | 1.011 | 3.9% / 5.6% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 12.9 MiB | 12.9 MiB | 1.002 | 2.2% / 3.2% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 13.0 MiB | 1.009 | 2.0% / 1.6% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 13.0 MiB | 1.003 | 2.3% / 2.9% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.4 MiB | 13.5 MiB | 1.005 | 2.3% / 0.4% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 14.9 MiB | 15.0 MiB | 1.012 | 1.2% / 1.2% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.2 MiB | 13.3 MiB | 1.007 | 2.2% / 0.9% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.6 MiB | 12.7 MiB | 1.011 | 1.7% / 1.3% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | 12.9 MiB | 13.0 MiB | 1.010 | 2.5% / 1.3% | ~ |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | 13.1 MiB | 13.2 MiB | 1.008 | 1.6% / 2.7% | ~ |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | 13.0 MiB | 13.1 MiB | 1.003 | 1.6% / 1.5% | ~ |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | 13.2 MiB | 13.3 MiB | 1.004 | 1.1% / 0.4% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 12.5 MiB | 12.6 MiB | 1.010 | 1.8% / 2.3% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | 12.7 MiB | 12.8 MiB | 1.004 | 1.6% / 1.3% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 12.4 MiB | 12.6 MiB | 1.013 | 1.4% / 1.9% | ~ |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | 13.8 MiB | 13.9 MiB | 1.008 | 0.8% / 1.5% | ~ |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | 16.0 MiB | 15.9 MiB | 0.997 | 0.7% / 2.4% | ~ |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 13.2 MiB | 13.2 MiB | 1.002 | 2.5% / 1.7% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | 13.5 MiB | 13.5 MiB | 0.999 | 2.0% / 0.9% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 20.7 MiB | 20.9 MiB | 1.009 | 0.8% / 5.7% | ~ |

Peak memory (common cases): geomean B/A = 1.003; largest B/A: `expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp` 12.4 MiB -> 12.6 MiB (1.013x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.0 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.1%
Round durations (s): 92, 92, 92, 92, 92 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 459s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
