# WP-62 — state of the art (question 1)

Sources were read on 2026-11-17 (web search + page / PDF text). **[V]** = verified in the source
text in this session, **[R]** = recalled, not verified here. Notation: ℓ multi-index, Δ_ℓ = ⊗_k
(Q_{ℓ_k} − Q_{ℓ_k − 1}) hierarchical increment, O old set, A active set, I = O ∪ A.

## 1. Gerstner–Griebel (GG), Computing 71 (2003) [V]
Source: <https://ins.uni-bonn.de/media/public/publication-media/dimadapt.pdf> (Fig. 1, §3, §4).
- Algorithm: O = ∅, A = {1}. Loop: take i ∈ A with the largest indicator g_i, move it to O,
  η −= g_i; for every forward neighbour j = i + e_k that is admissible (j − e_q ∈ **O** for all
  q): A ∪= {j}, compute s = Δ_j f, r += s, η += g_j. Stop when η = Σ_{i∈A} g_i < TOL "or the work
  count exceeds a given maximal amount". The result r is the sum over **O ∪ A** (everything
  evaluated is used).
- Indicator: g_k = |Δ_k f| in the basic form. Generalized (work weighting, against premature
  stops): `g_k = max{ w·|Δ_k f| / |Δ_1 f| , (1 − w)·n_1 / n_k }`, n_k = Π n_{k_j} = number of
  function evaluations of the index (nested rules), w ∈ [0, 1]; w = 1 greedy, w = 0 "classical
  sparse grids ... where only the involved work is counted", values in between "safeguard
  against both comparatively too high work and comparatively too small error".
