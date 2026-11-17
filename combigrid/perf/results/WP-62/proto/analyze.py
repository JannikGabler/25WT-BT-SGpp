#!/usr/bin/env python3
"""Tables from the WP-62 traces (raw/<group>.jsonl.gz). Main metric: N(eps) = evaluations after
which the relative error stays <= eps until the end of the run ("-" = not reached within the
budget). Usage: analyze.py <group> [eps,eps,...]. Stdlib only."""
import collections
import gzip
import json
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))


def load(group):
    runs = collections.OrderedDict()
    with gzip.open(os.path.join(HERE, "..", "raw", group + ".jsonl.gz"), "rt") as f:
        for line in f:
            r = json.loads(line)
            key = (r["func"], int(r["d"]), r["decay"], r["perm"], r["nodes"], r["growth"], r["b"],
                   r["store"], r["tag"], r["method"])
            runs.setdefault(key, []).append(r)
    return runs


def reach(trace, eps):
    """Evaluations after which the error stays <= eps (None if not reached)."""
    n = None
    for r in trace:
        if r["err"] <= eps:
            if n is None:
                n = r["evals"]
        else:
            n = None
    return n


def err_at(trace, budget):
    best = [r for r in trace if r["evals"] <= budget]
    return best[-1]["err"] if best else None


def fmt(n):
    return "-" if n is None else str(n)


def geomean(values):
    return math.exp(sum(math.log(v) for v in values) / len(values)) if values else float("nan")


def table_reach(runs, epsilons, case_fields):
    """One row per case and method: N(eps)."""
    print("| case | method | " + " | ".join("N(%g)" % e for e in epsilons) +
          " | final evals | final err | eta / err (median) | wall [s] | bookkeeping share |")
    print("|---|---|" + "---|" * (len(epsilons) + 5))
    for key, trace in runs.items():
        case = " ".join(str(key[i]) for i in case_fields)
        last = trace[-1]
        ratios = sorted(r["eta"] / r["err"] for r in trace if r["err"] > 1e-13 and r["eta"] > 0)
        med = "%.2g" % ratios[len(ratios) // 2] if ratios else "-"
        share = 1 - last["evalTime"] / last["wall"] if last["wall"] > 0 else 0
        print("| %s | %s | %s | %d | %.1e | %s | %.3f | %.0f %% |" % (
            case, key[-1], " | ".join(fmt(reach(trace, e)) for e in epsilons), last["evals"],
            last["err"], med, last["wall"], 100 * share))


def table_ratio(runs, epsilons, reference, group_fields):
    """Geometric mean over the cases of N_method(eps) / N_reference(eps), per group."""
    groups = collections.OrderedDict()
    for key, trace in runs.items():
        group = tuple(key[i] for i in group_fields)
        case = key[:-1]
        groups.setdefault(group, collections.OrderedDict()).setdefault(key[-1], {})[case] = trace
    methods = []
    for by_method in groups.values():
        for m in by_method:
            if m not in methods:
                methods.append(m)
    print("Ratio N_method(eps) / N_%s(eps), geometric mean over the cases where both reach eps "
          "(in brackets: cases reached / cases); < 1 = fewer evaluations than %s.\n" %
          (reference, reference))
    print("| group | eps | " + " | ".join(methods) + " |")
    print("|---|---|" + "---|" * len(methods))
    for group, by_method in groups.items():
        for eps in epsilons:
            cells = []
            for m in methods:
                ratios, reached, total = [], 0, 0
                for case, trace in by_method.get(m, {}).items():
                    total += 1
                    n = reach(trace, eps)
                    reached += n is not None
                    ref = by_method.get(reference, {}).get(case)
                    nref = reach(ref, eps) if ref else None
                    if n is not None and nref is not None:
                        ratios.append(n / nref)
                cells.append("%.2f (%d/%d)" % (geomean(ratios), reached, total) if ratios
                             else "- (%d/%d)" % (reached, total))
            print("| %s | %g | %s |" % (" ".join(str(g) for g in group), eps, " | ".join(cells)))


def table_budget(runs, budgets, case_fields):
    print("| case | method | " + " | ".join("err @ %d" % b for b in budgets) + " | rounds |")
    print("|---|---|" + "---|" * (len(budgets) + 1))
    for key, trace in runs.items():
        case = " ".join(str(key[i]) for i in case_fields)
        cells = []
        for b in budgets:
            e = err_at(trace, b)
            cells.append("-" if e is None else "%.1e" % e)
        print("| %s | %s | %s | %d |" % (case, key[-1], " | ".join(cells), trace[-1]["rounds"]))


if __name__ == "__main__":
    group = sys.argv[1]
    kind = sys.argv[2] if len(sys.argv) > 2 else "reach"
    numbers = [float(x) for x in sys.argv[3].split(",")] if len(sys.argv) > 3 else [1e-4, 1e-6, 1e-8]
    runs = load(group)
    fields = [0, 1, 2, 3, 4, 5, 6, 7, 8]
    varying = [i for i in fields if len({k[i] for k in runs}) > 1]
    if kind == "reach":
        table_reach(runs, numbers, varying)
    elif kind == "ratio":
        reference = sys.argv[4] if len(sys.argv) > 4 else "ggp"
        table_ratio(runs, numbers, reference, [i for i in varying if i in (1, 2, 3)])
    elif kind == "budget":
        table_budget(runs, [int(b) for b in numbers], varying)
