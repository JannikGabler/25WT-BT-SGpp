A/B `e2e`: A=`hist-rp-5` B=`rp-6`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | 1.566s | 1.538s | 1.008 | [0.976, 1.041] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | operator | 1.563s | 1.535s | 1.008 | [0.976, 1.041] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | generation | 2.40ms | 2.22ms | 1.029 | [0.601, 1.760] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | total | 572.56ms | 571.03ms | 1.000 | [0.994, 1.005] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | operator | 568.57ms | 568.74ms | 0.998 | [0.992, 1.003] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | generation | 3.57ms | 2.04ms | 1.440 | [0.585, 3.544] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | 1.040s | 1.036s | 1.003 | [0.988, 1.017] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | operator | 1.037s | 1.034s | 1.003 | [0.989, 1.017] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | generation | 2.08ms | 2.10ms | 0.901 | [0.640, 1.268] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | total | 520.54ms | 520.46ms | 0.998 | [0.974, 1.023] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | operator | 518.63ms | 518.40ms | 0.999 | [0.979, 1.020] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | generation | 1.88ms | 1.91ms | 0.878 | [0.394, 1.958] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | 2.060s | 2.156s | 0.955 | [0.952, 0.959] | SLOWER |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | operator | 2.059s | 2.155s | 0.955 | [0.952, 0.959] | SLOWER |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.15ms | 1.15ms | 1.007 | [0.990, 1.023] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | total | 881.38ms | 881.02ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | operator | 880.10ms | 879.64ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.13ms | 1.12ms | 1.002 | [0.968, 1.038] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | total | 1.073s | 1.073s | 0.999 | [0.985, 1.013] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | operator | 1.073s | 1.073s | 0.999 | [0.985, 1.013] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | generation | 69.49us | 71.10us | 0.952 | [0.843, 1.076] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | total | 502.98ms | 503.02ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | operator | 502.91ms | 502.95ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | generation | 46.16us | 41.97us | 1.026 | [0.852, 1.235] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | total | 826.56ms | 833.12ms | 0.986 | [0.959, 1.014] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | operator | 826.50ms | 833.06ms | 0.986 | [0.959, 1.014] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | generation | 41.42us | 42.25us | 0.923 | [0.800, 1.065] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | total | 345.36ms | 345.08ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | operator | 345.30ms | 345.02ms | 1.001 | [0.997, 1.005] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | generation | 35.90us | 32.90us | 1.101 | [1.005, 1.206] | faster |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | 2.400s | 2.477s | 0.969 | [0.966, 0.973] | SLOWER |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | operator | 2.398s | 2.476s | 0.969 | [0.966, 0.973] | SLOWER |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | generation | 1.17ms | 1.16ms | 1.020 | [1.001, 1.041] | faster |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | total | 398.38ms | 397.80ms | 1.008 | [0.995, 1.020] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | operator | 398.29ms | 397.74ms | 1.007 | [0.995, 1.020] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | generation | 50.01us | 37.16us | 1.334 | [1.102, 1.616] | faster |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | total | 1.289s | 1.289s | 1.001 | [0.969, 1.033] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | operator | 1.289s | 1.289s | 1.001 | [0.969, 1.033] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | generation | 47.63us | 47.07us | 1.009 | [0.922, 1.105] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | 1.179s | 1.153s | 1.023 | [1.002, 1.044] | faster |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | operator | 1.177s | 1.150s | 1.023 | [1.002, 1.044] | faster |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | generation | 1.86ms | 1.89ms | 0.994 | [0.967, 1.021] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | total | 349.69ms | 350.10ms | 0.999 | [0.990, 1.007] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | operator | 348.84ms | 348.82ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | generation | 328.25us | 322.94us | 1.012 | [0.953, 1.074] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | 751.33ms | 666.24ms | 1.128 | [1.102, 1.156] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | operator | 747.77ms | 662.68ms | 1.129 | [1.102, 1.157] | faster |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.37ms | 3.34ms | 1.001 | [0.971, 1.031] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | total | 333.25ms | 334.71ms | 0.997 | [0.993, 1.002] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | operator | 329.67ms | 331.13ms | 0.997 | [0.993, 1.001] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.25ms | 3.23ms | 1.012 | [0.986, 1.037] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | total | 257.39ms | 257.62ms | 1.000 | [0.819, 1.220] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | operator | 255.36ms | 255.61ms | 0.999 | [0.823, 1.213] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | generation | 1.73ms | 1.99ms | 1.036 | [0.638, 1.682] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | total | 98.72ms | 98.79ms | 0.998 | [0.981, 1.015] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | operator | 96.64ms | 96.72ms | 0.999 | [0.994, 1.003] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | generation | 2.03ms | 2.01ms | 0.977 | [0.540, 1.769] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | total | 1.070s | 1.076s | 0.993 | [0.879, 1.121] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | operator | 1.068s | 1.073s | 0.993 | [0.879, 1.122] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | generation | 2.08ms | 2.09ms | 0.912 | [0.626, 1.329] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | total | 231.82ms | 208.80ms | 1.116 | [1.098, 1.135] | faster |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | operator | 229.66ms | 205.64ms | 1.117 | [1.105, 1.129] | faster |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | generation | 1.98ms | 1.94ms | 1.055 | [0.353, 3.155] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | 598.13ms | 577.82ms | 1.035 | [1.032, 1.039] | faster |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | operator | 594.46ms | 572.67ms | 1.038 | [1.034, 1.042] | faster |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | generation | 2.13ms | 3.64ms | 0.679 | [0.435, 1.058] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | total | 70.60ms | 70.78ms | 0.987 | [0.953, 1.021] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | operator | 70.50ms | 70.68ms | 0.986 | [0.953, 1.021] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | generation | 44.63us | 42.05us | 1.062 | [1.000, 1.128] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | total | 243.48ms | 243.54ms | 1.000 | [0.912, 1.096] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | operator | 243.37ms | 243.45ms | 1.000 | [0.912, 1.096] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | generation | 36.11us | 35.13us | 1.106 | [0.947, 1.292] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | total | 153.64ms | 153.06ms | 1.006 | [0.977, 1.035] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | operator | 144.93ms | 143.76ms | 1.004 | [0.981, 1.028] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | generation | 56.78us | 54.90us | 1.141 | [0.957, 1.361] | ~ (noise) |

