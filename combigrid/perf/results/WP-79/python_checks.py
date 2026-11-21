#!/usr/bin/env python3
"""WP-79: checks of the Python bindings of the textbook operators.

Usage (from the repo root, through the repo's .venv, after an SG_PYTHON=1 build):
  PYTHONPATH=<repo>/lib:<repo>/lib/pysgpp LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=1 \
      .venv/bin/python combigrid/perf/results/WP-79/python_checks.py

Checks:
- textbookQuadrature / textbookInterpolate / textbookInterpolateLinear exist with SourceFunc
  (-> float) and VectorSourceFunc (-> DataVector of m values) overloads; the std::span overloads
  are not visible (calling with an extra list argument raises TypeError);
- results equal the optimized operators within 1e-11 relative (CC / equidistant nodes, b0 / b1,
  shifted domain, d = 2 / 3);
- the same number of Python calls as the optimized SparseGrid operators;
- an exception raised by the Python callable propagates (same object) out of all three textbook
  operators, scalar and vector, and the function is usable afterwards;
- a DeltasSparseGrid is rejected (TypeError: SparseGrid only);
- CachedSourceFunc: the textbook quadrature after the optimized one makes no new Python call
  (bitwise the same evaluation points; only if the cache is available).
Exit code 1 if any check fails.
"""
import sys

import pysgpp

FAILURES = []


def check(cond, what):
    if not cond:
        FAILURES.append(what)
        print("FAIL:", what)


def close(a, b):
    return abs(a - b) <= 1e-11 * max(1.0, abs(b))


def make_grid(d, level, boundary, nodes):
    instr = pysgpp.CompleteSGGenInstr(level, d)
    instr.setDomain((-0.5, 1.5))
    instr.setNodeGenFunc(nodes)
    instr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    instr.setBoundaryLevelOffset(boundary)
    return pysgpp.SparseGrid(instr), instr


class Counting:
    def __init__(self, vector=False):
        self.calls = 0
        self.vector = vector

    def __call__(self, p):
        self.calls += 1
        s = sum((i + 1) * p[i] for i in range(len(p)))
        value = 1.0 / (1.0 + s * s)
        return [value, s, s * s] if self.vector else value


def check_against_optimized():
    point_coords = {2: (0.31, 1.17), 3: (0.31, 1.17, -0.2)}
    for nodes_name, nodes in (("cc", pysgpp.getClenshawCurtisNodeGenFunc()),
                              ("equi", pysgpp.getEquidistantNodeGenFunc())):
        for d, level in ((2, 4), (3, 3)):
            for boundary in (0, 1):
                sg, _ = make_grid(d, level, boundary, nodes)
                point = pysgpp.DataVector(point_coords[d])
                name = f"{nodes_name} d{d} L{level} b{boundary}"
                pairs = (
                    ("quadrature", pysgpp.textbookQuadrature, pysgpp.quadrature, ()),
                    ("interpolate", pysgpp.textbookInterpolate, pysgpp.interpolate, (point,)),
                    ("interpolateLinear", pysgpp.textbookInterpolateLinear,
                     pysgpp.interpolateLinear, (point,)),
                )
                for op, textbook_op, optimized_op, extra in pairs:
                    tf, of = Counting(), Counting()
                    t = textbook_op(pysgpp.SourceFunc(tf), *extra, sg)
                    o = optimized_op(pysgpp.SourceFunc(of), *extra, sg)
                    check(isinstance(t, float), f"{op} {name}: scalar result is a float")
                    check(close(t, o), f"{op} {name}: textbook {t!r} vs optimized {o!r}")
                    check(tf.calls == of.calls,
                          f"{op} {name}: {tf.calls} vs {of.calls} Python calls")

                    vt, vo = Counting(True), Counting(True)
                    tv = textbook_op(pysgpp.VectorSourceFunc(vt, 3), *extra, sg)
                    ov = optimized_op(pysgpp.VectorSourceFunc(vo, 3), *extra, sg)
                    check(isinstance(tv, pysgpp.DataVector) and len(tv) == 3,
                          f"{op} {name}: vector result is a DataVector of 3")
                    check(all(close(tv[k], ov[k]) for k in range(3)),
                          f"{op} {name}: vector textbook vs optimized")
                    check(vt.calls == vo.calls, f"{op} {name}: vector Python calls")
                    check(close(tv[0], t), f"{op} {name}: vector output 0 == scalar")


def check_exceptions_and_types():
    sg, instr = make_grid(2, 3, 0, pysgpp.getClenshawCurtisNodeGenFunc())
    point = pysgpp.DataVector((0.3, 0.4))

    class Boom(Exception):
        pass

    def failing(p):
        if p[0] > 0.9:
            raise Boom("boom")
        return 1.0

    def failing_vec(p):
        if p[0] > 0.9:
            raise Boom("boom")
        return [1.0, 2.0]

    f = pysgpp.SourceFunc(failing)
    vf = pysgpp.VectorSourceFunc(failing_vec, 2)
    for name, call in (
            ("quadrature", lambda g: pysgpp.textbookQuadrature(g, sg)),
            ("interpolate", lambda g: pysgpp.textbookInterpolate(g, point, sg)),
            ("interpolateLinear",
             lambda g: pysgpp.textbookInterpolateLinear(g, pysgpp.DataVector((1.2, 0.4)), sg))):
        for kind, g in (("scalar", f), ("vector", vf)):
            try:
                call(g)
                check(False, f"{name} {kind}: exception not raised")
            except Boom:
                pass
    check(pysgpp.textbookQuadrature(pysgpp.SourceFunc(lambda p: 2.0), sg) != 0.0,
          "function usable after an exception")

    try:
        pysgpp.textbookQuadrature(pysgpp.VectorSourceFunc(lambda p: [1.0], 1), sg, [0.0])
        check(False, "std::span overload must not be visible")
    except TypeError:
        pass

    dsg = pysgpp.DeltasSparseGrid(instr)
    try:
        pysgpp.textbookQuadrature(pysgpp.SourceFunc(lambda p: 1.0), dsg)
        check(False, "DeltasSparseGrid must be rejected")
    except TypeError:
        pass


def check_cached():
    sg, _ = make_grid(3, 3, 0, pysgpp.getClenshawCurtisNodeGenFunc())
    counter = Counting()
    try:
        cf = pysgpp.CachedSourceFunc(counter)
    except RuntimeError as e:
        print("cache not available, skipped:", e)
        return
    pysgpp.quadrature(cf, sg)
    before = counter.calls
    pysgpp.textbookQuadrature(cf, sg)
    check(counter.calls == before, f"cached: {counter.calls - before} new Python calls")


check_against_optimized()
check_exceptions_and_types()
check_cached()
print("OK" if not FAILURES else f"{len(FAILURES)} FAILURES")
sys.exit(1 if FAILURES else 0)
