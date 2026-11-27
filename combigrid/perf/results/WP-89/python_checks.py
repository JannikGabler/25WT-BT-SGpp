# Python checks of the SparseGridSetup decoupling (run with the bindings on PYTHONPATH):
# getSetup returns an owned copy, instructions derive from SparseGridSetup, grids do not need
# their instruction (or its LvlMIVec) after construction, getGenInstr is gone.
import gc

import pysgpp

g = pysgpp.CompleteSGGenInstr(2, 3)
g.setDomain((-1.0, 2.0))
g.setBoundaryLevelOffset(1)
g.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
sg = pysgpp.SparseGrid(g)
s = sg.getSetup()
assert isinstance(s, pysgpp.SparseGridSetup) and s.nDim() == 3 and s.getBoundaryLevelOffset() == 1
assert s.getDomainForDim(2) == (-1.0, 2.0) and s == g
s.setBoundaryLevelOffset(0)  # a copy: the grid keeps its setup
assert sg.getSetup().getBoundaryLevelOffset() == 1
assert isinstance(g, pysgpp.SparseGridSetup)
del g
gc.collect()
f = pysgpp.SourceFunc(lambda x: sum(x))
print("quadrature", pysgpp.quadrature(f, sg), "getSetup OK")
mis = pysgpp.LvlMIVec([[2, 0], [0, 1]])
gi = pysgpp.MIVecSGGenInstr(mis)
dsg = pysgpp.DeltasSparseGrid(gi)
del gi, mis
gc.collect()
print("DSG after the instruction is gone:", dsg.nTG(), pysgpp.quadrature(f, dsg))
assert not hasattr(sg, "getGenInstr")
