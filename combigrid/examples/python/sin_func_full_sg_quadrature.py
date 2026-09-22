#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_sin_func_full_sg_quadrature_py Quadrature on a complete sparse grid (Python)
##
## Python version of combigrid/examples/c++/quadrature/sin_func_full_sg_quadrature.cpp.
##
## The function
## \f[
##   f\colon [0, 1]^3 \to \mathbb{R},\quad
##   f(x) := \prod_{k=0}^{2} \cos(4 \pi x_k)
## \f]
## (exact integral 0) is integrated with Clenshaw-Curtis quadrature on a complete sparse grid of
## maximum level 10.

import math
import pysgpp


def func(point):
  result = 1.0

  for dim in range(len(point)):
    result *= math.cos(4 * math.pi * point[dim])

  return result


sourceFunc = pysgpp.SourceFunc(func)

genInstr = pysgpp.CompleteSGGenInstr(10, 3)
genInstr.setDomain((0, 1))
genInstr.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
genInstr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
genInstr.setBoundaryLevelOffset(0)

sg = pysgpp.SparseGrid(genInstr)

result = pysgpp.quadrature(sourceFunc, sg)

print(f"Result: {result}")
