A/B `e2e`: A=`WP-16-old` B=`WP-16-new`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-sin/b0/d7/L7 | total | 2.413s | 2.412s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.411s | 2.409s | 1.000 | [0.992, 1.008] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.93ms | 1.89ms | 1.014 | [0.913, 1.127] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 7.032s | 7.031s | 0.999 | [0.964, 1.036] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.942s | 6.938s | 0.999 | [0.963, 1.037] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 50.50ms | 51.11ms | 1.035 | [0.913, 1.173] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 1.989s | 1.988s | 1.001 | [0.989, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | operator | 1.989s | 1.988s | 1.001 | [0.989, 1.014] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | generation | 34.50us | 33.38us | 1.080 | [0.873, 1.337] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.452s | 2.438s | 1.015 | [0.979, 1.053] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.449s | 2.436s | 1.015 | [0.979, 1.053] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.74ms | 1.73ms | 1.011 | [0.911, 1.122] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 2.074s | 2.070s | 1.014 | [0.980, 1.049] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.07ms | 2.12ms | 0.981 | [0.905, 1.062] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 2.072s | 2.068s | 1.014 | [0.980, 1.049] | ~ (noise) |

Sum of medians (total, common cases): A = 15.961s, B = 15.940s, saved = 21.14ms (1.001x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b1/d20/L6`: A 6.12e-09 -> B 6.59e-09 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-sin/b0/d7/L7 | 14.6 MiB | 14.6 MiB | 0.995 | 3.7% / 3.8% | ~ |
| quad/dsg/equi-exp/b1/d20/L6 | 327.7 MiB | 328.8 MiB | 1.004 | 2.0% / 0.9% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.1 MiB | 24.1 MiB | 0.999 | 5.7% / 6.9% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.5 MiB | 13.3 MiB | 0.985 | 4.9% / 4.1% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.4 MiB | 12.2 MiB | 0.988 | 3.7% / 1.7% | ~ |

Peak memory (common cases): geomean B/A = 0.994; largest B/A: `quad/dsg/equi-exp/b1/d20/L6` 327.7 MiB -> 328.8 MiB (1.004x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.6 MiB, B 10.5 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.2%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 65, 65, 66, 66, 65 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 327s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
