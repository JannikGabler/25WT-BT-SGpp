A/B `e2e`: A=`rp-2` B=`rp-3`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | total | 176.43ms | 175.92ms | 1.147 | [0.783, 1.678] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 176.01ms | 175.55ms | 1.147 | [0.783, 1.680] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 213.85us | 208.13us | 1.000 | [0.915, 1.092] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 243.75ms | 246.01ms | 0.993 | [0.973, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 243.60ms | 245.85ms | 0.993 | [0.973, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 74.52us | 73.82us | 1.013 | [0.941, 1.091] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.448s | 1.450s | 1.000 | [0.989, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.447s | 1.450s | 1.000 | [0.989, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 114.05us | 123.48us | 0.969 | [0.773, 1.215] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.262s | 1.261s | 1.002 | [0.989, 1.015] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.262s | 1.261s | 1.002 | [0.989, 1.015] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 66.91us | 66.28us | 1.018 | [0.965, 1.074] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.000s | 2.004s | 1.001 | [0.990, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.000s | 2.004s | 1.001 | [0.990, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 35.20us | 34.78us | 1.043 | [0.766, 1.420] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.67ms | 181.89ms | 1.005 | [0.988, 1.023] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.58ms | 181.81ms | 1.005 | [0.988, 1.023] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 27.59us | 27.24us | 1.023 | [0.715, 1.464] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 337.12ms | 341.65ms | 0.980 | [0.962, 0.998] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 337.00ms | 341.54ms | 0.980 | [0.962, 0.998] | SLOWER |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 35.55us | 35.76us | 0.990 | [0.770, 1.275] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.29ms | 51.09ms | 1.025 | [0.966, 1.088] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 51.22ms | 51.02ms | 1.025 | [0.966, 1.088] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 23.40us | 21.44us | 1.138 | [0.896, 1.446] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 109.63ms | 108.71ms | 1.006 | [0.969, 1.045] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 103.26ms | 102.49ms | 1.007 | [0.967, 1.050] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.58ms | 1.71ms | 0.930 | [0.855, 1.011] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 438.69ms | 441.56ms | 0.996 | [0.971, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 435.61ms | 438.66ms | 0.997 | [0.971, 1.024] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.24ms | 2.19ms | 0.872 | [0.419, 1.818] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.509s | 1.547s | 0.974 | [0.942, 1.007] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.506s | 1.544s | 0.974 | [0.942, 1.007] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.27ms | 2.35ms | 1.045 | [0.691, 1.582] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.474s | 2.468s | 1.003 | [0.983, 1.024] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.471s | 2.466s | 1.003 | [0.983, 1.024] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.79ms | 1.77ms | 0.983 | [0.910, 1.062] | ~ (noise) |

Sum of medians (total, common cases): A = 10.231s, B = 10.277s, saved = -46318852.0ns (0.995x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 1.13e-16 -> B 4.52e-16 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 14.8 MiB | 14.9 MiB | 1.006 | 0.7% / 3.9% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.0 MiB | 12.0 MiB | 1.006 | 0.5% / 1.7% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.0 MiB | 12.0 MiB | 0.996 | 1.8% / 2.3% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.9 MiB | 13.0 MiB | 1.005 | 3.3% / 2.4% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.7 MiB | 24.3 MiB | 0.984 | 8.2% / 6.5% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 11.9 MiB | 11.7 MiB | 0.981 | 2.6% / 1.6% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.0 MiB | 11.9 MiB | 0.990 | 2.2% / 3.5% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 11.8 MiB | 0.994 | 1.5% / 2.3% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.5 MiB | 19.7 MiB | 1.011 | 2.3% / 3.2% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.0 MiB | 12.8 MiB | 0.990 | 2.3% / 5.3% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.8 MiB | 12.7 MiB | 0.992 | 3.7% / 5.0% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 13.4 MiB | 1.018 | 5.8% / 6.2% | ~ |

Peak memory (common cases): geomean B/A = 0.998; largest B/A: `interplin/sg/equi/b1/d10/L5/p32` 13.2 MiB -> 13.4 MiB (1.018x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.1 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.3%, 0.2%, 0.3%
Round durations (s): 44, 44, 44, 44, 44 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 219s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
