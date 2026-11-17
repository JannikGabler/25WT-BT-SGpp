#!/usr/bin/env python3
"""Runs the WP-62 quadrature experiment groups (bin/run_quad) and stores the JSON traces in
raw/<group>.jsonl.gz. Usage: campaign.py [group ...] (default: all). Stdlib only."""
import gzip
import itertools
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
RUN = os.path.join(HERE, "bin", "run_quad")
RAW = os.path.join(HERE, "..", "raw")

METHODS = {
    "level": "mode=level",
    "fit": "mode=fit",
    "qo": "mode=qo",
    "gg": "mode=gg profit=0",
    "ggp": "mode=gg profit=1",
    "ggp-buf3": "mode=gg profit=1 buffer=3",
    "ggp-buf10": "mode=gg profit=1 buffer=10",
    "ggp-w": "mode=gg profit=1 w=0.999",
    "lazy": "mode=lazy",
    "lazy-buf3": "mode=lazy buffer=3",
    "lazy-x10": "mode=lazy explore=0.1",
    "lazy-x25": "mode=lazy explore=0.25",
    "lazy-max": "mode=lazy pred=max",
    "lazy-min": "mode=lazy pred=min",
}


def runs_of(group):
    if group == "lowd":
        for d, f, m in itertools.product([2, 5], ["gauss", "osc", "prodpeak", "corner", "cont", "uq"],
                                         ["level", "fit", "qo", "gg", "ggp", "lazy"]):
            yield f"func={f} d={d} b=0 decay=alg2 c=4 budget=200000", m
    if group == "highd":
        for d, f, dec, m in itertools.product(
                [10, 20, 50, 100], ["gauss", "osc", "prodpeak", "corner"], ["alg2", "geo", "eff5"],
                ["level", "qo", "gg", "ggp", "ggp-buf3", "lazy", "lazy-x10", "lazy-max", "lazy-min"]):
            yield f"func={f} d={d} b=1 decay={dec} c=4 budget=300000", m
    if group == "perm":
        for d, f, perm, m in itertools.product(
                [50, 100, 1000], ["gauss", "corner"], [0, 1],
                ["ggp", "ggp-buf3", "ggp-buf10", "lazy", "lazy-buf3"]):
            yield f"func={f} d={d} b=1 decay=alg2 c=4 perm={perm} budget=30000 trace=1.02", m
    if group == "bulk":
        for d, f, m, (theta, batch) in itertools.product(
                [10, 50], ["gauss", "corner"], ["ggp", "lazy"],
                [(1, 1), (0.5, 16), (0.5, 64), (0.1, 64), (0.1, 1024), (0, 64)]):
            yield (f"func={f} d={d} b=1 decay=alg2 c=4 budget=300000 theta={theta} batch={batch} "
                   f"tag=t{theta}b{batch}"), m
    if group == "store":
        for d, f, st in itertools.product([10, 50], ["gauss", "corner"], ["delta", "tg"]):
            yield f"func={f} d={d} b=1 decay=alg2 c=4 budget=1000000 store={st}", "ggp"
        for (d, b), f, m in itertools.product([(2, 0), (5, 0), (10, 1)], ["gauss", "corner"],
                                              ["level", "gg", "ggp"]):
            yield f"func={f} d={d} b={b} nodes=equi growth=linear decay=alg2 c=4 budget=300000", m
            yield f"func={f} d={d} b={b} nodes=equi growth=doubling decay=alg2 c=4 budget=300000", m
    if group == "inter":
        for d, m in itertools.product([10, 50], ["gg", "ggp", "ggp-w", "lazy", "lazy-x10", "lazy-x25", "qo"]):
            yield f"func=inter d={d} b=1 decay=alg2 c=4 budget=300000", m
    if group == "vector":
        for d, f in itertools.product([10, 50], ["vec3", "gauss", "osc", "corner"]):
            yield f"func={f} d={d} b=1 decay=alg2 c=4 budget=300000", "ggp"
    if group == "iso":
        for (d, b), f, m in itertools.product([(5, 0), (10, 1)], ["uq", "gauss"],
                                              ["level", "qo", "gg", "ggp", "lazy"]):
            yield f"func={f} d={d} b={b} decay=iso c=1 budget=300000", m
    if group == "growth":
        for d, g, m in itertools.product([50, 100], ["doubling", "std"], ["ggp", "lazy"]):
            yield f"func=gauss d={d} b=1 decay=alg2 c=4 growth={g} budget=100000 trace=1.02", m


GROUPS = ["lowd", "highd", "perm", "bulk", "store", "inter", "vector", "iso", "growth"]

if __name__ == "__main__":
    os.makedirs(RAW, exist_ok=True)
    for group in sys.argv[1:] or GROUPS:
        path = os.path.join(RAW, group + ".jsonl.gz")
        n = 0
        with gzip.open(path, "wt") as out, open(os.path.join(RAW, group + ".log"), "w") as log:
            for config, method in runs_of(group):
                args = config.split() + METHODS[method].split()
                proc = subprocess.run([RUN] + args, capture_output=True, text=True)
                log.write(f"# {config} [{method}]\n{proc.stderr}")
                for line in proc.stdout.splitlines():
                    if line.startswith("{"):
                        out.write('{"method":"%s",' % method + line[1:] + "\n")
                n += 1
        print(f"{group}: {n} runs -> {path}")
