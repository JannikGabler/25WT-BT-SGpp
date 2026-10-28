A/B `e2e`: A=`WP-14-after` B=`WP-14-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b1/d20/L5 | total | 235.53ms | 234.87ms | 0.994 | [0.974, 1.014] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 208.55ms | 208.32ms | 0.998 | [0.990, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | generation | 19.16ms | 17.83ms | 0.943 | [0.698, 1.274] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.412s | 2.412s | 1.001 | [0.992, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.409s | 2.409s | 1.001 | [0.993, 1.009] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.82ms | 1.84ms | 1.022 | [0.920, 1.135] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.202s | 1.198s | 1.007 | [0.966, 1.050] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.116s | 1.114s | 1.002 | [0.955, 1.052] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 80.90ms | 72.80ms | 1.133 | [0.921, 1.394] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 7.040s | 7.029s | 1.002 | [0.999, 1.006] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.949s | 6.938s | 1.002 | [0.998, 1.007] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 50.29ms | 51.45ms | 0.992 | [0.805, 1.223] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | total | 7.022s | 7.011s | 0.999 | [0.993, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 7.012s | 7.001s | 0.999 | [0.993, 1.005] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 4.71ms | 4.69ms | 0.976 | [0.638, 1.491] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.992s | 1.992s | 1.004 | [0.994, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.992s | 1.992s | 1.004 | [0.994, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 33.87us | 37.37us | 0.826 | [0.631, 1.082] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.42ms | 50.96ms | 1.027 | [0.976, 1.081] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 51.35ms | 50.89ms | 1.027 | [0.976, 1.081] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 20.11us | 21.79us | 0.930 | [0.675, 1.281] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.454s | 2.460s | 1.097 | [0.852, 1.414] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.452s | 2.458s | 1.097 | [0.852, 1.414] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.78ms | 1.73ms | 1.164 | [0.783, 1.728] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 2.074s | 2.073s | 0.983 | [0.935, 1.033] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.09ms | 2.09ms | 0.985 | [0.910, 1.067] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 2.071s | 2.071s | 0.983 | [0.935, 1.033] | ~ (noise) |

Sum of medians (total, common cases): A = 24.482s, B = 24.461s, saved = 20.90ms (1.001x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d20/L6`: A 6.57e-09 -> B 7.12e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; flag beyond ±2%; spread = max/min - 1 over the rounds):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b1/d20/L5 | 57.7 MiB | 57.1 MiB | 0.989 | 12.1% / 12.5% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.7 MiB | 14.7 MiB | 0.999 | 6.5% / 5.4% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1067.2 MiB | 1067.1 MiB | 1.000 | 4.8% / 1.4% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 328.7 MiB | 326.5 MiB | 0.993 | 1.9% / 1.9% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 37.9 MiB | 37.4 MiB | 0.988 | 4.8% / 3.6% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.6 MiB | 24.3 MiB | 0.990 | 2.7% / 6.3% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 11.9 MiB | 1.002 | 3.3% / 1.9% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 13.0 MiB | 0.983 | 4.8% / 3.8% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.0 MiB | 12.1 MiB | 1.004 | 2.1% / 3.8% | ~ |

Peak memory (common cases): geomean B/A = 0.994; largest B/A: `mivec/sg/cc-exp/b1/d20/S7/n300` 12.0 MiB -> 12.1 MiB (1.004x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.1 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.5%, 0.2%, 1.0%, 0.2%, 0.2%
Round durations (s): 100, 100, 105, 100, 100 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 506s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
