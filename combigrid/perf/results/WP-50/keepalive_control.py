# WP-50 control for python-checks-asan.txt: run with "keep" or "remove" (removes the keep-alive
# reference of the DeltasSparseGrid to its generation instruction by hand; under ASan this
# must report a heap-use-after-free, proving that the keep-alive check is meaningful).
import gc, sys, pysgpp
def build(remove):
    gen = pysgpp.MIVecSGGenInstr(pysgpp.LvlMIVec([[0, 3], [2, 1], [1, 2]]))
    gen.setNodeGenFunc(pysgpp.getClenshawCurtisNodeGenFunc())
    gen.setLvl2GPCntFunc(pysgpp.doublingLvl2GPCntFunction)
    dsg = pysgpp.DeltasSparseGrid(gen)
    if remove:
        del dsg._genInstr
    return dsg
dsg = build(sys.argv[1] == "remove")
gc.collect()
print(pysgpp.quadrature(pysgpp.SourceFunc(lambda p: sum(p)), dsg))
print(dsg.getGenInstr().genFullMIVec().nMI())
