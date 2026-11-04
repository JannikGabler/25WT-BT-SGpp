#!/usr/bin/env python3
"""WP-17: checks of the Python bindings of the batched source-function evaluation.

Usage (from the repo root, through the repo's .venv):
  PYTHONPATH=<pysgpp> LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=1 \
      .venv/bin/python combigrid/perf/results/WP-17/python_checks.py [--timing-only]

- Per-point callables keep working (results of SourceFunc / VectorSourceFunc / cached variants).
- Batch callables (SourceFunc.fromBatch, VectorSourceFunc.fromBatch, CachedSourceFunc.fromBatch,
  CachedVectorSourceFunc.fromBatch) receive NumPy arrays of shape (n, d) and give the same results
  as the per-point callable: bitwise at OMP_NUM_THREADS=1 if the batch callable computes the same
  floats (it calls the per-point function row by row), within round-off if it vectorizes with
  NumPy.
- evaluate(DataMatrix) -> DataVector; wrong result sizes raise ValueError, exceptions inside a
  batch are re-raised after the operator, the function is reusable afterwards; NaN values are not
  cached; reference counts of the callables are restored.
- Timing (also with --timing-only, the only part that works with the bindings before WP-17):
  quadrature of a per-point Python callable, microseconds per evaluation, at the thread count of
  OMP_NUM_THREADS (the GIL is taken once per point before WP-17, once per block after it).
Exit code 1 if any check fails.
"""
import argparse
import gc
import math
import sys
import time

import numpy as np
import pysgpp

failures = []


def check(cond, msg):
    if not cond:
        failures.append(msg)
        print("FAIL:", msg)


def tolist(v):
    return [v[i] for i in range(len(v))]  # DataVector has no __iter__


def make_grid(nodes, boundary, n_dim, level, domain):
    gen = pysgpp.CompleteSGGenInstr(level, n_dim)
    gen.setDomain(domain)
    gen.setNodeGenFunc(nodes)
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    gen.setBoundaryLevelOffset(boundary)
    return gen, pysgpp.SparseGrid(gen)


def f_point(p):
    return math.exp(0.3 * sum(p)) + math.sin(p[0])


def f_rows(points):  # the per-point function row by row: the same floats
    # tolist(): Python floats (sum() of exact floats is compensated, of NumPy scalars it is not)
    return [f_point(tuple(row)) for row in points.tolist()]


def f_numpy(points):  # vectorized: equal within round-off
    return np.exp(0.3 * points.sum(axis=1)) + np.sin(points[:, 0])


M = 3


def v_point(p):
    return [f_point(p) * (k + 1) for k in range(M)]


def v_rows(points):
    return np.array([v_point(tuple(row)) for row in points.tolist()])


def grids():
    for nodes in (pysgpp.getEquidistantNodeGenFunc(), pysgpp.getClenshawCurtisNodeGenFunc()):
        for boundary in (0, 1):
            for n_dim in (1, 2, 3):
                domain = [(-0.5 + 0.1 * k, 1.0 + 0.2 * k) for k in range(n_dim)]
                yield nodes, boundary, n_dim, domain


def operators(func, grid, point):
    return [pysgpp.quadrature(func, grid), pysgpp.interpolate(func, point, grid),
            pysgpp.interpolateLinear(func, point, grid)]


def check_scalar(bitwise):
    for nodes, boundary, n_dim, domain in grids():
        _, grid = make_grid(nodes, boundary, n_dim, 5, domain)
        point = pysgpp.DataVector([0.5 * (a + b) + 0.01 for a, b in domain])
        ref = operators(pysgpp.SourceFunc(f_point), grid, point)
        rows = operators(pysgpp.SourceFunc.fromBatch(f_rows), grid, point)
        vec = operators(pysgpp.SourceFunc.fromBatch(f_numpy), grid, point)
        tag = f"scalar b{boundary} d{n_dim}"
        if bitwise:
            check(rows == ref, f"{tag}: batch (rows) {rows} != per point {ref}")
        for a, b in zip(vec + rows, ref + ref):
            check(abs(a - b) <= 1e-12 * max(1.0, abs(b)), f"{tag}: batch {a} vs per point {b}")


def check_vector(bitwise):
    for nodes, boundary, n_dim, domain in grids():
        _, grid = make_grid(nodes, boundary, n_dim, 4, domain)
        point = pysgpp.DataVector([0.5 * (a + b) for a, b in domain])
        ref = [tolist(r) for r in operators(pysgpp.VectorSourceFunc(v_point, M), grid, point)]
        bat = [tolist(r) for r in
               operators(pysgpp.VectorSourceFunc.fromBatch(v_rows, M), grid, point)]
        tag = f"vector b{boundary} d{n_dim}"
        if bitwise:
            check(bat == ref, f"{tag}: batch {bat} != per point {ref}")
        for ra, rb in zip(bat, ref):
            for a, b in zip(ra, rb):
                check(abs(a - b) <= 1e-12 * max(1.0, abs(b)), f"{tag}: {a} vs {b}")


def close(a, b):
    return all(abs(x - y) <= 1e-12 * max(1.0, abs(y)) for x, y in zip(a, b))


