#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_sin_func_multiindex_vector_sg_quadrature_py Quadrature on a custom multi-index set (Python)
##
## Python version of combigrid/examples/c++/quadrature/sin_func_multiindex_vector_sg_quadrature.cpp.
##
## The sparse grid is generated from a user-defined set of level multi-indices via MIVecSGGenInstr.

import math

import pysgpp


def func(point):
  result = 1.0

  for dim in range(1, len(point)):
    result *= math.cos(4 * math.pi * point[dim])

  return result


sourceFunc = pysgpp.SourceFunc(func)

mis = pysgpp.LvlMIVec([[0, 5, 0], [0, 4, 1], [0, 3, 2], [0, 2, 3], [0, 1, 4], [0, 0, 5]])

genInstr = pysgpp.MIVecSGGenInstr(mis)
genInstr.setDomain((0, 1))
genInstr.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
genInstr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
genInstr.setBoundaryLevelOffset(0)

sg = pysgpp.SparseGrid(genInstr)

result = pysgpp.quadrature(sourceFunc, sg)

print(f"Result: {result}")