Sum of medians (total, common cases, group expensive/): A = 17.746s, B = 17.766s, saved = -20524886.0ns (0.999x)
Sum of medians (total, common cases, group expensive/ cached): A = 1.066s, B = 1.045s, saved = 20.64ms (1.020x)
Result sanity (rel. error vs exact): worst growth in `expensive/quad/sg/cc-exp/b1/d15/L5/c1us`: A 5.11e-09 -> B 5.59e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | 12.8 MiB | 13.1 MiB | 1.018 | 2.0% / 0.7% | ~ |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | 13.0 MiB | 13.2 MiB | 1.016 | 2.0% / 2.2% | ~ |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | 12.8 MiB | 13.1 MiB | 1.021 | 1.9% / 1.1% | ~ |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | 13.1 MiB | 13.2 MiB | 1.008 | 1.6% / 0.6% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 12.4 MiB | 12.6 MiB | 1.015 | 3.2% / 1.6% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | 12.5 MiB | 12.7 MiB | 1.020 | 3.1% / 1.1% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | 12.1 MiB | 12.3 MiB | 1.012 | 2.2% / 1.1% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | 12.4 MiB | 12.6 MiB | 1.016 | 4.0% / 1.1% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | 12.2 MiB | 12.3 MiB | 1.012 | 1.2% / 2.0% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | 12.1 MiB | 12.3 MiB | 1.023 | 2.1% / 2.6% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 12.4 MiB | 12.6 MiB | 1.014 | 1.5% / 1.8% | ~ |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | 12.2 MiB | 12.2 MiB | 1.006 | 4.5% / 2.3% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | 12.2 MiB | 12.3 MiB | 1.007 | 1.0% / 2.7% | ~ |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | 13.6 MiB | 13.7 MiB | 1.013 | 1.6% / 2.1% | ~ |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | 15.7 MiB | 15.9 MiB | 1.013 | 0.9% / 1.4% | ~ |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 12.9 MiB | 13.3 MiB | 1.027 | 2.0% / 0.6% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | 13.4 MiB | 13.6 MiB | 1.017 | 1.3% / 1.1% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | 12.8 MiB | 12.9 MiB | 1.004 | 1.8% / 2.5% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | 13.0 MiB | 13.2 MiB | 1.010 | 1.8% / 1.0% | ~ |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | 12.6 MiB | 12.9 MiB | 1.022 | 2.7% / 1.7% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | 13.8 MiB | 14.2 MiB | 1.029 | 2.8% / 2.5% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 20.6 MiB | 21.0 MiB | 1.016 | 2.4% / 1.8% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | 13.2 MiB | 13.5 MiB | 1.025 | 6.0% / 1.3% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | 13.5 MiB | 13.4 MiB | 0.988 | 3.2% / 4.9% | ~ |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | 59.2 MiB | 59.5 MiB | 1.005 | 0.8% / 0.4% | ~ |

Peak memory (common cases): geomean B/A = 1.014; largest B/A: `expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us` 13.8 MiB -> 14.2 MiB (1.029x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.3%, 0.1%, 0.1%, 0.1%
Round durations (s): 80, 80, 80, 80, 80 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 401s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
