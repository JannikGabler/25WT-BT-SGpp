
### 1 thread(s): ns per evaluation (speedup vs. none / vs. always)

| config | cost | none | always | warm | adaptive (new cache per call) | adaptive steady | mode / switches / c_f ns / hit ns (steady) |
|---|---|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 | 0 | 27.6 | 91.3 (0.30x) | 68.4 (0.40x) | 32.1 (0.86x / 2.85x) | 32.2 (0.86x / 2.84x) | None / 9 / 25 / 51 |
| quad/sg/cc/b0/d5/L4 | 1000 | 960.4 | 260.0 (3.69x) | 68.9 (13.94x) | 268.1 (3.58x / 0.97x) | 74.4 (12.90x / 3.49x) | MemoryOnly / 0 / 993 / 66 |
| quad/sg/equi/b1/d20/L3 | 0 | 33.5 | 242.4 (0.14x) | 125.5 (0.27x) | 47.4 (0.71x / 5.12x) | 39.6 (0.85x / 6.12x) | None / 9 / 44 / 76 |
| quad/sg/equi/b1/d20/L3 | 1000 | 940.0 | 709.8 (1.32x) | 122.8 (7.65x) | 717.0 (1.31x / 0.99x) | 130.6 (7.20x / 5.43x) | MemoryOnly / 0 / 989 / 100 |
| quad/sg/equi/b0/d2/L11 | 0 | 16.6 | 86.5 (0.19x) | 60.2 (0.28x) | 22.8 (0.73x / 3.80x) | 20.2 (0.82x / 4.29x) | None / 11 / 15 / 49 |
| quad/sg/equi/b0/d2/L11 | 1000 | 906.2 | 438.6 (2.07x) | 64.9 (13.96x) | 454.0 (2.00x / 0.97x) | 65.7 (13.80x / 6.68x) | MemoryOnly / 0 / 986 / 67 |
| quad/sg/equi/b1/d10/L4 | 0 | 23.3 | 148.8 (0.16x) | 87.5 (0.27x) | 28.7 (0.81x / 5.18x) | 27.3 (0.85x / 5.44x) | None / 9 / 28 / 66 |
| quad/sg/equi/b1/d10/L4 | 1000 | 926.5 | 505.1 (1.83x) | 87.2 (10.63x) | 514.4 (1.80x / 0.98x) | 99.0 (9.36x / 5.10x) | MemoryOnly / 0 / 989 / 78 |
| quad/dsg/cc/b0/d5/L5 | 0 | 36.6 | 208.2 (0.18x) | 90.1 (0.41x) | 46.7 (0.78x / 4.46x) | 40.4 (0.91x / 5.16x) | None / 11 / 26 / 65 |
| quad/dsg/cc/b0/d5/L5 | 1000 | 978.5 | 1160.1 (0.84x) | 91.8 (10.65x) | 1141.3 (0.86x / 1.02x) | 182.0 (5.38x / 6.37x) | MemoryOnly / 4 / 990 / 92 |
| interp/sg/cc/b0/d5/L3/p8 | 0 | 111.9 | 97.1 (1.15x) | 93.1 (1.20x) | 101.2 (1.11x / 0.96x) | 95.8 (1.17x / 1.01x) | MemoryOnly / 0 / 131 / 54 |
| interp/sg/cc/b0/d5/L3/p8 | 1000 | 1095.8 | 123.8 (8.85x) | 92.4 (11.85x) | 128.3 (8.54x / 0.97x) | 95.9 (11.42x / 1.29x) | MemoryOnly / 0 / 1091 / 55 |
| interplin/sg/equi/b1/d8/L3/p8 | 0 | 181.5 | 189.7 (0.96x) | 114.0 (1.59x) | 196.3 (0.92x / 0.97x) | 117.8 (1.54x / 1.61x) | MemoryOnly / 0 / 187 / 71 |
| interplin/sg/equi/b1/d8/L3/p8 | 1000 | 1156.3 | 409.4 (2.82x) | 113.1 (10.22x) | 419.9 (2.75x / 0.98x) | 116.1 (9.96x / 3.53x) | MemoryOnly / 0 / 1149 / 72 |

Storage variants (memory limit 0): temporary_spill (TemporaryStorage) vs. adaptive_spill (ns per evaluation, final mode of the last call)

| case | ns | mode |
|---|---|---|
| interp/sg/cc/b0/d5/L3/p8/adaptive_spill_production/c0 | 155.2 | None |
| interp/sg/cc/b0/d5/L3/p8/adaptive_spill_production/c1000 | 512.0 | TemporaryStorage |
| interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c0 | 458.4 | - |
| interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c1000 | 484.0 | - |
| quad/sg/cc/b0/d5/L4/adaptive_spill_production/c0 | 32.5 | None |
| quad/sg/cc/b0/d5/L4/adaptive_spill_production/c1000 | 976.2 | None |
| quad/sg/cc/b0/d5/L4/temporary_spill_production/c0 | 995.1 | - |
| quad/sg/cc/b0/d5/L4/temporary_spill_production/c1000 | 1170.9 | - |
| quad/sg/equi/b1/d20/L3/adaptive_spill_production/c0 | 70.9 | None |
| quad/sg/equi/b1/d20/L3/adaptive_spill_production/c1000 | 1768.4 | None |
| quad/sg/equi/b1/d20/L3/temporary_spill_production/c0 | 2781.7 | - |
| quad/sg/equi/b1/d20/L3/temporary_spill_production/c1000 | 3256.0 | - |

Cost of one window decision (64 samples + rules): 2.32 us

