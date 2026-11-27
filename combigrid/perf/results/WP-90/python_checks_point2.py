# Python checks of WP-90 point 2 (run with the bindings on PYTHONPATH): NodeGenFunc was renamed to
# NodeFamily, the getters to get*Nodes(); the setup methods to set/getNodeFamily(ForDim),
# set/getNodeFamilies, getUniqueNodeFamilies; the old names are gone.
import pysgpp

old = ["NodeGenFunc", "NodeGenFuncVector", "getEquidistantNodeGenFunc", "getClenshawCurtisNodeGenFunc",
       "getFirstTypeChebyshevNodeGenFunc", "getSecondTypeChebyshevNodeGenFunc"]
for name in old:
    assert not hasattr(pysgpp, name), name
for name in ["setNodeGenFunc", "setNodeGenFuncForDim", "setNodeGenFuncs", "getNodeGenFuncs",
             "getNodeGenFuncForDim", "getUniqueNodeGenFuncs"]:
    assert not hasattr(pysgpp.SparseGridSetup, name), name

equi = pysgpp.getEquidistantNodes()
cc = pysgpp.getClenshawCurtisNodes()
cheb2 = pysgpp.getSecondTypeChebyshevNodes()
cheb1 = pysgpp.getFirstTypeChebyshevNodes()
for f in (equi, cc, cheb1):
    assert isinstance(f, pysgpp.NodeFamily)
assert cc == cheb2 and cc.id() == cheb2.id()  # the same singleton
assert not (cc == equi) and cc.id() != cheb1.id()

g = pysgpp.CompleteSGGenInstr(3, 3)
g.setNodeFamily(cc)
g.setNodeFamilyForDim(cheb1, 1)
assert g.getNodeFamilyForDim(1) == cheb1 and g.getNodeFamilyForDim(0) == cc
assert len(g.getNodeFamilies()) == 3 and len(g.getUniqueNodeFamilies()) == 2
g.setNodeFamilies(pysgpp.NodeFamilyVector([equi, equi, cc]))
assert g.getNodeFamilyForDim(2) == cc and g.getNodeFamilyForDim(0) == equi
g.setNodeFamilies([cc, cc, cc])
sg = pysgpp.SparseGrid(g)
assert sg.getSetup().getNodeFamilyForDim(1) == cc
nodes = cc.genNodesWithBoundary(5)  # list(DataVector) is unsafe (no __iter__, WP-37)
assert nodes.getSize() == 5 and nodes[0] == 0.0 and nodes[4] == 1.0
print("all point-2 Python checks passed")
