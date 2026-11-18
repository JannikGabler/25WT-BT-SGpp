#!/usr/bin/env python3
"""WP-73: checks of the Python bindings of the cross-call operator data cache.

Usage (from the repo root, through the repo's .venv):
  PYTHONPATH=<repo>/lib:<repo>/lib/pysgpp LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=1 \
      .venv/bin/python combigrid/perf/results/WP-73/python_checks.py

Checks:
- option classes QuadratureOptions / GlobalInterpolationOptions / LinearInterpolationOptions exist
  with the C++ defaults (False / True / True) and a settable useCrossCallOperatorDataCache;
- SparseGrid and DeltasSparseGrid have clearCrossCallOperatorDataCache,
  getCrossCallOperatorDataCacheEntries (sequence of CrossCallOperatorDataInfo with operatorName,
  description, key, bytes) and getCrossCallOperatorDataCacheBytes; the cache itself and the span
  overloads are not exposed;
- defaults: only the DSG interpolate / interpolateLinear store an entry; quadrature with the
  option on stores one per grid type; option off stores nothing;
- results with and without reuse equal (bitwise at 1 thread, else within 1e-12 relative) for all four operators, scalar
  (SourceFunc) and vector (VectorSourceFunc, m = 2, the overloads returning a DataVector);
- clear empties the cache (bytes 0), the next call stores again and gives the same results.
Exit code 1 if any check fails.
"""
import math
import os
import sys

import pysgpp

FAILURES = []
# Bitwise at 1 thread; with more threads the reductions of the quadratures depend on the work split
BITWISE = os.environ.get("OMP_NUM_THREADS") == "1"


def same(a, b):
    if BITWISE:
        return a == b
    return len(a) == len(b) and all(abs(x - y) <= 1e-12 * (1 + abs(y)) for x, y in zip(a, b))


def check(cond, what):
    if not cond:
        FAILURES.append(what)
        print("FAIL:", what)


def make_instr(nodes, d, level):
    instr = pysgpp.CompleteSGGenInstr(level, d)
    instr.setDomain((-0.5, 1.5))
    instr.setNodeGenFunc(nodes)
    instr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    instr.setBoundaryLevelOffset(1)
    return instr


def f(x):
    return math.prod(math.cos(0.7 * x[k]) + 0.1 * x[k] for k in range(len(x)))


def fv(x):
    return [f(x), sum(x[k] for k in range(len(x)))]


def names(grid):
    return [e.operatorName for e in grid.getCrossCallOperatorDataCacheEntries()]


def dv(values):
    return [values[k] for k in range(values.getSize())]


def all_results(sg, dsg, point, use):
    sf = pysgpp.SourceFunc(f)
    vf = pysgpp.VectorSourceFunc(fv, 2)
    q = pysgpp.QuadratureOptions()
    q.useCrossCallOperatorDataCache = use
    gi = pysgpp.GlobalInterpolationOptions()
    gi.useCrossCallOperatorDataCache = use
    li = pysgpp.LinearInterpolationOptions()
    li.useCrossCallOperatorDataCache = use
    return [pysgpp.quadrature(sf, sg, q), *dv(pysgpp.quadrature(vf, sg, q)),
            pysgpp.quadrature(sf, dsg, q), *dv(pysgpp.quadrature(vf, dsg, q)),
            pysgpp.interpolate(sf, point, dsg, gi), *dv(pysgpp.interpolate(vf, point, dsg, gi)),
            pysgpp.interpolateLinear(sf, point, dsg, li),
            *dv(pysgpp.interpolateLinear(vf, point, dsg, li))]


def main():
    # Options and their defaults
    check(pysgpp.QuadratureOptions().useCrossCallOperatorDataCache is False, "quad default")
    check(pysgpp.GlobalInterpolationOptions().useCrossCallOperatorDataCache is True,
          "interp default")
    check(pysgpp.LinearInterpolationOptions().useCrossCallOperatorDataCache is True,
          "interplin default")

    for nodes in (pysgpp.getEquidistantNodeGenFunc(), pysgpp.getClenshawCurtisNodeGenFunc()):
        for d in (1, 2, 3):
            instr = make_instr(nodes, d, 4)
            sg = pysgpp.SparseGrid(instr)
            dsg = pysgpp.DeltasSparseGrid(instr)
            point = pysgpp.DataVector([-0.3 + 0.37 * (k + 1) for k in range(d)])
            sf = pysgpp.SourceFunc(f)

            # Internals not exposed
            for grid in (sg, dsg):
                check(not hasattr(grid, "getCrossCallOperatorDataCache"), "cache exposed")
                check(not hasattr(grid, "getOperatorData"), "old accessor exposed")

            # Defaults
            pysgpp.quadrature(sf, sg)
            pysgpp.interpolate(sf, point, sg)
            pysgpp.interpolateLinear(sf, point, sg)
            pysgpp.quadrature(sf, dsg)
            check(len(sg.getCrossCallOperatorDataCacheEntries()) == 0, "SG default stores")
            check(len(dsg.getCrossCallOperatorDataCacheEntries()) == 0, "DSG quad default stores")
            pysgpp.interpolate(sf, point, dsg)
            pysgpp.interpolateLinear(sf, point, dsg)
            check(names(dsg) == ["interpolate (DeltasSparseGrid)",
                                 "interpolateLinear (DeltasSparseGrid)"], f"DSG defaults {names(dsg)}")
            dsg.clearCrossCallOperatorDataCache()
            check(dsg.getCrossCallOperatorDataCacheBytes() == 0, "clear bytes")

            # Option off stores nothing; on stores one entry per operator and grid
            off = all_results(sg, dsg, point, False)
            check(len(names(sg)) == 0 and len(names(dsg)) == 0, "option off stores")
            on = all_results(sg, dsg, point, True)
            on2 = all_results(sg, dsg, point, True)
            check(names(sg) == ["quadrature (SparseGrid)"], f"SG on {names(sg)}")
            check(sorted(names(dsg)) == sorted(["quadrature (DeltasSparseGrid)",
                                                "interpolate (DeltasSparseGrid)",
                                                "interpolateLinear (DeltasSparseGrid)"]),
                  f"DSG on {names(dsg)}")
            check(same(on, off) and same(on2, off), f"results differ {d}")
            entries = dsg.getCrossCallOperatorDataCacheEntries()
            check(all(e.bytes > 0 and e.description and e.key == "" for e in entries),
                  "entry fields")
            check(sum(e.bytes for e in entries) == dsg.getCrossCallOperatorDataCacheBytes(),
                  "total bytes")

            # Clear and rebuild
            sg.clearCrossCallOperatorDataCache()
            dsg.clearCrossCallOperatorDataCache()
            check(len(names(sg)) == 0 and dsg.getCrossCallOperatorDataCacheBytes() == 0, "clear")
            check(same(all_results(sg, dsg, point, True), off), "results after clear")
            check(len(names(dsg)) == 3, "rebuilt after clear")

    # The span overloads are hidden: a list as 4th argument is not accepted
    instr = make_instr(pysgpp.getEquidistantNodeGenFunc(), 2, 3)
    dsg = pysgpp.DeltasSparseGrid(instr)
    try:
        pysgpp.interpolateLinear(pysgpp.VectorSourceFunc(fv, 2), pysgpp.DataVector([0.1, 0.2]),
                                 dsg, [0.0, 0.0])
        check(False, "span overload accepted a list")
    except TypeError:
        pass

    print("WP-73 python checks:", "FAILED" if FAILURES else "all passed")
    return 1 if FAILURES else 0


if __name__ == "__main__":
    sys.exit(main())