- Stated weakness: an index with small Δ may hide large forward neighbours ("this just puts the
  problem off" if one looks one neighbour ahead); a saturation assumption is needed; η
  "typically underestimates the error", TOL is met "up to a constant ... (up to 10)".
- Data structures: relative addressing of indices, a heap for A, per-dimension neighbour links
  for the admissibility test (no hashing); overhead negligible against the evaluations.
- Rules: nested (Patterson in the experiments; Clenshaw–Curtis mentioned); "if the univariate
  quadrature formulas are nested, then Δ_k f requires the same number of function evaluations as
  Q_k f". Tested up to d = 256 (CMO problem).

## 2. Nobile–Tamellini–Tempone, quasi-optimal ("knapsack") sparse grids [V]
Source: MATHICSE report 12.2014 (Numer. Math. 2016),
<https://infoscience.epfl.ch/record/263221/files/12-2014NEW_FN-LT-RT.pdf>.
- Profit P(i) = ΔE(i) / ΔW(i); the grid takes the indices with the largest profits (knapsack).
- Work contribution: **nested** ΔW(i) = Π_n (m(i_n) − m(i_n − 1)) = the new points (exact,
  independent of the set); **non-nested** ΔW(i) = Π_n m(i_n) = the whole tensor grid
  ("reasonable if one builds the (non nested) sparse grid incrementally ... one does not exploit
  the fact that many tensor grids in the final formula are multiplied by zero coefficients").
- Error contribution: a-priori bound with product structure over the dimensions, constants /
  rates g_n "numerically tuned" (hybrid a-priori / a-posteriori).
- A monotone profit sequence gives a downward-closed set; otherwise the missing indices are
  added.
- The a-posteriori algorithm (GG as implemented by Klimke, "KL", with ω̃ = 0.9) is counted "in
  terms of the total number of points, i.e. including also those necessary to explore the set
  of hierarchical surpluses"; the a-priori sets are "competitive" with it.

## 3. Sparse Grids Matlab Kit, `adapt_sparse_grid` [V: paper §3.1 + the Python port's source]
Sources: <https://arxiv.org/html/2203.09314v3>,
<https://uncertaintyhub.github.io/sgpykit-doc/_modules/sgpykit/main/adapt_sparse_grid.html>.
- GG loop on the reduced margin; the chosen index is the candidate with the highest profit; the
  profits of its admissible neighbours are computed at once (they enter the grid immediately),
  never recomputed. Result = all evaluated indices. The state can be passed back to resume.
- Profits (`controls.prof`): `'Linf'` = max over the **new points** of |f − previous sparse-grid
  interpolant|, `'Linf/new_points'` (default), `'weighted Linf'` (× pdf), `'weighted
  Linf/new_points'`, `'deltaint'` = |ΔQ|, `'deltaint/new_points'`. Work = new points (nested) or
  the whole tensor grid Π m(i_n) (non-nested; there the difference of the new and the old sparse
  grid is evaluated on the new points, the approximation is not interpolatory).
- Vector-valued: `controls.op_vect`, default the Euclidean norm over the outputs, then the max
  over the points.
- **Dimension buffering** (`controls.var_buffer_size`): "The algorithm starts exploring N_curr =
  <var_buffer_size> dimensions. As soon as points are placed in one dimension, a new dimension is
  added": there are always `var_buffer_size` explored but non-activated variables. The next
  variable is always N_curr + 1 (input order); the paper assumes variables "sorted decreasingly
  according to their importance". Adding a variable appends the midpoint coordinate to all
  stored points, so the function on the subset must equal the full function with the other
  variables at the midpoint ("Use this option with care!"). Earlier works used N_buf = 1 only.
- Stopping: `max_pts` (checked per iteration, may overshoot), `prof_tol` (max profit).
- Evaluations: nested → only new points (recycling); parallelism only over the points of one
  candidate; batching all candidates was rejected because it breaks the recycling.
- Stated cost problem: "when N ≫ 1, the size of the reduced margin grows very quickly, which in
  turn implies a quick growth of the number of evaluations of f" — the motivation of buffering;
  the a-posteriori method is "in some sense, a sub-optimal procedure, since computational work is
  invested in assessing the profit of multi-indices".

## 4. Tasmanian [V: API docs; the estimator itself is not documented there]
Source: <https://ornl.github.io/TASMANIAN/stable/classTasGrid_1_1TasmanianSparseGrid.html>.
- `setAnisotropicRefinement(type, min_growth, output)`: "Computes the anisotropic coefficients
  based on the current set of loaded points, then adds more points to the grid by selecting the
  points with largest coefficients"; at least `min_growth` points per step (a **batch**).
  `estimateAnisotropicCoefficients` returns integer weights ξ (and η for the "curved" types).
  Global grids need one output, sequence grids accept −1 = all outputs (largest coefficient).
  [R] the weights come from a least-squares fit of the decay of the hierarchical / orthogonal
  coefficients (Stoyanov–Webster, arXiv:1508.01125); the new set is the weighted total-degree
  set one level further.
- `setSurplusRefinement(tol, output)` (sequence rules only): keeps refining where the relative
  surplus is above the tolerance; documented as "sensitive to the non-monotonic coefficient
  decay and the choice of the tolerance".
- Dynamic construction (`getCandidateConstructionPoints`): a list of candidate points "weighted
  by descending importance"; "enough points should be used from the top of the list to ensure
  occupancy across computing resources"; points may be loaded in any order — a design for
  parallel / asynchronous evaluation.

## 5. Sensitivity-driven adaptivity (Farcaş, Görler, Bungartz, Jenko, Neckel, JCP 2020) [V]
Source: <https://ar5iv.labs.arxiv.org/html/1812.00080>.
- Standard indicator there: ‖Δ_ℓ‖_{L2} / cost. Their change: ‖Δ_ℓ‖² is split (Parseval, in an
  orthogonal polynomial basis) into Sobol'-type parts per direction and one interaction part;
  the score of ℓ counts how many parts exceed user tolerances τ_1 … τ_{d+1}; the index with the
  highest score is refined; stop when all scores are 0. Reported savings 3–13x in 3–12 d
  against the standard indicator. Needs d + 1 tolerances and a spectral basis (Leja points).

## 6. Others
- Hegland, "Adaptive sparse grids", ANZIAM J. 44 (2003) [V: abstract only]: adaptive sparse
  grids as lattices of function spaces; greedy choice of the next space; identifies the ANOVA
  structure. [R] Hegland–Garcke–Challis 2007 (thesis reference): generalized combination
  coefficients for arbitrary downward-closed sets — the formula `computeCTCoeffs` implements.
- Chkifa–Cohen–Schwab, Found. Comput. Math. 14 (2014) [V: abstract]: Lagrange interpolation on
  downward-closed sets with nested (Leja) sequences, one new point per index; greedy selection
  among the neighbours. [R] its robust variants evaluate the neighbour set and add the largest
  interpolation error, plus occasionally the "oldest" neighbour to avoid starvation.
- sparseSpACE (Obersteiner–Bungartz) [V: abstracts]: spatially adaptive CT ("Split-Extend",
  and dimension-wise refinement with rectilinear grids). It changes the grids themselves, not
  only the index set: out of scope for a dimension-adaptive CT on the existing tensor grids.
  [R] it also contains a standard dimension-adaptive CT with the GG scheme.
- Jakeman–Roberts, arXiv:1110.0010 [V: search extract]: same O / A scheme, stop when the sum of
  the active indicators is below the tolerance.

## 7. What fits a CT over full tensor grids
| Topic | Finding | Consequence for combigrid |
|---|---|---|
| Unit of refinement | all methods refine one multi-index = one hierarchical increment Δ_ℓ | same unit as a CT tensor grid; the CT value is Σ_{ℓ∈I} Δ_ℓ = Σ c_ℓ Q_ℓ |
| Indicator, quadrature | \|Δ_ℓ f\| (GG), /work (NTT, SGMK `deltaint/new_points`) | needs Δ_ℓ per MI: from stored per-TG results (any node family) or from difference weights on stored values (nested) |
| Indicator, interpolation | max over the new points of \|f − previous approximation\| (SGMK `Linf`) | nested: equals the hierarchical surplus on the new points; non-nested: no interpolation property → evaluate old and new approximation on the new TG's points |
| Work | nested: new points; non-nested: the whole TG (NTT eq. 5 / 6, SGMK) | a per-family work model; cheb1 and linear growth are "whole TG" |
| Vector values | a norm over the outputs (SGMK: Euclidean; Tasmanian: largest coefficient) | a user-replaceable norm; outputs should be scaled (relative per output) |
| High d | margin grows like d per accepted index; SGMK: buffering with an **assumed ordering** | buffering is cheap to implement but its ordering assumption is a correctness risk; alternatives are measured in this WP |
| Robustness | GG: work-weighted indicator w; Tasmanian: fitted weights (batches); CCS: starvation guard | a safeguard is needed, premature stops are documented by all sources |
| Parallelism | SGMK: only inside one candidate; Tasmanian: candidate batches | batches of MIs per step are compatible with the O / A scheme (admissibility against O) |
| Boundary offset | not a topic in the literature (CC with 1, 3, 5, 9 … points is standard) | the offset changes n(ℓ_k) only; the work model must take the counts from the node table |
