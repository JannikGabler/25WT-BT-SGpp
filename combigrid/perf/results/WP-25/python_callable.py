#!/usr/bin/env python3
"""WP-25: cost per evaluation of a Python source function through pysgpp, and what a cache
written in Python (dict / functools.lru_cache inside the callable) achieves today.

Run once per thread count (OpenMP reads the environment at import):
  PYTHONPATH=<pysgpp> LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=1 python3 python_callable.py
Prints one Markdown table row per variant: ns per evaluation seen by the operator (median of
5 runs), speedup vs. the plain callable, evaluations of the Python function.
"""
import functools
import math
import os
import statistics
import time

import numpy as np
import pysgpp


def make_grid(nodes, boundary, n_dim, level):
    gen = pysgpp.CompleteSGGenInstr(level, n_dim)
    gen.setDomain((0, 1))
    gen.setNodeGenFunc(nodes)
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    gen.setBoundaryLevelOffset(boundary)
    return pysgpp.SparseGrid(gen)


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
    threads = os.environ.get("OMP_NUM_THREADS", "?")
    grids = {
        "quad/sg/cc/b0/d5/L4": make_grid(pysgpp.getClenshawCurtisNodeGenFunc(), 0, 5, 4),
        "quad/sg/equi/b1/d10/L4": make_grid(pysgpp.getEquidistantNodeGenFunc(), 1, 10, 4),
    }
    print(f"threads: {threads}\n")
    print("| grid | callable | variant | ns/eval | speedup | Python calls / evals |")
    print("|---|---|---|---|---|---|")
    for grid_name, sg in grids.items():
        for func_name, func in (("math", exp_sum), ("numpy", exp_sum_numpy)):
            calls = [0]

            def counted(p, func=func):
                calls[0] += 1
                return func(p)

            evals = [0]

            def plain(p, func=func):
                evals[0] += 1
                return func(p)

            sf = pysgpp.SourceFunc(plain)
            base = timed(lambda: pysgpp.quadrature(sf, sg))
            n_eval = evals[0] // 5
            print(f"| {grid_name} | {func_name} | none | {1e9 * base / n_eval:.0f} | 1.00 | "
                  f"{n_eval} / {n_eval} |")

            # dict cache, fresh per run (cold: misses + hits within one quadrature)
            def cold_run(func=func):
                cache = {}

                def cached(p):
                    v = cache.get(p)
                    if v is None:
                        calls[0] += 1
                        v = cache[p] = func(p)
                    return v

                pysgpp.quadrature(pysgpp.SourceFunc(cached), sg)

            calls[0] = 0
            t = timed(cold_run)
            print(f"| {grid_name} | {func_name} | dict, cold | {1e9 * t / n_eval:.0f} | "
                  f"{base / t:.2f} | {calls[0] // 5} / {n_eval} |")

            # dict cache, warm (filled by a previous operator call: all hits)
            cache = {}

            def warm(p, func=func):
                v = cache.get(p)
                if v is None:
                    v = cache[p] = func(p)
                return v

            sf_warm = pysgpp.SourceFunc(warm)
            pysgpp.quadrature(sf_warm, sg)
            t = timed(lambda: pysgpp.quadrature(sf_warm, sg))
            print(f"| {grid_name} | {func_name} | dict, warm | {1e9 * t / n_eval:.0f} | "
                  f"{base / t:.2f} | 0 / {n_eval} |")

            # C++ cache in front of the Python callable (scratch binding wp25CachedSourceFunc of
            # the WP-25 prototype, sharded spin table): hits do not enter Python / the GIL
            if hasattr(pysgpp, "wp25CachedSourceFunc"):
                n_dim = 5 if "d5" in grid_name else 10

                def cpp_cold(sf=sf, n_dim=n_dim):
                    pysgpp.quadrature(pysgpp.wp25CachedSourceFunc(sf, n_dim), sg)

                evals[0] = 0
                t = timed(cpp_cold)
                print(f"| {grid_name} | {func_name} | C++ cache, cold | {1e9 * t / n_eval:.0f} | "
                      f"{base / t:.2f} | {evals[0] // 5} / {n_eval} |")
                sf_cpp = pysgpp.wp25CachedSourceFunc(sf, n_dim)
                pysgpp.quadrature(sf_cpp, sg)
                evals[0] = 0
                t = timed(lambda: pysgpp.quadrature(sf_cpp, sg))
                print(f"| {grid_name} | {func_name} | C++ cache, warm | {1e9 * t / n_eval:.0f} | "
                      f"{base / t:.2f} | {evals[0] // 5} / {n_eval} |")

            # functools.lru_cache (unbounded), cold
            def lru_run(func=func):
                pysgpp.quadrature(pysgpp.SourceFunc(functools.lru_cache(maxsize=None)(func)), sg)

            t = timed(lru_run)
            print(f"| {grid_name} | {func_name} | lru_cache, cold | {1e9 * t / n_eval:.0f} | "
                  f"{base / t:.2f} | - / {n_eval} |")


if __name__ == "__main__":
    main()
