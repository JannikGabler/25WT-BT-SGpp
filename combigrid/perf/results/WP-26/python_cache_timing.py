#!/usr/bin/env python3
"""WP-26: Python callables with the production cache (pysgpp.CachedSourceFunc).

Same grids and callables as WP-25's python_callable.py (math / NumPy exp of the scaled sum), plus
global interpolation on a shifted domain (p = 8 points) with the cache warmed by a quadrature on
the same grid (cross-operator reuse). Variants: none (pysgpp.SourceFunc), cold (a new cache per
call), warm (the cache of an earlier call). ns per evaluation = median time / evaluations of one
call; Python calls are counted in a separate counting run.

Usage (from the repo root, through the repo's .venv; OMP environment as perf.py):
  PYTHONPATH=<repo>/lib:<repo>/lib/pysgpp LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=<t> \
      OMP_PROC_BIND=close OMP_PLACES=<places> .venv/bin/python python_cache_timing.py
"""
import math
import os
import statistics
import time

import numpy as np
import pysgpp


def make_grid(nodes, boundary, n_dim, level, domain):
    gen = pysgpp.CompleteSGGenInstr(level, n_dim)
    gen.setDomain(domain)
    gen.setNodeGenFunc(nodes)
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    gen.setBoundaryLevelOffset(boundary)
    return gen, pysgpp.SparseGrid(gen)


def exp_sum(p):
    return math.exp(0.5 * sum(p))


def exp_sum_numpy(p):
    return float(np.exp(0.5 * np.sum(np.asarray(p))))


def timed(fn, repeats=5):
    times = []
    for _ in range(repeats):
        start = time.perf_counter()
        fn()
        times.append(time.perf_counter() - start)
    return statistics.median(times)


def main():
    shifted = [(-0.5 + 0.1 * k, 1.0 + 0.2 * k) for k in range(5)]
    cases = {
        "quad/sg/cc/b0/d5/L4": (make_grid(pysgpp.getClenshawCurtisNodeGenFunc(), 0, 5, 4,
                                          [(0.0, 1.0)] * 5), None),
        "quad/sg/equi/b1/d10/L4": (make_grid(pysgpp.getEquidistantNodeGenFunc(), 1, 10, 4,
                                             [(0.0, 1.0)] * 10), None),
        "interp/sg/cc/b0/d5/L3/p8-shifted": (
            make_grid(pysgpp.getClenshawCurtisNodeGenFunc(), 0, 5, 3, shifted),
            [pysgpp.DataVector([lo + ((0.13 + 0.29 * i + 0.07 * k) % 1.0) * (hi - lo)
                                for k, (lo, hi) in enumerate(shifted)]) for i in range(8)]),
    }
    print(f"threads: {os.environ.get('OMP_NUM_THREADS', 'default')}\n")
    print("| case | callable | variant | ns/eval | speedup | Python calls / evals |")
    print("|---|---|---|---|---|---|")
    for case, ((_, grid), pts) in cases.items():
        def run(f, grid=grid, pts=pts):
            if pts is None:
                return pysgpp.quadrature(f, grid)
            return sum(pysgpp.interpolate(f, p, grid) for p in pts)

        for func_name, func in (("math", exp_sum), ("numpy", exp_sum_numpy)):
            calls = [0]

            def counted(p, func=func):
                calls[0] += 1
                return func(p)

            run(pysgpp.SourceFunc(counted))
            n_eval = calls[0]
            plain = pysgpp.SourceFunc(func)
            base = timed(lambda: run(plain))
            print(f"| {case} | {func_name} | none | {1e9 * base / n_eval:.0f} | 1.00 | "
                  f"{n_eval} / {n_eval} |")

            calls[0] = 0
            counting = pysgpp.CachedSourceFunc(counted)
            run(counting)
            cold_calls = calls[0]
            t = timed(lambda: run(pysgpp.CachedSourceFunc(func)))
            print(f"| {case} | {func_name} | cold | {1e9 * t / n_eval:.0f} | {base / t:.2f} | "
                  f"{cold_calls} / {n_eval} |")

            warm = pysgpp.CachedSourceFunc(func)
            if pts is None:
                run(warm)
                label = "warm"
            else:
                pysgpp.quadrature(warm, grid)  # cross-operator: filled by a quadrature
                label = "warm (by quadrature)"
            before = warm.statistics()
            t = timed(lambda: run(warm))
            after = warm.statistics()
            misses = (after.lookups - before.lookups) - (after.hits - before.hits)
            print(f"| {case} | {func_name} | {label} | {1e9 * t / n_eval:.0f} | {base / t:.2f} | "
                  f"{misses} / {after.lookups - before.lookups} |")


if __name__ == "__main__":
    main()
