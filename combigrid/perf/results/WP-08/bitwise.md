# WP-08 — bitwise comparison of the corner loop (before vs. after)

`perf_reference` (all 104 cases) run with `OMP_NUM_THREADS=1` against `WP-08-before` and the
after build (deterministic: two runs of `WP-08-before` are identical):

- quad / dsg / mivec / interp: bitwise identical.
- interplin d1–d3 (8 cases): bitwise identical. interplin d5 (4 cases): values differ by
  ≤ 7.1e-15 absolute (refcheck OK at 1 and 16 threads, error vs. exact unchanged).

Cause (drivers in this directory, compiled against each library; `-I` an overlay with the
`HEAD` version of `linear_interpolation_scratch.hpp` for the old library):

- `tgdiff.cpp`: per TG of interplin equi b0 d5 L4 at the 8 reference points (1008 TG × point
  rows): `processTensorGrid` value, hash and count of the evaluated points, and every corner
  weight (indicator functions) are **bitwise identical** old vs. new.
- `opdiff.cpp`: the operator result equals, at 1 thread, `Σ (c · v)` with a separate multiply
  and add in the old library and `Σ fma(c, v, ·)` in the new one. The driver
  (`operators/linear_interpolation/linear_interpolation.cpp`, unchanged source) is compiled
  differently after the kernel change (LTO inlining of the new kernel), and GCC now contracts the
  combination `result += coefficient * tgValue` into an FMA. A code-generation artifact of the
  combination step, not of the corner loop.

Follow-up candidate: pin the combination to `std::fma` (as `tools::mapToDomain` pins the mapping)
so that results do not depend on inlining decisions.
