#!/usr/bin/env python3
"""WP-76: history table of the expensive/ cases (median over the rounds of run_history.sh)."""
import glob, json, os, re, statistics
HERE = os.path.dirname(os.path.abspath(__file__))
STATES = ["baseline", "rp-1", "rp-2", "rp-3", "rp-4", "rp-5", "head"]
data, foreign = {}, []
for path in glob.glob(os.path.join(HERE, "runs", "*.json")):
    state = re.match(r"(.+)-r\d+\.json", os.path.basename(path)).group(1)
    run = json.load(open(path))
    for rec in run["records"]:
        data.setdefault(rec["case"], {}).setdefault(state, []).append(statistics.median(rec["metrics"]["total"]))
cases = sorted(data, key=lambda c: (c.endswith("-cached"), "-ramp" in c, "c1us" in c))
fmt = lambda t: "–" if t is None else (f"{t:.3f}" if t >= 1 else f"{t:.3f}")
print("Total time per case [s], 16 threads, median over the rounds (spread max/min − 1 of the rounds in the last column for head); ratio = baseline (or first state) / head.\n")
print("| case | " + " | ".join(STATES) + " | first/head | head spread |")
print("|---|" + "---|" * (len(STATES) + 2))
for case in cases:
    med = {s: statistics.median(v) for s, v in data[case].items()}
    first = next(s for s in STATES if s in med)
    h = data[case].get("head", [])
    spread = f"{max(h) / min(h) - 1:.1%}" if len(h) > 1 else "–"
    print(f"| {case[len('expensive/'):]} | " + " | ".join(fmt(med.get(s)) for s in STATES) +
          f" | {med[first] / med['head']:.2f}x ({first}) | {spread} |")

common = [c for c in cases if all(s in data[c] for s in STATES)]
late = [c for c in cases if "rp-4" in data[c] and c not in common]
for label, sel, states in (("cases present at all states", common, STATES),
                           ("cases from rp-4 on", late, STATES[4:])):
    tot = {s: sum(statistics.median(data[c][s]) for c in sel) for s in states}
    print(f"\nSum over {len(sel)} {label}: " + ", ".join(f"{s} {tot[s]:.2f} s" for s in states))
