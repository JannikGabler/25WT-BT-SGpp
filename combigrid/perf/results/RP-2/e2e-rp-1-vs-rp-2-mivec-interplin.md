A/B `e2e`: A=`rp-1` B=`rp-2`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | total | 110.76ms | 109.98ms | 0.986 | [0.933, 1.041] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | operator | 104.83ms | 104.18ms | 0.985 | [0.930, 1.042] | ~ (noise) |
| interplin/sg/equi/b1/d2/L16/p16384 | generation | 1.60ms | 1.64ms | 0.998 | [0.918, 1.085] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | total | 444.78ms | 443.55ms | 0.997 | [0.972, 1.023] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | operator | 440.76ms | 440.51ms | 0.995 | [0.969, 1.022] | ~ (noise) |
| interplin/sg/equi/b1/d5/L8/p1024 | generation | 2.80ms | 2.19ms | 1.304 | [0.982, 1.731] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | total | 1.532s | 1.610s | 0.984 | [0.900, 1.076] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | operator | 1.528s | 1.605s | 0.984 | [0.900, 1.076] | ~ (noise) |
| interplin/sg/equi/b1/d8/L5/p256 | generation | 2.20ms | 2.22ms | 0.976 | [0.645, 1.477] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.489s | 2.517s | 0.981 | [0.957, 1.006] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.487s | 2.515s | 0.982 | [0.957, 1.006] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.73ms | 1.76ms | 0.968 | [0.896, 1.046] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 739.93ms | 742.89ms | 0.996 | [0.944, 1.051] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 734.14ms | 737.66ms | 0.996 | [0.944, 1.050] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 5.06ms | 4.58ms | 1.071 | [0.941, 1.219] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 1.993s | 1.913s | 1.046 | [1.025, 1.067] | faster |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 479.26ms | 481.10ms | 0.995 | [0.969, 1.021] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 1.512s | 1.437s | 1.063 | [1.033, 1.095] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 2.099s | 1.866s | 1.130 | [1.120, 1.140] | faster |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.49ms | 23.96ms | 1.020 | [0.970, 1.073] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.074s | 1.841s | 1.131 | [1.121, 1.142] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 2.094s | 1.850s | 1.131 | [1.107, 1.154] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.15ms | 2.09ms | 1.183 | [0.765, 1.829] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 2.090s | 1.848s | 1.130 | [1.107, 1.154] | faster |

Sum of medians (total, common cases): A = 11.503s, B = 11.052s, saved = 451.37ms (1.041x)
Result sanity (rel. error vs exact): worst growth in `mivec/sg/cc-exp/b1/d5/S16/n1000`: A 1.35e-13 -> B 1.39e-13 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| interplin/sg/equi/b1/d2/L16/p16384 | 19.4 MiB | 19.6 MiB | 1.007 | 3.1% / 1.3% | ~ |
| interplin/sg/equi/b1/d5/L8/p1024 | 13.1 MiB | 13.2 MiB | 1.007 | 4.5% / 2.2% | ~ |
| interplin/sg/equi/b1/d8/L5/p256 | 13.0 MiB | 12.9 MiB | 0.999 | 2.5% / 2.8% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.2 MiB | 13.4 MiB | 1.017 | 5.0% / 6.1% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.1 MiB | 16.2 MiB | 1.001 | 2.5% / 2.4% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.5 MiB | 17.7 MiB | 1.007 | 1.4% / 1.8% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.6 MiB | 12.7 MiB | 1.004 | 2.6% / 4.2% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.4 MiB | 1.018 | 4.8% / 1.7% | ~ |

Peak memory (common cases): geomean B/A = 1.007; largest B/A: `mivec/sg/cc-exp/b1/d20/S7/n300` 12.2 MiB -> 12.4 MiB (1.018x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.2 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.4%, 0.4%, 0.5%, 0.5%
Round durations (s): 47, 47, 47, 47, 47 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 234s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
