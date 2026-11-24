"""WP-48: prints the bit patterns of every operator's result (scalar + vector, SG + DSG).

Run with OMP_NUM_THREADS=1 and OMP_NUM_THREADS=16 and compare the outputs: they must be equal.
Does not iterate a DataVector and uses no negative indices (abort in OPT=0 builds).
"""
import math
import struct

import pysgpp


def func(point):
  s = 0.0
  p = 1.0
  for i in range(len(point)):
    s += (0.7 + 0.3 * i) * point[i]
    p *= math.cos(1.3 * point[i] + 0.2 * i)
  return math.exp(s) - 3.0 * p


def funcVec(point):
  base = func(point)
  n = len(point)
  return [base, math.sin(3.0 * point[0]) * base, 1e3 * point[n - 1] * point[n - 1] - base]


def bits(x):
  return struct.unpack("<Q", struct.pack("<d", x))[0]


def show(name, value):
  if isinstance(value, float):
    print(f"{name}: {bits(value):016x}")
  else:
    print(f"{name}: " + " ".join(f"{bits(value[k]):016x}" for k in range(len(value))))


f = pysgpp.SourceFunc(func)
fVec = pysgpp.VectorSourceFunc(funcVec, 3)

for nodes, name in ((pysgpp.getClenshawCurtisNodeGenFunc(), "cc"),
                    (pysgpp.getEquidistantNodeGenFunc(), "equi")):
  for boundary in (0, 1):
    genInstr = pysgpp.CompleteSGGenInstr(5, 4)
    genInstr.setDomain((-0.3, 1.2))
    genInstr.setNodeGenFunc(nodes)
    genInstr.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    genInstr.setBoundaryLevelOffset(boundary)
    sg = pysgpp.SparseGrid(genInstr)
    dsg = pysgpp.DeltasSparseGrid(genInstr)
    point = pysgpp.DataVector((0.137, 1.05, -0.2, 0.61))
    prefix = f"{name} b{boundary}"
    for gridName, grid in (("sg", sg), ("dsg", dsg)):
      show(f"{prefix} {gridName} quadrature", pysgpp.quadrature(f, grid))
      show(f"{prefix} {gridName} quadrature vec", pysgpp.quadrature(fVec, grid))
      show(f"{prefix} {gridName} interpolate", pysgpp.interpolate(f, point, grid))
      show(f"{prefix} {gridName} interpolate vec", pysgpp.interpolate(fVec, point, grid))
      show(f"{prefix} {gridName} interpolateLinear", pysgpp.interpolateLinear(f, point, grid))
      show(f"{prefix} {gridName} interpolateLinear vec",
           pysgpp.interpolateLinear(fVec, point, grid))
