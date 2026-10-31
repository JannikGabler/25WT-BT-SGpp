A/B `e2e`: A=`WP-29d-before` B=`WP-29d-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | total | 175.24ms | 176.05ms | 0.997 | [0.979, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 174.90ms | 175.69ms | 0.997 | [0.979, 1.016] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 210.29us | 205.54us | 1.013 | [0.927, 1.106] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 244.18ms | 244.07ms | 1.000 | [0.980, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 244.03ms | 243.93ms | 1.000 | [0.980, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 72.98us | 75.01us | 0.990 | [0.920, 1.066] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.448s | 1.448s | 1.001 | [0.994, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.447s | 1.447s | 1.001 | [0.994, 1.007] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 118.94us | 114.05us | 1.036 | [0.819, 1.311] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.261s | 1.258s | 1.001 | [0.988, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.261s | 1.258s | 1.001 | [0.988, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 67.54us | 66.35us | 0.742 | [0.307, 1.791] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.985s | 1.989s | 1.001 | [0.985, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.985s | 1.989s | 1.001 | [0.985, 1.017] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.36us | 35.27us | 1.007 | [0.933, 1.087] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 181.84ms | 181.44ms | 0.998 | [0.984, 1.013] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 181.75ms | 181.36ms | 0.998 | [0.984, 1.013] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 31.01us | 28.15us | 1.175 | [0.821, 1.681] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 335.33ms | 334.91ms | 1.000 | [0.987, 1.013] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 335.22ms | 334.80ms | 1.000 | [0.987, 1.013] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 35.20us | 35.27us | 1.078 | [0.833, 1.395] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 51.27ms | 50.91ms | 1.015 | [0.992, 1.038] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 51.21ms | 50.83ms | 1.015 | [0.992, 1.038] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 20.53us | 20.46us | 1.016 | [0.855, 1.207] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 108.59ms | 109.05ms | 0.995 | [0.895, 1.107] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 102.41ms | 103.03ms | 0.994 | [0.888, 1.113] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.73ms | 1.67ms | 1.028 | [0.945, 1.117] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 445.22ms | 443.57ms | 0.995 | [0.963, 1.029] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 442.32ms | 440.59ms | 0.996 | [0.963, 1.029] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.18ms | 2.24ms | 0.981 | [0.863, 1.115] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.528s | 1.548s | 0.988 | [0.962, 1.014] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.526s | 1.545s | 0.988 | [0.962, 1.014] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.20ms | 2.17ms | 0.965 | [0.518, 1.799] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.467s | 2.494s | 0.990 | [0.970, 1.010] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.464s | 2.491s | 0.990 | [0.970, 1.010] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.78ms | 1.73ms | 0.985 | [0.912, 1.064] | ~ (noise) |

Sum of medians (total, common cases): A = 10.230s, B = 10.276s, saved = -46359822.0ns (0.995x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cc/b0/d3/L9/p64`: A 1.13e-15 -> B 1.70e-15 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 14.8 MiB | 15.1 MiB | 1.022 | 3.1% / 3.3% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.1 MiB | 12.0 MiB | 0.993 | 1.6% / 2.1% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.1 MiB | 12.1 MiB | 0.994 | 2.2% / 1.4% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 13.1 MiB | 13.1 MiB | 0.998 | 2.9% / 3.7% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.9 MiB | 24.9 MiB | 0.997 | 1.7% / 4.1% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.0 MiB | 12.0 MiB | 0.998 | 1.4% / 0.5% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.0 MiB | 12.0 MiB | 0.999 | 3.0% / 1.5% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 11.9 MiB | 11.9 MiB | 1.005 | 2.2% / 1.9% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 19.7 MiB | 19.6 MiB | 0.997 | 2.2% / 3.0% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.2 MiB | 13.0 MiB | 0.991 | 2.7% / 3.3% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.9 MiB | 1.016 | 2.8% / 4.7% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 13.2 MiB | 1.001 | 6.4% / 5.1% | ~ |

Peak memory (common cases): geomean B/A = 1.001; largest B/A: `interp/sg/cc/b0/d2/L12/p64` 14.8 MiB -> 15.1 MiB (1.022x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.4 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 44, 44, 44, 44, 44 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 219s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
