#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_extended_const_func_full_sg_quadrature_py Quadrature on a custom domain (Python)
##
## Python version of combigrid/examples/c++/quadrature/extended_const_func_full_sg_quadrature.cpp.
##
## A constant function is integrated over [-1, 1]^2 (volume 4) using Clenshaw-Curtis nodes with
## linear growth. The quadrature result is scaled by the volume of the domain automatically.

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

result = pysgpp.quadrature(sourceFunc, sg)

print(f"Result: {result}")
