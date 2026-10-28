# WP-15: `run e2e` one process per case (new) vs. one process for all cases (old)

Snapshot `WP-14-after`, full suite, 16 threads, 1 warm-up + 1 sample per case, 5 passes alternating new (perf.py of WP-15: one process per case through GNU time, as `ab`) and old (perf.py of HEAD 3dc286c3d: one process runs all cases). Foreign load 0.1–0.2 % per pass.

Sum of `total` per pass: new 53.40 s, old 53.59 s, new 53.47 s, old 53.46 s, new 53.33 s

| case | old (2 passes) | new (3 passes) | new/old | peak RSS new (3 passes, MiB) |
|---|---|---|---|---|
| quad/sg/equi-exp/b0/d2/L20 | 383.4, 387.0 ms | 414.2, 421.8, 398.6 ms | 1.075 ** | 619.6, 619.5, 619.7 |
| quad/sg/equi-exp/b0/d5/L10 | 252.8, 253.2 ms | 252.5, 252.1, 252.7 ms | 0.998 | 15.3, 15.3, 15.8 |
| quad/sg/equi-exp/b0/d7/L7 | 502.0, 501.0 ms | 505.4, 500.8, 502.7 ms | 1.002 | 14.5, 14.2, 14.0 |
| quad/sg/equi-exp/b0/d10/L4 | 937.6, 941.5 ms | 933.5, 939.8, 937.9 ms | 0.998 | 12.6, 12.8, 12.4 |
| quad/sg/equi-exp/b1/d10/L7 | 300.1, 304.2 ms | 302.9, 307.7, 302.5 ms | 1.003 | 25.7, 25.6, 26.0 |
| quad/sg/equi-exp/b1/d15/L6 | 512.4, 513.4 ms | 517.2, 522.0, 521.3 ms | 1.016 | 51.4, 53.1, 53.8 |
| quad/sg/equi-exp/b1/d20/L5 | 226.4, 224.9 ms | 234.2, 230.1, 233.8 ms | 1.036 | 57.9, 53.6, 56.8 |
| quad/sg/equi-sin/b0/d7/L7 | 2412.7, 2410.8 ms | 2415.6, 2412.4, 2411.1 ms | 1.000 | 14.2, 14.1, 14.6 |
| quad/sg/cc-uq/b0/d2/L20 | 1187.7, 1180.1 ms | 1185.9, 1195.8, 1167.4 ms | 1.002 | 1070.7, 1076.1, 1067.4 |
| quad/sg/cc-uq/b0/d3/L14 | 84.1, 84.7 ms | 88.6, 88.3, 85.7 ms | 1.046 | 42.3, 40.0, 41.1 |
| quad/sg/cc-uq/b0/d5/L10 | 326.4, 325.7 ms | 340.3, 326.8, 326.7 ms | 1.002 | 17.7, 18.1, 17.6 |
| quad/sg/cc-uq/b0/d7/L7 | 704.6, 703.5 ms | 700.4, 700.6, 701.5 ms | 0.995 | 15.3, 14.9, 14.8 |
| quad/sg/cc-uq/b0/d10/L4 | 1307.2, 1310.4 ms | 1311.3, 1313.9, 1339.1 ms | 1.004 | 12.4, 12.8, 13.0 |
| quad/dsg/equi-exp/b0/d3/L16 | 83.1, 81.6 ms | 96.2, 95.9, 96.3 ms | 1.169 ** | 63.1, 61.6, 62.8 |
| quad/dsg/equi-exp/b0/d5/L12 | 211.5, 211.5 ms | 213.1, 213.0, 212.6 ms | 1.007 | 24.5, 24.4, 23.9 |
| quad/dsg/equi-exp/b0/d7/L10 | 1214.3, 1216.8 ms | 1216.4, 1214.2, 1218.2 ms | 1.001 | 37.3, 36.4, 36.2 |
| quad/dsg/equi-exp/b0/d10/L7 | 5674.9, 5662.1 ms | 5693.9, 5661.2, 5656.0 ms | 0.999 | 37.8, 36.6, 37.4 |
| quad/dsg/equi-exp/b1/d15/L7 | 4103.9, 4111.3 ms | 4094.1, 4093.1, 4085.1 ms | 0.996 | 236.1, 235.4, 232.4 |
| quad/dsg/equi-exp/b1/d20/L6 | 7037.7, 7030.0 ms | 7036.6, 7025.8, 7006.5 ms | 0.999 | 327.3, 326.9, 326.1 |
| quad/dsg/cc-uq/b0/d5/L12 | 256.3, 256.4 ms | 258.1, 257.7, 257.9 ms | 1.006 | 24.2, 24.0, 24.1 |
| quad/dsg/cc-uq/b0/d7/L10 | 1520.1, 1519.7 ms | 1507.1, 1512.8, 1512.2 ms | 0.995 | 37.3, 36.6, 36.9 |
| quad/dsg/cc-uq/b0/d10/L7 | 7058.2, 7046.2 ms | 7015.8, 7009.3, 7020.7 ms | 0.995 | 37.6, 38.0, 37.7 |
| interp/sg/cc/b0/d2/L12/p64 | 175.2, 175.1 ms | 175.7, 174.7, 176.3 ms | 1.003 | 14.7, 14.7, 14.8 |
| interp/sg/cc/b0/d3/L9/p64 | 244.7, 244.3 ms | 244.7, 249.7, 244.6 ms | 1.001 | 11.9, 12.0, 12.2 |
| interp/sg/cc/b0/d5/L6/p64 | 1463.5, 1454.3 ms | 1446.5, 1460.7, 1452.8 ms | 0.996 | 11.9, 12.0, 11.9 |
| interp/sg/cc/b0/d7/L4/p16 | 1280.4, 1278.3 ms | 1259.0, 1257.0, 1268.6 ms | 0.984 | 12.9, 13.1, 13.2 |
| interp/sg/cc/b0/d10/L2/p8 | 1989.7, 1995.6 ms | 1994.2, 2011.3, 1995.6 ms | 1.001 | 24.3, 25.0, 25.0 |
| interp/sg/cheb1/b0/d3/L6/p64 | 181.2, 181.8 ms | 182.3, 182.5, 181.6 ms | 1.004 | 12.0, 11.8, 11.9 |
| interp/sg/cheb1/b0/d5/L4/p64 | 384.8, 385.5 ms | 336.4, 339.5, 335.8 ms | 0.874 ** | 11.8, 11.8, 11.8 |
| interp/sg/equi/b0/d3/L5/p64 | 51.2, 51.6 ms | 51.7, 51.0, 50.8 ms | 0.992 | 11.9, 11.5, 11.9 |
| interplin/sg/equi/b1/d2/L16/p16384 | 111.7, 108.9 ms | 111.4, 108.7, 108.2 ms | 0.985 | 19.1, 19.8, 19.4 |
| interplin/sg/equi/b1/d5/L8/p1024 | 436.0, 440.7 ms | 434.5, 433.7, 435.0 ms | 0.991 | 12.7, 12.5, 12.9 |
| interplin/sg/equi/b1/d8/L5/p256 | 1518.9, 1510.3 ms | 1494.1, 1510.5, 1493.2 ms | 0.986 | 12.9, 12.7, 12.6 |
| interplin/sg/equi/b1/d10/L5/p32 | 2440.8, 2476.2 ms | 2450.4, 2438.7, 2454.5 ms | 0.997 | 13.5, 13.1, 13.5 |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 753.3, 730.2 ms | 730.7, 803.3, 728.2 ms | 0.985 | 16.3, 16.1, 15.9 |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 1988.8, 1993.4 ms | 1993.2, 1986.4, 1984.1 ms | 0.998 | 17.6, 17.7, 17.1 |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 2101.4, 2094.9 ms | 2097.9, 2096.4, 2090.8 ms | 0.999 | 12.6, 12.7, 12.8 |
| mivec/sg/cc-exp/b1/d20/S7/n300 | 2168.5, 2058.6 ms | 2068.3, 2070.6, 2078.5 ms | 0.980 | 12.2, 12.3, 12.2 |

Geomean new/old over the 38 cases: 1.0025. `**`: |new/old - 1| > 5 %. `quad/sg/equi-exp/b0/d2/L20` (first case in both modes) is within its noise (baseline noise model: 8.8 % per round). `quad/dsg/equi-exp/b0/d3/L16` (+17 %, noise 0.4 %) and `interp/sg/cheb1/b0/d5/L4/p64` (-13 %, noise 1.1 %) are systematic (all passes agree) and come from the process structure, not from the launcher (launcher-on-off-*.md: 0.993 and 1.000); presumably a case in a shared process runs on the heap and thread state left by the cases before it (not analysed). `ab` always ran one process per case, so `run e2e` now measures like `ab`; `baseline/e2e_t16.jsonl` was recorded with one process for all cases.

Process floor (`perf_e2e --list`) in the 3 new passes: 10572 KiB, 10396 KiB, 10572 KiB
