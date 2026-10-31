#!/usr/bin/python
# Copyright (C) 2008-today The SG++ project
# This file is part of the SG++ project. For conditions of distribution and
# use, please see the copyright notice provided with SG++ or at
# sgpp.sparsegrids.org

## \page example_combigrid_cached_func_full_sg_quadrature_interpolation_py Cached source function (Python)
##
## An "expensive" function
## \f[
##   f\colon [-1, 1]^4 \to \mathbb{R},\quad f(x) := e^{-\|x\|^2}
## \f]
## is integrated and interpolated on a complete sparse grid of maximum level 5 with
## Clenshaw-Curtis nodes. A CachedSourceFunc stores every value it computes in a SourceFuncCache
## (in memory, keyed by the point): the combination technique evaluates shared grid points of
## different tensor grids only once, the interpolations after the quadrature call the Python
## function not at all (all grid points are cached), and cache hits do not take Python's GIL. The
## results equal those of a plain SourceFunc.
##
## The cache needs Boost >= 1.81 and xxHash >= 0.8 when SG++ is built; otherwise the classes exist,
## but creating them raises RuntimeError (SourceFuncCache.isAvailable() tells which build this is).

import math
import sys

import pysgpp

if not pysgpp.SourceFuncCache.isAvailable():
  print("SG++ was built without the source-function cache (needs Boost >= 1.81 and xxHash >= 0.8)")
  sys.exit(0)

calls = 0


## The callable receives the coordinates of a point as a tuple of floats.
def func(point):
  global calls
  calls += 1
  return math.exp(-sum(x * x for x in point))


genInstr = pysgpp.CompleteSGGenInstr(5, 4)
genInstr.setDomain((-1, 1))
genInstr.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
genInstr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
genInstr.setBoundaryLevelOffset(0)
sg = pysgpp.SparseGrid(genInstr)

## A cache of its own, configured by a CacheConfig (soft memory limit, default 1 GiB). A
## SourceFuncCache object can also be created explicitly and shared:
## cache = pysgpp.SourceFuncCache(); f = pysgpp.CachedSourceFunc(func, cache).
config = pysgpp.CacheConfig()
config.memoryLimit = 64 * 1024 * 1024
cachedFunc = pysgpp.CachedSourceFunc(func, config)

integral = pysgpp.quadrature(cachedFunc, sg)
callsOfQuadrature = calls
points = [pysgpp.DataVector([0.1 * k - 0.3, 0.25, -0.5 + 0.2 * k, 0.7]) for k in range(5)]
values = [pysgpp.interpolate(cachedFunc, p, sg) for p in points]

stats = cachedFunc.statistics()
print(f"quadrature: {integral:.15g} (exact {(math.sqrt(math.pi) * math.erf(1)) ** 4:.15g})")
print(f"function calls: {callsOfQuadrature} by the quadrature, {calls - callsOfQuadrature} by "
      f"{len(points)} interpolations")
print(f"cache: {stats.lookups} lookups, {stats.hits} hits, {stats.entries} points stored "
      f"({stats.bytes / 1024:.0f} KiB), {stats.operatorCalls} operator calls")

## Same results as without the cache.
plainFunc = pysgpp.SourceFunc(lambda p: math.exp(-sum(x * x for x in p)))
assert abs(integral - pysgpp.quadrature(plainFunc, sg)) <= 1e-13
for p, value in zip(points, values):
  assert abs(value - pysgpp.interpolate(plainFunc, p, sg)) <= 1e-13
assert calls == callsOfQuadrature
