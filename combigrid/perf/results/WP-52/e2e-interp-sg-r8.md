A/B `e2e`: A=`WP-52-before` B=`WP-52-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | total | 129.25ms | 128.94ms | 0.997 | [0.985, 1.010] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | operator | 128.99ms | 128.68ms | 0.997 | [0.985, 1.010] | ~ (noise) |
| interp/sg/cc/b0/d2/L12/p64 | generation | 207.53us | 206.55us | 1.003 | [0.945, 1.064] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | total | 187.63ms | 186.60ms | 1.023 | [0.987, 1.060] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | operator | 187.49ms | 186.45ms | 1.023 | [0.987, 1.060] | ~ (noise) |
| interp/sg/cc/b0/d3/L9/p64 | generation | 73.37us | 74.31us | 0.963 | [0.894, 1.037] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | total | 1.128s | 1.126s | 1.002 | [0.999, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | operator | 1.127s | 1.125s | 1.002 | [0.999, 1.006] | ~ (noise) |
| interp/sg/cc/b0/d5/L6/p64 | generation | 173.38us | 154.10us | 1.064 | [0.849, 1.334] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | total | 1.003s | 1.009s | 0.995 | [0.987, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | operator | 1.003s | 1.009s | 0.995 | [0.987, 1.004] | ~ (noise) |
| interp/sg/cc/b0/d7/L4/p16 | generation | 106.40us | 120.51us | 0.951 | [0.718, 1.260] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.599s | 1.615s | 0.990 | [0.984, 0.996] | SLOWER |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.599s | 1.615s | 0.990 | [0.984, 0.996] | SLOWER |
| interp/sg/cc/b0/d10/L2/p8 | generation | 43.23us | 49.80us | 0.927 | [0.774, 1.112] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 34.64ms | 34.45ms | 0.966 | [0.904, 1.033] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 34.57ms | 34.36ms | 0.966 | [0.903, 1.033] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 26.71us | 26.96us | 0.992 | [0.779, 1.262] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 116.42ms | 116.68ms | 0.979 | [0.935, 1.026] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 116.29ms | 116.57ms | 0.979 | [0.935, 1.026] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 39.91us | 38.55us | 1.172 | [0.937, 1.465] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 11.95ms | 11.99ms | 0.987 | [0.955, 1.020] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | operator | 11.88ms | 11.92ms | 0.987 | [0.955, 1.020] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | generation | 22.28us | 22.21us | 1.069 | [0.862, 1.326] | ~ (noise) |

Sum of medians (total, common cases): A = 4.210s, B = 4.228s, saved = -18596319.0ns (0.996x)
Result sanity (rel. error vs exact): worst growth in `interp/sg/cheb1/b0/d5/L4/p64`: A 2.28e-04 -> B 2.28e-04 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interp/sg/cc/b0/d2/L12/p64 | 16.1 MiB | 15.9 MiB | 0.986 | 3.5% / 2.5% | ~ |
| interp/sg/cc/b0/d3/L9/p64 | 12.3 MiB | 12.3 MiB | 0.998 | 1.2% / 2.0% | ~ |
| interp/sg/cc/b0/d5/L6/p64 | 12.3 MiB | 12.2 MiB | 0.996 | 2.3% / 1.8% | ~ |
| interp/sg/cc/b0/d7/L4/p16 | 12.2 MiB | 12.2 MiB | 0.995 | 1.1% / 2.5% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 12.1 MiB | 12.1 MiB | 0.999 | 2.3% / 2.0% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.2 MiB | 12.2 MiB | 1.002 | 2.8% / 2.4% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.3 MiB | 12.2 MiB | 0.995 | 1.7% / 3.2% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.3 MiB | 12.1 MiB | 0.985 | 2.3% / 2.7% | ~ |

Peak memory (common cases): geomean B/A = 0.994; largest B/A: `interp/sg/cheb1/b0/d3/L6/p64` 12.2 MiB -> 12.2 MiB (1.002x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 19, 19, 19, 19, 19, 19, 19, 19 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 150s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
