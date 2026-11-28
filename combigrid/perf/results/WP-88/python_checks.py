"""WP-88: checks of the renamed pysgpp names (run with the repo .venv, PYTHONPATH=lib:lib/pysgpp)."""
import pysgpp

# New names exist, old names are gone
for name in ["NodeFamily", "NodeFamilyVector", "getEquidistantNodes", "getClenshawCurtisNodes",
             "getFirstTypeChebyshevNodes", "getSecondTypeChebyshevNodes"]:
    assert hasattr(pysgpp, name), name
for name in ["NodeGenFunc", "NodeGenFuncVector", "getEquidistantNodeGenFunc",
             "getClenshawCurtisNodeGenFunc", "getFirstTypeChebyshevNodeGenFunc",
             "getSecondTypeChebyshevNodeGenFunc"]:
    assert not hasattr(pysgpp, name), name
assert not hasattr(pysgpp.NodeFamily, "id")
for name in ["setNodeFamily", "setNodeFamilyForDim", "getNodeFamilyForDim", "setNodeFamilies",
             "getNodeFamilies", "getUniqueNodeFamilies"]:
    assert hasattr(pysgpp.CompleteSGGenInstr, name), name
    assert not hasattr(pysgpp.CompleteSGGenInstr, name.replace("NodeFamilies", "NodeGenFuncs")
                       .replace("NodeFamily", "NodeGenFunc")), name

# Singleton semantics: CC and second-kind Chebyshev are the same family
cc, cheb2 = pysgpp.getClenshawCurtisNodes(), pysgpp.getSecondTypeChebyshevNodes()
assert cc == cheb2 and not (cc == pysgpp.getEquidistantNodes())
nodes = cc.genNodesWithBoundary(3)
assert [nodes[i] for i in range(nodes.getSize())] == [0.0, 0.5, 1.0]

# A small pipeline with per-dimension families
gi = pysgpp.CompleteSGGenInstr(4, 2)
gi.setDomain((0, 1))
gi.setNodeFamilyForDim(pysgpp.getClenshawCurtisNodes(), 0)
gi.setNodeFamilyForDim(pysgpp.getFirstTypeChebyshevNodes(), 1)
gi.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
fams = gi.getNodeFamilies()
assert len(fams) == 2 and fams[0] == cc and len(gi.getUniqueNodeFamilies()) == 2
sg = pysgpp.SparseGrid(gi)
val = pysgpp.quadrature(pysgpp.SourceFunc(lambda x: x[0] * x[1]), sg)
assert abs(val - 0.25) < 1e-12, val
print("WP-88 python checks OK, quadrature =", val)
