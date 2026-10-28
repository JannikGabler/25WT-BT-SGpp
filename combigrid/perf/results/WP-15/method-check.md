# WP-15 method check: peak RSS of e2e processes (2026-10-28)

Machine idle (envcheck OK, boost off), snapshot `WP-14-after`, 16 threads
(`OMP_PROC_BIND=close OMP_PLACES=threads`).

## 1. GNU time `%M` equals the kernel's VmHWM

Test program `touch_mem N` (malloc + memset of N MiB, prints `VmHWM` of `/proc/self/status` at
the end), run as `/usr/bin/time -f %M ./touch_mem N`:

| N | VmHWM (kB) | GNU time %M (KiB) |
|---|---|---|
| 0 | 1752 | 1700 |
| 100 | 104152 | 103920 |
| 500 | 513752 | 513516 |

No unit error (old GNU time versions reported 4x); `%M` is ≤ 0.24 MiB below the VmHWM read
shortly before exit, irrelevant at the scale of the cases.

## 2. The pitfall and the launcher (from perf.py)

Python driver that touched a 300 MiB bytearray (its own `ru_maxrss` = 330472 KiB), loading
`combigrid/perf/scripts/perf.py` as a module:

| method | reported peak of `/bin/true` |
|---|---|
| `subprocess.Popen` + `os.wait4` (direct child) | 330472 KiB = the driver's own peak |
| `perf.launch(..., peak_rss=True)` (GNU time `-f %M -o FILE`) | 1536 KiB |

## 3. One case: perf.py vs. a manual `/usr/bin/time -v` run

`interp/sg/cc/b0/d10/L2/p8`, `--samples 1 --warmups 1`, 5 processes each:

| method | peak RSS (KiB) |
|---|---|
| manual `/usr/bin/time -v` from the shell ("Maximum resident set size") | 25668 25332 24712 25168 25188 |
| `perf.run_exe(..., peak_rss=True)` | 24388 24404 25184 24552 25064 |

Same distribution; the 1.3 MiB (5 %) spread is process to process (presumably thread-dependent malloc
arenas), present in both.

## 4. Timing and foreign-load accounting through the launcher

Same case, one process each, measured from the 300 MiB driver:

| launch | total (in the binary) | wall | `RUSAGE_CHILDREN` CPU delta | foreign load |
|---|---|---|---|---|
| direct (old path) | 1.996 s | 4.09 s | 61.96 s | 0.16 % |
| via GNU time | 2.009 s | 4.11 s | 62.02 s | 0.11 % |
| manual `time -v` | – | 4.07 s | user 61.83 s + sys 0.02 s | – |

The benchmark's CPU time reaches the driver's `RUSAGE_CHILDREN` through GNU time (it waits for
its child), so `ForeignLoad` still counts it as own load. Launcher overhead: one extra
fork+exec of a 1.5 MiB program per process, outside the timed region (section 5).

## 5. Launcher overhead

`hyperfine -N --warmup 20 --runs 300`: `/bin/true` 0.95 ± 0.04 ms, `/usr/bin/time -f %M -o FILE
/bin/true` 2.1 ± 0.1 ms, so ≈ 1.2 ms wall per benchmark process, outside the timed region (a
full-suite `ab e2e`, 380 processes: ≈ 0.5 s of ≈ 23 min).

## 6. First look: where `quad/sg/cc-uq/b0/d2/L20` gained memory

`ab e2e baseline WP-14-after` flagged it `MEM+` (805.9 → 1075.8 MiB). One `run e2e` process per
snapshot, 3 repetitions (peak RSS, MiB):

| snapshot | peaks | rel. error |
|---|---|---|
| baseline | 839.7, 882.8, 841.8 | 9.93e-12 |
| WP-01-before | 843.2, 812.5, 811.3 | 9.93e-12 |
| WP-01-after | 1071.6, 1054.1, 1067.0 | 3.63e-13 |
| WP-13-before / -after | 1104.7, 1075.1, 1069.8 / 1055.1, 1084.4, 1068.2 | 3.63e-13 |
| WP-14-before / -after | 1070.4, 1096.3, 1062.2 / 1058.6, 1067.5, 1042.8 | 3.63e-13 |

The step (+≈ 240 MiB) comes with WP-01 (SG quadrature per row; nodes mapped onto the domain
once per tensor grid), not with the flag changes. Cause not analysed (RP-1).

## 7. Process floor

| process | peak RSS |
|---|---|
| `perf_e2e --build-info` | 10324 KiB |
| `perf_e2e --list` (reported as "process floor") | 10296 KiB |
| smallest case `interp/sg/equi/b0/d3/L5/p64` | 11.8–12.1 MiB |
