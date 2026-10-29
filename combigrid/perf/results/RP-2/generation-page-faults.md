# RP-2 — `generation` of quad/dsg b1 slower with unchanged code (WP-02 question)

WP-02's projection A/B reported `quad/dsg/equi-exp/b1/d20/L6` generation 50.97 → 59.54 ms
(0.852) although the generation code was not touched.

**It persists**: direct `ab e2e rp-1 rp-2 --filter '^quad/dsg/equi-exp/b1/'`
(`e2e-rp-1-vs-rp-2-dsg-b1.md`, 5 rounds, foreign load ≤ 0.5 %):
- d20 generation 50.86 → 60.90 ms (0.843 [0.818, 0.868], SLOWER);
- d15 35.56 → 42.72 ms (0.851 [0.729, 0.993]).

**Cause: page faults, not code.** `perf_e2e --filter '^quad/dsg/equi-exp/b1/d20/L6$'`,
16 threads, 5 processes each, generation in ms:

| snapshot | cold (0 warm-ups) | warm (1 warm-up, as in `ab`) | warm, glibc never returns memory (`GLIBC_TUNABLES=glibc.malloc.trim_threshold=16G:glibc.malloc.mmap_threshold=4G:glibc.malloc.top_pad=1G`, diagnostic only) |
|---|---|---|---|
| rp-1 | 180–209 | 50.2–50.4 | 30.5–31.2 |
| rp-2 | 192–214 | 59.7–61.2 | 30.2–31.2 |

With trimming off, generation costs ≈ 31 ms in both builds: the code and its malloc calls. The
rest of the warm generation, 20 ms (rp-1) or 30 ms (rp-2), comes from re-faulting the pages for the
DSG's 126 MB. glibc returned them to the kernel when the warm-up's grid and operator memory were
freed. rp-1's operator allocated and freed 154 MB of per-thread sub-trie vectors, which left more
freed memory mapped for the next generation to reuse; WP-02 removed those vectors. The cold numbers
also include first-touch faults and OpenMP thread creation.

Consequences:
- No regression to fix; the comparison window of a warm pipeline depends on what the previous
  phase freed. Absolute generation timings of allocation-heavy cases carry ±20 ms of this effect.
- About half of the warm b1 d20 generation is the memory volume of the grid. WP-22 / WP-18
  (fewer bytes, 1 instead of 3 allocations per tensor grid) shrink both parts.
- Changing the malloc tunables is an environment / build decision (not proposed). The tunables
  above were only used for this diagnosis.
