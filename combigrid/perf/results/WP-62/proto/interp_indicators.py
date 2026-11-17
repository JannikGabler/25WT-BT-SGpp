#!/usr/bin/env python3
"""WP-62: indicators for dimension-adaptive *interpolation* with the combination technique.

Stand-alone NumPy model (Clenshaw-Curtis nodes on [0, 1], doubling growth, global polynomial
interpolation per tensor grid, Gerstner-Griebel selection). Compared indicators of a candidate
multi-index l (Delta_l = hierarchical increment of the interpolant):
  deltaint   |integral of Delta_l f|                       (the quadrature indicator)
  linf       max over the NEW grid points of |Delta_l f|   (hierarchical surplus, SGMK 'Linf')
  val        max over fixed random indicator points of |Delta_l f|
each plain and divided by the number of new points ("/w"). Error: max and RMS of
|f - interpolant| at 1000 fixed-seed validation points (another seed than the indicator points).

Usage: .venv/bin/python interp_indicators.py   -> prints a table, writes raw/interp.jsonl (stored gzipped)
"""
import heapq
import itertools
import json
import os

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))


def cc_nodes(level, offset):
    """Nodes of a level: 2^(level+1) - 1 inner nodes, boundary nodes from level `offset` on."""
    n_inner = 2 ** (level + 1) - 1
    j = np.arange(0, n_inner + 2)
    x = 0.5 * (1.0 - np.cos(np.pi * j / (n_inner + 1)))
    return x if level >= offset else x[1:-1]


def cc_weights(x):
    """Interpolatory quadrature weights on [0, 1] for the nodes x (small systems only)."""
    n = len(x)
    if n == 1:
        return np.array([1.0])
    t = 2.0 * x - 1.0
    v = np.polynomial.chebyshev.chebvander(t, n - 1).T
    k = np.arange(n)
    moments = np.where(k % 2 == 0, 2.0 / (1.0 - k.astype(float) ** 2 + (k == 1)), 0.0)
    return 0.5 * np.linalg.solve(v, moments)


def basis_matrix(x, points):
    """Lagrange basis values of the nodes x at the points (barycentric formula)."""
    n = len(x)
    if n == 1:
        return np.ones((len(points), 1))
    w = np.array([1.0 / np.prod(x[j] - np.delete(x, j)) for j in range(n)])
    w /= np.max(np.abs(w))
    diff = points[:, None] - x[None, :]
    exact = np.abs(diff) < 1e-14
    diff[exact] = 1.0
    terms = w[None, :] / diff
    basis = terms / terms.sum(axis=1, keepdims=True)
    rows = exact.any(axis=1)
    basis[rows] = exact[rows].astype(float)
    return basis


class Model:
    def __init__(self, func, d, offset, n_indicator=200, n_validation=1000):
        self.func, self.d, self.offset = func, d, offset
        self.x_ind = np.random.default_rng(1).random((n_indicator, d))
        self.x_val = np.random.default_rng(2).random((n_validation, d))
        self.f_val = func(self.x_val)
        self.nodes, self.weights, self.new_idx = {}, {}, {}
        self.values, self.quad, self.at_ind, self.at_val = {}, {}, {}, {}

    def level(self, lvl):
        if lvl not in self.nodes:
            x = cc_nodes(lvl, self.offset)
            self.nodes[lvl] = x
            self.weights[lvl] = cc_weights(x)
            if lvl == 0:
                self.new_idx[lvl] = np.arange(len(x))
            else:
                prev = self.level(lvl - 1)
                old = np.abs(x[:, None] - prev[None, :]).min(axis=1) < 1e-13
                self.new_idx[lvl] = np.nonzero(~old)[0]
        return self.nodes[lvl]

    def work(self, mi):
        for lvl in mi:
            self.level(lvl)
        return int(np.prod([len(self.new_idx[lvl]) for lvl in mi]))

    def interpolate(self, mi, points):
        """Interpolant of the tensor grid mi at scattered points."""
        a = self.values[mi]
        first = True
        for k, lvl in enumerate(mi):
            basis = basis_matrix(self.nodes[lvl], points[:, k])
            if first:
                a = np.tensordot(basis, a, axes=(1, 0))
                first = False
            else:
                a = np.einsum("pi,pi...->p...", basis, a)
        return a

    def evaluate(self, mi):
        """Function values, quadrature and interpolants at the indicator / validation points."""
        grids = np.meshgrid(*[self.level(lvl) for lvl in mi], indexing="ij")
        pts = np.stack([g.ravel() for g in grids], axis=1)
        self.values[mi] = self.func(pts).reshape([len(self.nodes[lvl]) for lvl in mi])
        q = self.values[mi]
        for lvl in mi:
            q = np.tensordot(self.weights[lvl], q, axes=(0, 0))
        self.quad[mi] = float(q)
        self.at_ind[mi] = self.interpolate(mi, self.x_ind)
        self.at_val[mi] = self.interpolate(mi, self.x_val)

    def backward_box(self, mi):
        dims = [k for k in range(self.d) if mi[k] > 0]
        for mask in itertools.product([0, 1], repeat=len(dims)):
            sub = list(mi)
            for k, bit in zip(dims, mask):
                sub[k] -= bit
            yield (-1) ** sum(mask), tuple(sub)

    def indicators(self, mi):
        """Increments of the candidate: (integral, at the validation points, indicators)."""
        new_grids = np.meshgrid(*[self.nodes[lvl][self.new_idx[lvl]] for lvl in mi], indexing="ij")
        new_pts = np.stack([g.ravel() for g in new_grids], axis=1)
        d_int, d_ind, d_val, d_new = 0.0, 0.0, 0.0, self.func(new_pts)
        for sign, sub in self.backward_box(mi):
            d_int += sign * self.quad[sub]
            d_ind = d_ind + sign * self.at_ind[sub]
            d_val = d_val + sign * self.at_val[sub]
            if sub != mi:
                d_new = d_new + sign * self.interpolate(sub, new_pts)
        return d_val, {"deltaint": abs(d_int), "linf": float(np.max(np.abs(d_new))),
                       "val": float(np.max(np.abs(d_ind)))}


