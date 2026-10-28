A/B `e2e`: A=`baseline` B=`WP-14-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b1/d20/L5 | total | 1.110s | 235.37ms | 4.797 | [4.604, 4.998] | faster |
| quad/sg/equi-exp/b1/d20/L5 | operator | 1.082s | 208.62ms | 5.267 | [4.994, 5.555] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 18.73ms | 18.70ms | 1.174 | [0.843, 1.634] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 3.521s | 2.414s | 1.460 | [1.448, 1.472] | faster |
| quad/sg/equi-sin/b0/d7/L7 | operator | 3.519s | 2.411s | 1.460 | [1.448, 1.472] | faster |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.83ms | 1.85ms | 1.019 | [0.918, 1.133] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.409s | 1.231s | 1.147 | [1.096, 1.200] | faster |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.317s | 1.144s | 1.157 | [1.090, 1.228] | faster |
| quad/sg/cc-uq/b0/d2/L20 | generation | 81.77ms | 81.89ms | 0.998 | [0.864, 1.154] | ~ (noise) |
| quad/dsg/equi-exp/b1/d20/L6 | total | 6.762s | 7.031s | 0.962 | [0.959, 0.966] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | operator | 6.657s | 6.937s | 0.960 | [0.956, 0.964] | SLOWER |
| quad/dsg/equi-exp/b1/d20/L6 | generation | 60.32ms | 50.29ms | 1.235 | [1.096, 1.392] | faster |
| quad/dsg/cc-uq/b0/d10/L7 | total | 6.980s | 7.006s | 0.995 | [0.989, 1.001] | ~ (noise) |
| quad/dsg/cc-uq/b0/d10/L7 | operator | 6.971s | 6.998s | 0.995 | [0.990, 0.999] | SLOWER |
| quad/dsg/cc-uq/b0/d10/L7 | generation | 5.18ms | 3.67ms | 1.285 | [0.841, 1.964] | ~ (noise) |
| interp/sg/cc/b0/d10/L2/p8 | total | 2.226s | 1.989s | 1.124 | [1.114, 1.134] | faster |
| interp/sg/cc/b0/d10/L2/p8 | operator | 2.226s | 1.989s | 1.124 | [1.114, 1.134] | faster |
| interp/sg/cc/b0/d10/L2/p8 | generation | 38.55us | 33.87us | 1.769 | [0.565, 5.544] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 53.30ms | 51.22ms | 1.039 | [1.031, 1.047] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 53.14ms | 51.15ms | 1.038 | [1.031, 1.045] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 22.00us | 21.37us | 1.680 | [0.393, 7.183] | ~ (noise) |
| interplin/sg/equi/b1/d10/L5/p32 | total | 2.678s | 2.439s | 1.103 | [1.078, 1.128] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | operator | 2.675s | 2.436s | 1.103 | [1.078, 1.128] | faster |
| interplin/sg/equi/b1/d10/L5/p32 | generation | 1.84ms | 1.76ms | 1.042 | [0.925, 1.174] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 1.915s | 2.073s | 0.922 | [0.917, 0.927] | SLOWER |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 9.48ms | 2.07ms | 4.637 | [4.282, 5.021] | faster |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 1.905s | 2.071s | 0.918 | [0.913, 0.923] | SLOWER |

Sum of medians (total, common cases): A = 26.654s, B = 24.470s, saved = 2.184s (1.089x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/equi-sin/b0/d7/L7`: A 7.48e-12 -> B 1.38e-11 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b1/d20/L5 | 58.1 MiB | 57.8 MiB | 0.995 | 5.8% / 13.6% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.4 MiB | 14.7 MiB | 1.018 | 5.2% / 4.0% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 805.9 MiB | 1075.8 MiB | 1.335 | 12.5% / 1.6% | MEM+ |
| quad/dsg/equi-exp/b1/d20/L6 | 327.2 MiB | 327.8 MiB | 1.002 | 2.6% / 2.3% | ~ |
| quad/dsg/cc-uq/b0/d10/L7 | 37.1 MiB | 37.3 MiB | 1.007 | 3.7% / 3.4% | ~ |
| interp/sg/cc/b0/d10/L2/p8 | 24.5 MiB | 24.9 MiB | 1.015 | 3.9% / 3.9% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.0 MiB | 11.7 MiB | 0.982 | 2.9% / 2.6% | ~ |
| interplin/sg/equi/b1/d10/L5/p32 | 13.3 MiB | 13.1 MiB | 0.988 | 7.2% / 6.8% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.2 MiB | 1.001 | 3.2% / 1.7% | ~ |

Peak memory (common cases): geomean B/A = 1.034; largest B/A: `quad/sg/cc-uq/b0/d2/L20` 805.9 MiB -> 1075.8 MiB (1.335x); 1 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.2 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.3%, 0.2%, 0.2%, 0.2%, 0.1%
Round durations (s): 105, 105, 105, 104, 105 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 523s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
