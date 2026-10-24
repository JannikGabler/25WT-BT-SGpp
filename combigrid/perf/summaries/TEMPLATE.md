# WP-NN — <title>

Status: PROPOSED | NO-GAIN | BLOCKED | ABORTED · Date: YYYY-MM-DD · Base commit: <short rev>
Recommendation: accept | reject | owner decision needed — <one sentence why>
Machine: `perf.py calibrate` ratios <e2e x.xxx, micro x.xxx> (1.000 = as at baseline time)

## Change
- <what was changed, where (file: function), why it is faster — 1–4 bullets>
- Files: <list of changed/added files>

## Gain vs. cost
- Gain: <headline: e2e speedups of the main cases at 16 threads, absolute time saved; say
  explicitly if the gain is minimal or within noise>
- API / functionality: none | <description — must have been agreed with the owner>
- Maintainability / readability: <added complexity, code size, clarity>

## Tests
- Pinning tests (passed on the unchanged code before the change): <test names>
- New / extended tests: <test names>
- Results: debug `test --repeat 3` ✓/✗ · normal ✓/✗ · ASan ✓/✗/n.a.

## Reference check
- `perf.py refcheck`: OK | INFO | WARN | FAIL (1 and 16 threads) — <for INFO/WARN: cases,
  error growth, justification>

## Benchmarks (A = WP-NN-before, B = WP-NN-after; speedup = A/B, > 1 is faster)
L1 micro (1 thread):

| case | A | B | speedup | 95% CI |
|---|---|---|---|---|

L2/L3 e2e (16 threads; `total` and the changed phase):

| case | metric | A | B | speedup | 95% CI |
|---|---|---|---|---|---|

Sum of medians (compared cases): A = …, B = …, saved = … · Full reports: `combigrid/perf/results/WP-NN/`

## Attempts not kept
- <variant: numbers: why dropped> | none

## Profile shift / follow-ups
- <new top hot spots after the change; ideas for PLAN.md; one line each>

## Revert
`python3 combigrid/perf/scripts/perf.py reject --apply` (keeps `summaries/` and `results/`;
add `--keep <path>` to keep e.g. new tests)