### 16 thread(s): ns per evaluation (speedup vs. none / vs. always)

| config | cost | none | always | warm | adaptive (new cache per call) | adaptive steady | mode / switches / c_f ns / hit ns (steady) |
|---|---|---|---|---|---|---|---|
| quad/sg/cc/b0/d5/L4 | 0 | 3.1 | 20.8 (0.15x) | 12.4 (0.25x) | 4.9 (0.65x / 4.29x) | 3.5 (0.89x / 5.90x) | None / 17 / 47 / 118 |
| quad/sg/cc/b0/d5/L4 | 1000 | 72.6 | 32.5 (2.24x) | 12.4 (5.85x) | 33.4 (2.18x / 0.97x) | 12.9 (5.65x / 2.53x) | MemoryOnly / 0 / 1010 / 116 |
| quad/sg/equi/b1/d20/L3 | 0 | 3.6 | 51.4 (0.07x) | 15.1 (0.24x) | 6.0 (0.60x / 8.50x) | 4.0 (0.90x / 12.78x) | None / 19 / 62 / 172 |
| quad/sg/equi/b1/d20/L3 | 1000 | 65.4 | 76.6 (0.85x) | 15.1 (4.33x) | 76.2 (0.86x / 1.01x) | 15.3 (4.29x / 5.02x) | MemoryOnly / 0 / 1021 / 194 |
| quad/sg/equi/b0/d2/L11 | 0 | 3.0 | 31.1 (0.10x) | 15.6 (0.19x) | 5.2 (0.57x / 6.04x) | 3.5 (0.85x / 8.96x) | None / 19 / 25 / 138 |
| quad/sg/equi/b0/d2/L11 | 1000 | 112.3 | 82.2 (1.37x) | 19.2 (5.85x) | 82.6 (1.36x / 1.00x) | 15.8 (7.11x / 5.21x) | MemoryOnly / 0 / 999 / 90 |
| quad/sg/equi/b1/d10/L4 | 0 | 2.6 | 32.1 (0.08x) | 12.3 (0.21x) | 4.1 (0.62x / 7.81x) | 3.0 (0.85x / 10.67x) | None / 17 / 42 / 145 |
| quad/sg/equi/b1/d10/L4 | 1000 | 67.1 | 49.0 (1.37x) | 12.4 (5.42x) | 53.7 (1.25x / 0.91x) | 12.7 (5.28x / 3.86x) | MemoryOnly / 0 / 1012 / 139 |
| quad/dsg/cc/b0/d5/L5 | 0 | 7.4 | 43.0 (0.17x) | 16.7 (0.44x) | 9.0 (0.82x / 4.76x) | 6.8 (1.09x / 6.36x) | None / 19 / 38 / 137 |
| quad/dsg/cc/b0/d5/L5 | 1000 | 86.9 | 115.8 (0.75x) | 16.4 (5.32x) | 112.6 (0.77x / 1.03x) | 16.8 (5.17x / 6.89x) | MemoryOnly / 4 / 1016 / 127 |
| interp/sg/cc/b0/d5/L3/p8 | 0 | 11.8 | 16.8 (0.70x) | 13.5 (0.87x) | 16.5 (0.72x / 1.02x) | 13.9 (0.85x / 1.21x) | MemoryOnly / 0 / 159 / 115 |
| interp/sg/cc/b0/d5/L3/p8 | 1000 | 85.4 | 17.9 (4.78x) | 13.5 (6.32x) | 19.1 (4.47x / 0.94x) | 13.9 (6.13x / 1.28x) | MemoryOnly / 0 / 1131 / 116 |
| interplin/sg/equi/b1/d8/L3/p8 | 0 | 22.5 | 31.9 (0.71x) | 17.3 (1.30x) | 24.6 (0.91x / 1.29x) | 20.8 (1.08x / 1.53x) | MemoryOnly / 14 / 242 / 133 |
| interplin/sg/equi/b1/d8/L3/p8 | 1000 | 88.9 | 53.1 (1.67x) | 14.8 (6.03x) | 54.2 (1.64x / 0.98x) | 15.5 (5.73x / 3.42x) | MemoryOnly / 0 / 1219 / 127 |

Storage variants (memory limit 0): temporary_spill (TemporaryStorage) vs. adaptive_spill (ns per evaluation, final mode of the last call)

| case | ns | mode |
|---|---|---|
| interp/sg/cc/b0/d5/L3/p8/adaptive_spill_production/c0 | 14.8 | None |
| interp/sg/cc/b0/d5/L3/p8/adaptive_spill_production/c1000 | 105.3 | None |
| interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c0 | 108.7 | - |
| interp/sg/cc/b0/d5/L3/p8/temporary_spill_production/c1000 | 110.9 | - |
| quad/sg/cc/b0/d5/L4/adaptive_spill_production/c0 | 4.5 | None |
| quad/sg/cc/b0/d5/L4/adaptive_spill_production/c1000 | 243.8 | None |
| quad/sg/cc/b0/d5/L4/temporary_spill_production/c0 | 490.7 | - |
| quad/sg/cc/b0/d5/L4/temporary_spill_production/c1000 | 488.6 | - |
| quad/sg/equi/b1/d20/L3/adaptive_spill_production/c0 | 5.3 | None |
| quad/sg/equi/b1/d20/L3/adaptive_spill_production/c1000 | 875.5 | None |
| quad/sg/equi/b1/d20/L3/temporary_spill_production/c0 | 2326.8 | - |
| quad/sg/equi/b1/d20/L3/temporary_spill_production/c1000 | 2352.5 | - |

Cost of one window decision (64 samples + rules): 2.41 us
