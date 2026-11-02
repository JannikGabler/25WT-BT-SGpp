# WP-28 re-check: disk tier, ns per lookup (median)

C = WP-28-checkpoint (73d6b3ac6), R = WP-28-refactor (HEAD + fixes), S = WP-28 summary run (`disk-t{1,16}.json`, WP-28-after; no `none` cases there). c1000 = 1 µs function, c0 = free function. One `perf.py run micro` per snapshot and thread count (no A/B CI).

## 1 thread(s)

| case | uncached | memory cold | persistent cold | persistent warm | temporary spill | spill warm |
|---|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 c1000 | C 954 / R 957 | C 259 / R 259 (S 261) | C 330 / R 333 (S 329) | C 109 / R 108 (S 107) | C 1158 / R 1165 (S 1161) | C 539 / R 541 (S 537) |
| quad/sg/equi/b1/d20/L3 c1000 | C 936 / R 937 | C 708 / R 694 (S 696) | C 956 / R 956 (S 958) | C 299 / R 300 (S 305) | C 3166 / R 3235 (S 3151) | C 624 / R 635 (S 608) |
| interp/sg/cc/b0/d5/L3/p8 c1000 | C 1093 / R 1096 | C 123 / R 123 (S 123) | C 136 / R 135 (S 135) | C 106 / R 104 (S 104) | C 493 / R 485 (S 494) | C 481 / R 475 (S 482) |
| quad/sg/cc/b0/d5/L4 c0 | C 28 / R 28 | C 90 / R 92 (S 90) | C 163 / R 161 (S 162) | C 109 / R 107 (S 108) | C 980 / R 1009 (S 983) | C 537 / R 543 (S 535) |
| quad/sg/equi/b1/d20/L3 c0 | C 33 / R 33 | C 244 / R 225 (S 237) | C 486 / R 493 (S 498) | C 299 / R 299 (S 313) | C 2674 / R 2740 (S 2682) | C 614 / R 623 (S 602) |
| interp/sg/cc/b0/d5/L3/p8 c0 | C 112 / R 112 | C 97 / R 96 (S 97) | C 110 / R 109 (S 108) | C 105 / R 103 (S 104) | C 463 / R 460 (S 461) | C 481 / R 476 (S 483) |

| disk primitive (1 t) | C | R | R/C | S |
|---|---|---|---|---|
| append/persistent/d20/n100000/e1000000 | 3253 | 3256 | 1.001 | 3227 |
| append/persistent/d5/n100000/e1000000 | 1529 | 1521 | 0.995 | 1532 |
| load/persistent/d20/n100000 | 270 | 269 | 0.998 | 269 |
| load/persistent/d5/n100000 | 151 | 154 | 1.019 | 153 |
| lookup/memory/d20/n100000 | 169 | 169 | 1.004 | 168 |
| lookup/memory/d5/n100000 | 83 | 87 | 1.051 | 82 |
| lookup/temporary0/d20/n100000 | 749 | 735 | 0.981 | 759 |
| lookup/temporary0/d5/n100000 | 581 | 575 | 0.990 | 581 |
| write/memory/d20/n100000 | 178 | 179 | 1.008 | 180 |
| write/memory/d5/n100000 | 175 | 176 | 1.006 | 172 |
| write/persistent/d20/n100000 | 888 | 887 | 0.999 | 890 |
| write/persistent/d5/n100000 | 719 | 724 | 1.008 | 724 |
| write/temporary0/d20/n100000 | 4879 | 4871 | 0.998 | 4809 |
| write/temporary0/d5/n100000 | 3658 | 3739 | 1.022 | 3679 |

## 16 thread(s)

| case | uncached | memory cold | persistent cold | persistent warm | temporary spill | spill warm |
|---|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 c1000 | C 72 / R 72 | C 33 / R 32 (S 32) | C 124 / R 125 (S 125) | C 44 / R 45 (S 45) | C 476 / R 477 (S 480) | C 102 / R 104 (S 102) |
| quad/sg/equi/b1/d20/L3 c1000 | C 65 / R 65 | C 75 / R 75 (S 76) | C 408 / R 409 (S 403) | C 164 / R 162 (S 162) | C 2237 / R 2247 (S 2226) | C 102 / R 103 (S 103) |
| interp/sg/cc/b0/d5/L3/p8 c1000 | C 93 / R 86 | C 18 / R 18 (S 18) | C 33 / R 33 (S 33) | C 19 / R 19 (S 20) | C 112 / R 112 (S 111) | C 102 / R 99 (S 99) |
| quad/sg/cc/b0/d5/L4 c0 | C 3 / R 3 | C 20 / R 20 (S 21) | C 113 / R 114 (S 112) | C 45 / R 44 (S 45) | C 463 / R 465 (S 464) | C 112 / R 102 (S 101) |
| quad/sg/equi/b1/d20/L3 c0 | C 4 / R 4 | C 53 / R 55 (S 54) | C 377 / R 375 (S 381) | C 163 / R 162 (S 161) | C 2208 / R 2249 (S 2195) | C 104 / R 102 (S 102) |
| interp/sg/cc/b0/d5/L3/p8 c0 | C 12 / R 12 | C 15 / R 16 (S 16) | C 31 / R 31 (S 31) | C 19 / R 19 (S 19) | C 113 / R 113 (S 108) | C 100 / R 101 (S 103) |

| disk primitive (16 t) | C | R | R/C | S |
|---|---|---|---|---|
| append/persistent/d20/n100000/e1000000 | 3101 | 3051 | 0.984 | 3036 |
| append/persistent/d5/n100000/e1000000 | 1418 | 1398 | 0.986 | 1397 |
| load/persistent/d20/n100000 | 270 | 276 | 1.025 | 263 |
| load/persistent/d5/n100000 | 154 | 153 | 0.990 | 154 |
| lookup/memory/d20/n100000 | 23 | 22 | 0.940 | 22 |
| lookup/memory/d5/n100000 | 13 | 12 | 0.952 | 12 |
| lookup/temporary0/d20/n100000 | 108 | 109 | 1.010 | 108 |
| lookup/temporary0/d5/n100000 | 102 | 101 | 0.992 | 103 |
| write/memory/d20/n100000 | 48 | 48 | 0.991 | 47 |
| write/memory/d5/n100000 | 34 | 33 | 0.989 | 33 |
| write/persistent/d20/n100000 | 767 | 749 | 0.976 | 759 |
| write/persistent/d5/n100000 | 548 | 549 | 1.002 | 552 |
| write/temporary0/d20/n100000 | 4539 | 4568 | 1.007 | 4538 |
| write/temporary0/d5/n100000 | 3440 | 3453 | 1.004 | 3474 |
