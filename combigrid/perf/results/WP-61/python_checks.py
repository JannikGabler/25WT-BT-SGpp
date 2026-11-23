#!/usr/bin/env python3
"""WP-61: Python check of the bitwise thread-count independence of the SG quadrature.

Usage (from the repo root, through the repo's .venv, after an SG_PYTHON=1 build):
  PYTHONPATH=<repo>/lib:<repo>/lib/pysgpp LD_LIBRARY_PATH=<repo>/lib OMP_NUM_THREADS=<n> \
      .venv/bin/python combigrid/perf/results/WP-61/python_checks.py
Prints one line per grid with the bit patterns (float.hex) of the scalar result and the 3
vector outputs of 3 repeated calls; the output for OMP_NUM_THREADS=1 and 16 must be identical.
Exit code 1 if the repeated calls of one process differ.
"""
import math
import sys

import pysgpp


def func(p):
    s = sum((0.7 + 0.3 * i) * p[i] for i in range(len(p)))
    # Index access only: DataVector has no bounds-checked iteration / negative indices (WP-37)
    return math.exp(s) - 3.0 * math.prod(math.cos(1.3 * p[i]) for i in range(len(p)))


failed = False
for nodes in ("cc", "equi"):
    for d, level in ((3, 6), (5, 4)):
        for boundary in (0, 1):
            instr = pysgpp.CompleteSGGenInstr(level, d)
            instr.setDomain((-0.4, 1.3))
            instr.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc() if nodes == "cc"
                                 else pysgpp.getEquidistantNodeGenFunc())
            instr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
            instr.setBoundaryLevelOffset(boundary)
            sg = pysgpp.SparseGrid(instr)
            f = pysgpp.SourceFunc(func)
            vf = pysgpp.VectorSourceFunc(lambda p: [func(p), p[0], p[len(p) - 1] ** 2], 3)
            runs = []
            for _ in range(3):
                vec = pysgpp.quadrature(vf, sg)
                runs.append([pysgpp.quadrature(f, sg).hex()] + [vec[k].hex() for k in range(3)])
            if any(r != runs[0] for r in runs):
                failed = True
                print("FAIL: repeated calls differ", nodes, d, level, boundary)
            print(f"{nodes} d{d} L{level} b{boundary} nTG {sg.nTG()}:", " ".join(runs[0]))
sys.exit(1 if failed else 0)
