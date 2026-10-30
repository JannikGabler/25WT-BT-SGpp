threads: 1

| grid | callable | variant | ns/eval | speedup | Python calls / evals |
|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 | math | none | 673 | 1.00 | 206358 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | dict, cold | 692 | 0.97 | 36033 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | dict, warm | 549 | 1.23 | 0 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | C++ cache, cold | 201 | 3.34 | 36033 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | C++ cache, warm | 65 | 10.33 | 0 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | lru_cache, cold | 664 | 1.01 | - / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | none | 4962 | 1.00 | 206358 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | dict, cold | 1479 | 3.35 | 36033 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | dict, warm | 546 | 9.08 | 0 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | C++ cache, cold | 977 | 5.08 | 36033 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | C++ cache, warm | 67 | 74.03 | 0 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | lru_cache, cold | 1446 | 3.43 | - / 206358 |
| quad/sg/equi/b1/d10/L4 | math | none | 823 | 1.00 | 244361 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | dict, cold | 1207 | 0.68 | 90561 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | dict, warm | 699 | 1.18 | 0 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | C++ cache, cold | 425 | 1.94 | 90561 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | C++ cache, warm | 90 | 9.18 | 0 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | lru_cache, cold | 1243 | 0.66 | - / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | none | 5335 | 1.00 | 244361 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | dict, cold | 2952 | 1.81 | 90561 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | dict, warm | 703 | 7.59 | 0 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | C++ cache, cold | 2142 | 2.49 | 90561 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | C++ cache, warm | 86 | 61.75 | 0 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | lru_cache, cold | 2965 | 1.80 | - / 244361 |

threads: 16

| grid | callable | variant | ns/eval | speedup | Python calls / evals |
|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 | math | none | 24095 | 1.00 | 206358 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | dict, cold | 24099 | 1.00 | 36033 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | dict, warm | 23936 | 1.01 | 0 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | C++ cache, cold | 4468 | 5.39 | 36107 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | C++ cache, warm | 12 | 2010.19 | 0 / 206358 |
| quad/sg/cc/b0/d5/L4 | math | lru_cache, cold | 13026 | 1.85 | - / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | none | 30014 | 1.00 | 206358 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | dict, cold | 25728 | 1.17 | 36033 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | dict, warm | 23911 | 1.26 | 0 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | C++ cache, cold | 5544 | 5.41 | 36082 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | C++ cache, warm | 12 | 2501.71 | 0 / 206358 |
| quad/sg/cc/b0/d5/L4 | numpy | lru_cache, cold | 14570 | 2.06 | - / 206358 |
| quad/sg/equi/b1/d10/L4 | math | none | 24171 | 1.00 | 244361 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | dict, cold | 25700 | 0.94 | 90561 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | dict, warm | 24359 | 0.99 | 0 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | C++ cache, cold | 9401 | 2.57 | 90590 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | C++ cache, warm | 13 | 1925.01 | 0 / 244361 |
| quad/sg/equi/b1/d10/L4 | math | lru_cache, cold | 16176 | 1.49 | - / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | none | 30067 | 1.00 | 244361 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | dict, cold | 28517 | 1.05 | 90561 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | dict, warm | 24090 | 1.25 | 0 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | C++ cache, cold | 11828 | 2.54 | 90582 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | C++ cache, warm | 12 | 2483.72 | 0 / 244361 |
| quad/sg/equi/b1/d10/L4 | numpy | lru_cache, cold | 18901 | 1.59 | - / 244361 |
