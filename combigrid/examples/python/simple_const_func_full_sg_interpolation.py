#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_simple_const_func_full_sg_interpolation_py Interpolation with default settings (Python)
##
## Python version of
## combigrid/examples/c++/global_interpolation/simple_const_func_full_sg_interpolation.cpp.
##
## A constant function is interpolated on a complete sparse grid using the default settings of the
## generation instruction (domain [0, 1]^d, equidistant nodes, doubling growth, boundary offset 0).

import pysgpp

def func(point):
  return -13.7


sourceFunc = pysgpp.SourceFunc(func)

genInstr = pysgpp.CompleteSGGenInstr(2, 2)

sg = pysgpp.SparseGrid(genInstr)

point = pysgpp.DataVector([0.9173, 0.13494])
result = pysgpp.interpolate(sourceFunc, point, sg)

print(f"Result: {result}")
