# RP-3 — OpenBLAS pool: cold first call vs. warm calls (WP-10 decision data)

Diagnostic only, no code or build change: the `rp-3` `perf_e2e` run directly with perf.py's
16-thread OpenMP environment (`OMP_PROC_BIND=close OMP_PLACES=threads`), default vs.
`OPENBLAS_NUM_THREADS=1` (the no-code proxy for "OpenBLAS without its thread pool" named in
WP-10), interleaved per round. `libsgppbase.so` links Armadillo → `libblas`/`liblapack` →
`libopenblas.so.0` (`ldd`), so every process starts the pool (15 extra threads), although the
CC FFT uses Eigen.

## Process wall time vs. in-process `total` (`openblas_diag.py`, 10 rounds, 1 warm-up + 1 sample as `ab e2e`)

| case | process wall, default | `OPENBLAS_NUM_THREADS=1` | ratio | in-process `total`, default | `OPENBLAS_NUM_THREADS=1` | ratio |
|---|---|---|---|---|---|---|
| mivec/sg/cc-exp/b1/d20/S7/n300 | 153.1 ms | 18.6 ms | 8.24 | 3.13 ms | 3.18 ms | 0.986 |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 200.4 ms | 66.4 ms | 3.02 | 26.18 ms | 26.51 ms | 0.988 |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 1124.0 ms | 999.8 ms | 1.12 | 485.4 ms | 500.6 ms | 0.970 |
| mivec/sg/cc-exp/b1/d5/S16/n1000 | 1582.1 ms | 1501.4 ms | 1.05 | 730.1 ms | 743.9 ms | 0.981 |
| quad/sg/equi-exp/b1/d20/L5 | 579.9 ms | 508.2 ms | 1.14 | 235.5 ms | 230.4 ms | 1.022 |
| interp/sg/equi/b0/d3/L5/p64 | 234.0 ms | 115.3 ms | 2.03 | 51.18 ms | 51.13 ms | 1.001 |
| quad/dsg/equi-exp/b0/d3/L16 | 288.0 ms | 210.4 ms | 1.37 | 93.90 ms | 93.79 ms | 1.001 |
| quad/dsg/equi-exp/b1/d20/L6 | 2641.3 ms | 2585.4 ms | 1.02 | 1226.6 ms | 1225.9 ms | 1.001 |

The judged metric (`total` after a warm-up) is unaffected (0.97–1.02, within the per-case
noise). The process is 56–135 ms longer with the pool. Raw: `openblas-diag-raw.json`.

## Where the time goes: the first operator call

`perf_e2e --list` (libraries loaded, no case run): 12–13 vs. 10 ms per process, so loading and
starting the pool alone costs only 2–3 ms. `strace -f -ttt` of the mivec d20 process: the
time is spent before the measured sample, in OpenMP barrier wake-ups (`futex` wakes) that
complete only every 4 ms (one scheduler tick at 250 Hz) while the 15 OpenBLAS threads are still
busy-waiting on the same hardware threads.

Cold first call measured directly (`--warmups 0 --samples 3`, 5 rounds, medians):

| case | 1st call, default | `OPENBLAS_NUM_THREADS=1` | cold penalty | 3rd call, default | `OPENBLAS_NUM_THREADS=1` |
|---|---|---|---|---|---|
| mivec/sg/cc-exp/b1/d20/S7/n300 | 135.7 ms | 6.2 ms | **+129.5 ms (22x)** | 3.12 ms | 3.18 ms |
| mivec/sg/cc-exp/b1/d15/S10/n300 | 157.1 ms | 29.2 ms | +127.9 ms (5.4x) | 26.72 ms | 26.47 ms |
| interp/sg/equi/b0/d3/L5/p64 | 169.3 ms | 53.7 ms | +115.6 ms (3.2x) | 51.29 ms | 51.08 ms |
| quad/dsg/equi-exp/b0/d3/L16 | 170.7 ms | 100.5 ms | +70.2 ms | 93.54 ms | 93.19 ms |
| quad/sg/equi-exp/b1/d20/L5 | 322.6 ms | 259.5 ms | +63.2 ms | 238.2 ms | 225.9 ms |
| mivec/sg/cc-exp/b1/d10/S12/n1000 | 624.9 ms | 497.4 ms | +127.5 ms | 491.4 ms | 484.8 ms |
| quad/dsg/equi-exp/b0/d10/L7 | 5475.5 ms | 5408.1 ms | +67.4 ms | 5403.1 ms | 5404.1 ms |

- **Cold first call: +63…+130 ms per process at 16 threads**, independent of the pipeline size
  (up to 22x for millisecond pipelines). Warm calls are equal.
- The judged suite does not see it: `ab e2e` runs a warm-up first, and the profiles'
  "OpenBLAS x %" samples (`profile-splits.md`) are this spinning during the warm-up.
- Who pays: every process whose first combigrid call matters: one-shot C++ programs, the Boost
  tests, Python scripts (`pysgpp` loads `libsgppbase.so`, so presumably the same; not measured
  here), short adaptive-refinement steps in fresh processes.
- WP-10 ("measure first"): the effect is no longer "start-up only" in the sense of the setup
  (loading the libraries costs 2–3 ms). The cost is +63…+130 ms on the first parallel operator
  call of each process, i.e. a cold-call latency the owner may weigh.
