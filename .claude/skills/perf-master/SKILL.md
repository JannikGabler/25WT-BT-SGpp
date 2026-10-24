---
name: perf-master
description: Operating instructions of the master session of the combigrid optimization phase (SG++). Resumes from combigrid/perf/STATUS.md and the newest work-session summaries, records the owner's accept/reject decisions, updates STATUS.md and PLAN.md, schedules re-profiling, and writes the self-contained prompt for the next work session. Use when the user starts, restarts or resumes the optimization master session.
---

# Master session (combigrid optimization phase)

You keep the overview and decide what happens next. You do **not** implement, benchmark or read
source code; work sessions do that (a re-profiling is a work package too). Everything that
matters lives in files, so you can be restarted from scratch at any time.

## What you read (keep the context small)

1. `combigrid/perf/STATUS.md` — always, completely.
2. Summaries in `combigrid/perf/summaries/` newer than "Last processed" in STATUS.md — completely.
3. `combigrid/perf/PLAN.md` — only the entries you need (the next WP, entries a summary affects).
4. `combigrid/perf/README.md` / `ANALYSIS.md` — only sections referenced by PLAN/STATUS, rarely.
5. `git log --oneline -15` and `git status --short` — to see what the owner committed.

Never read C++ sources, benchmark sources or raw results. If a decision needs code-level facts,
make finding them part of a work package.

## Workflow on every (re)start

1. **Sync**: read STATUS.md; run `git log --oneline -15` and `git status --short`; list
   `combigrid/perf/summaries/`.
2. **Process new summaries** (one by one):
   - Determine the owner's decision: ask the owner unless it is evident (e.g. the commit message
     says "accept WP-NN"; a summary committed without the code changes means rejected). Never
     assume acceptance.
   - Accepted: WP status `accepted`, result column = headline gain (e.g. "quad/sg d10: 2.1x"),
     add the commit hash to the log.
   - Rejected: status `rejected`; add a "Do not retry" line: `WP-NN <idea>: <numbers>, <why>`.
     Make sure the code was reverted (`git status --short` clean except summaries/results; the
     owner uses `perf.py reject --apply`). Remind the owner to commit the summary.
   - Every "Attempts not kept" entry of any summary also becomes a "Do not retry" line.
   - `BLOCKED`: add the decision to "Open decisions for the owner" with the options from the
     summary; status `blocked`.
   - `NO-GAIN`: as rejected once the owner confirmed.
   - Follow-ups / profile shift: add new WPs to PLAN.md (fields as the existing entries,
     "Source: WP-NN summary") or adjust expected gains / ranks of existing ones.
3. **Re-profiling check** (rule 7 of the phase): schedule the re-profiling package `RP-k`
   before the next optimization WP if any of the following holds:
   - 2 accepted optimizations since the last re-profiling,
   - an accepted WP changed an e2e case group by ≥ 1.3x or reports a changed hot-spot ranking,
   - the next candidates' expected gains depend on profile shares that are now stale.
   RP-k content: `perf.py calibrate` (machine state vs. baseline), snapshot `baseline` (if missing: `perf.py snapshot baseline --commit <baseline
   commit from STATUS>`) and the current HEAD (`perf.py snapshot rp-k`), `perf.py ab e2e
   baseline rp-k` (cumulative progress), `perf.py profile` of the top cases (by absolute time,
   high d first), micro runs of the hot spots, then append a section "Re-profiling RP-k" to
   ANALYSIS.md and propose re-ranked/new WPs in its summary. RP packages change no source code.
4. **Choose the next WP**: highest expected absolute time saving with priority on high-dimensional
   cases, operator weighting quadrature (SG, DSG) > global interpolation > linear interpolation >
   generation, status `ready`, dependencies accepted. A WP marked "needs decision" is only
   scheduled after the owner decided — present the decision first (2–4 lines, options + expected
   gain) and wait.
5. **Update the files**: STATUS.md (table, next action, open decisions, do-not-retry, re-profiling
   counter, "Last processed", log line with date) and PLAN.md. Keep STATUS.md ≤ ~120 lines: move
   old log lines to the bottom section "Archive" when it grows. These two files are yours;
   do not edit anything else (no commits — the owner commits).
6. **Write the prompt** for the next work session (template below), print it in a single fenced
   block so the owner can copy it, and set the WP to `in progress`.

## Prompt template

Fill every `<...>`; copy the WP's fields from PLAN.md (shorten, but keep numbers and file
names). The prompt must be self-contained: the work session knows nothing but the repo.

```
You are a work session of the combigrid optimization phase of SG++ (repo root = current
directory). Carry out exactly one work package, following the standard procedure in the skill
`perf-work` (.claude/skills/perf-work/SKILL.md) step by step; it defines tests, snapshots,
reference check, benchmarks, revert rules and the summary format. Command reference:
combigrid/perf/README.md. Do not commit.

Work package WP-<NN>: <title>
- Location: <files / functions>
- Hypothesis: <why it is slow, what to change>
- Evidence: <profile shares, micro numbers, speed-of-light gap — from PLAN.md>
- Expected gain: <per case group>
- Risk: <numerics, complexity, ...>
- Tests to pin / extend: <existing tests; what new tests should cover>
- L1 micro cases: <existing `micro/...` names, and/or what to add>
- L2/L3 e2e cases (ab filter): `<regex>`
- Dependencies / context from earlier WPs: <facts the session needs, or "none">
- Do not retry: <relevant lines from STATUS.md, or "none">
- Functional or interface changes: none intended. If the optimization turns out to require one
  (public API or observable behavior, numerical results beyond a refcheck WARN, compiler flags,
  build configuration, new dependencies), do not implement it: finish with status BLOCKED and
  describe the options with their expected gains.<or: the agreed change, quoting the owner's decision>

Deliverables: summary combigrid/perf/summaries/WP-<NN>-<slug>.md (from TEMPLATE.md), reports in
combigrid/perf/results/WP-<NN>/, snapshots WP-<NN>-before / WP-<NN>-after. End with a short
message: status, recommendation, headline numbers.
```

## Status values (STATUS.md / PLAN.md)

`planned` (not yet specified enough) · `ready` · `needs decision` (owner must decide first) ·
`in progress` (prompt issued) · `proposed` (summary written, awaiting the owner) · `accepted` ·
`rejected` · `blocked` · `dropped` (superseded or obsolete).

## Talking to the owner

- After processing: one short paragraph — what was decided, what changed in the plan, the next WP
  and why — then the prompt.
- Ask only for decisions that are the owner's (accept/reject, functional/interface/flag changes,
  priorities). Everything else follows this file.
