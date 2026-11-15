A/B `e2e`: A=`WP-56-c-lazy` B=`WP-56-c`, 16 thread(s), 4 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 280.65ms | 283.21ms | 0.990 | [0.860, 1.139] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 242.93ms | 244.36ms | 0.995 | [0.863, 1.147] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 37.68ms | 37.86ms | 1.003 | [0.874, 1.151] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 264.42ms | 263.35ms | 1.003 | [0.989, 1.019] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 261.68ms | 261.68ms | 1.000 | [0.985, 1.015] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 2.43ms | 1.58ms | 1.458 | [0.699, 3.041] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 512.32ms | 512.20ms | 1.021 | [0.957, 1.090] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 510.72ms | 510.68ms | 1.021 | [0.957, 1.090] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.43ms | 1.28ms | 1.110 | [1.024, 1.203] | faster |
| quad/sg/equi-exp/b0/d10/L4 | total | 908.44ms | 908.77ms | 1.000 | [0.993, 1.008] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 905.60ms | 904.75ms | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.89ms | 3.00ms | 0.914 | [0.694, 1.204] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 241.60ms | 241.74ms | 1.005 | [0.952, 1.061] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 239.35ms | 239.47ms | 1.005 | [0.952, 1.062] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 1.55ms | 1.55ms | 0.999 | [0.783, 1.274] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 490.86ms | 490.34ms | 1.000 | [0.984, 1.016] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 481.48ms | 481.06ms | 1.000 | [0.982, 1.018] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 6.69ms | 6.61ms | 1.010 | [0.873, 1.168] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 213.24ms | 212.91ms | 1.004 | [0.988, 1.020] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 201.42ms | 201.15ms | 1.004 | [0.994, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 8.39ms | 8.44ms | 0.992 | [0.675, 1.459] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.396s | 2.395s | 1.002 | [0.991, 1.012] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.394s | 2.393s | 1.002 | [0.991, 1.012] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.55ms | 1.51ms | 1.033 | [0.881, 1.212] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 370.76ms | 376.21ms | 0.990 | [0.940, 1.043] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 295.05ms | 306.75ms | 0.966 | [0.935, 0.999] | SLOWER |
| quad/sg/cc-uq/b0/d2/L20 | generation | 72.26ms | 69.28ms | 1.063 | [0.883, 1.279] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 76.05ms | 75.86ms | 0.999 | [0.981, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 74.76ms | 74.51ms | 1.002 | [0.986, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 1.28ms | 1.28ms | 0.904 | [0.606, 1.349] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 319.50ms | 319.48ms | 1.002 | [0.990, 1.014] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 316.88ms | 317.34ms | 0.999 | [0.993, 1.004] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.40ms | 1.57ms | 1.334 | [0.451, 3.949] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 688.36ms | 689.81ms | 1.013 | [0.966, 1.061] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | operator | 686.82ms | 687.90ms | 1.014 | [0.968, 1.061] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | generation | 1.42ms | 1.56ms | 0.771 | [0.366, 1.623] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.310s | 1.313s | 0.998 | [0.990, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.307s | 1.310s | 0.997 | [0.990, 1.005] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.35ms | 2.09ms | 1.162 | [0.551, 2.453] | ~ (noise) |

Sum of medians (total, common cases): A = 8.071s, B = 8.082s, saved = -10396648.5ns (0.999x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-exp/b1/d15/L6`: A 1.16e-09 -> B 7.71e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 109.0 MiB | 120.0 MiB | 1.101 | 3.1% / 6.5% | MEM+ |
| quad/sg/equi-exp/b0/d5/L10 | 12.8 MiB | 12.7 MiB | 0.989 | 0.7% / 2.3% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 12.8 MiB | 12.8 MiB | 0.996 | 1.7% / 2.6% | ~ |
| quad/sg/equi-exp/b0/d10/L4 | 12.9 MiB | 12.8 MiB | 0.993 | 1.8% / 1.5% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 13.2 MiB | 13.1 MiB | 0.992 | 2.0% / 1.5% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 18.5 MiB | 18.5 MiB | 0.998 | 1.3% / 1.5% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 20.3 MiB | 20.3 MiB | 1.002 | 1.7% / 0.5% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 12.8 MiB | 12.8 MiB | 1.001 | 1.2% / 1.8% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 194.1 MiB | 201.1 MiB | 1.036 | 6.1% / 8.7% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 14.9 MiB | 15.4 MiB | 1.031 | 5.5% / 3.7% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 12.9 MiB | 12.7 MiB | 0.986 | 2.9% / 4.9% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 12.9 MiB | 12.9 MiB | 0.997 | 2.4% / 1.1% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.9 MiB | 12.9 MiB | 1.006 | 1.4% / 2.1% | ~ |

Peak memory (common cases): geomean B/A = 1.009; largest B/A: `quad/sg/equi-exp/b0/d2/L20` 109.0 MiB -> 120.0 MiB (1.101x); 1 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.4 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%
Round durations (s): 35, 35, 35, 35 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 139s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
