A/B `e2e`: A=`WP-76-after` B=`WP-76-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | total | 1.538s | 1.538s | 0.999 | [0.994, 1.005] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | operator | 1.534s | 1.534s | 0.999 | [0.994, 1.005] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | generation | 2.97ms | 2.15ms | 1.125 | [0.729, 1.736] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | total | 570.82ms | 571.40ms | 0.998 | [0.984, 1.012] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | operator | 568.82ms | 569.03ms | 1.000 | [0.989, 1.011] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | generation | 2.10ms | 2.21ms | 0.762 | [0.439, 1.321] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | total | 1.039s | 1.038s | 1.000 | [0.991, 1.010] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | operator | 1.036s | 1.035s | 1.000 | [0.991, 1.010] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | generation | 2.11ms | 2.09ms | 0.978 | [0.726, 1.316] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | total | 521.53ms | 521.41ms | 1.000 | [0.980, 1.020] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | operator | 518.48ms | 518.46ms | 0.999 | [0.982, 1.017] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | generation | 2.00ms | 1.91ms | 1.096 | [0.685, 1.752] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | total | 2.156s | 2.158s | 0.999 | [0.997, 1.002] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | operator | 2.155s | 2.157s | 0.999 | [0.997, 1.002] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.15ms | 1.14ms | 1.005 | [0.987, 1.024] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | total | 880.86ms | 880.49ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | operator | 879.57ms | 879.15ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.12ms | 1.12ms | 0.991 | [0.955, 1.028] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | total | 1.073s | 1.074s | 0.994 | [0.982, 1.007] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | operator | 1.073s | 1.074s | 0.994 | [0.982, 1.007] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | generation | 65.96us | 68.03us | 0.952 | [0.873, 1.038] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | total | 502.98ms | 503.10ms | 1.000 | [0.984, 1.016] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | operator | 502.90ms | 503.01ms | 1.000 | [0.984, 1.016] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | generation | 45.19us | 44.56us | 1.007 | [0.951, 1.066] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | total | 830.09ms | 832.44ms | 0.964 | [0.923, 1.007] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | operator | 830.03ms | 832.37ms | 0.964 | [0.923, 1.007] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | generation | 45.85us | 47.70us | 0.987 | [0.881, 1.106] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | total | 345.27ms | 345.11ms | 1.001 | [0.998, 1.004] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | operator | 345.21ms | 345.06ms | 1.001 | [0.998, 1.004] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | generation | 38.27us | 35.51us | 1.092 | [0.958, 1.245] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | total | 2.476s | 2.476s | 1.000 | [0.997, 1.002] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | operator | 2.475s | 2.474s | 1.000 | [0.997, 1.002] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | generation | 1.16ms | 1.14ms | 1.022 | [1.010, 1.035] | faster |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | total | 398.10ms | 398.27ms | 0.999 | [0.994, 1.003] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | operator | 398.04ms | 398.20ms | 0.999 | [0.994, 1.003] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | generation | 37.99us | 39.39us | 0.936 | [0.803, 1.092] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | total | 1.295s | 1.289s | 1.008 | [0.990, 1.026] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | operator | 1.295s | 1.289s | 1.008 | [0.990, 1.026] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | generation | 49.97us | 48.99us | 0.990 | [0.904, 1.084] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | total | 1.153s | 1.154s | 0.998 | [0.989, 1.007] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | operator | 1.151s | 1.152s | 0.998 | [0.989, 1.007] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | generation | 1.86ms | 1.86ms | 0.994 | [0.976, 1.012] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | total | 349.88ms | 350.27ms | 1.000 | [0.984, 1.016] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | operator | 349.06ms | 349.37ms | 1.000 | [0.985, 1.016] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | generation | 334.99us | 339.22us | 0.767 | [0.426, 1.382] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | total | 662.83ms | 664.61ms | 1.000 | [0.980, 1.020] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | operator | 659.26ms | 660.98ms | 1.000 | [0.980, 1.020] | ~ (noise) |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.37ms | 3.40ms | 0.994 | [0.977, 1.012] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | total | 333.56ms | 333.52ms | 1.001 | [0.998, 1.003] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | operator | 329.97ms | 329.78ms | 1.001 | [0.998, 1.003] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.24ms | 3.23ms | 0.989 | [0.960, 1.019] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | total | 259.97ms | 257.95ms | 1.057 | [0.919, 1.215] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | operator | 255.83ms | 255.58ms | 1.055 | [0.917, 1.214] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | generation | 2.12ms | 1.94ms | 1.197 | [0.738, 1.942] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | total | 99.14ms | 98.71ms | 1.049 | [0.975, 1.129] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | operator | 96.81ms | 96.70ms | 1.049 | [0.974, 1.131] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | generation | 1.93ms | 1.99ms | 1.018 | [0.934, 1.110] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | total | 1.074s | 1.075s | 0.994 | [0.937, 1.054] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | operator | 1.072s | 1.072s | 0.994 | [0.937, 1.054] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | generation | 2.13ms | 2.00ms | 0.959 | [0.744, 1.236] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | total | 209.12ms | 207.83ms | 1.021 | [0.967, 1.079] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | operator | 205.71ms | 205.68ms | 1.024 | [0.973, 1.077] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | generation | 2.15ms | 2.05ms | 1.002 | [0.523, 1.919] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | 575.91ms | 575.93ms | 1.000 | [0.998, 1.003] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | operator | 572.16ms | 572.18ms | 1.000 | [0.998, 1.003] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | generation | 2.08ms | 2.16ms | 0.969 | [0.707, 1.329] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | total | 70.60ms | 70.23ms | 1.005 | [0.998, 1.013] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | operator | 70.48ms | 70.11ms | 1.005 | [0.998, 1.013] | ~ (noise) |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | generation | 44.35us | 44.24us | 0.988 | [0.839, 1.164] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | total | 246.40ms | 243.40ms | 1.029 | [0.974, 1.086] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | operator | 246.24ms | 243.26ms | 1.029 | [0.975, 1.086] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | generation | 38.83us | 36.63us | 1.096 | [0.950, 1.265] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | total | 153.40ms | 152.74ms | 1.096 | [0.935, 1.285] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | operator | 144.55ms | 144.11ms | 1.100 | [0.932, 1.297] | ~ (noise) |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | generation | 55.77us | 55.77us | 0.962 | [0.866, 1.069] | ~ (noise) |

Sum of medians (total, common cases, group expensive/): A = 17.768s, B = 17.766s, saved = 2.77ms (1.000x)
Sum of medians (total, common cases, group expensive/ cached): A = 1.046s, B = 1.042s, saved = 4.02ms (1.004x)
Result sanity (rel. error vs exact): worst growth in `expensive/quad/sg/cc-exp/b1/d15/L5/c1us`: A 5.65e-09 -> B 5.66e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us | 12.9 MiB | 12.9 MiB | 1.004 | 3.2% / 2.5% | ~ |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | 13.1 MiB | 13.2 MiB | 1.004 | 2.7% / 6.2% | ~ |
| expensive/quad/sg/cc-exp/b1/d20/L3/c100us | 13.1 MiB | 13.1 MiB | 1.004 | 2.5% / 1.4% | ~ |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | 13.3 MiB | 13.2 MiB | 0.990 | 1.9% / 2.9% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us | 12.5 MiB | 12.5 MiB | 1.001 | 3.1% / 2.7% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | 12.7 MiB | 12.7 MiB | 0.997 | 2.5% / 2.2% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us | 12.2 MiB | 12.2 MiB | 1.001 | 2.7% / 2.2% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | 12.5 MiB | 12.5 MiB | 1.001 | 1.6% / 3.9% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us | 12.2 MiB | 12.1 MiB | 0.997 | 2.9% / 3.8% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | 12.4 MiB | 12.3 MiB | 0.994 | 2.1% / 3.0% | ~ |
| expensive/mivec/sg/cc-exp/b1/d15/S6/n300/c100us-ramp | 12.5 MiB | 12.5 MiB | 1.005 | 1.7% / 3.2% | ~ |
| expensive/interp/sg/cc-exp/b1/d7/L3/p8/c100us-ramp | 12.2 MiB | 12.1 MiB | 0.990 | 3.9% / 2.0% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-ramp | 12.0 MiB | 12.0 MiB | 1.001 | 3.9% / 3.3% | ~ |
| expensive/quad/sg/cc-exp/b1/d15/L5/c1us | 13.8 MiB | 13.8 MiB | 1.002 | 1.8% / 1.0% | ~ |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | 15.9 MiB | 15.9 MiB | 0.998 | 1.9% / 1.6% | ~ |
| expensive/mivec/sg/cc-exp/b1/d20/S10/n300/c1us | 13.2 MiB | 13.2 MiB | 0.998 | 1.4% / 1.2% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | 13.5 MiB | 13.6 MiB | 1.007 | 1.6% / 2.3% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L4/p16/c1us | 12.7 MiB | 12.7 MiB | 0.999 | 3.1% / 3.1% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | 13.1 MiB | 13.1 MiB | 1.001 | 3.2% / 2.3% | ~ |
| expensive/interplin/sg/equi-exp/b1/d10/L4/p16/c1us | 12.8 MiB | 12.7 MiB | 0.993 | 3.6% / 3.1% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | 14.1 MiB | 14.2 MiB | 1.003 | 3.6% / 5.6% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 20.8 MiB | 20.7 MiB | 0.995 | 4.5% / 2.9% | ~ |
| expensive/interp/sg/cc-exp/b1/d10/L3/p8/c100us-cached | 13.6 MiB | 13.5 MiB | 0.993 | 6.7% / 4.7% | ~ |
| expensive/interplin/sg/equi-exp/b1/d5/L5/p16/c100us-cached | 13.5 MiB | 13.6 MiB | 1.002 | 3.3% / 2.7% | ~ |
| expensive/interplin/sg/equi-exp/b1/d8/L4/p32/c1us-cached | 59.3 MiB | 59.4 MiB | 1.002 | 1.2% / 1.0% | ~ |

Peak memory (common cases): geomean B/A = 0.999; largest B/A: `expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us` 13.5 MiB -> 13.6 MiB (1.007x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.6%, 0.5%, 0.4%, 0.4%, 0.5%, 0.4%, 0.4%, 0.4%
Round durations (s): 81, 81, 80, 80, 80, 80, 80, 81 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 644s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