def check_cached():
    _, grid = make_grid(pysgpp.getClenshawCurtisNodeGenFunc(), 0, 3, 5, [(0, 1)] * 3)
    points = [0]

    def counted(pts):
        points[0] += len(pts)
        values = f_numpy(pts)
        values[0] = math.nan  # one NaN per batch: never stored
        return values

    plain = pysgpp.quadrature(pysgpp.SourceFunc.fromBatch(f_numpy), grid)
    cached = pysgpp.CachedSourceFunc.fromBatch(f_numpy)
    first = pysgpp.quadrature(cached, grid)
    after_first = cached.statistics()
    second = pysgpp.quadrature(cached, grid)
    after_second = cached.statistics()
    check(close([first, second], [plain, plain]), f"cached batch {first}, {second} != {plain}")
    check(after_second.hits - after_first.hits == after_second.lookups - after_first.lookups,
          "second call of a cached batch function not all hits")

    nan_func = pysgpp.CachedSourceFunc.fromBatch(counted)
    pysgpp.quadrature(nan_func, grid)
    first_points = points[0]
    points[0] = 0
    pysgpp.quadrature(nan_func, grid)
    check(0 < points[0] < first_points,
          f"NaN points not re-evaluated alone: {points[0]} of {first_points} points")

    vcached = pysgpp.CachedVectorSourceFunc.fromBatch(v_rows, M)
    vref = tolist(pysgpp.quadrature(pysgpp.VectorSourceFunc(v_point, M), grid))
    check(close(tolist(pysgpp.quadrature(vcached, grid)), vref), "cached vector batch differs")
    check(close(tolist(pysgpp.quadrature(vcached, grid)), vref), "cached vector batch (hits) differs")


def check_errors():
    _, grid = make_grid(pysgpp.getEquidistantNodeGenFunc(), 0, 2, 5, [(0, 1)] * 2)
    calls = [0]

    def failing(points):
        calls[0] += 1
        if calls[0] == 3:
            raise KeyError("third batch")
        return f_numpy(points)

    func = pysgpp.SourceFunc.fromBatch(failing)
    try:
        pysgpp.quadrature(func, grid)
        check(False, "exception in a batch not re-raised")
    except KeyError:
        pass
    check(math.isfinite(pysgpp.quadrature(func, grid)), "batch function not reusable")

    for bad, kind in ((lambda pts: np.zeros(len(pts) + 1), ValueError),
                      (lambda pts: "x", ValueError)):
        try:
            pysgpp.quadrature(pysgpp.SourceFunc.fromBatch(bad), grid)
            check(False, f"bad batch result not rejected ({kind.__name__})")
        except (ValueError, TypeError):
            pass

    def raising(p):
        raise RuntimeError("per point")

    try:
        pysgpp.quadrature(pysgpp.SourceFunc(raising), grid)
        check(False, "exception of a per-point callable inside a block not re-raised")
    except RuntimeError as error:
        check("per point" in str(error), f"wrong exception: {error}")

    matrix = pysgpp.DataMatrix(4, 2)
    for r in range(4):
        matrix.set(r, 0, 0.1 * r)
        matrix.set(r, 1, 0.2)
    for func in (pysgpp.SourceFunc(f_point), pysgpp.SourceFunc.fromBatch(f_numpy)):
        values = tolist(func.evaluate(matrix))
        expect = [f_point((0.1 * r, 0.2)) for r in range(4)]
        check(all(abs(a - b) <= 1e-14 for a, b in zip(values, expect)), f"evaluate(matrix) {values}")
    vvalues = tolist(pysgpp.VectorSourceFunc.fromBatch(v_rows, M).evaluate(matrix))
    check(len(vvalues) == 4 * M and abs(vvalues[M + 1] - v_point((0.1, 0.2))[1]) < 1e-14,
          f"vector evaluate(matrix) {vvalues}")

    del func
    gc.collect()

    def fresh(pts):
        return f_numpy(pts)

    before = sys.getrefcount(fresh)
    func = pysgpp.CachedSourceFunc.fromBatch(fresh)
    pysgpp.quadrature(func, grid)
    del func
    gc.collect()
    check(sys.getrefcount(fresh) == before, "reference count of the batch callable not restored")


def timing():
    _, grid = make_grid(pysgpp.getClenshawCurtisNodeGenFunc(), 0, 5, 5, [(0, 1)] * 5)
    n_eval = [0]

    def counted(p):
        n_eval[0] += 1
        return f_point(p)

    func = pysgpp.SourceFunc(counted)
    pysgpp.quadrature(func, grid)  # warm-up
    n_eval[0] = 0
    start = time.perf_counter()
    pysgpp.quadrature(func, grid)
    elapsed = time.perf_counter() - start
    print(f"timing per-point callable: {n_eval[0]} evaluations, "
          f"{1e6 * elapsed / n_eval[0]:.2f} us per evaluation")
    if hasattr(pysgpp.SourceFunc, "fromBatch"):
        for name, fb in (("rows", f_rows), ("numpy", f_numpy)):
            bfunc = pysgpp.SourceFunc.fromBatch(fb)
            pysgpp.quadrature(bfunc, grid)
            start = time.perf_counter()
            pysgpp.quadrature(bfunc, grid)
            elapsed = time.perf_counter() - start
            print(f"timing batch callable ({name}): {1e6 * elapsed / n_eval[0]:.3f} us per point")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--timing-only", action="store_true")
    parser.add_argument("--bitwise", action="store_true",
                        help="require bitwise equality (OMP_NUM_THREADS=1)")
    args = parser.parse_args()
    if not args.timing_only:
        check_scalar(args.bitwise)
        check_vector(args.bitwise)
        check_cached()
        check_errors()
    timing()
    print("FAILURES:", len(failures) if failures else 0)
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
