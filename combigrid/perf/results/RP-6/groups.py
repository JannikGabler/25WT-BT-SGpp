#!/usr/bin/env python3
"""Per-group progress for RP-6 (adapted from RP-5/groups.py).

Usage: groups.py CUM.json DIRECT.json [EXP.json]
  CUM.json    = `ab e2e baseline rp-6` (A = baseline, B = rp-6; full suite)
  DIRECT.json = `ab e2e rp-5 rp-6`     (A = rp-5,     B = rp-6; same session)
  EXP.json    = optional `ab e2e hist-rp-5 rp-6 --filter '^expensive/'` (rp-5 libraries with the
                WP-76 benchmark block, see results/WP-76/history/)
Groups: quad/sg, quad/dsg (b0, b1), interp/sg, interp/dsg, interplin/sg, interplin/dsg, mivec,
expensive (judged), expensive cached. The DSG interpolation and expensive cases have no baseline
values (absolute times and shares only). The suite share and the "all" line exclude expensive/.
"""
import json
import math
import re
import statistics
import sys


def table(path):
    out = {}
    for key, recs in json.load(open(path))["data"].items():
        side, case = key.split("|", 1)
        out.setdefault(case, {})[side] = recs
    return out


def med(recs, metric="total"):
    return statistics.median(r["metrics"][metric][0] for r in recs)


def rss(recs):
    return statistics.median(r["peak_rss_kib"] for r in recs) / 1024


def group(case):
    p = case.split("/")
    if p[0] == "expensive":
        return "expensive cached" if case.endswith("-cached") else "expensive"
    return "mivec" if p[0] == "mivec" else f"{p[0]}/{p[1]}"


def dim(case):
    return int(re.search(r"/d(\d+)/", case).group(1))


C, D = table(sys.argv[1]), table(sys.argv[2])
E = table(sys.argv[3]) if len(sys.argv) > 3 else {}
groups = ["quad/sg", "quad/dsg", "interp/sg", "interp/dsg", "interplin/sg", "interplin/dsg",
          "mivec"]
keys = groups + ["common", "all", "expensive", "expensive cached"]
agg = {g: dict(a=0, b=0, r5=0, b5=0, a7=0, b7=0, n=0, lr=[], lr5=[]) for g in keys}
for case, s in C.items():
    g = group(case)
    b = med(s["B"])
    a = med(s["A"]) if "A" in s else None
    d = D.get(case)
    if d is None or "A" not in d:
        d = E.get(case)
    exp = g.startswith("expensive")
    for key in (g,) + (() if exp else ("all",)) + (("common",) if a is not None else ()):
        x = agg[key]
        x["n"] += 1
        x["b"] += b
        if a is not None:
            x["a"] += a
            x["lr"].append(math.log(rss(s["B"]) / rss(s["A"])))
            if dim(case) >= 7:
                x["a7"] += a
                x["b7"] += b
        if d is not None and "A" in d:
            x["r5"] += med(d["A"])
            x["b5"] += med(d["B"])
            x["lr5"].append(math.log(rss(d["B"]) / rss(d["A"])))
total = agg["all"]["b"]
print("| group | cases | baseline | rp-5 (direct A) | rp-6 (direct B) | rp-6 (cum. B) "
      "| baseline → rp-6 | rp-5 → rp-6 | d ≥ 7: baseline → rp-6 | peak RSS geomean baseline → rp-6 "
      "| rp-5 → rp-6 | share of rp-6 suite |")
print("|---|---|---|---|---|---|---|---|---|---|---|---|")
for g in keys:
    x = agg[g]
    sp = f"{x['a'] / x['b']:.3f}x" if x["a"] else "–"
    s5 = f"{x['r5'] / x['b5']:.3f}x" if x["b5"] else "–"
    d7 = f"{x['a7']:.2f} → {x['b7']:.2f} s ({x['a7'] / x['b7']:.2f}x)" if x["a7"] else "–"
    gm = f"{math.exp(statistics.mean(x['lr'])):.3f}" if x["lr"] else "–"
    gm5 = f"{math.exp(statistics.mean(x['lr5'])):.3f}" if x["lr5"] else "–"
    sh = "–" if g.startswith("expensive") else f"{100 * x['b'] / total:.1f} %"
    print(f"| {g} | {x['n']} | {x['a']:.3f} s | {x['r5']:.3f} s | {x['b5']:.3f} s | {x['b']:.3f} s "
          f"| {sp} | {s5} | {d7} | {gm} | {gm5} | {sh} |")
b0 = sum(med(s["B"]) for c, s in C.items() if c.startswith("quad/dsg") and "/b0/" in c)
b1 = sum(med(s["B"]) for c, s in C.items() if c.startswith("quad/dsg") and "/b1/" in c)
print(f"\nquad/dsg b0 {b0:.3f} s ({100*b0/total:.1f} %), b1 {b1:.3f} s ({100*b1/total:.1f} %)")
print("\n| case | rp-5 | rp-6 | rp-5 → rp-6 (direct, ratio of medians) | gen rp-6 | op rp-6 "
      "| peak RSS baseline → rp-5 → rp-6 |")
print("|---|---|---|---|---|---|---|")
for case, s in sorted(C.items(), key=lambda kv: -med(kv[1]["B"])):
    d = D.get(case)
    if d is None or "A" not in d:
        d = E.get(case)
    has = d is not None and "A" in d
    b = med(s["B"])
    r5 = med(d["A"]) if has else None
    b5 = med(d["B"]) if has else None
    ra = f"{rss(s['A']):.1f} → " if "A" in s else ""
    rr = f"{rss(d['A']):.1f} → " if has else ""
    print(f"| {case} | {f'{r5:.3f}' if r5 else '–'} | {b:.3f} | {f'{r5 / b5:.3f}x' if has else '–'} "
          f"| {med(s['B'], 'generation') * 1e3:.2f} ms | {med(s['B'], 'operator'):.3f} "
          f"| {ra}{rr}{rss(s['B']):.1f} MiB |")
