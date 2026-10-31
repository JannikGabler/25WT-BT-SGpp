#!/usr/bin/env python3
"""WP-29d: one VectorSourceFunc operator call vs. m SourceFunc calls from Python.

Run once per thread count (OpenMP reads the environment at import), through the repo's .venv:
  PYTHONPATH=<pysgpp> LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=1 \
      .venv/bin/python combigrid/perf/results/WP-29d/python_vector_timing.py [--small]
--small uses one level less (quad/interp L3, interplin L4), for 16 threads (GIL contention makes
a Python call ~30x more expensive there).
Prints one Markdown table row per (case, callable, m): time of the m scalar runs, of the vector
run (median of the repeats), speedup = scalar / vector, and the cost per Python call.
Callables (cheap, as in WP-25: one exp of the coordinate sum per output):
- "same work": the vector callable computes every output independently (m exps), so the vector
  run saves only the per-call overhead (tuple, call, GIL); a lower bound of the gain;
- "shared": the vector callable computes the exp once and derives the m outputs from it, as
  vector-valued functions with shared work do.
"""
import argparse
import math
import os
import statistics
import time

import pysgpp


def make_grid(nodes, boundary, n_dim, level):
    gen = pysgpp.CompleteSGGenInstr(level, n_dim)
    gen.setDomain((0, 1))
    gen.setNodeGenFunc(nodes)
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    gen.setBoundaryLevelOffset(boundary)
    return gen, pysgpp.SparseGrid(gen)


def timed(fn, repeats):
    times = []
    for _ in range(repeats):
        start = time.perf_counter()
        fn()
        times.append(time.perf_counter() - start)
    return statistics.median(times)


def scalar_func(k, calls):
    def f(p):
        calls[0] += 1
        return math.exp(0.5 * sum(p)) + k
    return f


def vector_func(m, shared, calls):
    if shared:
        def f(p):
            calls[0] += 1
            s = math.exp(0.5 * sum(p))
            return [s + k for k in range(m)]
    else:
        def f(p):
            calls[0] += 1
            return [math.exp(0.5 * sum(p)) + k for k in range(m)]
    return f


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--small", action="store_true")
    small = parser.parse_args().small
    threads = os.environ.get("OMP_NUM_THREADS", "?")
    lq, ll = (3, 4) if small else (4, 5)
    point = pysgpp.DataVector([0.31, 0.47, 0.73, 0.19, 0.58])
    grids = {
        f"quad/sg/cc/b0/d5/L{lq}": make_grid(pysgpp.getClenshawCurtisNodeGenFunc(), 0, 5, lq),
        f"interp/sg/cc/b0/d5/L{lq}": make_grid(pysgpp.getClenshawCurtisNodeGenFunc(), 0, 5, lq),
        f"interplin/sg/equi/b1/d5/L{ll}": make_grid(pysgpp.getEquidistantNodeGenFunc(), 1, 5,
                                                    ll),
    }
    ops = {
        "quad": lambda f, sg: pysgpp.quadrature(f, sg),
        "interp": lambda f, sg: pysgpp.interpolate(f, point, sg),
        "interplin": lambda f, sg: pysgpp.interpolateLinear(f, point, sg),
    }
    repeats = 5 if threads == "1" else 3
    print(f"threads: {threads}, repeats: {repeats} (median)\n")
    print("| case | callable | m | m scalar runs | vector run | speedup | µs per Python call "
          "(scalar / vector) |")
    print("|---|---|---|---|---|---|---|")
    for case, (_, sg) in grids.items():
        op = ops[case.split("/")[0]]
        for m in (1, 4, 32):
            calls = [0]
            sfs = [pysgpp.SourceFunc(scalar_func(k, calls)) for k in range(m)]
            t_scalar = timed(lambda: [op(sf, sg) for sf in sfs], repeats)
            scalar_calls = calls[0] / repeats
            for shared in (False, True):
                calls = [0]
                vsf = pysgpp.VectorSourceFunc(vector_func(m, shared, calls), m)
                t_vector = timed(lambda: op(vsf, sg), repeats)
                vector_calls = calls[0] / repeats
                name = "shared" if shared else "same work"
                print(f"| {case} | {name} | {m} | {1e3 * t_scalar:.1f} ms | "
                      f"{1e3 * t_vector:.1f} ms | {t_scalar / t_vector:.2f} | "
                      f"{1e6 * t_scalar / scalar_calls:.2f} / {1e6 * t_vector / vector_calls:.2f} |",
                      flush=True)


if __name__ == "__main__":
    main()
