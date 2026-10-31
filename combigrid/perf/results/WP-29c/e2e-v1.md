A/B `e2e`: A=`WP-29c-before` B=`WP-29c-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | total | 177.65ms | 176.92ms | 1.004 | [0.984, 1.025] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 177.21ms | 176.52ms | 1.004 | [0.984, 1.024] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 210.92us | 208.54us | 1.020 | [0.934, 1.114] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 244.47ms | 245.57ms | 0.997 | [0.977, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 244.32ms | 245.42ms | 0.997 | [0.977, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 84.86us | 72.77us | 1.176 | [0.873, 1.585] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.446s | 1.452s | 0.997 | [0.989, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.445s | 1.452s | 0.997 | [0.989, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 122.64us | 108.60us | 1.128 | [0.900, 1.415] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.259s | 1.267s | 0.995 | [0.982, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.259s | 1.267s | 0.995 | [0.982, 1.008] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 67.74us | 67.19us | 0.979 | [0.904, 1.061] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.997s | 2.008s | 0.997 | [0.981, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.996s | 2.008s | 0.997 | [0.981, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 33.94us | 34.64us | 1.449 | [0.462, 4.543] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.93ms | 181.25ms | 1.012 | [0.987, 1.037] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.85ms | 181.17ms | 1.012 | [0.987, 1.037] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 26.96us | 28.56us | 0.944 | [0.660, 1.351] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 342.93ms | 336.77ms | 1.018 | [0.994, 1.042] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 342.83ms | 336.65ms | 1.018 | [0.994, 1.042] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 34.78us | 35.55us | 0.986 | [0.766, 1.269] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.30ms | 50.96ms | 1.010 | [0.992, 1.028] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 51.23ms | 50.89ms | 1.010 | [0.992, 1.028] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 19.97us | 20.88us | 0.914 | [0.712, 1.173] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 110.55ms | 110.35ms | 1.012 | [0.975, 1.051] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.47ms | 104.16ms | 1.014 | [0.973, 1.056] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.65ms | 1.66ms | 1.002 | [0.921, 1.089] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 447.92ms | 445.03ms | 0.988 | [0.898, 1.087] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 444.89ms | 441.96ms | 0.987 | [0.897, 1.086] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.30ms | 2.16ms | 1.196 | [0.839, 1.703] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.526s | 1.549s | 0.986 | [0.964, 1.009] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.523s | 1.547s | 0.986 | [0.964, 1.009] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.38ms | 2.24ms | 1.013 | [0.669, 1.533] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.481s | 2.479s | 1.002 | [0.982, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.478s | 2.476s | 1.002 | [0.982, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.82ms | 1.67ms | 1.078 | [0.998, 1.164] | ~ (noise) |

Sum of medians (total, common cases): A = 10.265s, B = 10.302s, saved = -37621452.0ns (0.996x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cheb1/b0/d3/L6/p64`: A 1.94e-13 -> B 1.95e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 14.7 MiB | 14.8 MiB | 1.005 | 6.9% / 5.1% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.1 MiB | 12.0 MiB | 0.998 | 0.5% / 2.3% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.0 MiB | 12.0 MiB | 0.999 | 1.8% / 1.3% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.0 MiB | 12.9 MiB | 0.990 | 5.3% / 3.0% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.4 MiB | 24.8 MiB | 1.018 | 7.1% / 2.1% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.0 MiB | 12.0 MiB | 0.999 | 1.6% / 2.7% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.9 MiB | 11.9 MiB | 1.004 | 3.0% / 1.4% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 11.9 MiB | 0.998 | 1.8% / 2.1% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.8 MiB | 19.6 MiB | 0.990 | 4.1% / 2.9% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.1 MiB | 12.9 MiB | 0.991 | 3.3% / 1.6% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 12.9 MiB | 0.993 | 3.0% / 1.5% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.1 MiB | 13.3 MiB | 1.016 | 1.4% / 3.6% | ~ |

Peak memory (common cases): geomean B/A = 1.000; largest B/A: `interp/sg/cc/b0/d10/L2/p8` 24.4 MiB -> 24.8 MiB (1.018x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 44, 44, 44, 44, 44 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 219s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
