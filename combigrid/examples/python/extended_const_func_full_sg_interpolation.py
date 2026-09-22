#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_extended_const_func_full_sg_interpolation_py Interpolation on a custom domain (Python)
##
## Python version of
## combigrid/examples/c++/global_interpolation/extended_const_func_full_sg_interpolation.cpp.
##
## A constant function is interpolated on [-1, 1]^2 using Clenshaw-Curtis nodes with linear growth.
## Boundary nodes are only added to tensor grids whose level is at least 2 in that dimension.

import pysgpp


def func(point):
  return -13.7


sourceFunc = pysgpp.SourceFunc(func)

genInstr = pysgpp.CompleteSGGenInstr(16, 2)
genInstr.setDomain((-1, 1))
genInstr.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
genInstr.setLvl2GPCntFunc(pysgpp.linearLvl2GPCntFunction)
genInstr.setBoundaryLevelOffset(2)

sg = pysgpp.SparseGrid(genInstr)

point = pysgpp.DataVector([0.9173, 0.13494])
result = pysgpp.interpolate(sourceFunc, point, sg)

print(f"Result: {result}")
