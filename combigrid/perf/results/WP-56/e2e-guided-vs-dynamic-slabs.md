A/B `e2e`: A=`WP-56-c-guided` B=`WP-56-c`, 16 thread(s), 4 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 283.02ms | 284.55ms | 0.997 | [0.867, 1.148] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 244.34ms | 244.99ms | 0.995 | [0.863, 1.147] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 38.64ms | 37.71ms | 1.013 | [0.883, 1.162] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 264.06ms | 264.47ms | 0.997 | [0.974, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 262.40ms | 262.85ms | 0.997 | [0.981, 1.013] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.63ms | 1.59ms | 0.964 | [0.365, 2.547] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 515.20ms | 511.78ms | 1.008 | [0.994, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 512.30ms | 509.87ms | 1.007 | [0.992, 1.023] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 2.42ms | 1.40ms | 1.121 | [0.198, 6.329] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 910.03ms | 911.03ms | 1.001 | [0.993, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 907.91ms | 908.03ms | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.08ms | 2.00ms | 1.045 | [0.408, 2.678] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 291.08ms | 244.32ms | 1.191 | [1.164, 1.218] | faster |
| quad/sg/equi-exp/b1/d10/L7 | operator | 288.75ms | 242.04ms | 1.192 | [1.165, 1.219] | faster |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.57ms | 1.56ms | 1.002 | [0.785, 1.278] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 504.40ms | 491.50ms | 1.027 | [1.011, 1.044] | faster |
| quad/sg/equi-exp/b1/d15/L6 | operator | 495.08ms | 482.14ms | 1.028 | [1.009, 1.047] | faster |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.61ms | 6.68ms | 0.988 | [0.855, 1.143] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 213.42ms | 213.18ms | 1.005 | [0.988, 1.021] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.64ms | 201.32ms | 1.006 | [0.990, 1.022] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.39ms | 8.62ms | 0.980 | [0.667, 1.442] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.405s | 2.397s | 1.003 | [0.993, 1.014] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.403s | 2.395s | 1.003 | [0.993, 1.014] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.53ms | 1.44ms | 1.079 | [0.910, 1.279] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 371.46ms | 382.01ms | 0.982 | [0.933, 1.034] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 299.51ms | 303.18ms | 0.989 | [0.957, 1.022] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 69.61ms | 74.47ms | 0.980 | [0.768, 1.251] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 75.32ms | 75.86ms | 0.985 | [0.938, 1.034] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.07ms | 74.46ms | 0.999 | [0.982, 1.016] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.23ms | 1.39ms | 0.645 | [0.215, 1.939] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 319.52ms | 319.85ms | 1.000 | [0.992, 1.009] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 316.71ms | 318.20ms | 0.998 | [0.991, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.19ms | 1.59ms | 1.457 | [0.700, 3.035] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 687.75ms | 690.70ms | 0.997 | [0.989, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 685.42ms | 689.15ms | 0.996 | [0.989, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.46ms | 1.41ms | 1.194 | [0.722, 1.975] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.310s | 1.311s | 1.000 | [0.992, 1.007] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.307s | 1.309s | 1.000 | [0.993, 1.006] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.78ms | 2.82ms | 1.017 | [0.489, 2.115] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 724.40ms | 722.18ms | 1.003 | [0.937, 1.074] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 721.23ms | 719.00ms | 1.003 | [0.937, 1.075] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 2.65ms | 2.66ms | 0.999 | [0.985, 1.014] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 371.71ms | 339.55ms | 1.092 | [1.070, 1.116] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 364.05ms | 331.90ms | 1.097 | [1.064, 1.131] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.10ms | 7.11ms | 0.915 | [0.691, 1.212] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 26.30ms | 27.01ms | 0.985 | [0.904, 1.072] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 23.76ms | 24.47ms | 0.983 | [0.894, 1.081] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.36ms | 2.36ms | 0.999 | [0.985, 1.013] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 2.85ms | 3.03ms | 0.845 | [0.591, 1.210] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 1.95ms | 2.12ms | 0.926 | [0.836, 1.026] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 745.62us | 757.67us | 0.740 | [0.296, 1.850] | ~ (noise) |

Sum of medians (total, common cases): A = 9.275s, B = 9.189s, saved = 86.29ms (1.009x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 1.18e-12 -> B 4.25e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 122.9 MiB | 119.4 MiB | 0.972 | 19.7% / 13.8% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 12.9 MiB | 12.7 MiB | 0.990 | 0.5% / 1.9% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.9 MiB | 12.8 MiB | 0.994 | 1.0% / 3.2% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.7 MiB | 12.8 MiB | 1.014 | 2.4% / 3.2% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.4 MiB | 13.3 MiB | 0.993 | 1.6% / 1.0% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.5 MiB | 18.5 MiB | 0.999 | 1.5% / 2.0% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.3 MiB | 20.3 MiB | 0.997 | 0.8% / 1.6% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.8 MiB | 12.7 MiB | 0.995 | 0.9% / 3.5% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 188.6 MiB | 194.7 MiB | 1.033 | 12.7% / 10.0% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 14.8 MiB | 15.4 MiB | 1.036 | 6.0% / 2.3% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 12.9 MiB | 12.9 MiB | 0.997 | 1.5% / 1.5% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 12.9 MiB | 0.998 | 1.7% / 2.1% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 13.0 MiB | 1.002 | 1.5% / 1.1% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 13.4 MiB | 13.3 MiB | 0.997 | 0.4% / 1.2% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 14.9 MiB | 15.0 MiB | 1.004 | 1.4% / 1.0% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 13.3 MiB | 13.4 MiB | 1.005 | 2.1% / 1.3% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.7 MiB | 12.6 MiB | 0.997 | 2.4% / 2.3% | ~ |

Peak memory (common cases): geomean B/A = 1.001; largest B/A: `quad/sg/cc-uq/b0/d3/L14` 14.8 MiB -> 15.4 MiB (1.036x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.3%
Round durations (s): 40, 40, 40, 40 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 161s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
