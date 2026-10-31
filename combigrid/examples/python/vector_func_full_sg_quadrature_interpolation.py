#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_vector_func_full_sg_quadrature_interpolation_py Vector-valued source function (Python)
##
## The vector-valued function
## \f[
##   f\colon [0, 1]^3 \to \mathbb{R}^3,\quad
##   f(x) := \left(e^{x_0 + x_1 + x_2},\ x_0 x_1 x_2,\ \prod_{k=0}^{2} \cos(4 \pi x_k)\right)
## \f]
## (exact integrals \f$(e - 1)^3\f$, \f$1/8\f$ and 0) is integrated and interpolated on a complete
## sparse grid of maximum level 6 with Clenshaw-Curtis nodes. A VectorSourceFunc evaluates the
## function once per grid point for all outputs; output k of each operator equals the scalar
## operator applied to output k (up to round-off), which the example checks.

import math
import pysgpp


## The callable receives the coordinates of a point as a tuple of floats and returns the m = 3
## values as any sequence of numbers (list, tuple or NumPy array).
def func(point):
  cosProduct = 1.0

  for dim in range(len(point)):
    cosProduct *= math.cos(4 * math.pi * point[dim])

  return [math.exp(sum(point)), point[0] * point[1] * point[2], cosProduct]


nOutputs = 3
vectorSourceFunc = pysgpp.VectorSourceFunc(func, nOutputs)

genInstr = pysgpp.CompleteSGGenInstr(6, 3)
genInstr.setDomain((0, 1))
genInstr.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
genInstr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
genInstr.setBoundaryLevelOffset(0)

sg = pysgpp.SparseGrid(genInstr)

## The vector overloads return a DataVector with one entry per output. (Index it with
## range(len(...)) or convert it with .array(): iterating over a DataVector, e.g. with list() or
## sum(), does not stop at its end.)
coords = (0.9173, 0.13494, 0.4184)
point = pysgpp.DataVector(coords)
integrals = pysgpp.quadrature(vectorSourceFunc, sg)
values = pysgpp.interpolate(vectorSourceFunc, point, sg)
linearValues = pysgpp.interpolateLinear(vectorSourceFunc, point, sg)

exactIntegrals = [(math.e - 1) ** 3, 1 / 8, 0.0]
exactValues = func(coords)

## For comparison: one scalar SourceFunc per output.
for k in range(nOutputs):
  scalarSourceFunc = pysgpp.SourceFunc(lambda p, k=k: func(p)[k])
  scalarIntegral = pysgpp.quadrature(scalarSourceFunc, sg)
  scalarValue = pysgpp.interpolate(scalarSourceFunc, point, sg)
  scalarLinearValue = pysgpp.interpolateLinear(scalarSourceFunc, point, sg)

  print(f"Output {k}:")
  print(f"  quadrature:          {integrals[k]:.15g} (exact {exactIntegrals[k]:.15g}, "
        f"scalar run differs by {abs(integrals[k] - scalarIntegral):.1e})")
  print(f"  interpolate:         {values[k]:.15g} (exact {exactValues[k]:.15g}, "
        f"scalar run differs by {abs(values[k] - scalarValue):.1e})")
  print(f"  interpolateLinear:   {linearValues[k]:.15g} "
        f"(scalar run differs by {abs(linearValues[k] - scalarLinearValue):.1e})")

  assert abs(integrals[k] - scalarIntegral) <= 1e-12
  assert abs(values[k] - scalarValue) <= 1e-12
  assert abs(linearValues[k] - scalarLinearValue) <= 1e-12
