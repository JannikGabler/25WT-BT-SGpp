#!/usr/bin/env python3
"""WP-50: Python, DeltasSparseGrid operators vs. SparseGrid operators with Python callables.

Run once per thread count (OpenMP reads the environment at import), through the repo's .venv:
  PYTHONPATH=<pysgpp> LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=1 OMP_PLACES={4} \
      .venv/bin/python combigrid/perf/results/WP-50/python_dsg_timing.py [--small]
  (16 threads: OMP_NUM_THREADS=16 OMP_PROC_BIND=close OMP_PLACES=threads ... --small)
--small uses one level less per case (a Python call costs ~30x more at 16 threads: GIL
contention).
Prints one Markdown table row per (case, function): time of the SparseGrid run and of the
DeltasSparseGrid run (median of the repeats, grids built beforehand; the DSG operators' per-grid
index is built by an untimed warm-up call), speedup = SG / DSG, the numbers of Python calls of
both and their ratio, and the time per Python call.
Functions (cheap, as in WP-29d: one exp of the coordinate sum): "scalar" = SourceFunc;
"vector m=4" = VectorSourceFunc with 4 outputs derived from one exp (shared work).
"""
import argparse
import itertools
import math
import os
import statistics
import time

import pysgpp

calls = itertools.count()


def scalar(p):
    next(calls)
    return math.exp(0.5 * sum(p))


def vector4(p):
    next(calls)
    s = math.exp(0.5 * sum(p))
    return [s, s + 1.0, 2.0 * s, s - 1.0]


def make_gen(nodes, boundary, n_dim, level):
    gen = pysgpp.CompleteSGGenInstr(level, n_dim)
    gen.setDomain((0, 1))
    gen.setNodeGenFunc(nodes)
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    gen.setBoundaryLevelOffset(boundary)
    return gen


def timed(fn, repeats):
    times, counts = [], []
    for _ in range(repeats):
        start_calls = next(calls)
        start = time.perf_counter()
        fn()
        times.append(time.perf_counter() - start)
        counts.append(next(calls) - start_calls - 1)
    return statistics.median(times), counts[0]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--small", action="store_true")
    small = 1 if parser.parse_args().small else 0
    threads = os.environ.get("OMP_NUM_THREADS", "?")
    cc = pysgpp.getClenshawCurtisNodeGenFunc()
    equi = pysgpp.getEquidistantNodeGenFunc()
    # (operator, nodes name, nodes, boundary, d, level)
    cases = [("quad", "cc", cc, 0, 5, 5), ("quad", "cc", cc, 0, 10, 2),
             ("interp", "cc", cc, 0, 5, 5), ("interp", "cc", cc, 0, 10, 2),
             ("interp", "equi", equi, 0, 3, 5),
             ("interplin", "equi", equi, 1, 5, 6), ("interplin", "equi", equi, 1, 8, 5)]
    repeats = 5 if threads == "1" else 3
    print(f"threads: {threads}, repeats: {repeats} (median)\n")
    print("| case | function | SparseGrid | DeltasSparseGrid | speedup | Python calls SG / DSG "
          "(ratio) | µs per call SG / DSG |")
    print("|---|---|---|---|---|---|---|")
    for op_name, nodes_name, nodes, boundary, n_dim, level in cases:
        level -= small
        gen = make_gen(nodes, boundary, n_dim, level)
        sg = pysgpp.SparseGrid(gen)
        dsg = pysgpp.DeltasSparseGrid(gen)
        point = pysgpp.DataVector([(0.31 + 0.37 * k) % 0.9 + 0.05 for k in range(n_dim)])
        ops = {"quad": lambda f, g: pysgpp.quadrature(f, g),
               "interp": lambda f, g: pysgpp.interpolate(f, point, g),
               "interplin": lambda f, g: pysgpp.interpolateLinear(f, point, g)}
        op = ops[op_name]
        case = f"{op_name}/{nodes_name}/b{boundary}/d{n_dim}/L{level}"
        for func_name, func in (("scalar", pysgpp.SourceFunc(scalar)),
                                ("vector m=4", pysgpp.VectorSourceFunc(vector4, 4))):
            op(func, dsg)  # builds the DSG operator's per-grid index (kept in the grid)
            t_sg, n_sg = timed(lambda: op(func, sg), repeats)
            t_dsg, n_dsg = timed(lambda: op(func, dsg), repeats)
            print(f"| {case} | {func_name} | {1e3 * t_sg:.1f} ms | {1e3 * t_dsg:.1f} ms | "
                  f"{t_sg / t_dsg:.2f} | {n_sg} / {n_dsg} ({n_sg / n_dsg:.2f}) | "
                  f"{1e6 * t_sg / n_sg:.2f} / {1e6 * t_dsg / n_dsg:.2f} |", flush=True)


if __name__ == "__main__":
    main()
