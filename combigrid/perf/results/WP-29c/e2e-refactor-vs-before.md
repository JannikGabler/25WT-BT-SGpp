A/B `e2e`: A=`WP-29c-before` B=`WP-29c-refactor`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | total | 175.47ms | 180.17ms | 0.977 | [0.950, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 175.14ms | 179.78ms | 0.977 | [0.950, 1.005] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 207.78us | 208.55us | 1.013 | [0.927, 1.106] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 244.06ms | 251.89ms | 0.967 | [0.928, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 243.91ms | 251.74ms | 0.967 | [0.928, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 77.24us | 73.61us | 1.069 | [0.933, 1.225] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.450s | 1.447s | 1.001 | [0.996, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.450s | 1.447s | 1.001 | [0.996, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 108.74us | 114.47us | 0.956 | [0.762, 1.199] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.261s | 1.260s | 1.001 | [0.988, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.261s | 1.260s | 1.001 | [0.988, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 67.05us | 69.07us | 0.975 | [0.924, 1.029] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.990s | 1.995s | 0.996 | [0.983, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.990s | 1.995s | 0.996 | [0.983, 1.009] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.36us | 36.74us | 0.975 | [0.885, 1.074] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.65ms | 181.13ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.57ms | 181.05ms | 1.002 | [0.994, 1.010] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 28.08us | 27.17us | 1.051 | [0.734, 1.503] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 344.12ms | 334.15ms | 1.026 | [1.012, 1.039] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 344.01ms | 334.04ms | 1.026 | [1.012, 1.039] | faster |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 33.80us | 35.41us | 0.905 | [0.703, 1.164] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.37ms | 50.78ms | 1.002 | [0.987, 1.018] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 51.30ms | 50.72ms | 1.002 | [0.987, 1.018] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 21.86us | 18.72us | 1.054 | [0.810, 1.371] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 110.87ms | 110.11ms | 1.000 | [0.942, 1.062] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.67ms | 104.05ms | 1.000 | [0.940, 1.064] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.65ms | 1.70ms | 0.953 | [0.863, 1.053] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 441.74ms | 448.21ms | 0.983 | [0.950, 1.017] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 439.06ms | 443.63ms | 0.984 | [0.953, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.17ms | 2.16ms | 0.849 | [0.541, 1.332] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.532s | 1.534s | 0.999 | [0.976, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.528s | 1.529s | 0.999 | [0.977, 1.021] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.27ms | 2.22ms | 1.024 | [0.525, 1.997] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.474s | 2.470s | 1.004 | [0.984, 1.025] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.471s | 2.467s | 1.004 | [0.984, 1.025] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.82ms | 1.85ms | 1.008 | [0.898, 1.133] | ~ (noise) |

Sum of medians (total, common cases): A = 10.258s, B = 10.263s, saved = -5257971.0ns (0.999x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d5/L6/p64`: A 7.11e-08 -> B 7.11e-08 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 14.9 MiB | 15.0 MiB | 1.007 | 1.5% / 2.9% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.0 MiB | 11.9 MiB | 0.993 | 2.3% / 2.4% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.1 MiB | 1.001 | 1.6% / 1.3% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.1 MiB | 13.1 MiB | 0.999 | 3.9% / 0.6% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.5 MiB | 24.7 MiB | 1.007 | 2.6% / 5.2% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 12.0 MiB | 1.008 | 2.3% / 2.9% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 11.9 MiB | 11.9 MiB | 1.007 | 1.9% / 2.0% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 12.0 MiB | 1.014 | 3.2% / 2.4% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.4 MiB | 19.5 MiB | 1.006 | 1.0% / 1.8% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.1 MiB | 13.1 MiB | 0.995 | 2.5% / 2.5% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.7 MiB | 1.001 | 1.9% / 3.5% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 13.4 MiB | 1.015 | 4.3% / 5.9% | ~ |

Peak memory (common cases): geomean B/A = 1.004; largest B/A: `interplin/sg/equi/b1/d10/L5/p32` 13.2 MiB -> 13.4 MiB (1.015x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 44, 44, 44, 44, 44 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 219s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