def run(func, d, offset, indicator, budget, max_tg=60000):
    """Gerstner-Griebel loop; returns the trace [(points, max error, rms error)]."""
    name, per_work = indicator.split("/")[0], indicator.endswith("/w")
    model = Model(func, d, offset)
    old, known, heap, trace = set(), set(), [], []
    interp = np.zeros(len(model.x_val))
    points = 0

    def add(mi):
        nonlocal interp, points
        w = model.work(mi)
        if int(np.prod([len(model.nodes[lvl]) for lvl in mi])) > max_tg or points + w > budget:
            return False
        model.evaluate(mi)
        d_val, ind = model.indicators(mi)
        interp = interp + d_val
        points += w
        known.add(mi)
        heapq.heappush(heap, (-(ind[name] / w if per_work else ind[name]), mi))
        err = np.abs(model.f_val - interp)
        trace.append((points, float(err.max()), float(np.sqrt(np.mean(err ** 2)))))
        return True

    add(tuple([0] * d))
    while heap:
        _, mi = heapq.heappop(heap)
        old.add(mi)
        for k in range(d):
            fwd = mi[:k] + (mi[k] + 1,) + mi[k + 1:]
            if fwd in known:
                continue
            if all(fwd[:q] + (fwd[q] - 1,) + fwd[q + 1:] in old for q in range(d) if fwd[q] > 0):
                if not add(fwd):
                    continue
    return trace


def genz(name, d, c=4.0):
    a = c / (np.arange(1, d + 1) ** 2)
    frac = 0.6180339887498949 * np.arange(1, d + 1)
    u = 0.3 + 0.4 * (frac - np.floor(frac))
    if name == "gauss":
        return lambda x: np.exp(-np.sum((a * (x - u)) ** 2, axis=1))
    if name == "osc":
        return lambda x: np.cos(2 * np.pi * u[0] + x @ a)
    if name == "corner":
        return lambda x: 1.0 / (1.0 + x @ a) ** 2
    if name == "prodpeak":
        return lambda x: 1.0 / np.prod(1.0 + (a * (x - u)) ** 2, axis=1)
    raise ValueError(name)


if __name__ == "__main__":
    indicators = ["deltaint", "deltaint/w", "linf", "linf/w", "val", "val/w"]
    cases = [(2, 0, 3000), (4, 1, 6000), (8, 1, 6000)]
    out_path = os.path.join(HERE, "..", "raw", "interp.jsonl")
    with open(out_path, "w") as out:
        for (d, offset, budget), fname in itertools.product(cases, ["gauss", "osc", "corner", "prodpeak"]):
            print(f"\n{fname} d={d} offset={offset}: max error (rms error) at the validation points")
            marks = [budget // 30, budget // 10, budget // 3, budget]
            print("  %-12s " % "indicator" + " ".join("%20s" % f"<= {m} pts" for m in marks))
            for indicator in indicators:
                trace = run(genz(fname, d), d, offset, indicator, budget)
                cells = []
                for m in marks:
                    best = [t for t in trace if t[0] <= m]
                    cells.append("%9.1e (%7.1e)" % (best[-1][1], best[-1][2]) if best else "-")
                print("  %-12s " % indicator + " ".join("%20s" % c for c in cells))
                out.write(json.dumps({"func": fname, "d": d, "offset": offset,
                                      "indicator": indicator, "trace": trace}) + "\n")
