A/B `e2e`: A=`WP-82-before` B=`WP-82-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 94.63ms | 93.78ms | 1.003 | [0.993, 1.014] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 93.70ms | 90.51ms | 1.032 | [1.022, 1.042] | faster |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 816.44us | 877.06us | 0.833 | [0.423, 1.641] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 202.78ms | 202.66ms | 1.000 | [0.993, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 202.19ms | 201.86ms | 1.001 | [0.994, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 465.84us | 444.96us | 1.042 | [0.808, 1.345] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.148s | 1.147s | 1.003 | [0.997, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.148s | 1.145s | 1.003 | [0.998, 1.009] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 180.19us | 196.39us | 0.824 | [0.448, 1.518] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.407s | 5.408s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.406s | 5.407s | 1.000 | [0.996, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 252.33us | 476.80us | 0.751 | [0.376, 1.502] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.163s | 1.163s | 1.001 | [0.997, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.154s | 1.148s | 1.005 | [1.001, 1.009] | faster |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 4.06ms | 4.08ms | 0.994 | [0.852, 1.161] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.138s | 1.136s | 1.003 | [0.999, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.118s | 1.108s | 1.010 | [1.006, 1.014] | faster |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 11.27ms | 11.22ms | 1.014 | [0.984, 1.044] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 247.65ms | 247.55ms | 1.000 | [0.997, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 246.91ms | 246.55ms | 1.002 | [0.998, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 630.66us | 643.44us | 1.005 | [0.933, 1.082] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.452s | 1.453s | 0.998 | [0.992, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.451s | 1.451s | 0.998 | [0.992, 1.004] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 236.55us | 225.24us | 1.030 | [0.899, 1.179] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.877s | 6.875s | 1.001 | [0.995, 1.007] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.876s | 6.874s | 1.001 | [0.996, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 474.92us | 633.39us | 0.736 | [0.481, 1.125] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | total | 571.31ms | 571.40ms | 0.999 | [0.994, 1.005] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | operator | 569.14ms | 568.95ms | 1.000 | [0.996, 1.004] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | generation | 2.12ms | 2.18ms | 0.910 | [0.419, 1.979] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | total | 520.78ms | 520.61ms | 1.002 | [0.977, 1.026] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | operator | 518.71ms | 518.56ms | 1.001 | [0.980, 1.022] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | generation | 1.82ms | 1.75ms | 1.218 | [0.546, 2.717] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | total | 881.28ms | 882.63ms | 0.996 | [0.988, 1.004] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | operator | 879.80ms | 881.15ms | 0.996 | [0.988, 1.004] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | generation | 1.13ms | 1.12ms | 1.015 | [0.980, 1.051] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | total | 502.97ms | 502.98ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | operator | 502.89ms | 502.91ms | 1.000 | [0.996, 1.003] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | generation | 47.14us | 43.51us | 1.031 | [0.948, 1.121] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | total | 345.52ms | 345.54ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | operator | 345.46ms | 345.48ms | 1.000 | [0.995, 1.005] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | generation | 33.87us | 34.29us | 0.993 | [0.925, 1.066] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | total | 350.01ms | 350.02ms | 0.999 | [0.991, 1.008] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | operator | 349.20ms | 348.74ms | 1.001 | [0.997, 1.004] | ~ (noise) |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | generation | 310.58us | 320.57us | 0.990 | [0.885, 1.108] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | total | 333.35ms | 333.94ms | 0.998 | [0.995, 1.002] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | operator | 329.79ms | 330.31ms | 0.998 | [0.995, 1.002] | ~ (noise) |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | generation | 3.23ms | 3.23ms | 1.003 | [0.979, 1.028] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | total | 99.93ms | 99.04ms | 0.987 | [0.939, 1.038] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | operator | 96.84ms | 96.83ms | 0.983 | [0.937, 1.032] | ~ (noise) |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | generation | 2.04ms | 2.03ms | 1.108 | [0.786, 1.561] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | total | 207.71ms | 209.93ms | 0.994 | [0.984, 1.005] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | operator | 205.64ms | 205.76ms | 0.996 | [0.988, 1.003] | ~ (noise) |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | generation | 2.02ms | 1.89ms | 0.929 | [0.311, 2.779] | ~ (noise) |

Sum of medians (total, common cases, group suite): A = 17.730s, B = 17.726s, saved = 4.28ms (1.000x)
Sum of medians (total, common cases, group expensive/): A = 3.813s, B = 3.816s, saved = -3222594.0ns (0.999x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b0/d5/L12`: A 3.53e-12 -> B 3.57e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 44.3 MiB | 44.5 MiB | 1.004 | 3.2% / 4.6% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 17.9 MiB | 18.1 MiB | 1.011 | 3.8% / 3.1% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 28.6 MiB | 28.9 MiB | 1.010 | 5.0% / 7.1% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 30.1 MiB | 30.2 MiB | 1.004 | 1.3% / 3.8% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 51.6 MiB | 51.8 MiB | 1.004 | 0.5% / 0.4% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 82.9 MiB | 82.9 MiB | 1.001 | 0.4% / 0.3% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 18.2 MiB | 18.3 MiB | 1.004 | 4.5% / 3.3% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 29.6 MiB | 28.8 MiB | 0.970 | 2.6% / 3.7% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 30.4 MiB | 30.1 MiB | 0.990 | 3.7% / 2.6% | ~ |
| expensive/quad/dsg/cc-exp/b1/d10/L4/c100us | 13.0 MiB | 13.3 MiB | 1.020 | 2.9% / 1.1% | ~ |
| expensive/quad/dsg/cc-exp/b1/d20/L3/c100us | 13.2 MiB | 13.3 MiB | 1.012 | 1.8% / 1.5% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d15/S6/n300/c100us | 12.7 MiB | 12.7 MiB | 1.002 | 2.6% / 1.8% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L3/p8/c100us | 12.4 MiB | 12.5 MiB | 1.007 | 2.2% / 1.2% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d5/L5/p16/c100us | 12.4 MiB | 12.4 MiB | 1.002 | 2.5% / 0.8% | ~ |
| expensive/quad/dsg/cc-exp/b1/d15/L5/c1us | 15.9 MiB | 16.0 MiB | 1.003 | 0.7% / 1.3% | ~ |
| expensive/mivec/dsg/cc-exp/b1/d20/S10/n300/c1us | 13.6 MiB | 13.6 MiB | 1.005 | 1.2% / 1.6% | ~ |
| expensive/interp/dsg/cc-exp/b1/d10/L4/p16/c1us | 13.1 MiB | 13.0 MiB | 0.999 | 1.1% / 2.4% | ~ |
| expensive/interplin/dsg/equi-exp/b1/d10/L4/p16/c1us | 14.0 MiB | 14.2 MiB | 1.015 | 3.8% / 4.8% | ~ |

Peak memory (common cases): geomean B/A = 1.004; largest B/A: `expensive/quad/dsg/cc-exp/b1/d10/L4/c100us` 13.0 MiB -> 13.3 MiB (1.020x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.1%, 0.1%, 0.2%, 0.2%, 0.1%
Round durations (s): 90, 90, 90, 90, 90 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 449s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
