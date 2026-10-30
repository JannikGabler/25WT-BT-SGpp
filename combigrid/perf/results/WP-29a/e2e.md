A/B `e2e`: A=`WP-29a-before` B=`WP-29a-after`, 16 thread(s), 5 rounds (interleaved processes) x 1 sample(s), speedup = median(A)/median(B) (> 1: B faster) as geometric mean of the per-round ratios, 95% t-CI with the spread bounded below by the noise model (baseline/noise_e2e.json)

| case | metric | A | B | speedup | 95% CI | verdict |
|---|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | total | 414.44ms | 397.42ms | 1.017 | [0.912, 1.135] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | operator | 345.06ms | 330.77ms | 1.021 | [0.914, 1.141] | ~ (noise) |
| quad/sg/equi-exp/b0/d2/L20 | generation | 32.20ms | 30.03ms | 1.037 | [0.932, 1.155] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | total | 252.46ms | 252.92ms | 1.011 | [0.976, 1.047] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | operator | 250.04ms | 250.10ms | 1.007 | [0.980, 1.034] | ~ (noise) |
| quad/sg/equi-exp/b0/d5/L10 | generation | 1.98ms | 2.13ms | 1.361 | [0.505, 3.664] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | total | 501.51ms | 502.60ms | 0.991 | [0.978, 1.004] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | operator | 499.06ms | 499.83ms | 0.995 | [0.985, 1.006] | ~ (noise) |
| quad/sg/equi-exp/b0/d7/L7 | generation | 1.80ms | 1.90ms | 0.659 | [0.237, 1.830] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | total | 934.76ms | 928.58ms | 1.001 | [0.977, 1.026] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | operator | 932.61ms | 926.24ms | 1.001 | [0.977, 1.025] | ~ (noise) |
| quad/sg/equi-exp/b0/d10/L4 | generation | 2.12ms | 2.11ms | 1.012 | [0.850, 1.205] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | total | 304.69ms | 302.45ms | 1.015 | [0.989, 1.041] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | operator | 295.58ms | 293.77ms | 1.018 | [0.992, 1.046] | ~ (noise) |
| quad/sg/equi-exp/b1/d10/L7 | generation | 4.47ms | 5.28ms | 0.852 | [0.705, 1.030] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | total | 520.72ms | 517.75ms | 1.012 | [0.995, 1.030] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | operator | 498.15ms | 494.31ms | 1.012 | [0.991, 1.033] | ~ (noise) |
| quad/sg/equi-exp/b1/d15/L6 | generation | 15.60ms | 15.91ms | 1.032 | [0.813, 1.310] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | total | 237.12ms | 236.43ms | 1.008 | [0.979, 1.038] | ~ (noise) |
| quad/sg/equi-exp/b1/d20/L5 | operator | 208.49ms | 206.23ms | 1.010 | [1.000, 1.019] | faster |
| quad/sg/equi-exp/b1/d20/L5 | generation | 18.25ms | 20.29ms | 1.014 | [0.751, 1.371] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | total | 2.419s | 2.412s | 1.004 | [0.996, 1.012] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | operator | 2.416s | 2.410s | 1.004 | [0.996, 1.012] | ~ (noise) |
| quad/sg/equi-sin/b0/d7/L7 | generation | 1.82ms | 1.84ms | 0.992 | [0.893, 1.102] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | total | 1.199s | 1.208s | 0.992 | [0.948, 1.038] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | operator | 1.118s | 1.133s | 0.990 | [0.941, 1.040] | ~ (noise) |
| quad/sg/cc-uq/b0/d2/L20 | generation | 85.45ms | 79.74ms | 1.043 | [0.903, 1.205] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | total | 88.07ms | 87.01ms | 0.980 | [0.904, 1.062] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | operator | 85.70ms | 84.24ms | 0.982 | [0.904, 1.067] | ~ (noise) |
| quad/sg/cc-uq/b0/d3/L14 | generation | 2.04ms | 2.05ms | 0.942 | [0.846, 1.048] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | total | 326.78ms | 322.33ms | 1.009 | [0.989, 1.029] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | operator | 324.00ms | 319.01ms | 1.011 | [0.992, 1.029] | ~ (noise) |
| quad/sg/cc-uq/b0/d5/L10 | generation | 2.10ms | 2.47ms | 0.822 | [0.629, 1.073] | ~ (noise) |
| quad/sg/cc-uq/b0/d7/L7 | total | 703.25ms | 692.89ms | 1.020 | [1.008, 1.033] | faster |
| quad/sg/cc-uq/b0/d7/L7 | operator | 700.53ms | 690.38ms | 1.020 | [1.008, 1.033] | faster |
| quad/sg/cc-uq/b0/d7/L7 | generation | 2.00ms | 1.95ms | 1.012 | [0.939, 1.091] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | total | 1.317s | 1.304s | 1.009 | [1.000, 1.018] | ~ (noise) |
| quad/sg/cc-uq/b0/d10/L4 | operator | 1.313s | 1.302s | 1.008 | [1.001, 1.016] | faster |
| quad/sg/cc-uq/b0/d10/L4 | generation | 2.36ms | 2.36ms | 1.096 | [0.558, 2.155] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | total | 741.03ms | 768.78ms | 0.987 | [0.936, 1.041] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | operator | 737.11ms | 765.03ms | 0.987 | [0.936, 1.042] | ~ (noise) |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | generation | 3.03ms | 2.98ms | 1.015 | [0.996, 1.034] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | total | 492.59ms | 493.04ms | 0.996 | [0.913, 1.087] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | operator | 484.15ms | 482.30ms | 0.999 | [0.917, 1.088] | ~ (noise) |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | generation | 7.55ms | 7.61ms | 0.869 | [0.673, 1.121] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | total | 27.15ms | 27.36ms | 0.975 | [0.888, 1.072] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | operator | 24.47ms | 24.65ms | 0.999 | [0.947, 1.053] | ~ (noise) |
| mivec/sg/cc-exp/b1/d15/S10/n300 | generation | 2.43ms | 2.46ms | 0.833 | [0.520, 1.334] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | total | 3.15ms | 3.14ms | 1.042 | [0.941, 1.153] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | operator | 2.07ms | 2.10ms | 1.000 | [0.923, 1.083] | ~ (noise) |
| mivec/sg/cc-exp/b1/d20/S7/n300 | generation | 858.97us | 810.22us | 1.145 | [0.845, 1.552] | ~ (noise) |

