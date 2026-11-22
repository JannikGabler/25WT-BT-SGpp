#!/usr/bin/env python3
"""WP-83: Python checks of the SG quadrature with first-kind Chebyshev (cheb1) nodes.

Usage (from the repo root, through the repo's .venv, after an SG_PYTHON=1 build):
  PYTHONPATH=<repo>/lib:<repo>/lib/pysgpp LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=<1|16> \
      .venv/bin/python combigrid/perf/results/WP-83/python_checks.py

Checks:
- quadrature with cheb1 nodes runs (before WP-83 the process terminated) for SourceFunc and
  VectorSourceFunc (m = 3), with and without the cross-call cache, b0 / b1 / no boundary nodes;
- the result equals textbookQuadrature within 1e-11 relative and is exact (1e-12) for a
  polynomial in the exactness space of the grid (degree 2^(l+1) per dimension of level l with
  boundary nodes, 2^(l+1) - 2 without);
- the vector outputs equal the scalar runs of the components within 1e-12;
- the Python callable's exception still propagates out of quadrature with cheb1 nodes;
- the bindings do not expose getQuadRule (both overloads stay %ignore'd).
Exit code 1 if any check fails.
"""
import math
import sys

import pysgpp

FAILURES = []


def check(cond, what):
    if not cond:
        FAILURES.append(what)
        print("FAIL:", what)


def make_grid(d, level, boundary, domain=(-0.5, 1.5)):
    instr = pysgpp.CompleteSGGenInstr(level, d)
    instr.setDomain(domain)
    instr.setNodeGenFunc(pysgpp.getFirstTypeChebyshevNodeGenFunc())
    instr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    instr.setBoundaryLevelOffset(boundary)
    return pysgpp.SparseGrid(instr), instr


def smooth(p):
    s = sum((i + 1) * p[i] for i in range(len(p)))
    return 1.0 / (1.0 + s * s)


def check_results():
    for d, level in ((1, 5), (2, 4), (3, 3)):
        for boundary in (0, 1, 20):
            sg, _ = make_grid(d, level, boundary)
            name = f"cheb1 d{d} L{level} b{boundary}"
            f = pysgpp.SourceFunc(smooth)
            vf = pysgpp.VectorSourceFunc(lambda p: [smooth(p), p[0], p[0] * p[0]], 3)
            value = pysgpp.quadrature(f, sg)
            textbook = pysgpp.textbookQuadrature(f, sg)
            check(abs(value - textbook) <= 1e-11 * max(1.0, abs(textbook)),
                  f"{name}: quadrature {value} vs textbook {textbook}")
            options = pysgpp.QuadratureOptions()
            options.useCrossCallOperatorDataCache = True
            check(abs(pysgpp.quadrature(f, sg, options) - value) <= 1e-14,
                  f"{name}: cached option changes the result")
            vector = pysgpp.quadrature(vf, sg)
            # Scalar runs of components 1 and 2: x0 and x0^2 on [-0.5, 1.5]^d
            vol = 2.0 ** d
            check(abs(vector[0] - value) <= 1e-12, f"{name}: vector output 0")
            check(abs(vector[1] - 0.5 * vol) <= 1e-12, f"{name}: vector output 1 (x0)")
            check(abs(vector[2] - (1.5 ** 3 + 0.5 ** 3) / 3 / 2.0 * vol) <= 1e-12,
                  f"{name}: vector output 2 (x0^2)")


def check_exactness():
    # Level-l 1D rule with boundary nodes: exact to degree 2^(l+1); 1D grid of level 4, b0: the
    # Chebyshev polynomial T_32 on [0, 1] (mean 1 / (1 - 32^2))
    for boundary, degree in ((0, 32), (20, 30)):
        sg, _ = make_grid(1, 4, boundary, (0.0, 1.0))
        f = pysgpp.SourceFunc(lambda p: math.cos(degree * math.acos(2.0 * p[0] - 1.0)))
        exact = 1.0 / (1.0 - degree * degree)
        value = pysgpp.quadrature(f, sg)
        check(abs(value - exact) <= 1e-12, f"exactness b{boundary}: {value} vs {exact}")


def check_exception():
    sg, _ = make_grid(2, 3, 0)

    class Boom(Exception):
        pass

    def failing(p):
        raise Boom("boom")

    try:
        pysgpp.quadrature(pysgpp.SourceFunc(failing), sg)
        check(False, "exception of the callable not propagated")
    except Boom:
        pass
    check(abs(pysgpp.quadrature(pysgpp.SourceFunc(smooth), sg)) > 0, "usable after exception")


def check_hidden():
    nodes = pysgpp.getFirstTypeChebyshevNodeGenFunc()
    check(not hasattr(nodes, "getQuadRule"), "getQuadRule is exposed")


if __name__ == "__main__":
    import os
    print("OMP_NUM_THREADS =", os.environ.get("OMP_NUM_THREADS"))
    check_results()
    check_exactness()
    check_exception()
    check_hidden()
    print("FAILURES:", len(FAILURES))
    sys.exit(1 if FAILURES else 0)
