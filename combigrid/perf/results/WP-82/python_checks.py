#!/usr/bin/env python3
"""WP-82: checks of the Python bindings for the DSG quadrature default (cross-call cache on).

Usage (from the repo root, through the repo's .venv, after an SG_PYTHON=1 build):
  PYTHONPATH=<repo>/lib:<repo>/lib/pysgpp LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=1 \
      .venv/bin/python combigrid/perf/results/WP-82/python_checks.py

Checks:
- option classes and their defaults agree with C++: QuadratureOptions False (SparseGrid),
  DeltasSparseGridQuadratureOptions True, GlobalInterpolationOptions True,
  LinearInterpolationOptions True; the field is settable;
- defaults: SparseGrid quadrature (scalar + vector) stores nothing; repeated DSG quadrature calls
  (scalar + vector) store exactly one entry "quadrature (DeltasSparseGrid)";
- DSG quadrature with DeltasSparseGridQuadratureOptions(off) stores nothing, with explicit on one
  entry; results with default / on / off equal (bitwise at 1 thread, else 1e-12 relative);
- a QuadratureOptions object is rejected by the DSG quadrature (TypeError: the options types differ
  per grid type) and a DeltasSparseGridQuadratureOptions object by the SparseGrid quadrature;
- clearCrossCallOperatorDataCache empties the cache, the next default call stores again.
Exit code 1 if any check fails.
"""
import math
import os
import sys

import pysgpp

FAILURES = []
BITWISE = os.environ.get("OMP_NUM_THREADS") == "1"


def same(a, b):
    if BITWISE:
        return a == b
    return len(a) == len(b) and all(abs(x - y) <= 1e-12 * (1 + abs(y)) for x, y in zip(a, b))


def check(cond, what):
    if not cond:
        FAILURES.append(what)
        print("FAIL:", what)


def make_instr(d, level, boundary):
    instr = pysgpp.CompleteSGGenInstr(level, d)
    instr.setDomain((-0.5, 1.5))
    instr.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
    instr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    instr.setBoundaryLevelOffset(boundary)
    return instr


def f(x):
    return math.prod(math.cos(0.7 * x[k]) + 0.1 * x[k] for k in range(len(x)))


def fv(x):
    return [f(x), sum(x[k] for k in range(len(x)))]


def names(grid):
    return [e.operatorName for e in grid.getCrossCallOperatorDataCacheEntries()]


def dv(values):
    return [values[k] for k in range(values.getSize())]


def dsg_options(use):
    options = pysgpp.DeltasSparseGridQuadratureOptions()
    options.useCrossCallOperatorDataCache = use
    return options


def check_defaults():
    check(pysgpp.QuadratureOptions().useCrossCallOperatorDataCache is False,
          "QuadratureOptions default False")
    check(pysgpp.DeltasSparseGridQuadratureOptions().useCrossCallOperatorDataCache is True,
          "DeltasSparseGridQuadratureOptions default True")
    check(pysgpp.GlobalInterpolationOptions().useCrossCallOperatorDataCache is True,
          "GlobalInterpolationOptions default True")
    check(pysgpp.LinearInterpolationOptions().useCrossCallOperatorDataCache is True,
          "LinearInterpolationOptions default True")
    check(dsg_options(False).useCrossCallOperatorDataCache is False, "field settable")


def dsg_results(dsg, options=None):
    sf = pysgpp.SourceFunc(f)
    vf = pysgpp.VectorSourceFunc(fv, 2)
    if options is None:
        return [pysgpp.quadrature(sf, dsg), *dv(pysgpp.quadrature(vf, dsg))]
    return [pysgpp.quadrature(sf, dsg, options), *dv(pysgpp.quadrature(vf, dsg, options))]


def check_grid(d, level, boundary):
    tag = f"d{d} L{level} b{boundary}"
    instr = make_instr(d, level, boundary)
    sg = pysgpp.SparseGrid(instr)
    sf = pysgpp.SourceFunc(f)
    vf = pysgpp.VectorSourceFunc(fv, 2)
    pysgpp.quadrature(sf, sg)
    pysgpp.quadrature(vf, sg)
    check(names(sg) == [], f"{tag}: SG quadrature default stores nothing")

    off_grid = pysgpp.DeltasSparseGrid(instr)
    off = dsg_results(off_grid, dsg_options(False))
    check(names(off_grid) == [], f"{tag}: DSG quadrature off stores nothing")

    dsg = pysgpp.DeltasSparseGrid(instr)
    first = dsg_results(dsg)
    later = dsg_results(dsg)
    check(names(dsg) == ["quadrature (DeltasSparseGrid)"],
          f"{tag}: DSG quadrature default stores one entry: {names(dsg)}")
    check(dsg.getCrossCallOperatorDataCacheBytes() > 0, f"{tag}: bytes > 0")
    check(same(first, off) and same(later, off), f"{tag}: default results equal off")
    check(same(dsg_results(dsg, dsg_options(True)), off), f"{tag}: on results equal off")

    dsg.clearCrossCallOperatorDataCache()
    check(names(dsg) == [] and dsg.getCrossCallOperatorDataCacheBytes() == 0, f"{tag}: clear")
    check(same(dsg_results(dsg), off), f"{tag}: results after clear")
    check(len(names(dsg)) == 1, f"{tag}: stored again after clear")

    for grid, wrong, what in ((dsg, pysgpp.QuadratureOptions(), "DSG + QuadratureOptions"),
                              (sg, dsg_options(True), "SG + DeltasSparseGridQuadratureOptions")):
        try:
            pysgpp.quadrature(sf, grid, wrong)
            check(False, f"{tag}: {what} accepted")
        except TypeError:
            pass


def main():
    check_defaults()
    for d, level, boundary in ((1, 5, 0), (2, 5, 1), (3, 4, 0), (4, 3, 1)):
        check_grid(d, level, boundary)
    print(f"python_checks (OMP_NUM_THREADS={os.environ.get('OMP_NUM_THREADS')}):",
          "OK" if not FAILURES else f"{len(FAILURES)} FAILURES")
    return 1 if FAILURES else 0


if __name__ == "__main__":
    sys.exit(main())