Sum of medians (total, common cases): A = 10.482s, B = 10.457s, saved = 24.95ms (1.002x)
Result sanity (rel. error vs exact): worst growth in `quad/sg/cc-uq/b0/d7/L7`: A 2.57e-12 -> B 1.08e-11 (the binding check is `refcheck`)

Peak memory (max RSS of each benchmark process, measured outside it by GNU time; median over the rounds; B/A > 1: B needs more; spread = max/min - 1 over the rounds; flag MEM+/MEM- if |B/A - 1| > 3% and the rounds of A and B do not overlap):

| case | A | B | B/A | spread A / B | flag |
|---|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 619.7 MiB | 619.7 MiB | 1.000 | 0.0% / 0.1% | ~ |
| quad/sg/equi-exp/b0/d5/L10 | 15.7 MiB | 15.7 MiB | 0.998 | 9.1% / 2.7% | ~ |
| quad/sg/equi-exp/b0/d7/L7 | 14.3 MiB | 14.8 MiB | 1.033 | 1.7% / 5.9% | MEM+ |
| quad/sg/equi-exp/b0/d10/L4 | 12.8 MiB | 12.7 MiB | 0.994 | 0.9% / 4.6% | ~ |
| quad/sg/equi-exp/b1/d10/L7 | 25.9 MiB | 25.5 MiB | 0.984 | 4.9% / 2.3% | ~ |
| quad/sg/equi-exp/b1/d15/L6 | 52.9 MiB | 53.1 MiB | 1.003 | 3.9% / 8.0% | ~ |
| quad/sg/equi-exp/b1/d20/L5 | 58.2 MiB | 59.1 MiB | 1.016 | 7.7% / 4.2% | ~ |
| quad/sg/equi-sin/b0/d7/L7 | 14.4 MiB | 14.5 MiB | 1.005 | 3.5% / 3.5% | ~ |
| quad/sg/cc-uq/b0/d2/L20 | 1081.0 MiB | 1064.4 MiB | 0.985 | 2.9% / 6.1% | ~ |
| quad/sg/cc-uq/b0/d3/L14 | 40.0 MiB | 41.4 MiB | 1.035 | 4.5% / 7.1% | ~ |
| quad/sg/cc-uq/b0/d5/L10 | 17.2 MiB | 17.4 MiB | 1.007 | 2.0% / 2.0% | ~ |
| quad/sg/cc-uq/b0/d7/L7 | 14.7 MiB | 14.6 MiB | 0.998 | 2.5% / 4.3% | ~ |
| quad/sg/cc-uq/b0/d10/L4 | 12.8 MiB | 12.9 MiB | 1.002 | 3.1% / 2.4% | ~ |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 16.1 MiB | 16.0 MiB | 0.993 | 3.0% / 3.6% | ~ |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 17.1 MiB | 17.0 MiB | 0.990 | 2.2% / 5.7% | ~ |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 12.8 MiB | 12.7 MiB | 0.991 | 2.2% / 3.0% | ~ |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 12.2 MiB | 12.1 MiB | 0.995 | 2.3% / 2.1% | ~ |

Peak memory (common cases): geomean B/A = 1.002; largest B/A: `quad/sg/cc-uq/b0/d3/L14` 40.0 MiB -> 41.4 MiB (1.035x); 1 MEM+, 0 MEM-
Process floor (`perf_e2e --list`, libraries only; case peaks read as floor + case memory): A 10.1 MiB, B 10.3 MiB
Foreign CPU load per round (other processes, share of all CPUs): 0.7%, 0.8%, 0.6%, 0.5%, 0.6%
Round durations (s): 46, 46, 46, 46, 46 (rounds getting slower = the laptop throttles under sustained load; A and B are affected alike)
Duration: 229s. Environment: {"governor": "performance", "boost": "0", "ac_online": "1"}
