#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_simple_const_func_full_sg_quadrature_py Quadrature with default settings (Python)
##
## Python version of combigrid/examples/c++/quadrature/simple_const_func_full_sg_quadrature.cpp.
##
## A constant function is integrated over [0, 1]^2 on a complete sparse grid using the default
## settings of the generation instruction.

import pysgpp


def func(point):
  return -13.7


sourceFunc = pysgpp.SourceFunc(func)

genInstr = pysgpp.CompleteSGGenInstr(2, 2)

sg = pysgpp.SparseGrid(genInstr)

result = pysgpp.quadrature(sourceFunc, sg)

print(f"Result: {result}")
