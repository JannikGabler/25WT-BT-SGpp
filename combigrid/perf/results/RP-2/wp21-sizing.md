# RP-2 — sizing of WP-21 (DSG b1: reuse of projected sub-tries)

Cases: `quad/dsg/equi-exp/b1/d20/L6` and `.../b1/d15/L7` (complete grids |ℓ|₁ ≤ L; all
coefficients are non-zero there, so the coefficient trie holds every delta grid's level). At b ≥ 1 a
delta grid m has one node exactly in the dimensions with m_k = 0. Its projected dimensions are
therefore P = supp(m), and the projected coefficient function f_P depends on P only. Delta grids
with the same P differ only in the lower bound m_P.

Counting model (exact for complete grids; script in the session scratchpad, pure combinatorics).
The pruned DFS of `ProjectedSubTrieBuilder::addChildren` (bound `maxSuffixLvlSum`) visits at
depth k the prefixes (ℓ_0..ℓ_k) with ℓ_j ≥ m_j and Σ_{j≤k}(ℓ_j − m_j) ≤ slack = L − |m|₁, i.e.
C(slack + k + 1, k + 1) nodes. Per visited parent it also iterates over Σ_{j>k} m_j children that
the bound rejects with `continue`, because the children are sorted by level. The model
reproduces WP-02's 6.2e7 "useful" nodes at d20.

| | d20 L6 | d15 L7 |
|---|---|---|
| delta grids | 230 230 | 170 544 |
| distinct single-node-dimension sets P | 60 460 | 16 384 |
| delta grids per set | 3.8 | 10.4 |
| DFS: visited nodes now | 6.20e7 | 4.84e7 |
| DFS: children rejected by the bound (`continue`) now | 4.83e7 (44 % of the child iterations) | 3.57e7 (42 %) |
| per-set cache: one DFS per set (for m = 1_P) | 4.06e7 visited (**−34.5 %**) | 2.43e7 (**−49.8 %**) |
| + restricting the cached projected trie per delta grid | 1.8e6 nodes | 1.8e6 nodes |

Why a cache saves only 35–50 %: the set's smallest member (m = 1_P) has the largest sub-trie and
must still be built. The other members have a larger |m| and a smaller slack, so their sub-tries
were cheap already. Most sets have one or few members: at d20, 38 760 of the 60 460 sets have
|P| = 6 and a single delta grid each.

Measured time of the projection builder (16-thread profiles, `profile-splits.md`):
- d20 (1.225 s): 13.6 % (+ memset 1.0 %) ≈ 0.18 s.
- d15 (1.218 s): 9.6 % (+ 1.0 %) ≈ 0.13 s.
- Together ≈ 0.31 s.

Levers and expected saving (b1 d15 + d20):
- (a) **Slack break**: track the remaining slack (maxLvlSum − |m| − Σ excess over all dimensions
  on the path) and `break` as soon as a child's level exceeds deltaLvl + remaining slack (sorted
  children). The bound is necessary for any set and exact for complete grids. It removes the 44 % /
  42 % rejected child iterations, which are cheaper than visits: ≈ 15–30 % of the builder ≈
  0.05–0.09 s.
- (b) **Per-set cache**, or a visit order grouping the delta grids by P (one cached entry per
  thread, so no memory growth): −35 / −50 % of the visits ≈ 0.06 + 0.06 s, minus restriction and
  grouping overhead.
- (a) + (b): **≈ 0.1–0.2 s**.
- Optional extension, the serial preparation: `micro/dsg/prepare/equi/b1/d20/L6` takes 65.7 ms
  = 5.4 % of the d20 wall time. The radix pass of the `CTCoeffTrie` construction is 64 % of the
  constructor. Partly parallel radix passes and the max levels from the instruction instead of a
  tensor-grid scan give ≤ 0.03–0.05 s. Exact reservation of the trie's node `FlatNestedVec`
  removes 8.4 MB from the b1 d20 heap peak (the old buffer during the last doubling) and the
  growth copies.
