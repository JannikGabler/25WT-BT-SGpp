# Python checks of WP-90 point 1 (run with the bindings on PYTHONPATH): SGGenInstr::clone is gone,
# the generation instructions and SparseGridSetup are plain (not shared_ptr) proxies, getSetup
# returns an owned copy, grids do not need their instruction (or its LvlMIVec) after construction.
import gc

import pysgpp

for cls in (pysgpp.SGGenInstr, pysgpp.CompleteSGGenInstr, pysgpp.MIVecSGGenInstr):
    assert not hasattr(cls, "clone") and not hasattr(cls, "cloneShared"), cls

g = pysgpp.CompleteSGGenInstr(2, 3)
g.setDomain((-1.0, 2.0))
g.setBoundaryLevelOffset(1)
g.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
g.setLvl2GPCntFuncs([pysgpp.doublingLvl2GPCntFunction] * 3)
assert isinstance(g, pysgpp.SGGenInstr) and isinstance(g, pysgpp.SparseGridSetup)
assert "shared_ptr" not in repr(g.this), repr(g.this)  # plain proxy
sg = pysgpp.SparseGrid(g)
s = sg.getSetup()
assert "shared_ptr" not in repr(s.this), repr(s.this)
assert isinstance(s, pysgpp.SparseGridSetup) and s.nDim() == 3 and s.getBoundaryLevelOffset() == 1
assert s.getDomainForDim(2) == (-1.0, 2.0) and s == g
assert s.getVolumeOfDomain() == 27.0
s.setBoundaryLevelOffset(0)  # a copy: the grid keeps its setup
assert sg.getSetup().getBoundaryLevelOffset() == 1
del s
gc.collect()
del g
gc.collect()
f = pysgpp.SourceFunc(lambda x: sum(x))
print("quadrature", pysgpp.quadrature(f, sg), "getSetup OK")

mis = pysgpp.LvlMIVec([[2, 0], [0, 1]])
gi = pysgpp.MIVecSGGenInstr(mis)
assert gi.genFullMIVec().nMI() == 4  # closure: (0,0), (1,0), (2,0), (0,1)
dsg = pysgpp.DeltasSparseGrid(gi)
sg2 = pysgpp.SparseGrid(gi)
del gi, mis
gc.collect()
print("DSG / SG after the instruction is gone:", dsg.nTG(), pysgpp.quadrature(f, dsg),
      sg2.nTG(), pysgpp.quadrature(f, sg2))
assert not hasattr(sg, "getGenInstr")

# A setup built directly (base class) still works with its setters
st = pysgpp.SparseGridSetup(2)
st.setDomain((0.0, 2.0))
assert st.getVolumeOfDomain() == 4.0
print("all point-1 Python checks passed")
