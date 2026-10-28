# Optimization status — combigrid (master file)

Last processed: WP-14 (2026-10-28) · Next free WP number: 15 · Re-profilings done: 0 (RP-0 = setup; RP-1 in progress)

- **Judged**: `perf.py build normal` (= compileNormal.sh + `USE_EIGEN=1 USE_ARMADILLO=1`), 16
  threads (`OMP_PROC_BIND=close`, `OMP_PLACES=threads`), **CPU boost off** (all cores at
  2.0 GHz), platform profile `performance`, AC, idle machine. GCC 15.2 stays the judged compiler.
  Flags from site_scons (not compileNormal.sh/perf.py): `-fno-semantic-interposition` (WP-13, GCC
  `OPT=1`); strict aliasing on (WP-14: `-fno-strict-aliasing` only for the SWIG Java wrapper).
- **Baseline**: source of 84299eeca; baseline commit = setup commit **fed2cdb4e**. Snapshot
  `baseline` exists in `.work/snapshots/` (if `.work/` was cleaned:
  `perf.py snapshot baseline --commit fed2cdb4e`). Numbers: `baseline/`, ANALYSIS.md. It keeps its
  own site_scons flags (no nosi, with `-fno-strict-aliasing`); flag changes count towards the
  cumulative progress (WP-13 1.023x, WP-14 0.988x). No re-recording.
- **Machine note**: boost off since 2026-10-24 (throttling up to 1.6x with boost on); a reboot
  re-enables it, the owner disables it manually — if `envcheck` fails on boost, ask the owner.
  Full e2e A/B ≈ 18–23 min (sessions filter). `scons -c` is fine (owner 2026-10-25). Sanitizer
  runs need a memory cap (uncapped TySan crashed the PC).
- **Toolchain**: GCC 15.2 (judged); Clang 21.1.8 + libomp + lld; JDK 21 (`SG_JAVA=1`).
  `perf/scripts/toolchain.py` (WP-11; variant `gcc-sa` from WP-14) builds compiler/flag variants.
- **Owner's rules**: acceptance by the owner only; minimal gains may be proposed but must be
  labelled and weighed against API/functionality/maintainability/readability; functional/
  interface/flag changes only after consulting the owner; errors stay in the same order of
  magnitude (refcheck OK/WARN/FAIL); high dimensions first.
- **Refcheck WARN case** (owner 2026-10-25: tolerance stays): `interp/sg/equi/b0/d2/L5/cos-shifted`
  (65 equidistant nodes, Lebesgue constant ~1e16) WARNs whenever FMA placement in
  `BarycentricFormula::evaluateFormula` changes; the summary explains it, the owner judges.
- **Commits**: the owner commits after every session (one commit per WP: code + summary +
  `results/WP-NN/` + STATUS/PLAN, message "Accept WP-NN: …" / "Reject WP-NN: …"); the tree
  must be clean before the next work session starts.

## Work packages (ranked; details in PLAN.md)

| Rank | ID | Title | Status | Result | Summary |
|---|---|---|---|---|---|
| – | WP-00 | Setup: infrastructure, baseline, analysis, plan | done (committed by the setup session) | – | [WP-00](summaries/WP-00-setup.md) |
| – | WP-01 | SG quadrature: per-grid-point work | accepted | quad/sg 2.4x (22.3 → 9.4 s), mivec 1.45x; 15.8 s saved; b1 d20 4.7x | [WP-01](summaries/WP-01-sg-quad-per-point.md) |
| – | WP-11 | Toolchain measurement: GCC flags, Clang ThinLTO, Clang full LTO | accepted (gcc-nosi → WP-13) | gcc-nosi e2e 1.024x (interp 1.065x, mivec 1.043x, quad 1.02x); Clang not adopted | [WP-11](summaries/WP-11-toolchain.md) |
| – | WP-13 | `-fno-semantic-interposition` in site_scons (GCC, `OPT=1`) | accepted | e2e 1.023x (54.03 → 52.79 s), interp 1.062x, 0 SLOWER; all 8 modules' tests pass | [WP-13](summaries/WP-13-nosi-scons.md) |
| – | WP-14 | `-fno-strict-aliasing` only for the SWIG Java wrapper; datadriven punning → `std::bit_cast` | accepted (owner 2026-10-28) | e2e 0.988x (52.77 → 53.42 s): interp 1.10x, interplin 1.10x, quad/sg 0.995x, quad/dsg 0.970x, mivec 0.886x; refcheck OK; complete project identical | [WP-14](summaries/WP-14-strict-aliasing.md) |
| 1 | RP-1 | Re-profiling after WP-01/WP-13/WP-14 | in progress (prompt 2026-10-28) | | |
| 2 | WP-02 | DSG quadrature: per-delta-tensor-grid overhead at high d | ready | | |
| 3 | WP-03 | Downwards closure of MI sets | ready | | |
| 4 | WP-09 | Multi-point interpolation API (proposal; from D-1) | ready | | |
| 5 | WP-04 | DSG quadrature: row loop | ready (after WP-02) | | |
| 6 | WP-05 | SG quadrature: per-tensor-grid costs | ready | | |
| 7 | WP-06 | Global interpolation: operator overhead | ready (re-assess after WP-09) | | |
| 8 | WP-07 | Generic barycentric formula: cache the weights | ready | | |
| 9 | WP-08 | Linear interpolation: corner loop | ready (re-assess after WP-09) | | |
| 10 | WP-10 | Do not link Armadillo when Eigen provides the FFT (from D-3; measure first) | ready | | |
| 11 | WP-12 | Toolchain re-measurement after WP-02/WP-03 (optional) | planned (owner decides) | | |
| – | D-1 | Multi-point interpolation API | decided → WP-09 | | |
| – | D-2 | `-fno-semantic-interposition` | done: WP-13 accepted | | |
| – | D-3 | Armadillo/OpenBLAS thread pool | decided → WP-10 | | |

