import gc, math
import pysgpp as p

g = p.CompleteSGGenInstr(3, 2)
g.setNodeGenFunc(p.getClenshawCurtisNodeGenFunc())
g.setLvl2GPCntFunc(p.doublingLvl2GPCntFunction)
g.setBoundaryLevelOffset(1)
g.setDomain(p.HyperCubeArea([(-1.0, 3.0), (0.5, 0.75)]))
for Grid in (p.SparseGrid, p.DeltasSparseGrid):
    sg = Grid(g)
    tg = sg.getTensorGrid(0)
    levels = sg.getLevels()
    del sg; gc.collect()          # copies and keep-alive must survive the grid proxy
    assert levels.nMI() > 0 and tg.tensorGrid.nGP() > 0
    sg = Grid(g)
    n = sg.nTG(); assert n == len(sg) == len(sg.getTensorGrids()) == sg.getLevels().nMI()
    for i, t in enumerate(sg):
        assert t.mi == sg.getLevels()[i] if hasattr(sg.getLevels(), '__getitem__') else True
        found = sg.getTensorGrid(t.mi)
        assert found is not None and found.coefficient == t.coefficient
        nodes = t.tensorGrid.getNodesPerDim()
        off = 0
        for d in range(2):
            lvl = t.mi[d]
            expect = sg.getNodes(d, lvl)
            cnt = t.tensorGrid.getGPCntPerDim()[d]
            assert cnt == sg.nNodes(d, lvl) == len(expect)
            assert [nodes[off + k] for k in range(cnt)] == [expect[k] for k in range(cnt)]
            lo, hi = [(-1.0, 3.0), (0.5, 0.75)][d]
            assert all(lo - 1e-15 <= expect[k] <= hi + 1e-15 for k in range(cnt))  # on the domain
            off += cnt
    assert sg.getTensorGrid(p.LvlMI([7, 7])) is None
    assert sg.maxLvl(0) == 3
    assert sg == Grid(g)
    f = p.SourceFunc(lambda x: math.exp(x[0] + x[1]))
    print(Grid.__name__, n, p.quadrature(f, sg), p.interpolate(f, p.DataVector([0.3, 0.6]), sg),
          p.interpolateLinear(f, p.DataVector([0.3, 0.6]), sg))
print("python checks OK")
