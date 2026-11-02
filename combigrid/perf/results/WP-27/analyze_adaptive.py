"""WP-27: tables of the adaptive micro cases (adaptive-t{1,16}.json): ns per evaluation of none /
always / warm / adaptive / adaptive_steady (and the spill variants) and the speedups vs. none and
always, plus the counters of the adaptive cases (final mode, switches, measured costs).
Usage: python3 analyze_adaptive.py > adaptive.md"""
import json
import re
import sys
from collections import defaultdict

MODES = {0: "None", 1: "MemoryOnly", 2: "TemporaryStorage"}


def load(path):
    data = json.load(open(path))
    out = {}
    for r in data["records"]:
        samples = sorted(r["metrics"]["call"])
        median = samples[len(samples) // 2] * 1e9
        r["median_ns"] = median
        out[r["case"]] = (r, median / r["items"] if r.get("items") else None)
    return out


def table(path, threads):
    recs = load(path)
    print(f"\n### {threads} thread(s): ns per evaluation (speedup vs. none / vs. always)\n")
    print("| config | cost | none | always | warm | adaptive (new cache per call) | adaptive steady | "
          "mode / switches / c_f ns / hit ns (steady) |")
    print("|---|---|---|---|---|---|---|---|")
    groups = defaultdict(dict)
    for name, (r, ns) in recs.items():
        m = re.match(r"micro/cache/op/(.*)/(\w+?)(?:_production)?/c(\d+)$", name)
        if m:
            groups[(m.group(1), m.group(3))][m.group(2)] = (r, ns)
    for (cfg, cost), g in groups.items():
        if "none" not in g:
            continue
        none = g["none"][1]
        always = g.get("always", (None, None))[1]

        def cell(key):
            if key not in g or g[key][1] is None:
                return "-"
            ns = g[key][1]
            s = f"{ns:.1f} ({none / ns:.2f}x"
            if always and key.startswith("adaptive"):
                s += f" / {always / ns:.2f}x"
            return s + ")"
        steady = g.get("adaptive_steady", (None, None))[0]
        info = "-"
        if steady:
            c = steady.get("counters", {})
            info = (f"{MODES.get(int(c.get('mode', -1)), '?')} / {int(c.get('mode_switches', 0))} / "
                    f"{c.get('function_ns', 0):.0f} / {c.get('hit_ns', 0):.0f}")
        print(f"| {cfg} | {cost} | {none:.1f} | {cell('always')} | {cell('warm')} | "
              f"{cell('adaptive')} | {cell('adaptive_steady')} | {info} |")
    spill = {k: v for k, v in recs.items() if "spill" in k}
    if spill:
        print("\nStorage variants (memory limit 0): temporary_spill (TemporaryStorage) vs. "
              "adaptive_spill (ns per evaluation, final mode of the last call)\n")
        print("| case | ns | mode |")
        print("|---|---|---|")
        for k, (r, ns) in sorted(spill.items()):
            mode = r.get("counters", {}).get("mode")
            print(f"| {k.replace('micro/cache/op/', '')} | {ns:.1f} | "
                  f"{MODES.get(int(mode), '-') if mode is not None else '-'} |")
    if "micro/cache/adaptive/decide" in recs:
        r, ns = recs["micro/cache/adaptive/decide"]
        print(f"\nCost of one window decision (64 samples + rules): {r['median_ns'] / 1000:.2f} us")


for t in (1, 16):
    table(f"adaptive-t{t}.json", t)
