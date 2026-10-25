# Optimization status — combigrid (master file)

Last processed: WP-01 (2026-10-25) · Next free WP number: 12 · Re-profilings done: 0 (RP-0 = setup)

- **Judged**: `perf.py build normal` (= compileNormal.sh + `USE_EIGEN=1 USE_ARMADILLO=1`),
  16 threads (`OMP_PROC_BIND=close`, `OMP_PLACES=threads`), **CPU boost off** (all cores at
  2.0 GHz), platform profile `performance`, AC, idle machine.
- **Baseline**: source of commit 84299eeca (unchanged by the setup); baseline commit = the setup
  commit **fed2cdb4e** (found with
  `git log --diff-filter=A --format=%h -- combigrid/perf/baseline/e2e_t16.jsonl`).
  Snapshot name `baseline` (rebuild with `perf.py snapshot baseline --commit fed2cdb4e` if
  `.work/` was cleaned). Numbers: `baseline/`, overview in ANALYSIS.md.
- **Machine note**: boost was disabled and the baseline re-recorded on 2026-10-24 because the
  laptop throttled under sustained load with boost on (up to 1.6x). Boost comes back after a
  reboot; the owner disables it manually (decided 2026-10-24). If `envcheck` fails because of
  boost, ask the owner to disable it again (README, Pitfalls). A full e2e A/B took ≈ 23 min at
  baseline (less after WP-01); work sessions filter to relevant cases.
- **Toolchain on the machine**: GCC 15.2 (judged); Clang 21.1.8 with `libomp-21-dev`, `lld-21`
  (checked 2026-10-25: OpenMP + full/thin LTO test program OK; runtime loads `libomp5` 22.1.2).
- **Owner's rules**: acceptance by the owner only; minimal gains may be proposed but must be
  labelled and weighed against API/functionality/maintainability/readability; functional/
  interface/flag changes only after consulting the owner; errors stay in the same order of
  magnitude (refcheck OK/WARN/FAIL); high dimensions first.
- **Commits**: the owner commits after every session (one commit per WP: code + summary +
  `results/WP-NN/` + STATUS/PLAN, message "Accept WP-NN: …" / "Reject WP-NN: …"); the tree
  must be clean before the next work session starts.

## Work packages (ranked; details in PLAN.md)

| Rank | ID | Title | Status | Result | Summary |
|---|---|---|---|---|---|
| – | WP-00 | Setup: infrastructure, baseline, analysis, plan | done (committed by the setup session) | – | [WP-00](summaries/WP-00-setup.md) |
| – | WP-01 | SG quadrature: per-grid-point work | accepted | quad/sg 2.4x (22.3 → 9.4 s), mivec 1.45x; 15.8 s saved; b1 d20 4.7x | [WP-01](summaries/WP-01-sg-quad-per-point.md) |
| 1 | WP-11 | Toolchain measurement: GCC flags, Clang ThinLTO, Clang full LTO | in progress (prompt 2026-10-25) | | |
| 2 | RP-1 | Re-profiling after WP-01 | ready (after WP-11) | | |
| 3 | WP-02 | DSG quadrature: per-delta-tensor-grid overhead at high d | ready | | |
| 4 | WP-03 | Downwards closure of MI sets | ready | | |
| 5 | WP-09 | Multi-point interpolation API (proposal; from D-1) | ready | | |
| 6 | WP-04 | DSG quadrature: row loop | ready (after WP-02) | | |
| 7 | WP-05 | SG quadrature: per-tensor-grid costs | ready | | |
| 8 | WP-06 | Global interpolation: operator overhead | ready (re-assess after WP-09) | | |
| 9 | WP-07 | Generic barycentric formula: cache the weights | ready | | |
| 10 | WP-08 | Linear interpolation: corner loop | ready (re-assess after WP-09) | | |
| 11 | WP-10 | Do not link Armadillo when Eigen provides the FFT (from D-3; measure first) | ready | | |
| – | D-1 | Multi-point interpolation API | decided → WP-09 | | |
| – | D-2 | `-fno-semantic-interposition` | measured in WP-11 | | |
| – | D-3 | Armadillo/OpenBLAS thread pool | decided → WP-10 | | |

Ranks 3–11 are re-ranked by RP-1.

## Next action

WP-11 issued (2026-10-25, owner: next). On the next start: fill in WP-01's commit hash (log),
then process the WP-11 summary. Then RP-1 (due: WP-01 changed quad/sg by 2.4x; it profiles the
configuration judged after the WP-11 decision), re-rank, then the top optimization WP
(currently WP-02). WP-06/WP-08 only after the owner decided on adopting WP-09's API.

## Open decisions for the owner

- WP-11 (after its summary): change the judged toolchain/flags or not (judged-configuration
  change; baseline re-recording is owner-only).
- WP-09 (after its summary): adopt the multi-point interpolation API or not.
- WP-10 (after its summary): implement only if the measured start-up gain is worth it.

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

## Re-profiling

Accepted optimizations since the last profile: 1 (WP-01). Trigger met: WP-01 changed the quad/sg
group by 2.4x (≥ 1.3x) and shifted its hot spots (integrand now ≈ 60 %). RP-1 is due before the
next optimization WP; scheduled right after WP-11 (measurement only, owner's ordering).
Last profile: setup (ANALYSIS.md).

## Log

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
- 2026-10-25 WP-01 accepted by the owner (commit: pending, fill in): quad/sg 22.3 → 9.4 s, mivec
  9.3 → 6.4 s, refcheck 104/104 OK; follow-ups added to WP-04/05/06/08/10 and D-2; RP-1 triggered.
- 2026-10-25 owner: toolchain measurement WP-11 (GCC flags, Clang ThinLTO, Clang full LTO),
  scheduled next; Clang toolchain checked; WP-11 prompt issued.
