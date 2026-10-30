#!/usr/bin/env python3
"""WP-25: tables from the micro/cache/... runs (1 and 16 threads) of perf_cache_proto.hpp.

Usage: analyze_cache.py T1.json[,T1b.json...] T16.json[,T16b.json...] > cache-tables.md
(comma-separated runs are merged, later runs replace cases of earlier ones)
Times: median per call / items. Per-thread cost at 16 threads = wall time per item * 16.
"""
import json
import re
import statistics
import sys


def load(path):
    out = {}
    with open(path) as f:
        data = json.load(f)
    records = data["records"] if isinstance(data, dict) and "records" in data else data
    for r in records:
        if isinstance(r, str):
            r = json.loads(r)
        med = statistics.median(r["metrics"]["call"])
        out[r["case"]] = {"call": med, "per_item": med / r["items"], "items": r["items"],
                          "counters": r.get("counters", {})}
    return out


def ns(x):
    return f"{1e9 * x:.1f}"


def main():
    t1, t16 = {}, {}
    for path in sys.argv[1].split(","):
        t1.update(load(path))
    for path in sys.argv[2].split(","):
        t16.update(load(path))
    tables = ["oldproto", "sharded_mutex", "sharded_spin", "boost_cfs", "phmap", "sol"]

    print("## Hashing (ns per key, 1 thread, structured sparse-grid keys)\n")
    print("| d | wy (own) | fnv (removed prototype) | xxh3 |\n|---|---|---|---|")
    for d in (2, 5, 10, 20):
        row = [ns(t1[f"micro/cache/hash/{h}/d{d}"]["per_item"])
               if f"micro/cache/hash/{h}/d{d}" in t1 else "-" for h in ("wy", "fnv_proto", "xxh3")]
        print(f"| {d} | " + " | ".join(row) + " |")

    for kind in ("lookup", "insert"):
        print(f"\n## {kind.capitalize()} (ns per {kind}, incl. hashing; 16 t: wall per item and "
              "per-thread cost = wall x 16)\n")
        head = "| table | d | n | 1 t | 16 t wall | 16 t per thread |"
        if kind == "lookup":
            head += " bytes/entry | shard imbalance |"
        print(head)
        print("|" + "---|" * (head.count("|") - 1))
        for d in (2, 10, 20):
            for n in (10000, 1000000):
                for t in tables:
                    name = f"micro/cache/{kind}/{t}/d{d}/n{n}"
                    if name not in t1:
                        continue
                    a, b = t1[name]["per_item"], t16[name]["per_item"]
                    line = f"| {t} | {d} | {n} | {ns(a)} | {ns(b)} | {ns(16 * b)} |"
                    if kind == "lookup":
                        c = t1[name]["counters"]
                        imb = c.get("shard_imbalance")
                        line += (f" {c.get('bytes_per_entry', 0):.0f} | "
                                 f"{'-' if imb is None else f'{imb:.1f}'} |")
                    print(line)

    print("\n## Operators with a cached source function (ns per evaluation seen by the operator; "
          "speedup = none / variant)\n")
    pat = re.compile(r"micro/cache/op/(.*)/(none|always_\w+|warm_\w+|bypass_\w+)/c(\d+)$")
    ops = {}
    for name in t1:
        m = pat.match(name)
        if m:
            ops.setdefault(m.group(1), {}).setdefault(int(m.group(3)), {})[m.group(2)] = name
    for op, costs in ops.items():
        any_name = next(iter(next(iter(costs.values())).values()))
        c = t1[any_name]["counters"]
        print(f"\n### {op}: {int(c['evals_per_call'])} evaluations per call, "
              f"{int(c['distinct'])} distinct (ratio {c['evals_per_call'] / c['distinct']:.2f})\n")
        print("| cost | variant | 1 t ns/eval | 1 t speedup | 1 t hit rate | 16 t ns/eval | "
              "16 t speedup | 16 t hit rate |\n|---|---|---|---|---|---|---|---|")
        for cost in sorted(costs):
            variants = costs[cost]
            base1 = t1[variants["none"]]["per_item"] if "none" in variants else None
            base16 = t16[variants["none"]]["per_item"] if "none" in variants else None
            for v in sorted(variants, key=lambda s: (s != "none", s)):
                n = variants[v]
                a, b = t1[n]["per_item"], t16[n]["per_item"]
                h1 = t1[n]["counters"].get("hit_rate")
                h16 = t16[n]["counters"].get("hit_rate")
                s1 = f"{base1 / a:.2f}" if base1 else "-"
                s16 = f"{base16 / b:.2f}" if base16 else "-"
                print(f"| {cost} | {v} | {ns(a)} | {s1} | {'-' if h1 is None else f'{h1:.3f}'} "
                      f"| {ns(b)} | {s16} | {'-' if h16 is None else f'{h16:.3f}'} |")
        bypass = costs.get(0, {}).get("bypass_sharded_spin")
        if bypass and "none" in costs[0]:
            print(f"\nbypass (adaptive decided not to cache) vs none at cost 0: "
                  f"1 t {t1[costs[0]['none']]['per_item'] / t1[bypass]['per_item']:.3f}x, "
                  f"16 t {t16[costs[0]['none']]['per_item'] / t16[bypass]['per_item']:.3f}x")

    print("\n## Break-even integrand cost (ns at 1 thread) per design: cached == none\n")
    print("Linear in the cost between the measured 0 / 100 / 1000 ns runs (\"<0\": faster even "
          "at cost 0; \">1000\": extrapolated from 100 -> 1000).\n")
    print("| operator | ratio | always sharded_spin 1 t / 16 t | always boost_cfs 1 t / 16 t | "
          "warm sharded_spin 1 t / 16 t | warm boost_cfs 1 t / 16 t |\n|---|---|---|---|---|---|")

    def break_even(data, costs, variant):
        pts = []
        for cost in sorted(costs):
            v = costs[cost]
            if variant in v and "none" in v:
                pts.append((cost, data[v[variant]]["per_item"] - data[v["none"]]["per_item"]))
        if len(pts) < 2:
            return "-"
        if pts[0][1] <= 0:
            return "<0"
        for (c0, d0), (c1, d1) in zip(pts, pts[1:]):
            if d1 <= 0:
                return f"{c0 + d0 * (c1 - c0) / (d0 - d1):.0f}"
        (c0, d0), (c1, d1) = pts[-2], pts[-1]
        if d1 >= d0:
            return "never"
        return f">1000 (~{c0 + d0 * (c1 - c0) / (d0 - d1):.0f})"

    for op, costs in ops.items():
        any_name = next(iter(next(iter(costs.values())).values()))
        c = t1[any_name]["counters"]
        cells = []
        for variant in ("always_sharded_spin", "always_boost_cfs", "warm_sharded_spin",
                        "warm_boost_cfs"):
            cells.append(f"{break_even(t1, costs, variant)} / {break_even(t16, costs, variant)}")
        print(f"| {op} | {c['evals_per_call'] / c['distinct']:.2f} | " + " | ".join(cells) + " |")

    print("\n## Disk tier (flat file on the SSD, ext4; page cache warm for load / pread)\n")
    print("| case | 1 t ns/record | MB/s (1 t) | 16 t ns/record |\n|---|---|---|---|")
    for name in sorted(n for n in t1 if n.startswith("micro/cache/disk/")):
        d = int(re.search(r"/d(\d+)/", name).group(1))
        rec_bytes = 8 * (d + 2)
        a = t1[name]["per_item"]
        b = t16[name]["per_item"]
        print(f"| {name[len('micro/cache/disk/'):]} | {ns(a)} | {rec_bytes / a / 1e6:.0f} | "
              f"{ns(b)} |")


if __name__ == "__main__":
    main()
