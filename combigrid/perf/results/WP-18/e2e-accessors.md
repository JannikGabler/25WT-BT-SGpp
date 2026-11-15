A/B `e2e`: A=`WP-18-twotables` B=`WP-18-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | total | 127.61ms | 130.36ms | 0.937 | [0.840, 1.046] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 127.26ms | 130.07ms | 0.937 | [0.840, 1.046] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 235.36us | 222.79us | 1.061 | [0.954, 1.180] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 185.73ms | 199.06ms | 0.928 | [0.858, 1.002] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 185.59ms | 198.91ms | 0.928 | [0.858, 1.002] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 91.14us | 93.03us | 0.949 | [0.823, 1.094] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.150s | 1.153s | 0.999 | [0.978, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.150s | 1.153s | 0.999 | [0.978, 1.021] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 77.66us | 81.16us | 1.002 | [0.799, 1.257] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.013s | 1.014s | 0.992 | [0.971, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.013s | 1.014s | 0.992 | [0.971, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 72.84us | 71.52us | 1.062 | [0.927, 1.217] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.624s | 1.614s | 1.000 | [0.987, 1.012] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.624s | 1.614s | 1.000 | [0.987, 1.013] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 50.77us | 49.10us | 1.058 | [0.980, 1.142] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 38.06ms | 34.38ms | 1.100 | [0.731, 1.654] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 37.98ms | 34.29ms | 1.100 | [0.731, 1.656] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 35.69us | 35.27us | 0.973 | [0.680, 1.392] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 117.22ms | 117.16ms | 0.983 | [0.812, 1.190] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 117.13ms | 117.06ms | 0.983 | [0.812, 1.190] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 40.23us | 40.23us | 0.966 | [0.751, 1.244] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.81ms | 11.93ms | 0.979 | [0.943, 1.017] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.74ms | 11.85ms | 0.978 | [0.942, 1.016] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 32.90us | 26.33us | 1.222 | [1.022, 1.461] | faster |
| interp/dsg/cc/b0/d2/L12/p64 | total | 52.05ms | 52.25ms | 0.893 | [0.719, 1.108] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | operator | 51.68ms | 51.89ms | 0.892 | [0.718, 1.108] | ~ (noise) |
| interp/dsg/cc/b0/d2/L12/p64 | generation | 220.70us | 217.48us | 1.010 | [0.995, 1.024] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | total | 43.76ms | 43.25ms | 1.009 | [0.995, 1.023] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | operator | 43.59ms | 43.09ms | 1.010 | [0.996, 1.024] | ~ (noise) |
| interp/dsg/cc/b0/d3/L9/p64 | generation | 61.53us | 61.60us | 0.998 | [0.982, 1.014] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | total | 174.77ms | 173.84ms | 1.029 | [0.972, 1.089] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | operator | 174.67ms | 173.74ms | 1.029 | [0.972, 1.089] | ~ (noise) |
| interp/dsg/cc/b0/d5/L6/p64 | generation | 52.87us | 49.03us | 1.057 | [0.978, 1.142] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | total | 151.42ms | 151.14ms | 0.995 | [0.949, 1.044] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | operator | 151.34ms | 151.04ms | 0.995 | [0.949, 1.044] | ~ (noise) |
| interp/dsg/cc/b0/d7/L4/p16 | generation | 46.02us | 49.03us | 0.966 | [0.867, 1.077] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | total | 403.98ms | 401.39ms | 1.059 | [0.930, 1.206] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | operator | 403.91ms | 401.32ms | 1.059 | [0.930, 1.207] | ~ (noise) |
| interp/dsg/cc/b0/d10/L2/p8 | generation | 35.76us | 38.55us | 0.962 | [0.874, 1.059] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | total | 6.54ms | 6.80ms | 0.985 | [0.878, 1.105] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | operator | 6.47ms | 6.74ms | 0.984 | [0.877, 1.105] | ~ (noise) |
| interp/dsg/equi/b0/d3/L5/p64 | generation | 25.77us | 26.26us | 0.988 | [0.961, 1.016] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | total | 125.99ms | 110.48ms | 1.036 | [0.817, 1.315] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 119.09ms | 103.66ms | 1.039 | [0.807, 1.336] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 2.37ms | 2.36ms | 1.389 | [0.566, 3.405] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 368.14ms | 360.85ms | 1.022 | [0.857, 1.218] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 367.62ms | 360.31ms | 1.022 | [0.857, 1.219] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 89.19us | 88.35us | 0.982 | [0.825, 1.169] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.354s | 1.290s | 1.046 | [0.931, 1.176] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.352s | 1.286s | 1.047 | [0.931, 1.178] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.20ms | 2.30ms | 0.828 | [0.486, 1.408] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 1.920s | 1.865s | 1.024 | [0.999, 1.051] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 1.918s | 1.863s | 1.025 | [0.999, 1.051] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.61ms | 1.66ms | 0.980 | [0.833, 1.153] | ~ (noise) |
| interplin/dsg/equi/b1/d2/L16/p16384 | total | 179.32ms | 178.50ms | 1.005 | [1.001, 1.009] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | operator | 174.10ms | 173.33ms | 1.004 | [1.001, 1.008] | faster |
| interplin/dsg/equi/b1/d2/L16/p16384 | generation | 1.25ms | 1.24ms | 0.995 | [0.927, 1.068] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | total | 190.34ms | 189.60ms | 1.004 | [0.720, 1.402] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | operator | 189.79ms | 189.06ms | 1.004 | [0.719, 1.403] | ~ (noise) |
| interplin/dsg/equi/b1/d5/L8/p1024 | generation | 93.03us | 83.46us | 1.054 | [0.952, 1.168] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | total | 378.46ms | 376.23ms | 1.040 | [0.931, 1.162] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | operator | 376.52ms | 373.49ms | 1.040 | [0.929, 1.164] | ~ (noise) |
| interplin/dsg/equi/b1/d8/L5/p256 | generation | 2.13ms | 2.12ms | 0.988 | [0.568, 1.718] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | total | 464.61ms | 435.05ms | 1.012 | [0.907, 1.128] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | operator | 463.04ms | 433.54ms | 1.012 | [0.907, 1.131] | ~ (noise) |
| interplin/dsg/equi/b1/d10/L5/p32 | generation | 1.41ms | 1.42ms | 0.838 | [0.560, 1.255] | ~ (noise) |

Sum of medians (total, common cases): A = 10.082s, B = 9.908s, saved = 173.63ms (1.018x)
Result sanity (rel. error vs exact): worst growth in `interplin/sg/equi/b1/d2/L16/p16384`: A 1.92e-10 -> B 1.92e-10 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 14.5 MiB | 14.7 MiB | 1.009 | 2.7% / 4.0% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.4 MiB | 12.4 MiB | 1.001 | 1.1% / 1.3% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.4 MiB | 12.3 MiB | 0.997 | 3.0% / 1.9% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.2 MiB | 12.3 MiB | 1.012 | 2.4% / 1.4% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.4 MiB | 12.4 MiB | 1.002 | 2.4% / 1.9% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.3 MiB | 12.4 MiB | 1.006 | 0.8% / 2.0% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.4 MiB | 12.3 MiB | 0.998 | 3.3% / 3.1% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.4 MiB | 1.003 | 1.1% / 1.5% | ~ |
| interp/dsg/cc/b0/d2/L12/p64 | 15.3 MiB | 15.5 MiB | 1.014 | 1.3% / 2.5% | ~ |
| interp/dsg/cc/b0/d3/L9/p64 | 12.6 MiB | 12.8 MiB | 1.013 | 2.0% / 1.6% | ~ |
| interp/dsg/cc/b0/d5/L6/p64 | 12.5 MiB | 12.5 MiB | 1.004 | 1.7% / 2.4% | ~ |
| interp/dsg/cc/b0/d7/L4/p16 | 12.6 MiB | 12.6 MiB | 1.003 | 1.2% / 0.8% | ~ |
| interp/dsg/cc/b0/d10/L2/p8 | 12.3 MiB | 12.4 MiB | 1.003 | 2.3% / 1.8% | ~ |
| interp/dsg/equi/b0/d3/L5/p64 | 12.4 MiB | 12.3 MiB | 0.994 | 1.5% / 2.3% | ~ |
| interplin/sg/equi/b1/d2/L16/p16384 | 20.7 MiB | 20.8 MiB | 1.005 | 9.5% / 6.1% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 12.8 MiB | 12.7 MiB | 0.994 | 4.4% / 2.3% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.7 MiB | 0.999 | 2.7% / 1.6% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 12.7 MiB | 12.8 MiB | 1.008 | 3.4% / 3.5% | ~ |
| interplin/dsg/equi/b1/d2/L16/p16384 | 21.3 MiB | 21.5 MiB | 1.011 | 1.8% / 6.0% | ~ |
| interplin/dsg/equi/b1/d5/L8/p1024 | 13.1 MiB | 13.0 MiB | 0.990 | 4.8% / 3.4% | ~ |
| interplin/dsg/equi/b1/d8/L5/p256 | 12.7 MiB | 12.8 MiB | 1.006 | 3.1% / 3.8% | ~ |
| interplin/dsg/equi/b1/d10/L5/p32 | 15.3 MiB | 15.4 MiB | 1.006 | 3.6% / 3.6% | ~ |

Peak memory (common cases): geomean B/A = 1.003; largest B/A: `interp/dsg/cc/b0/d2/L12/p64` 15.3 MiB -> 15.5 MiB (1.014x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.3 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.6%, 1.9%, 0.7%, 0.6%, 0.6%
Round durations (s): 46, 56, 46, 45, 46 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 238s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