Ranks 2–11 are re-ranked by RP-1.

## Next action

Owner: commit WP-14 ("Accept WP-14: …": `site_scons`/`jsgpp` part is already in 50d552f00; add
the datadriven fix, `results/WP-14/`, the summary, STATUS/PLAN), so the tree is clean. Then start
RP-1 (prompt 2026-10-28; `baseline` snapshot exists). Next master start: process the RP-1
summary, re-rank PLAN.md, issue the top optimization WP (currently WP-02). WP-06/WP-08 only after
the owner decided on adopting WP-09's API.

## Open decisions for the owner

- WP-09 (after its summary): adopt the multi-point interpolation API or not.
- WP-10 (after its summary): implement only if the measured start-up gain is worth it.
- WP-12 (after WP-02/WP-03): re-run the GCC/Clang comparison or not.

## Do not retry

(one line per rejected attempt or dropped variant: `WP-NN <idea>: <numbers>, <why>`)
- WP-00 noise: bootstrap over samples within rounds (3×2): 18/114 false positives, noise is
  between processes → paired per-round t-interval over many short processes.
- WP-00 refcheck: error-ratio tolerance above a 16-unit round-off floor missed 5x error growth near
  round-off → explicit `e_new ≤ ratio·e_ref + 4 units`.
- WP-00 noise model from plain std: throttling-onset rounds inflated it (up to 20 %) → MAD.
- WP-00 recording order: micro right after the 16-thread e2e run was 10–25 % slow (throttled) →
  idle settle, calibrate, micro, e2e.
- WP-01 rows always along dimension 0: b1 much slower (process_all_tgs/equi/b1/d20/L4 47.2 vs.
  27.9 ns/point, cc/b1/d10/L5 30.7 vs. 23.9), every row walks the leading 1-node dimensions →
  iterate only the multi-node dimensions.
- WP-01 row helpers reading scratch data per row (`QuadratureScratch&` parameters 2–7 % slower,
  local class with pointer members 4–9 % slower): the opaque `SourceFunc::evaluate` call forces
  reloads after every call → copy scratch data into locals before the hot loop.
- WP-11 `-fno-plt -Wl,-Bsymbolic-functions` on top of nosi: e2e 1.015x vs. 1.024x for nosi
  alone; GOT-indirect calls into libm/base/libgomp do not pay.
- WP-11 Clang ThinLTO: e2e 1.034x but quad/sg 0.964x, quad/dsg 0.980x, 14/38 SLOWER, refcheck
  WARN, 612 warnings → GCC kept (owner 2026-10-25); full LTO 1.018x (quadrature 0.94–0.95x);
  + nosi 1.033x (no effect).
- WP-11 Clang without explicit `CC/CXX/LINK`: SConstruct's configure checks run with g++, which
  rejects `-flto=thin` → pass `CC=clang CXX=clang++ LINK=clang++` (and `-O3` at link for lld).
- WP-14 TySan (Clang 21 `-fsanitize=type`) over SG++: OpenBLAS × shadow memory > 12 GB per binary
  (crashed the PC uncapped), runtime crash in `isAliasingLegal` aborted 21/74 combigrid suites,
  all report classes false positives (vptr, OpenMP bounds, mt19937, layout), ~3 h → use the
  `-Wstrict-aliasing=3/2` scan instead; if ever rerun: `OPENBLAS_NUM_THREADS=1` + cgroup cap.

## Re-profiling

Accepted changes since the last profile: 3 (WP-01, WP-13, WP-14). Trigger met: WP-01 changed
quad/sg by 2.4x (≥ 1.3x) and shifted its hot spots (integrand now ≈ 60 %); WP-14 shifted mivec
generation (0.864x) and the dsg trie (~0.95x). RP-1 in progress (prompt 2026-10-28). Last
profile: setup (ANALYSIS.md).

## Log

- 2026-10-28 WP-14 accepted by the owner (recorded in the summary; commit pending): flag only on
  `javaEnv`; datadriven's 8 punning sites → `std::bit_cast` (owner's choice; bitwise identical);
  complete project identical to HEAD. e2e 0.988x: operators gain, closure and dsg trie lose →
  notes in WP-03/WP-02. Pre-existing outside combigrid: `example_learnerSGDE.java`, thesis
  comparison templates (2 args), numpy only in `.venv`. RP-1 prompt issued.

## Archive

- 2026-10-24 setup (WP-00, fed2cdb4e); baseline re-recorded boost off (931158248); D-1 → WP-09,
  D-3 → WP-10, D-2 answered. 2026-10-25 WP-01 accepted (5281e0b7d); WP-11 processed (gcc-nosi →
  WP-13, Clang not adopted; strict aliasing researched → WP-14). 2026-10-26 WP-11 committed
  (9ab8f642d); WP-13 accepted (a16733d65); WP-14 issued (TySan step after the owner's nosi ×
  strict-aliasing question), checkpoint 50d552f00. Progress report for the owner's supervisor
  (`results/progress-2026-10-26/`, 044d2794d): 28 cases 51.1 s (master) → 41.9 s → 25.3 s (WP-13);
  mivec generation d15/d20 faster on master (1.56/1.66 s) than at baseline (1.79/1.90 s) → RP-1.
