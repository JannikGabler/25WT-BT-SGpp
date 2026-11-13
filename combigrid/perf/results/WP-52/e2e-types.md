A/B `e2e`: A=`WP-52-after` B=`WP-52-types-inline`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | total | 96.75ms | 96.33ms | 1.002 | [0.985, 1.018] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | operator | 95.11ms | 95.10ms | 1.000 | [0.983, 1.016] | ~ (noise) |
| quad/dsg/equi-exp/b0/d3/L16 | generation | 1.06ms | 1.01ms | 1.223 | [0.999, 1.496] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | total | 210.74ms | 210.84ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 207.99ms | 208.46ms | 0.998 | [0.993, 1.003] | ~ (noise) |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 1.23ms | 1.35ms | 0.915 | [0.709, 1.181] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | total | 1.173s | 1.174s | 1.000 | [0.994, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | operator | 1.165s | 1.166s | 1.000 | [0.994, 1.005] | ~ (noise) |
| quad/dsg/equi-exp/b0/d7/L10 | generation | 3.05ms | 3.04ms | 0.999 | [0.977, 1.021] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | total | 5.362s | 5.365s | 0.999 | [0.995, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | operator | 5.353s | 5.357s | 0.999 | [0.995, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b0/d10/L7 | generation | 3.22ms | 3.34ms | 0.977 | [0.736, 1.297] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | total | 1.228s | 1.229s | 1.002 | [0.995, 1.008] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | operator | 1.153s | 1.151s | 1.002 | [0.994, 1.010] | ~ (noise) |
| quad/dsg/equi-exp/b1/d15/L7 | generation | 43.43ms | 47.01ms | 0.968 | [0.829, 1.130] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 1.216s | 1.218s | 0.998 | [0.994, 1.002] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 1.114s | 1.114s | 1.001 | [0.997, 1.004] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 60.28ms | 62.31ms | 0.952 | [0.864, 1.050] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | total | 249.86ms | 249.29ms | 1.002 | [0.998, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | operator | 245.95ms | 245.50ms | 1.001 | [0.997, 1.006] | ~ (noise) |
| quad/dsg/cc-uq/b0/d5/L12 | generation | 2.32ms | 2.29ms | 1.033 | [0.887, 1.204] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | total | 1.441s | 1.443s | 1.000 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | operator | 1.432s | 1.433s | 1.000 | [0.996, 1.003] | ~ (noise) |
| quad/dsg/cc-uq/b0/d7/L10 | generation | 4.21ms | 4.26ms | 0.978 | [0.808, 1.185] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.853s | 6.845s | 1.004 | [0.995, 1.012] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.844s | 6.836s | 1.003 | [0.995, 1.012] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 3.96ms | 3.95ms | 1.015 | [0.664, 1.551] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.49ms | 52.71ms | 0.982 | [0.945, 1.021] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 52.18ms | 52.42ms | 0.982 | [0.945, 1.021] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 135.07us | 135.77us | 1.003 | [0.956, 1.053] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | total | 43.09ms | 43.04ms | 1.004 | [0.988, 1.021] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 42.88ms | 42.83ms | 1.004 | [0.988, 1.021] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 64.88us | 65.23us | 0.997 | [0.975, 1.020] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | total | 170.48ms | 171.56ms | 0.991 | [0.978, 1.004] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 170.22ms | 171.32ms | 0.991 | [0.978, 1.004] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 75.64us | 74.80us | 0.960 | [0.819, 1.126] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | total | 149.91ms | 149.17ms | 1.007 | [0.996, 1.019] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 149.74ms | 149.00ms | 1.007 | [0.996, 1.018] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 57.69us | 58.04us | 1.105 | [0.908, 1.345] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | total | 392.80ms | 393.21ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 392.72ms | 393.13ms | 0.999 | [0.995, 1.003] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 32.41us | 32.82us | 0.891 | [0.670, 1.187] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.54ms | 6.55ms | 0.977 | [0.900, 1.061] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.45ms | 6.48ms | 0.976 | [0.898, 1.061] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 23.47us | 21.93us | 1.040 | [0.953, 1.135] | ~ (noise) |

Sum of medians (total, common cases): A = 18.645s, B = 18.646s, saved = -112093.0ns (1.000x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/cc-uq/b0/d5/L12`: A 8.94e-14 -> B 1.08e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d3/L16 | 62.5 MiB | 62.8 MiB | 1.004 | 2.3% / 2.3% | ~ |
| quad/dsg/equi-exp/b0/d5/L12 | 26.3 MiB | 26.3 MiB | 0.999 | 5.9% / 2.3% | ~ |
| quad/dsg/equi-exp/b0/d7/L10 | 41.9 MiB | 42.3 MiB | 1.010 | 4.2% / 3.9% | ~ |
| quad/dsg/equi-exp/b0/d10/L7 | 42.5 MiB | 42.1 MiB | 0.991 | 2.6% / 2.4% | ~ |
| quad/dsg/equi-exp/b1/d15/L7 | 151.7 MiB | 154.0 MiB | 1.015 | 4.0% / 3.3% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 217.7 MiB | 217.7 MiB | 1.000 | 0.2% / 0.8% | ~ |
| quad/dsg/cc-uq/b0/d5/L12 | 26.1 MiB | 26.0 MiB | 0.994 | 4.5% / 1.2% | ~ |
| quad/dsg/cc-uq/b0/d7/L10 | 44.2 MiB | 44.1 MiB | 0.998 | 7.0% / 7.5% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 42.7 MiB | 43.0 MiB | 1.009 | 4.2% / 4.1% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 16.0 MiB | 16.0 MiB | 1.003 | 2.1% / 2.8% | ~ |
| interp/dsg/cc/b0/d3/L9/p64 | 12.6 MiB | 12.7 MiB | 1.009 | 2.4% / 1.8% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.6 MiB | 12.6 MiB | 0.998 | 1.3% / 1.5% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.5 MiB | 12.5 MiB | 0.995 | 1.8% / 2.0% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.2 MiB | 0.997 | 2.2% / 1.3% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.2 MiB | 12.2 MiB | 0.997 | 2.7% / 3.0% | ~ |

Peak memory (common cases): geomean B/A = 1.001; largest B/A: `quad/dsg/equi-exp/b1/d15/L7` 151.7 MiB -> 154.0 MiB (1.015x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.6 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 78, 78, 78, 78, 78 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 391s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
