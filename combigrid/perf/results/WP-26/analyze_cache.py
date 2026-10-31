#!/usr/bin/env python3
"""WP-26: tables of the production source-function cache from the micro/cache/... runs (1 and 16
threads) of perf_cache.hpp (production) and perf_cache_proto.hpp (WP-25 prototypes, same binary).

Usage: analyze_cache.py cache-t1.json cache-t16.json > cache-production.md
Times: median per call / items (items = evaluations of one operator call, resp. keys).
"""
import json
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


def op_configs(runs):
    cfgs = []
    for name in runs:
        if name.startswith("micro/cache/op/") and name.endswith("/none/c0"):
            cfgs.append(name[len("micro/cache/op/"):-len("/none/c0")])
    return cfgs


def main():
    t1, t16 = load(sys.argv[1]), load(sys.argv[2])

    for kind in ("lookup", "insert"):
        print(f"## {kind.capitalize()} (ns per {kind} incl. hashing; production = XXH3 + "
              "Boost index, sharded_spin = WP-25 prototype with an own wyhash; 16 t: wall per "
              "item)\n")
        head = "| d | n | production 1 t | sharded_spin 1 t | production 16 t | sharded_spin 16 t |"
        if kind == "lookup":
            head += " bytes/entry production |"
        print(head)
        print("|" + "---|" * (head.count("|") - 1))
        for d in (2, 10, 20):
            for n in (10000, 1000000):
                row = []
                for runs in (t1, t16):
                    for table in ("production", "sharded_spin"):
                        r = runs.get(f"micro/cache/{kind}/{table}/d{d}/n{n}")
                        row.append(ns(r["per_item"]) if r else "-")
                line = f"| {d} | {n} | " + " | ".join(row) + " |"
                if kind == "lookup":
                    r = t1.get(f"micro/cache/lookup/production/d{d}/n{n}")
                    line += f" {r['counters'].get('bytes_per_entry', 0):.0f} |" if r else " - |"
                print(line)
        print()

    print("## Operators with a cached function (speedup = none / variant, > 1: the cache pays; "
          "always = new cache per call, warm = filled before; for interp / interplin by a "
          "quadrature on the same grid = cross-operator)\n")
    print("| operator (evals / distinct) | cost | none ns/eval 1 t / 16 t | always 1 t / 16 t | "
          "always WP-25 proto 1 t / 16 t | warm 1 t / 16 t | warm hit rate |")
    print("|---|---|---|---|---|---|---|")
    for cfg in op_configs(t1):
        for cost in (0, 1000):
            def get(runs, variant):
                return runs.get(f"micro/cache/op/{cfg}/{variant}/c{cost}")
            n1, n16 = get(t1, "none"), get(t16, "none")
            if not n1 or not n16:
                continue
            c = n1["counters"]
            ratio = c.get("evals_per_call", 0) / max(c.get("distinct", 1), 1)
            cells = [f"{ns(n1['per_item'])} / {ns(n16['per_item'])}"]
            for variant in ("always_production", "always_sharded_spin", "warm_production"):
                a1, a16 = get(t1, variant), get(t16, variant)
                cells.append(f"{n1['call'] / a1['call']:.2f} / {n16['call'] / a16['call']:.2f}"
                             if a1 and a16 else "-")
            w16 = get(t16, "warm_production")
            hit = f"{w16['counters'].get('hit_rate', 0):.3f}" if w16 else "-"
            print(f"| {cfg} ({ratio:.2f}) | {cost} ns | " + " | ".join(cells) + f" | {hit} |")
    print()

    print("## Memory of a cache filled by one quadrature, with and without the limit "
          "(quad/sg/equi/b0/d5/L8, expSum; measured: a further quadrature with that cache)\n")
    print("| limit | threads | entries | rejected | cache MiB | heap MiB | hit rate | ns/eval |")
    print("|---|---|---|---|---|---|---|---|")
    for limit in ("limit1GiB", "limit16MiB"):
        for label, runs in (("1", t1), ("16", t16)):
            r = runs.get(f"micro/cache/memory/production/quad/sg/equi/b0/d5/L8/{limit}")
            if not r:
                continue
            c = r["counters"]
            print(f"| {limit} | {label} | {c.get('entries', 0):.0f} | {c.get('rejected', 0):.0f} | "
                  f"{c.get('cache_mib', 0):.1f} | {c.get('heap_mib', 0):.1f} | "
                  f"{c.get('hit_rate', 0):.3f} | {ns(r['per_item'])} |")


if __name__ == "__main__":
    main()
