# TODO: Redo example (see also identical C++ example)

#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_sin_func_deltas_sg_quadrature_interpolation_py Deltas sparse grid (Python)
##
## The function
## \f[
##   f\colon [0, 1]^3 \to \mathbb{R},\quad f(x) := \prod_{k=0}^{2} \cos(4 \pi x_k)
## \f]
## is integrated and interpolated (globally and linearly) on a complete sparse grid of maximum
## level 6 with Clenshaw-Curtis nodes, once on a SparseGrid and once on the DeltasSparseGrid of
## the same generation instruction. Both give the same results up to round-off; the
## DeltasSparseGrid operators evaluate the function once per grid point (or once per distinct
## corner for the linear interpolation) instead of once per tensor grid the point belongs to,
## which matters most for expensive functions and for Python callables (every call takes the
## GIL). A DeltasSparseGrid requires nested nodes (equidistant or Clenshaw-Curtis nodes with the
## doubling growth): first-type Chebyshev nodes make its constructor raise RuntimeError.
## Port of examples/c++/deltas_sparse_grid/sin_func_deltas_sg_quadrature_interpolation.cpp.

import itertools
import math
import pysgpp

## Counts the evaluations of func. The operators call func from several threads (one at a time,
## each holding the GIL); next() of an itertools.count is atomic, unlike 'count += 1'.
evaluations = itertools.count()


def func(point):
  next(evaluations)
  result = 1.0
  for dim in range(len(point)):
    result *= math.cos(4 * math.pi * point[dim])
  return result


## Runs op and returns its result and the number of evaluations of func it made.
def countEvaluations(op):
  start = next(evaluations)
  result = op()
  return result, next(evaluations) - start - 1


def compare(name, sgRun, dsgRun):
  (sgResult, sgEvals), (dsgResult, dsgEvals) = sgRun, dsgRun
  print(f"{name}: SparseGrid {sgResult:.15g} ({sgEvals} evaluations), DeltasSparseGrid "
        f"{dsgResult:.15g} ({dsgEvals} evaluations), difference {abs(sgResult - dsgResult):.1e}")
  assert abs(sgResult - dsgResult) <= 1e-12 and dsgEvals <= sgEvals


sourceFunc = pysgpp.SourceFunc(func)

genInstr = pysgpp.CompleteSGGenInstr(6, 3)
genInstr.setDomain((0, 1))
genInstr.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
genInstr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
genInstr.setBoundaryLevelOffset(0)

## Both grids are built from the same generation instruction; the operators select the grid type
## by their arguments.
sg = pysgpp.SparseGrid(genInstr)
dsg = pysgpp.DeltasSparseGrid(genInstr)
print(f"SparseGrid: {sg.nTG()} tensor grids, DeltasSparseGrid: {dsg.nTG()} delta tensor grids")

compare("quadrature", countEvaluations(lambda: pysgpp.quadrature(sourceFunc, sg)),
        countEvaluations(lambda: pysgpp.quadrature(sourceFunc, dsg)))

point = pysgpp.DataVector([0.9173, 0.13494, 0.4184])
compare("interpolate", countEvaluations(lambda: pysgpp.interpolate(sourceFunc, point, sg)),
        countEvaluations(lambda: pysgpp.interpolate(sourceFunc, point, dsg)))
compare("interpolateLinear",
        countEvaluations(lambda: pysgpp.interpolateLinear(sourceFunc, point, sg)),
        countEvaluations(lambda: pysgpp.interpolateLinear(sourceFunc, point, dsg)))

## Vector-valued source functions (and the cached source functions) work on both grid types.
vectorSourceFunc = pysgpp.VectorSourceFunc(lambda p: [func(p), p[0] * p[1] * p[2]], 2)
integrals = pysgpp.quadrature(vectorSourceFunc, dsg)
print(f"vector quadrature on the DeltasSparseGrid: {integrals[0]:.15g}, {integrals[1]:.15g} "
      f"(exact 0, 0.125)")

## Non-nested nodes cannot form a deltas sparse grid.
genInstr.setNodeGenFunc(pysgpp.getFirstTypeChebyshevNodeGenFunc())
try:
  pysgpp.DeltasSparseGrid(genInstr)
  raise AssertionError("DeltasSparseGrid accepted non-nested nodes")
except RuntimeError as e:
  print(f"first-type Chebyshev nodes: {e}")
