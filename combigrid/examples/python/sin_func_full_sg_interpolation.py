#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_sin_func_full_sg_interpolation_py Global interpolation on a complete sparse grid (Python)
##
## Python version of combigrid/examples/c++/global_interpolation/sin_func_full_sg_interpolation.cpp.
##
## The function
## \f[
##   f\colon [0, 1]^3 \to \mathbb{R},\quad
##   f(x) := \prod_{k=0}^{2} \cos(4 \pi x_k)
## \f]
## is interpolated with the combination technique on a complete sparse grid of maximum level 10,
## using Clenshaw-Curtis nodes with doubling growth, and compared to the exact function value.

import math
import pysgpp


## The source function receives the coordinates of a point as a tuple of floats.
def func(point):
  result = 1.0

  for dim in range(len(point)):
    result *= math.cos(4 * math.pi * point[dim])

  return result


sourceFunc = pysgpp.SourceFunc(func)

## Configure the generation instruction: all level multi-indices with 1-norm at most 10 in three
## dimensions, Clenshaw-Curtis nodes, doubling growth, and boundary nodes on every level.
genInstr = pysgpp.CompleteSGGenInstr(10, 3)
genInstr.setDomain((0, 1))
genInstr.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
genInstr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
genInstr.setBoundaryLevelOffset(0)

## Generate the sparse grid, i.e., all tensor grids with a nonzero combination coefficient.
sg = pysgpp.SparseGrid(genInstr)

point = pysgpp.DataVector([0.9173, 0.13494, 0.4184])
result = pysgpp.interpolate(sourceFunc, point, sg)

print(f"Result: {result}")
print(f"Expected: {func(point)}")
