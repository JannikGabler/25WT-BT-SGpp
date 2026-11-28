A/B `e2e`: A=`WP-88-before` B=`WP-88-after`, 16 thread(s), 8 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d5/L12 | total | 197.49ms | 198.62ms | 0.996 | [0.992, 0.999] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | operator | 196.56ms | 197.72ms | 0.996 | [0.992, 0.999] | SLOWER |
| quad/dsg/equi-exp/b0/d5/L12 | generation | 545.67us | 533.83us | 1.016 | [0.855, 1.206] | ~ (noise) |
| interp/sg/cheb1/b0/d3/L6/p64 | total | 22.88ms | 22.20ms | 1.030 | [1.023, 1.038] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | operator | 22.81ms | 22.12ms | 1.031 | [1.023, 1.038] | faster |
| interp/sg/cheb1/b0/d3/L6/p64 | generation | 35.97us | 41.00us | 0.887 | [0.696, 1.129] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | total | 109.95ms | 108.09ms | 1.006 | [0.967, 1.047] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | operator | 109.86ms | 108.01ms | 1.006 | [0.966, 1.047] | ~ (noise) |
| interp/sg/cheb1/b0/d5/L4/p64 | generation | 41.10us | 42.64us | 0.976 | [0.824, 1.157] | ~ (noise) |
| interp/sg/equi/b0/d3/L5/p64 | total | 8.33ms | 7.48ms | 1.116 | [1.072, 1.162] | faster |
| interp/sg/equi/b0/d3/L5/p64 | operator | 8.26ms | 7.41ms | 1.117 | [1.073, 1.164] | faster |
| interp/sg/equi/b0/d3/L5/p64 | generation | 32.16us | 32.23us | 0.971 | [0.877, 1.076] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | total | 622.80ms | 621.61ms | 1.002 | [0.999, 1.004] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | operator | 618.69ms | 618.10ms | 1.001 | [0.999, 1.004] | ~ (noise) |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | generation | 2.13ms | 1.99ms | 1.182 | [0.747, 1.871] | ~ (noise) |

Sum of medians (total, common cases, group suite): A = 338.65ms, B = 336.40ms, saved = 2.26ms (1.007x)
Sum of medians (total, common cases, group expensive/ cached): A = 622.80ms, B = 621.61ms, saved = 1.18ms (1.002x)
Result sanity (rel. error vs exact): worst growth in `quad/dsg/equi-exp/b0/d5/L12`: A 3.50e-12 -> B 3.50e-12 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/dsg/equi-exp/b0/d5/L12 | 18.8 MiB | 18.7 MiB | 0.995 | 6.7% / 1.9% | ~ |
| interp/sg/cheb1/b0/d3/L6/p64 | 12.3 MiB | 12.4 MiB | 1.011 | 3.5% / 1.9% | ~ |
| interp/sg/cheb1/b0/d5/L4/p64 | 12.3 MiB | 12.5 MiB | 1.013 | 2.8% / 2.6% | ~ |
| interp/sg/equi/b0/d3/L5/p64 | 12.4 MiB | 12.4 MiB | 1.004 | 2.5% / 2.7% | ~ |
| expensive/quad/sg/cc-exp/b1/d10/L4/c100us-cached | 21.0 MiB | 20.9 MiB | 0.995 | 5.5% / 2.4% | ~ |

Peak memory (common cases): geomean B/A = 1.004; largest B/A: `interp/sg/cheb1/b0/d5/L4/p64` 12.3 MiB -> 12.5 MiB (1.013x); 0 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.0 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.4%, 0.2%, 0.2%, 0.3%, 0.2%, 0.2%, 0.2%, 0.2%
Round durations (s): 5, 5, 5, 5, 5, 5, 5, 5 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 40s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
