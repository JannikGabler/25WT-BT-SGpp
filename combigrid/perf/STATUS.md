# Optimization status — combigrid (master file)

Last processed: WP-11 (2026-10-25) · Next free WP number: 14 · Re-profilings done: 0 (RP-0 = setup)

- **Judged**: `perf.py build normal` (= compileNormal.sh + `USE_EIGEN=1 USE_ARMADILLO=1`), 16
  threads (`OMP_PROC_BIND=close`, `OMP_PLACES=threads`), **CPU boost off** (all cores at
  2.0 GHz), platform profile `performance`, AC, idle machine. GCC 15.2 stays the judged compiler.
  `-fno-semantic-interposition` comes via site_scons (WP-13: GCC, `OPT=1`), not via
  compileNormal.sh/perf.py; owner's decision 2026-10-25.
- **Baseline**: source of commit 84299eeca (unchanged by the setup); baseline commit = the setup
  commit **fed2cdb4e** (found with
  `git log --diff-filter=A --format=%h -- combigrid/perf/baseline/e2e_t16.jsonl`).
  Snapshot name `baseline` (rebuild with `perf.py snapshot baseline --commit fed2cdb4e` if
  `.work/` was cleaned). Numbers: `baseline/`, overview in ANALYSIS.md. The baseline stays
  without `-fno-semantic-interposition` (built from fed2cdb4e with its own site_scons); the
  flag's gain counts towards the cumulative progress (WP-13). No re-recording.
- **Machine note**: boost was disabled and the baseline re-recorded on 2026-10-24 because the
  laptop throttled under sustained load with boost on (up to 1.6x). Boost comes back after a
  reboot; the owner disables it manually (decided 2026-10-24). If `envcheck` fails because of
  boost, ask the owner to disable it again (README, Pitfalls). A full e2e A/B takes ≈ 18–23 min;
  work sessions filter to relevant cases. `scons -c` (clears all of `lib/`, incl. other modules'
  libraries and `lib/pysgpp`) is fine: everything there is rebuildable (owner 2026-10-25).
- **Toolchain**: GCC 15.2 (judged); Clang 21.1.8 + libomp + lld installed. `perf/scripts/toolchain.py`
  (WP-11) builds/snapshots compiler/flag variants; it preserves `lib/` across its `scons -c`.
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
| 1 | WP-13 | `-fno-semantic-interposition` in site_scons (GCC, `OPT=1`) | in progress (prompt 2026-10-25) | | |
| 2 | RP-1 | Re-profiling after WP-01/WP-13 | ready (after WP-13) | | |
| 3 | WP-02 | DSG quadrature: per-delta-tensor-grid overhead at high d | ready | | |
| 4 | WP-03 | Downwards closure of MI sets | ready | | |
| 5 | WP-09 | Multi-point interpolation API (proposal; from D-1) | ready | | |
| 6 | WP-04 | DSG quadrature: row loop | ready (after WP-02) | | |
| 7 | WP-05 | SG quadrature: per-tensor-grid costs | ready | | |
| 8 | WP-06 | Global interpolation: operator overhead | ready (re-assess after WP-09) | | |
| 9 | WP-07 | Generic barycentric formula: cache the weights | ready | | |
| 10 | WP-08 | Linear interpolation: corner loop | ready (re-assess after WP-09) | | |
| 11 | WP-10 | Do not link Armadillo when Eigen provides the FFT (from D-3; measure first) | ready | | |
| 12 | WP-12 | Toolchain re-measurement after WP-02/WP-03 (optional) | planned (owner decides) | | |
| – | D-1 | Multi-point interpolation API | decided → WP-09 | | |
| – | D-2 | `-fno-semantic-interposition` | decided 2026-10-25: site_scons, GCC, `OPT=1` → WP-13 | | |
| – | D-3 | Armadillo/OpenBLAS thread pool | decided → WP-10 | | |

Ranks 3–12 are re-ranked by RP-1.

## Next action

Owner: commit WP-11 (summary, `results/WP-11/`, `toolchain.py`, STATUS/PLAN), then start WP-13
(prompt issued 2026-10-25; the earlier RP-1 prompt with baseline re-recording is void). Next
master start: fill in WP-11's commit hash, process the WP-13 summary, then RP-1 (no baseline
step), re-rank, then the top optimization WP (currently WP-02). WP-06/WP-08 only after the
owner decided on adopting WP-09's API.

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

## Re-profiling

Accepted optimizations since the last profile: 1 (WP-01); WP-13 (flag) in progress. Trigger
met: WP-01 changed quad/sg by 2.4x (≥ 1.3x) and shifted its hot spots (integrand now ≈ 60 %).
RP-1 runs right after WP-13, so it profiles the code with the flag. Last profile: setup
(ANALYSIS.md).

## Log

- 2026-10-25 WP-01 accepted by the owner (commit 5281e0b7d): quad/sg 22.3 → 9.4 s, mivec
  9.3 → 6.4 s, refcheck 104/104 OK; follow-ups added to WP-04/05/06/08/10 and D-2; RP-1 triggered.
- 2026-10-25 owner: toolchain measurement WP-11 (GCC flags, Clang ThinLTO, Clang full LTO),
  scheduled next; Clang toolchain checked; WP-11 prompt issued.
- 2026-10-25 WP-11 processed. Owner: adopt gcc-nosi in the judged configuration only (not in
  site_scons; owner edits compileNormal.sh + perf.py, baseline re-recorded in RP-1), keep
  `toolchain.py`, refcheck tolerance of the WARN case stays. Commit: pending (fill in). Side
  effect reported: `scons -c` had deleted `lib/`; other module libs restored, `lib/pysgpp`
  rebuilt from HEAD (judged config + `SG_PYTHON=1`). Follow-ups added to WP-02/03/06/07; WP-12
  (optional toolchain re-run) planned. RP-1 prompt issued.
- 2026-10-25 owner: plain `scons -c` is not a problem (only rebuildable files; the owner runs
  nothing during sessions). Owner asked whether to add the flag via SCons (OPT=1) instead;
  answered (safe in the GNU path; baseline stays valid, no protected edits) → owner: switch to
  WP-13 (site_scons, GCC, `OPT=1`, not win32/cygwin, all modules). RP-1 prompt voided, WP-13
  prompt issued.

## Archive

- 2026-10-24 WP-00 setup finished: perf suite, baseline, reference, noise model, analysis, plan.
- 2026-10-24 boost disabled by the owner; baseline, calibration and noise model re-recorded
  (boost off), ANALYSIS/PLAN numbers updated; reference results unchanged (931158248).
- 2026-10-24 master start: WP-00 processed (setup commit fed2cdb4e, no decision needed);
  STATUS/PLAN consistent with the summary; D-1/D-2/D-3 and boost persistence presented to the
  owner; WP-01 prompt issued.
- 2026-10-24 owner: D-1 approved as WP-09 (API adoption decided after the results), D-3 "sure" →
  WP-10, boost toggled manually by the owner; D-2 effects explained, still open. WP-09 ranked 4
  (after WP-03: gain only for new-API callers), WP-10 last.
- 2026-10-24 owner asked about `-fPIC` (answered; variants added to D-2 in PLAN.md). WP-10: keep,
  measure first, owner decides after the results.
