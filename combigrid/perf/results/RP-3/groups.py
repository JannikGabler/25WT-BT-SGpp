#!/usr/bin/env python3
"""Per-group cumulative progress from an `ab e2e baseline rp-3` JSON (+ rp-2 numbers of RP-2)."""
import json
import math
import re
import statistics
import sys

cur = json.load(open(sys.argv[1]))
prev = json.load(open(sys.argv[2]))  # RP-2 e2e.json (B = rp-2)


def med(recs, metric):
    return statistics.median(r["metrics"][metric][0] for r in recs)


def rss(recs):
    return statistics.median(r["peak_rss_kib"] for r in recs) / 1024


def table(d):
    out = {}
    for key, recs in d["data"].items():
        side, case = key.split("|", 1)
        out.setdefault(case, {})[side] = recs
    return out


C = table(cur)
P = table(prev)


def group(case):
    op, grid = case.split("/")[:2]
    return "mivec" if op == "mivec" else ("interplin" if op == "interplin" else
                                          ("interp" if op == "interp" else f"quad/{grid}"))


def dim(case):
    return int(re.search(r"/d(\d+)/", case).group(1))


groups = ["quad/sg", "quad/dsg", "interp", "interplin", "mivec"]
rows = []
for case, s in C.items():
    a, b = med(s["A"], "total"), med(s["B"], "total")
    ratios = [ra["metrics"]["total"][0] / rb["metrics"]["total"][0]
              for ra, rb in zip(s["A"], s["B"])]
    sp = math.exp(statistics.mean(math.log(r) for r in ratios))
    p2 = med(P[case]["B"], "total") if case in P else float("nan")
    rows.append(dict(case=case, g=group(case), d=dim(case), a=a, b=b, sp=sp, p2=p2,
                     gen=med(s["B"], "generation"), op=med(s["B"], "operator"),
                     ra=rss(s["A"]), rb=rss(s["B"]),
                     rp2=rss(P[case]["B"]) if case in P else float("nan")))

print("| group | cases | baseline | rp-2 (RP-2 report) | rp-3 | baseline → rp-3 | rp-2 → rp-3 |"
      " d ≥ 7: baseline → rp-3 | peak RSS geomean B/A | rp-2 → rp-3 RSS |")
print("|---|---|---|---|---|---|---|---|---|---|")
tot = dict(a=0, b=0, p2=0, a7=0, b7=0)
for g in groups + ["all"]:
    rs = [r for r in rows if g == "all" or r["g"] == g]
    a = sum(r["a"] for r in rs)
    b = sum(r["b"] for r in rs)
    p2 = sum(r["p2"] for r in rs)
    a7 = sum(r["a"] for r in rs if r["d"] >= 7)
    b7 = sum(r["b"] for r in rs if r["d"] >= 7)
    gm = math.exp(statistics.mean(math.log(r["rb"] / r["ra"]) for r in rs))
    gm2 = math.exp(statistics.mean(math.log(r["rb"] / r["rp2"]) for r in rs))
    print(f"| {g} | {len(rs)} | {a:.2f} s | {p2:.2f} s | {b:.2f} s | {a/b:.3f}x | {p2/b:.3f}x |"
          f" {a7:.2f} → {b7:.2f} s ({a7/b7:.2f}x) | {gm:.3f} | {gm2:.3f} |")

g = lambda name: [r for r in rows if r["g"] == name]
print()
print("mivec rp-3: generation %.1f ms, operator %.3f s" % (
    1000 * sum(r["gen"] for r in g("mivec")), sum(r["op"] for r in g("mivec"))))
dsg = g("quad/dsg")
print("quad/dsg rp-3: b0 %.2f s, b1 %.2f s" % (
    sum(r["b"] for r in dsg if "/b0/" in r["case"]), sum(r["b"] for r in dsg if "/b1/" in r["case"])))
print()
print("| case | rp-3 total | generation | operator | baseline → rp-3 | rp-2 total (RP-2) | rp-2 → rp-3 |"
      " peak RSS baseline → rp-3 | rp-2 RSS |")
print("|---|---|---|---|---|---|---|---|---|")
for r in sorted(rows, key=lambda r: -r["b"]):
    print(f"| {r['case']} | {r['b']:.3f} s | {1000*r['gen']:.1f} ms | {r['op']:.3f} s | {r['sp']:.3f}x |"
          f" {r['p2']:.3f} s | {r['p2']/r['b']:.3f}x | {r['ra']:.1f} → {r['rb']:.1f} MiB | {r['rp2']:.1f} MiB |")
