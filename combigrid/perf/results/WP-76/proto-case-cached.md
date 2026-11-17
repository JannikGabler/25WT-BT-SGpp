gen chain, 1 us, uniform, 16 threads, 2.1747 ns/iter
| case | evals | mean cost us | median s | min s | max s | ideal s | eff |
|---|---|---|---|---|---|---|---|
  [cache: lookups 244361 hits 153795 inserts 90561 bytes 9666560]
| quad/sg/cc/b1/d10/L4 | 244361 | 1.00 | 0.0161 | 0.0155 | 0.0163 | 0.0152 | 0.947 |
  [cache: lookups 18015736 hits 14258162 inserts 3757569 bytes 400928768]
| quad/sg/cc/b1/d10/L6 | 18015736 | 1.00 | 1.0410 | 1.0407 | 1.0742 | 1.1239 | 1.080 |
  [cache: lookups 7679632 hits 2911537 inserts 4767673 bytes 879599616]
| mivec/sg/cc/b1/d20/S10/n300 | 7679632 | 1.00 | 1.2415 | 1.2070 | 1.2529 | 0.4791 | 0.386 |
  [cache: lookups 163888 hits 153887 inserts 10001 bytes 1638400]
| interp/sg/cc/b1/d10/L3/p8 | 163888 | 1.00 | 0.0042 | 0.0040 | 0.0042 | 0.0102 | 2.441 |
  [cache: lookups 514048 hits 456704 inserts 55705 bytes 3854336]
| interplin/sg/equi/b1/d5/L5/p64 | 514048 | 1.00 | 0.0181 | 0.0161 | 0.0236 | 0.0321 | 1.773 |
  [cache: lookups 4055040 hits 3500248 inserts 547118 bytes 49063936]
| interplin/sg/equi/b1/d8/L4/p32 | 4055040 | 1.00 | 0.1516 | 0.1510 | 0.1528 | 0.2530 | 1.669 |
gen chain, 100 us, uniform, 16 threads, 2.0423 ns/iter
| case | evals | mean cost us | median s | min s | max s | ideal s | eff |
|---|---|---|---|---|---|---|---|
  [cache: lookups 244361 hits 153798 inserts 90561 bytes 9666560]
| quad/sg/cc/b1/d10/L4 | 244361 | 100.00 | 0.5767 | 0.5759 | 0.5769 | 1.5272 | 2.648 |
  [cache: lookups 18015736 hits 14258162 inserts 3757569 bytes 400928768]
| quad/sg/cc/b1/d10/L6 | 18015736 | 100.00 | 24.1080 | 24.1049 | 24.1865 | 112.5975 | 4.671 |
  [cache: lookups 7679632 hits 2905691 inserts 4767673 bytes 879599616]
| mivec/sg/cc/b1/d20/S10/n300 | 7679632 | 100.00 | 46.0811 | 46.0738 | 46.1725 | 47.9973 | 1.042 |
  [cache: lookups 163888 hits 153864 inserts 10001 bytes 1638400]
| interp/sg/cc/b1/d10/L3/p8 | 163888 | 100.00 | 0.0704 | 0.0703 | 0.0705 | 1.0243 | 14.544 |
  [cache: lookups 514048 hits 454645 inserts 55705 bytes 3854336]
| interplin/sg/equi/b1/d5/L5/p64 | 514048 | 100.00 | 0.7184 | 0.7178 | 0.7281 | 3.2128 | 4.472 |
  [cache: lookups 4055040 hits 3489278 inserts 547118 bytes 49063936]
| interplin/sg/equi/b1/d8/L4/p32 | 4055040 | 100.00 | 5.5188 | 5.5176 | 5.5240 | 25.3438 | 4.592 |
