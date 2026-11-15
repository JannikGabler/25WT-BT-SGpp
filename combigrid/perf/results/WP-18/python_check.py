import gc, math
import pysgpp as p

g = p.CompleteSGGenInstr(3, 2)
g.setNodeGenFunc(p.getClenshawCurtisNodeGenFunc())
g.setLvl2GPCntFunc(p.doublingLvl2GPCntFunction)
g.setBoundaryLevelOffset(1)
g.setDomain(p.HyperCubeArea([(-1.0, 3.0), (0.5, 0.75)]))
for Grid in (p.SparseGrid, p.DeltasSparseGrid):
    sg = Grid(g)
    tg = sg.getDomainTensorGrid(0)
    levels = sg.getLevels()
    del sg; gc.collect()          # copies and keep-alive must survive the grid proxy
    assert levels.nMI() > 0 and tg.tensorGrid.nGP() > 0
    sg = Grid(g)
    n = sg.nTG(); assert n == len(sg) == len(sg.getDomainTensorGrids()) == sg.getLevels().nMI()
    assert len(sg.getUnitTensorGrids()) == n
    try:                          # no default iteration / indexing: the coordinates are explicit
        iter(sg); assert False
    except TypeError:
        pass
    for i, t in enumerate(sg.getDomainTensorGrids()):
        u = sg.getUnitTensorGrid(i)
        assert u.mi == t.mi and u.coefficient == t.coefficient
        assert u.tensorGrid.getGPCntPerDim() == t.tensorGrid.getGPCntPerDim()
        uNodes = u.tensorGrid.getNodesPerDim()
        uByMI = sg.getUnitTensorGrid(t.mi).tensorGrid.getNodesPerDim()
        assert [uByMI[k] for k in range(len(uByMI))] == [uNodes[k] for k in range(len(uNodes))]
        assert t.mi == sg.getLevels()[i] if hasattr(sg.getLevels(), '__getitem__') else True
        found = sg.getDomainTensorGrid(t.mi)
        assert found is not None and found.coefficient == t.coefficient
        nodes = t.tensorGrid.getNodesPerDim()
        off = 0
        for d in range(2):
            lvl = t.mi[d]
            expect = sg.getDomainNodes(d, lvl)
            cnt = t.tensorGrid.getGPCntPerDim()[d]
            assert cnt == sg.nNodes(d, lvl) == len(expect)
            assert [nodes[off + k] for k in range(cnt)] == [expect[k] for k in range(cnt)]
            lo, hi = [(-1.0, 3.0), (0.5, 0.75)][d]
            assert all(lo - 1e-15 <= expect[k] <= hi + 1e-15 for k in range(cnt))  # on the domain
            unit = sg.getUnitNodes(d, lvl)  # the same nodes on [0, 1]; domain = fma(u, w, a)
            assert len(unit) == cnt and all(0.0 <= unit[k] <= 1.0 for k in range(cnt))
            assert all(expect[k] == math.fma(unit[k], hi - lo, lo) for k in range(cnt))
            assert [uNodes[off + k] for k in range(cnt)] == [unit[k] for k in range(cnt)]
            off += cnt
    assert sg.getDomainTensorGrid(p.LvlMI([7, 7])) is None
    assert sg.getUnitTensorGrid(p.LvlMI([7, 7])) is None
    assert sg.maxLvl(0) == 3
    assert sg == Grid(g)
    f = p.SourceFunc(lambda x: math.exp(x[0] + x[1]))
    print(Grid.__name__, n, p.quadrature(f, sg), p.interpolate(f, p.DataVector([0.3, 0.6]), sg),
          p.interpolateLinear(f, p.DataVector([0.3, 0.6]), sg))
print("python checks OK")
