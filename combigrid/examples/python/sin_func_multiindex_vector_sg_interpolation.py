#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_sin_func_multiindex_vector_sg_interpolation_py Global interpolation on a custom multi-index set (Python)
##
## Python version of
## combigrid/examples/c++/global_interpolation/sin_func_multiindex_vector_sg_interpolation.cpp.
##
## Instead of a complete sparse grid, the sparse grid is generated from a user-defined set of level
## multi-indices (the entry point for adaptive schemes). MIVecSGGenInstr computes the downwards
## closure of the set and the combination coefficients automatically.

import math
import pysgpp


def func(point):
  result = 1.0

  for dim in range(1, len(point)):
    result *= math.sin(4 * math.pi * point[dim])

  return result


sourceFunc = pysgpp.SourceFunc(func)

## The level multi-indices; the equivalent of the C++ brace initialisation LvlMIVec{{0, 5, 0}, ...}.
## Note: MIVecSGGenInstr references this vector. The Python bindings keep it alive for as long as
## the generation instruction (or a sparse grid built from it) exists.
mis = pysgpp.LvlMIVec([[0, 5, 0], [0, 4, 1], [0, 3, 2], [0, 2, 3], [0, 1, 4], [0, 0, 5]])

genInstr = pysgpp.MIVecSGGenInstr(mis)
genInstr.setDomain((0, 1))
genInstr.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
genInstr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
genInstr.setBoundaryLevelOffset(0)

sg = pysgpp.SparseGrid(genInstr)

point = pysgpp.DataVector([0.9173, 0.13494, 0.4184])
result = pysgpp.interpolate(sourceFunc, point, sg)

print(f"Result: {result}")
print(f"Expected: {func(point)}")
