#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_sin_func_textbook_operators_py Textbook operators (Python)
##
## Quadrature, global and linear interpolation of
## \f[
##   f\colon [0, 1]^3 \to \mathbb{R},\quad f(x) := \prod_{k=0}^{2} \cos(4 \pi x_k)
## \f]
## on a complete sparse grid of maximum level 6 with Clenshaw-Curtis nodes, once with the textbook
## operators (textbookQuadrature, textbookInterpolate, textbookInterpolateLinear: a plain loop over
## the tensor grids, each solved on its own; written to be read) and once with the optimized
## operators (quadrature, interpolate, interpolateLinear). Both give the same results up to
## round-off and evaluate the function at the same points. Port of
## examples/c++/textbook/sin_func_textbook_operators.cpp.

import math
import pysgpp


def func(point):
  result = 1.0
  for dim in range(len(point)):
    result *= math.cos(4 * math.pi * point[dim])
  return result


sourceFunc = pysgpp.SourceFunc(func)

genInstr = pysgpp.CompleteSGGenInstr(6, 3)
genInstr.setDomain((0, 1))
genInstr.setNodeFamily(pysgpp.getClenshawCurtisNodes())
genInstr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
genInstr.setBoundaryLevelOffset(0)
sg = pysgpp.SparseGrid(genInstr)

point = pysgpp.DataVector((0.9173, 0.13494, 0.4184))
pairs = [
    ("quadrature (exact 0)", pysgpp.textbookQuadrature(sourceFunc, sg),
     pysgpp.quadrature(sourceFunc, sg)),
    ("interpolate", pysgpp.textbookInterpolate(sourceFunc, point, sg),
     pysgpp.interpolate(sourceFunc, point, sg)),
    ("interpolateLinear", pysgpp.textbookInterpolateLinear(sourceFunc, point, sg),
     pysgpp.interpolateLinear(sourceFunc, point, sg)),
]

print(f"f(point) = {func((0.9173, 0.13494, 0.4184)):.15g}")
for name, textbook, optimized in pairs:
  print(f"{name}: textbook {textbook:.15g}, optimized {optimized:.15g}, "
        f"difference {abs(textbook - optimized):.1e}")
  assert abs(textbook - optimized) <= 1e-12

## Vector-valued source function: output 1 is x_0 x_1 x_2 (exact integral 1/8). The result is a
## DataVector (index it with range(len(...)) or convert it with .array()).
vectorSourceFunc = pysgpp.VectorSourceFunc(
    lambda p: [func(p), p[0] * p[1] * p[2]], 2)
integrals = pysgpp.textbookQuadrature(vectorSourceFunc, sg)
print(f"vector textbook quadrature: {integrals[0]:.15g}, {integrals[1]:.15g} (exact 0, 0.125)")
assert abs(integrals[1] - 0.125) <= 1e-12
