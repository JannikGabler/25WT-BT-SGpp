"""Merges e2e medians of four states into one Markdown table.

usage (from the repo root): python3 combigrid/perf/results/progress-2026-10-26/make_report.py RESULTS_DIR BASELINE_JSONL WP13_AB_JSON OUT_MD HEADER_MD NOTES_MD
RESULTS_DIR holds run-master.json, run-dev.json (perf.py run) and run-dev-dsg.jsonl (timeout probes).
"""
import json
import statistics
import sys

res, base_jsonl, wp13_json, out_md, header_md, notes_md = sys.argv[1:7]
METRICS = ["total", "operator", "generation"]
COLS = ["master (thesis)", "e6a21c331 (first DSG)", "baseline (before WP-01)", "after WP-13"]


class Bound:
    """Lower bound of a runtime that was not measured to the end."""

    def __init__(self, seconds, text):
        self.s, self.text = seconds, text


def fmt(v):
    if v is None:
        return "-"
    if isinstance(v, Bound):
        return v.text
    if v >= 1:
        return f"{v:.3f}s"
    if v >= 1e-3:
        return f"{v * 1e3:.2f}ms"
    return f"{v * 1e6:.2f}us"


def err(r):
    return abs(r["result"] - r["exact"]) / abs(r["exact"]) if r.get("exact") else None


def from_records(recs):
    """Median per case over all samples of all given records (several runs: one record each)."""
    samples, out = {}, {}
    for r in recs:
        per = samples.setdefault(r["case"], {m: [] for m in METRICS})
        for m in METRICS:
            per[m] += r["metrics"][m]
        out[r["case"]] = {"_err": err(r)}
    for case, per in samples.items():
        out[case].update({m: statistics.median(v) for m, v in per.items()})
    return out


def runs(*names):
    recs = []
    for name in names:
        try:
            recs += json.load(open(f"{res}/{name}"))["records"]
        except FileNotFoundError:
            pass
    return recs


def from_ab(path, side):
    out = {}
    for key, recs in json.load(open(path))["data"].items():
        s, case = key.split("|", 1)
        if s == side:
            out[case] = {m: statistics.median([x for r in recs for x in r["metrics"][m]])
                         for m in METRICS}
            out[case]["_err"] = err(recs[-1])
    return out


master = from_records(runs("run-master.json", "run-master-2.json"))
dev = from_records(runs("run-dev.json", "run-dev-2.json"))
for line in open(f"{res}/run-dev-dsg.jsonl"):
    probe = json.loads(line)
    if probe["rc"] == 0:
        text = probe["out"].strip()
        recs = json.loads(text) if text.startswith("[") else [json.loads(x) for x in text.splitlines() if x.startswith("{")]
        dev.update(from_records(recs))
    else:  # killed by `timeout 45`
        b = Bound(45, "> 45 s (timeout)")
        dev[probe["case"]] = {"total": b, "operator": Bound(45, "> 45 s"), "generation": None,
                              "_err": None}
# stopped A/B: one process of this case ran 521 s for warm-up + sample without finishing
dev["quad/dsg/equi-exp/b0/d7/L10"] = {"total": Bound(260, "> 4.3 min"),
                                      "operator": Bound(260, "> 4.3 min"), "generation": None,
                                      "_err": None}
# not measurable on master / e6a21c331 (naive O(n^2) CC weights, see notes)
for c in (master, dev):
    c["quad/sg/cc-uq/b0/d2/L20"] = {"total": Bound(42 * 60, "≥ 42 min (est.)"),
                                    "operator": Bound(42 * 60, "≥ 42 min (est.)"),
                                    "generation": None, "_err": None}
cols = [master, dev, from_records([json.loads(x) for x in open(base_jsonl)]), from_ab(wp13_json, "B")]
order = [line.split("|")[1].strip() for line in open(wp13_json[:-5] + ".md")
         if line.startswith("| ") and "| total |" in line]


def speedup(vals):
    first = next(v for v in vals if v is not None)
    if isinstance(first, Bound):
        return f"> {first.s / vals[3]:.0f}x" if first.s / vals[3] >= 10 else f"> {first.s / vals[3]:.1f}x"
    return f"{first / vals[3]:.2f}x"


lines = [open(header_md).read().rstrip(), "",
         "| case | metric | " + " | ".join(COLS) + " | speedup (first → WP-13) |",
         "|---|---|---|---|---|---|---|"]
for case in order:
    for m in METRICS:
        vals = [c.get(case, {}).get(m) for c in cols]
        if m == "generation" and any(isinstance(c.get(case, {}).get("total"), Bound) for c in cols[:2]):
            vals = [("n/m" if isinstance(c.get(case, {}).get("total"), Bound) else c.get(case, {}).get(m))
                    for c in cols]
            cells = [v if isinstance(v, str) else fmt(v) for v in vals]
            first = next(v for v in vals if v is not None)
            sp = "-" if isinstance(first, str) else speedup(vals)
        else:
            cells = [fmt(v) for v in vals]
            sp = speedup(vals)
        lines.append(f"| {case} | {m} | " + " | ".join(cells) + f" | {sp} |")


def total_sum(c, cases):
    if any(case not in c for case in cases):
        return None
    vals = [c[case]["total"] for case in cases]
    s = sum(v.s if isinstance(v, Bound) else v for v in vals)
    return Bound(s, f"> {fmt(s)}") if any(isinstance(v, Bound) for v in vals) else s


lines += ["", "Sums of the `total` medians per case group (`quad/sg` without the estimated "
          "`cc-uq/b0/d2/L20`; `>`: contains lower bounds; `quad/dsg` does not exist on master):", "",
          "| group | cases | " + " | ".join(COLS) + " | speedup (first → WP-13) |",
          "|---|---|---|---|---|---|---|"]
groups = {}
for case in order:
    if case != "quad/sg/cc-uq/b0/d2/L20":
        groups.setdefault("/".join(case.split("/")[:2]), []).append(case)
for g, cases in groups.items():
    sums = [total_sum(c, cases) for c in cols]
    lines.append(f"| {g} | {len(cases)} | " + " | ".join(fmt(s) for s in sums) + f" | {speedup(sums)} |")
# everything except quad/sg/cc-uq/b0/d2/L20 and quad/dsg: measured exactly in all four states
common = [c for c in order if not c.startswith("quad/dsg/") and c != "quad/sg/cc-uq/b0/d2/L20"]
sums = [total_sum(c, common) for c in cols]
lines.append(f"| **all except quad/dsg and quad/sg/cc-uq/b0/d2/L20** | {len(common)} | " +
             " | ".join(f"**{fmt(s)}**" for s in sums) + f" | **{speedup(sums)}** |")

lines += ["", open(notes_md).read().rstrip(), "", "<details><summary>Relative errors vs. the exact "
          "value (last measured process of each state; the binding accuracy check is "
          "<code>refcheck</code>)</summary>", "",
          "| case | " + " | ".join(COLS) + " |", "|---|---|---|---|---|"]
for case in order:
    errs = [c.get(case, {}).get("_err") for c in cols]
    lines.append(f"| {case} | " + " | ".join("-" if e is None else f"{e:.2e}" for e in errs) + " |")
lines += ["", "</details>"]

open(out_md, "w").write("\n".join(lines) + "\n")
print("\n".join(lines))
