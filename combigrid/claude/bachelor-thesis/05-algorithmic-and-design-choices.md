---
source: bachelor thesis/thesis/chapters/algorithmic_and_design_choices.tex (+ chapters/sections/algorithmic_and_design_choices/{domain_handling,node_generation,tensor_grid_representation,interpolation,quadrature,sparse_grid_generation,boundary_management}.tex)
chapter: 5
title: Algorithmic and Design Choices
---

# 5 Algorithmic and Design Choices {#sec:algorithmic_and_design_choices}

This chapter discusses the central algorithmic and design choices underlying the new SG++ combination technique (CT) module.
After the previous chapter introduced the requirements, general approach and overall functionality of the module, the focus now shifts to the concrete design decisions that determine its mathematical consistency, performance, and capabilities.
In particular, we examine how the module handles arbitrary domains, generates nodes and sparse grids, performs interpolation and quadrature, and represents tensor grids.
These design choices are closely linked: some simplify the internal architecture by introducing abstraction layers, while others are motivated by numerical stability, memory efficiency, and runtime behavior.
The following sections therefore explain not only which solution was chosen, but also why it was considered appropriate with respect to the previously stated requirements in Section 4.1 (Requirements and Approach) in file [04-new-ct-module-for-sgpp.md](04-new-ct-module-for-sgpp.md).

## 5.1 Domain Handling {#sec:algorithmic_and_design_choices:domain_handling}

The new SG++ module supports arbitrary hyperrectangles

$$
[a_1, b_1] \times [a_2, b_2] \times \ldots \times [a_d, b_d], \quad a_i, b_i \in \mathbb{R}
$$

as the domain $\Omega$ for sparse grids.
However, the $d$-dimensional unit domain $[0, 1]^d$ is used internally.
All internal algorithms, nodes and grids utilize this unit domain.
The only exceptions are the interfaces that are used directly by client code, such as the source function.
All internal points in $[0, 1]^d$, however, are automatically transformed to $\Omega$ by the module.
Using the unit domain has two main advantages.

Firstly, mathematical concepts are commonly natively defined over it.
This makes it easier to implement mathematical concepts directly, reducing the risk of errors.
Some operators, such as the quadrature, may still require corrections to the result in the post-processing.

Secondly, it enables the abstraction of the user-specified domain $\Omega$ for all internal components.
For example, this means that nodes and quadrature weights are independent of $\Omega$.

Another important consideration is the overhead caused by the transformation

$$
T: [0,1]^d \rightarrow \Omega, \; x \mapsto \left( \begin{array}{c} (b_1 - a_1) \cdot \vec{x}[1] + a_1 \\ \vdots \\ (b_d - a_d) \cdot \vec{x}[d] + a_d \end{array} \right).
$$

One disadvantage of our two-domain approach is the need for a transformation with every actual (non-cached) source function evaluation.
While it is important to keep in mind that function evaluations are performed very frequently, it is also worth mentioning that the overhead is small.
One multiplication and addition per dimension are required, and the process is completely branch-free.
Ultimately, optimal domain handling with regard to the performance comes down to the operations performed.
While there is no significant impact on quadrature, interpolation would not require any additional handling in a one-domain approach.

## 5.2 Node Generation {#sec:algorithmic_and_design_choices:node_generation}

Node generation is performed by the operation `genNodes` of the abstract class `NodeGenFunc`.
Unlike the other frameworks, `genNodes` operates directly on the number of nodes required.
Levels are converted to node counts in advance, fully abstracting the generation process from the concept of levels.

The same applies to the boundary treatment.
The module fully manages the entire boundary logic, including decisions on where and when to add boundaries, and their inclusion itself.
Therefore, values for the number of required nodes always refer to inner nodes, and user implementations of node generation functions should never generate nodes on the boundary.

As a third abstraction, it is also independent of the domain $\Omega$.
Following on from Section 5.1 (Domain Handling) above, nodes should always be generated on the one-dimensional, internal unit domain $[0, 1]$.
The transformation to user-specified domains is also carried out by the module.

In conclusion, the generation process is fully abstracted from levels, the domain and the boundary treatment.
This allows for an easier implementation of the node generation, increased modularity through separation and more flexibility (e.g., custom level conversions).

An illustration of the node generation process can be seen in Listing 5.1 below.
It shows a simplified version of the generation code for the pre-defined equidistant nodes (Equation (2.22) in file [02-sparse-grids.md](02-sparse-grids.md)).

<!-- label: lst:algorithmic_and_design_choices:node_generation:example_equidistant_node_generation -->
**Listing 5.1.** Code to generate equidistant nodes.

```cpp
class EquidistantNodeGenFunc : public NodeGenFunc {

  /* Abstract inplace method of NodeGenFunc that has to be overwritten.
     nNodes:   Number of nodes to generate.
     out:      Outgoing vector of nodes.
     startIdx: Index where the resulting nodes should begin in out. */
  void genNodesWithoutBoundary(GPCntType nNodes, DataVector& out, size_t startIdx) {
    double factor = 1.0 / double(nNodes + 1);

    for (GPCntType i = 1; i <= nNodes; i++) {
      out[startIdx++] = double(i) * factor;
    }
  }

};
```

## 5.3 Tensor Grid Representation {#sec:algorithmic_and_design_choices:tensor_grid_representation}

The most apparent representation of a tensor grid is a simple $d$-dimensional array that stores all grid points alongside some meta-information, such as the node types.
This choice seems very straightforward as it aligns with the conception of tensor grids, all nodes are required eventually anyway, and they are iterable efficiently due to the linear memory representation.
This approach is fully used by the Sparse Grids Matlab Kit (SGMK) and partially used by sparseSpACE [Piazzola & Tamellini 2024, @sparse_grids_matlab_kit; Obersteiner, @sparseSpACE].
The latter one uses this representation only temporarily while evaluating mathematical operators on tensor grids.

We have also started the development of the new SG++ module utilizing this representation but later in the process realized its major downsides.
The most significant is the memory consumption.
When storing all tensor grids in full, the memory usage asymptotically scales as $\mathcal{O}(d^2 \cdot n \cdot (\log n)^{d-1})$ (derived from Table 2.2 in file [02-sparse-grids.md](02-sparse-grids.md)).
This is a matter of great concern, as subsequent comparisons will show.

Another major downside we have discovered through testing is the negative impact on the runtime performance.
While it is true that the $d$-dimensional array can be iterated efficiently, it is important to consider the time required to generate nodes.
Generally, node generation — and consequently, grid point generation — is one of the fastest steps in the entire workflow.
In most cases, it is not worthwhile to pre-compute all grid points, since the memory lookups take longer than assembling each node on demand.

We took some measurements during the development process, both before and after switching to an on-demand grid point assembly system.
Figure 5.1 below shows a selection of these.

The measurements clearly show that full assembly causes a much stronger increase in peak memory consumption than on-demand assembly.
For small maximum levels, both approaches are still relatively close, but as $l_{\max}$ grows, the fully assembled representation quickly becomes impractical, especially in higher dimensions.
This effect becomes more pronounced with increasing dimensionality: While the three-dimensional case remains manageable for longer, measurements for five and, in particular, seven dimensions exhibit a much earlier and steeper rise.
In contrast, on-demand assembly keeps the memory footprint substantially lower across all tested dimensions and avoids the rapid growth of the fully pre-computed representation.
The same trend was also visible in the runtime measurements: Averaged over the considered dimensions, on-demand assembly reduced the runtime by roughly one order of magnitude for sufficiently large values of $l_{\max}$.

<!-- label: fig:algorithmic_and_design_choices:tensor_grid_representation:comparison_pre_computations_vs_on_demand_assembly -->
**Figure 5.1.** Comparison of peak memory usage of the full assembly versus the hybrid on-demand assembly variant (note that these measurements originate from a prototype version).
*Image: `figures/tensor grid representation memory result.png`*

There are various ways to implement on-demand assembly.
In the following, we will discuss three approaches that we considered for the design.
The first approach involves temporarily assembling the tensor grid in full each time its grid points are required.
Tensor grids only permanently store the information required for assembly.
This approach is used by sparseSpACE.

As an alternative to assembling complete tensor grids, single grid points could be assembled instead.
This approach takes the on-demand assembly even further, requiring only a minimal amount of memory.
Similar to the previous approach, tensor grids only permanently store the information required for assembly.

We have settled for a hybrid approach between the original representation and the fine-grained assembly of single grid points.
The new combigrid module pre-computes and permanently stores the one-dimensional node distribution for each tensor grid.
In contrast to the exponential memory growth associated with the full tensor grid representation, this approach only requires a linear amount of memory proportional to the number of dimensions $d$ and the number of grid points per dimension $n$.
This ensures fewer memory lookups and an improved cache usage.
The single grid points are then assembled using the stored nodes on demand.
The main advantages over the complete assembly of single grid points are the potential for vectorization when generating multiple successive nodes at once, and the reduced number of required function calls.
Function calls for the node generation are potentially problematic in our module because they are virtual (i.e., vtable overhead, less compiler optimization).

The choice for this hybrid approach is made on theoretical considerations, experience and small scale benchmarks.
However, this might not be the optimal approach.
Future work may extend these considerations and conduct further comparisons and tests.

## 5.4 Global Interpolation {#sec:algorithmic_and_design_choices:global_interpolation}

One mathematical operator the module offers is the direct evaluation of the global polynomial interpolant of a source function $f$ over a sparse grid at a specific point $\vec{y}$ in the domain $\Omega$.
This evaluation is performed dimension-wise using one-dimensional interpolations over the different dimensions on each tensor grid [Trefethen 2009, @trefethen2009approximation; Zimmer, Piller & Uekermann 2024, @numerische_grundlagen_lecture; Schulte 2025, @gwr_lecture].
This can be illustrated by the following rearrangement

$$
\begin{split}
&P\left( f \,|\, x_{\vec{i}_1}, \ldots, x_{\vec{i}_m} \right)(\vec{y}) \\
&=\sum_{i_1 = 1, \ldots, i_d = 1}^{n_1, \ldots, n_d} f\left(\vec{x}_{(i_1, \ldots, i_d)}\right) \cdot \ell_{(i_1, \ldots, i_d)}(\vec{y}) \\
&=\sum_{i_1 = 1, \ldots, i_d = 1}^{n_1, \ldots, n_d} f\left(\vec{x}_{(i_1, \ldots, i_d)}\right) \cdot \ell_{i_1}^{(1)}(\vec{y}[1]) \cdot \ell_{i_d}^{(d)}(\vec{y}[d]) \\
&= \underbrace{\sum_{i_d=1}^{n_d} \ell_{i_d}^{(d)}(\vec{y}[d]) \cdot \left( \,\ldots\, \underbrace{\left( \sum_{i_2 = 1}^{n_2} \ell^{(2)}_{i_2}( \vec{y}[2] ) \cdot  \underbrace{\left( \sum_{i_1 = 1}^{n_1} \ell^{(1)}_{i_1}( \vec{y}[1] ) \cdot f\left( \vec{x}_{(i_1, \ldots, i_d)} \right) \right)}_{\text{1D interpolation in dimension 1}}  \right)}_{\text{1D interpolation in dimension 2}} \,\ldots\, \right)}_{\text{1D interpolation in dimension } d},
\end{split}
$$

where

- $d$ is the number of dimensions.
- $n_k$ is the number of grid points in dimension $k$.
- $m$ is the total number of grid points (i.e., $m = n_1 \cdot \ldots \cdot n_d$).
- $x^{(k)}_i$ is the $i$-th node in the dimension $k$.
- $\vec{x}_{\vec{i}}$ are the grid points of the tensor grid.
- $P\left(f \,|\, \vec{x}_{\vec{i}_1}, \ldots, \vec{x}_{\vec{i}_m} \right)$ is the polynomial interpolant of $f$.
- $\ell_{\vec{j}}$ is the $d$-dimensional Lagrange polynomial for the grid point $\vec{x}_{\vec{j}}$ over all grid points $\vec{x}_{\vec{i}}$.
- $\ell^{(k)}_{j}$ is the one-dimensional Lagrange polynomial for the node $x_j$ over all nodes $x^{(k)}_1, \ldots, x^{(k)}_{n_k}$ used in dimension $k$.

Figure 5.2 below illustrates the dimension-wise interpolation concept for three dimensions.

<!-- label: fig:algorithmic_and_design_choices:global_interpolation:visualization_dimensionwise_interpolation -->
**Figure 5.2.** Illustration of the multidimensional interpolation.
*Image: `figures/grids/tensor grids/dimension-wise interpolation.pdf`*

The one-dimensional interpolations are implemented based on the Barycentric formula of the second kind [Trefethen 2009, @trefethen2009approximation]

<!-- label: eq:algorithmic_and_design_choices:global_interpolation:barycentric_formula_second_kind -->
$$
P(f \,|\, x_1, \ldots, x_n)(x) = \sum^n_{i=1} \frac{\lambda_i \cdot f(x_i)}{x - x_i} \;\bigg/\; \sum^n_{i=1} \frac{\lambda_i}{x - x_i} \tag{5.4}
$$

with the special case $P(f \,|\, x_1, \ldots, x_n)(x_i) = f(x_i)$ and the weights

<!-- label: eq:algorithmic_and_design_choices:global_interpolation:barycentric_formula_second_kind_weights -->
$$
\lambda_i = \frac{1}{\prod_{j \neq i} (x_i - x_j)} \tag{5.5}
$$

for all $i \in \{ 1, \ldots, n \}$.
It is numerically stable [Trefethen 2009, @trefethen2009approximation; Higham 2004, @higham2004numerical] and only requires $\mathcal{O}(n)$ operations if the weights $\lambda_i$ are known.
Although computing the weights is more expensive with $\mathcal{O}(n^2)$ operations, they are easily pre-computable.
The pre-computation is performed in log space to minimize the risk of overflow or underflow.
For Chebyshev nodes of the second kind, there is also a trivial closed form for the weights, which we utilize.
Further details can be found in [Trefethen 2009, @trefethen2009approximation].

## 5.5 Quadrature {#sec:algorithmic_and_design_choices:quadrature}

The quadrature is achieved by applying standard quadrature rules on each tensor grid and combining their contributions according to the combination technique.
For each tensor grid, the module approximates the integral of a source function $f: \mathbb{R}^d \rightarrow \mathbb{R}$ by applying the standard quadrature rule

$$
\int_{\Omega} f(\vec{x})\, d\vec{x} \approx \sum_{\vec{i}} w_{\vec{i}} \cdot f(\vec{x}_{\vec{i}}),
$$

where $\vec{x}_{\vec{i}}$ are the grid points and $w_{\vec{i}}$ are the corresponding quadrature weights.

Following the domain handling described in Section 5.1 (Domain Handling) above, the quadrature is performed on the internal unit domain $[0,1]^d$.
The user-specified hyperrectangle $\Omega = [a_1,b_1] \times \ldots \times [a_d,b_d]$ is only used for the function evaluation.
Hence, the quadrature is first evaluated on the internal domain and then transformed to $\Omega$ by the affine mapping

$$
T: \mathbb{R}^d \rightarrow \mathbb{R}^d, \; \vec{u} \mapsto \left( \begin{array}{c} (b_1 - a_1) \cdot u_1 + a_1 \\ \vdots \\ (b_d - a_d) \cdot u_d + a_d  \end{array} \right).
$$

This yields

$$
\int_{\Omega} f(\vec{x})\, d\vec{x}
= |\Omega| \int_{[0,1]^d} f(T(\vec{u}))\, d\vec{u}
\approx |\Omega| \sum_{\vec{i}} w_{\vec{i}} \cdot f\!\bigl(T(\vec{x}_{\vec{i}})\bigr),
$$

with the volume $|\Omega| = \prod_{k=1}^{d} (b_k-a_k)$.
Thus, the only domain-dependent correction is the final multiplication by $|\Omega|$.
The quadrature weights themselves remain independent of $\Omega$.

Similarly to how tensor grids are Cartesian products of one-dimensional node distributions, the $d$-dimensional weights are also computed as the Cartesian product

$$
w_{\vec{i}} = \prod_{k=1}^{d} w^{(k)}_{\vec{i}[k]}
$$

of the one-dimensional weights $w^{(k)}_{i}$.
More precisely, $w^{(k)}_{i}$ denotes the one-dimensional weight of the $i$-th node in dimension $k$.
Each dimension has its own respective weights based on the chosen node type.
Consequently, the quadrature rule is assembled dimension-wise and remains fully compatible with the tensor-product structure of the grid.

For *equidistant nodes*, the one-dimensional weights depend on the number of nodes $n_k$ in the respective dimension $k$.
If $n_k$ is odd, the summated Simpson rule is used.
Let

$$
h_k = \frac{1}{n_k-1}.
$$

Then, for the equidistant nodes, the weights are given by

<!-- label: eq:algorithmic_and_design_choices:quadrature:simpson_quadrature_rule_weights -->
$$
w^{(k)}_i =
\begin{cases}
  \frac{1}{3} h_k, & i \in \{ 1, n_k \} \\[0.4em]
  \frac{4}{3} h_k, & i \notin \{ 1, n_k \} \land i \text{ even} \\[0.4em]
  \frac{2}{3} h_k, & i \notin \{ 1, n_k \} \land i \text{ odd}
\end{cases}. \tag{5.11}
$$

This corresponds to the well-known composite Simpson rule

<!-- label: eq:algorithmic_and_design_choices:quadrature:simpson_quadrature_rule -->
$$
\int_0^1 g(x)\,dx \approx \frac{h}{3}
\left(
  g(x_1)
  + 4 \sum_{\substack{i=2 \\ i \,\mathrm{even}}}^{n_k-1} g(x_i)
+ 2 \sum_{\substack{i=3 \\ i \,\mathrm{odd}}}^{n_k-2} g(x_i)
+ g(x_{n_k})
\right) \tag{5.12}
$$

with an asymptotic error decay in $\mathcal{O}(h_k^4)$ [Trefethen 2009, @trefethen2009approximation].

If the number of nodes is even, Simpson's rule is not applicable and the summated trapezoidal rule is used instead.
In this case, the weights are

<!-- label: eq:algorithmic_and_design_choices:quadrature:trapezoidal_quadrature_rule_weights -->
$$
w^{(k)}_i =
\begin{cases}
  \frac{1}{2} h_k, & i \in \{1, n_k \} \\[0.4em]
  h_k, & \text{otherwise}
\end{cases}, \tag{5.13}
$$

which yields

<!-- label: eq:algorithmic_and_design_choices:quadrature:trapezoidal_quadrature_rule -->
$$
\int_0^1 g(x)\,dx \approx \frac{h}{2}
\left(
  g(x_1) + 2 \sum_{j=2}^{n_k-1} g(x_j) + g(x_{n_k})
\right). \tag{5.14}
$$

The summated trapezoidal rule has an asymptotic error decay in $\mathcal{O}(h_k^2)$ [Trefethen 2009, @trefethen2009approximation].

Both formulas can be computed incredibly efficiently due to their simple structure.
They are implemented directly as stated and selected based on the parity of the number of nodes in the respective dimension.
The Simpson rule is preferred to the trapezoidal rule because it achieves a superior asymptotic error decay compared to the trapezoidal rule at a minimal increase in cost.

For *Chebyshev nodes* of the second kind, the module uses the corresponding Clenshaw-Curtis quadrature weights.
On the reference interval $[-1,1]$, the nodes are given by

$$
x_i = \cos\!\left(\frac{i \cdot \pi}{n - 1}\right), \qquad i = 0,\ldots, n - 1,
$$

where $n$ denotes their number.
The quadrature rule is the interpolatory rule associated with these nodes, i.e.,

<!-- label: eq:algorithmic_and_design_choices:quadrature:clenshaw_curtis_quadrature_rule -->
$$
\int_{-1}^{1} g(x) \, dx \approx \sum_{i=0}^{n} \omega_i^{\mathrm{CC}}\, g(x_i),
\qquad
\omega_i^{\mathrm{CC}} = \int_{-1}^{1} \ell_i(x) \, dx, \tag{5.16}
$$

where $\ell_j$ is the Lagrange polynomial of node $x_i$.

For the explicit construction of the weights, we use the Clenshaw-Curtis formula in the form derived by Waldvogel [Waldvogel 2006, @waldvogel2006fast].
With $\theta_i = (i\pi)/(n-1)$, the weights can be written as

<!-- label: eq:algorithmic_and_design_choices:quadrature:clenshaw_curtis_quadrature_rule_weights -->
$$
\omega_i^{\mathrm{CC}}
=
\frac{c_i}{n-1}
\left(
  1 - \sum_{l=1}^{\lfloor (n-1)/2 \rfloor}
  \frac{b_l}{4l^2-1}\cos(2l\theta_i)
\right), \tag{5.17}
$$

where

$$
c_i =
\begin{cases}
  1, & i \in \{ 0, \ldots, n - 1 \}\\
  2, & \text{otherwise}
\end{cases},
\qquad
b_l =
\begin{cases}
  1, & l = (n - 1) / 2,\\
  2, & \text{otherwise}
\end{cases}.
$$

This representation shows that the weight computation reduces to a cosine expansion with explicitly known coefficients.
Waldvogel's main contribution is to evaluate the resulting coefficient vector by an inverse discrete Fourier transform.
In practice, the coefficients are assembled once, extended by symmetry and endpoint corrections, and then transformed by an FFT-based routine.
This yields all one-dimensional weights simultaneously and reduces the asymptotic complexity from $\mathcal{O}(n^2)$ to $\mathcal{O}(n \log n)$.
We use this FFT-based construction in the implementation.

Since the module internally works on the unit interval $[0,1]$, the corresponding quadrature weights for $[-1, 1]$ are scaled accordingly by the interval transformation.
As a result, the one-dimensional Clenshaw-Curtis weights on $[0,1]$ are obtained directly from the weights on $[-1,1]$.

## 5.6 Sparse Grid Generation {#sec:algorithmic_and_design_choices:sparse_grid_generation}

The `SparseGrid` class constructor is responsible for the generation of sparse grids based on sparse grid generation instructions (blueprints).
This consists mainly of the generation of the tensor grid multi-indices and the combination technique coefficients.

As introduced in Section 4.5.5 (Sparse Grid Generation Instructions) in file [04-new-ct-module-for-sgpp.md](04-new-ct-module-for-sgpp.md), there are currently two types of instruction supported.
The first of them is the `CompleteSGGenInstr`.
It generates complete sparse grids directly based on the closed form Equation (2.37) in file [02-sparse-grids.md](02-sparse-grids.md).
This is very efficient because firstly, that equation directly states the tensor grids with a CT coefficient of 0 that can be omitted, and secondly, the CT coefficient of the non-omitted ones can be computed using a simple binomial coefficient.

The more difficult case is represented by `MIVecSGGenInstr`.
It generates sparse grids based on arbitrary vectors $V$ of multi-indices.
The processing starts by computing the minimal downwards-closed set of multi-indices $I$ that contains all multi-indices in the given vector.
The first step is to compute the Pareto maxima $P$ and the component-wise maxima of the multi-indices in $V$.
Using the component-wise maxima, a hyperrectangular bounding box $BB$ is drawn and all contained multi-indices are checked against the Pareto maxima $P$.
A multi-index $\vec{i} \in BB$ is considered to be a part of the resulting multi-index set $I$ if and only if it is $\le$ than at least one Pareto maximum.
This concept is illustrated in Figure 5.3 below.

<!-- label: fig:algorithmic_and_design_choices:sparse_grid_generation:visualization_downwards-closed_mis_generation -->
**Figure 5.3.** Pareto maxima in the two-dimensional tensor grid tableau and their induced downwards-closed multi-index set.
*Image: `figures/tensor grid tableaus/visualization downwards-closed mi set generation.pdf`*

After computing the multi-index set $I$, we can start with the CT coefficients.
There are commonly two approaches used for this.
The first one directly utilizes the inclusion-exclusion principle (Equation (2.35) in file [02-sparse-grids.md](02-sparse-grids.md)).
This has two significant downsides.
For $|P|$ Pareto maxima, a total of $2^{|P|} - 1$ summands (all combinations of $|P|$ elements except for $\emptyset$) must be considered.
Parallelizing this approach also requires decent coordination, because either a common state for the coefficients must be used, or each thread would require their own copy.

The other approach is based on the existence of neighbors.
It is implemented as shown in Listing 5.2 below [Piazzola & Tamellini 2024, @sparse_grids_matlab_kit].
The neighbor-existence approach determines the combination technique coefficient of a single multi-index $\vec{i} \in \mathbb{N}^d$ by inspecting its local neighborhood in the sparse grid index set $I \subseteq \mathbb{N}^d$.
For every binary offset vector in $\{0,1\}^d$, the algorithm checks whether the shifted multi-index $\vec{i} + \textit{offset}$ is contained in $I$.
Geometrically, this corresponds to testing all vertices of the unit hypercube that is anchored at $\vec{i}$ and extends in the positive coordinate directions.
Each valid neighbor contributes to the coefficient, and the contribution is weighted by the Hamming weight of the offset, i.e., by the number of coordinates in which the offset is equal to one.
In this way, the coefficient of $\vec{i}$ is computed solely from local membership information, without requiring a global inclusion-exclusion over all Pareto maxima.
As a consequence, the coefficients of different multi-indices can be computed independently, which makes the method particularly well suited for parallel execution.
Its main drawback is that all $2^d$ possible offsets must be checked for every multi-index, which leads to a considerable runtime in higher dimensions.
It is used by the SGMK when arbitrary multi-index sets are provided.

<!-- label: lst:algorithmic_and_design_choices:sparse_grid_generation:ct_coeff_neighbor_existance_algorithm -->
**Listing 5.2.** Algorithm for the combination technique coefficients based on neighbor existence [Piazzola & Tamellini 2024, @sparse_grids_matlab_kit].

```python
# Computes the CT coeff of mi in the SG specified through miSet.
# mi:    Multi-index
# miSet: Downwards-closed set of tensor grid multi-indices for a sparse grid
# d:     Number of dimensions
def CT-Coeff(mi, miSet, d):
  coeff = 0

  for offset in {0, 1}^d:
    if mi + offset in miSet:
      coeff += ||offset||_1

  return coeff
```

**Proof of correctness.**
Let $N_l^{(k)}$ be the set of nodes on level $l$ in dimension $k$ interpreted as multisets.
They are empty if the level $l$ is invalid, i.e., if $l < 1$.
We assume that the nodes $N_l^{(k)}$ are nested (i.e., $N_{l}^{(k)} \subseteq N_{l}^{(k)}$ holds true for all $l$ — this is how the source states the nesting assumption).

The tensor grid $T_{\vec{i}}$ corresponding to the multi-index $\vec{i} \in \mathbb{N}^d$ can be written as

$$
T_{\vec{i}} = N_{\vec{i}[1]}^{(1)} \times N_{\vec{i}[2]}^{(2)} \times \ldots \times N_{\vec{i}[d]}^{(d)},
$$

analogously to the initial definition Equation (2.25) in file [02-sparse-grids.md](02-sparse-grids.md).

With the assumption of nested nodes, we can now define the tensor grid differences $\Delta T_{\vec{i}}$ as

$$
\Delta T_{\vec{i}} := \left(N_{\vec{i}[1]}^{(1)} - N_{\vec{i}[1] - 1}^{(1)} \right) \times \ldots \times \left( N_{\vec{i}[d]}^{(d)} - N_{\vec{i}[d] - 1}^{(d)} \right).
$$

For readers familiar with the hierarchical representation of sparse grids, these are the non-nested tensor grids used for the construction of sparse grids.

Using the distributivity between the Cartesian product $\times$ and the subtraction $-$ of multisets, we can derive

<!-- label: eq:algorithmic_and_design_choices:sparse_grid_generation:ct_coeff_neighbor_existance_algorithm:proof:eq1 -->
$$
\begin{aligned}
\Delta T_{\vec{i}} &= \left(N_{\vec{i}[1]}^{(1)} - N_{\vec{i}[1] - 1}^{(1)} \right) \times \ldots \times \left( N_{\vec{i}[d]}^{(d)} - N_{\vec{i}[d] - 1}^{(d)} \right) \\
&= \left( N_{\vec{i}[1]}^{(1)} \times N_{\vec{i}[2]}^{(2)} \ldots \times N_{\vec{i}[d]}^{(d)} \right) \\
&\quad -\left( N_{\vec{i}[1]-1}^{(1)} \times N_{\vec{i}[2]}^{(2)} \ldots \times N_{\vec{i}[d]}^{(d)} \right) - \left( N_{\vec{i}[1]}^{(1)} \times N_{\vec{i}[2]-1}^{(2)} \ldots \times N_{\vec{i}[d]}^{(d)} \right) - \ldots \\
&\quad +\left( N_{\vec{i}[1]-1}^{(1)} \times N_{\vec{i}[2]-1}^{(2)} \ldots \times N_{\vec{i}[d]}^{(d)} \right) + \ldots \\
&\quad -\ldots \\
&= \sum_{\vec{o} \in \{ 0, 1 \}^d} (-1)^{\text{Number of ones in } \vec{o}} \cdot \left( N_{\vec{i}[1] - \vec{o}[1]}^{(1)} \times N_{\vec{i}[2] - \vec{o}[2]}^{(2)} \times \ldots \times N_{\vec{i}[d] - \vec{o}[d]}^{(d)} \right)
\end{aligned}
\tag{5.21}
$$

We can now describe the grid points of the sparse grid specified through the downwards-closed multi-index set $I \subseteq \mathbb{N}^d$ as (we again assume that the nodes are nested)

$$
\text{SG\_grid\_points}(I) = \sum_{\vec{i} \in I} \Delta T_{\vec{i}}
$$

and inserting Equation (5.21) above yields

<!-- label: eq:algorithmic_and_design_choices:sparse_grid_generation:ct_coeff_neighbor_existance_algorithm:proof:eq2 -->
$$
\begin{aligned}
\text{SG\_grid\_points}(I) &= \sum_{\vec{i} \in I} \left( \sum_{\vec{o} \in \{ 0, 1 \}^d} (-1)^{|| \vec{o} ||_1} \cdot \underbrace{\left( N_{\vec{i}[1] - \vec{o}(1)}^{(1)} \times \ldots \times N_{\vec{i}[d] - \vec{o}[d]}^{(d)} \right)}_{\text{Is empty if } \vec{i} - \vec{o} \notin I} \right) \\
&= \sum_{\vec{i} \in I} \left( \underbrace{\left( \sum_{\vec{o} \in \{ 0, 1 \}^d} (-1)^{|| \vec{o} ||_1} \cdot \mathds{1}_{\vec{i} + \vec{o} \in I} \right)}_{\text{Factors for the same tensor grid}} \cdot \left( N_{\vec{i}[1]}^{(1)} \times \ldots N_{\vec{i}[d]}^{(d)} \right) \right)
\end{aligned}
\tag{5.23}
$$

where $\mathds{1}_{expr}$ is $1$ if the expression $expr$ is true and $0$ otherwise.
The last equality holds because the Cartesian product is empty if there exists a dimension $k \in \{ 1, \ldots, d \}$ such that $\vec{i}[k] - \vec{o}[k] < 1$.

Equation (5.23) above proves that

<!-- label: eq:algorithmic_and_design_choices:sparse_grid_generation:ct_coeff_neighbor_existance_algorithm:proof:eq3 -->
$$
\sum_{\vec{o} \in \{ 0, 1 \}^d} (-1)^{|| \vec{o} ||_1} \cdot \mathds{1}_{\vec{i} + \vec{o} \in I} \tag{5.24}
$$

are the actual combination technique coefficients for $\vec{i}$. $\blacksquare$

Based on the neighbor existence approach, we managed to separate the dimension checks and developed the following iterative approach to compute the CT coefficients.
The algorithm can be seen in Listing 5.3 below.

The dimension-separated approach is a reformulation of the neighbor-existence method in which the $d$ coordinate directions are processed independently.
Instead of iterating over all $2^d$ binary offset vectors for each multi-index, the algorithm performs one sweep per dimension.
Initially, all coefficients are set to $1$.
For every multi-index $\vec{i} \in I$, it considers the direct successor of $\vec{i}$ in the current dimension, i.e., the multi-index obtained by increasing only the corresponding component by one.
If this successor is still contained in $I$, the coefficient of $\vec{i}$ is updated by subtracting the coefficient of the successor from the current value.
Otherwise, the coefficient remains unchanged.
This procedure is repeated for all dimensions, so that the final coefficient is obtained by successively propagating the influence of neighbors along each coordinate axis.

The updates can be implemented with two coefficient data structures that are swapped after each dimension.
Since the updates for different multi-indices within the same dimension are independent, the inner loop can be parallelized easily.
As a result, the algorithm reduces the computational effort to $\mathcal{O}(|I| \cdot d) \times C$, where $C$ denotes the cost of testing whether a multi-index is contained in the multi-index set $I$.

<!-- label: lst:algorithmic_and_design_choices:sparse_grid_generation:dimension-seperated_ct_coeff_algorithm -->
**Listing 5.3.** Dimension-separated algorithm for the combination technique coefficients. Referred to in this thesis as the "dimension-separated algorithm for the combination technique coefficients" (see also Section 8.1, Conclusion, in file [08-conclusion-and-outlook.md](08-conclusion-and-outlook.md)).

```python
# Computes the CT coeffs for all multi-indices in the SG specified through miSet.
# miSet: Downwards-closed set of tensor grid multi-indices for a sparse grid
# d:     Number of dimensions
def CT-Coeffs(miSet, d):
  # Initialize all coefficients to 1
  coeffs = {(mi, 1) for mi in miSet}   # Map
  coeffs_swap = {}   # Map

  for dim in {1, ..., d}:
    for mi in miSet:
      successor = mi   # Direct successor in dimension dim
      successor[dim] += 1

      if successor in miSet:
        coeffs_swap[mi] = coeffs[mi] - coeffs[successor]
      else:
        coeffs_swap[mi] = coeffs[mi]

    coeffs = coeffs_swap

  return coeffs
```

**Proof of correctness.**
Let $\text{Coeff}(\vec{i}, k)$ be the computed coefficient for the multi-index $\vec{i}$ after processing the dimension $k$.
The following applies

<!-- label: lst:algorithmic_and_design_choices:sparse_grid_generation:dimension-seperated_ct_coeff_algorithm:proof:recursive -->
$$
\begin{aligned}
&\text{Coeff}(\vec{i}, 0) = \mathds{1}_{\vec{i} \in I} \\
&\text{Coeff}(\vec{i}, k) = \text{Coeff}(\vec{i}, k-1) - \mathds{1}_{\vec{i} + \vec{e}_k \in I} \cdot \text{Coeff}(\vec{i} + \vec{e}_k, k-1), \qquad k = 1, \ldots, d
\end{aligned}
\tag{5.25}
$$

for all multi-indices $\vec{i} \in I$ ($I$ is the downwards-closed set of multi-indices for the sparse grid).
$\vec{e}_k$ is the $k$-th $d$-dimensional unit basis vector.

Furthermore, let

$$
M_k := \left\{ \vec{i} \in \{ 0, 1 \}^d \, | \, \forall j > k: \, \vec{i}[j] = 0 \right\}
$$

for every $k \in \{ 0, \ldots, d \}$.

We will prove through mathematical induction that $\text{Coeff}(\vec{i}, k)$ can be expressed in closed form as

<!-- label: lst:algorithmic_and_design_choices:sparse_grid_generation:dimension-seperated_ct_coeff_algorithm:proof:closed_form -->
$$
\text{Coeff}(\vec{i}, k) = \sum_{\vec{o} \in M_k} (-1)^{|| \vec{o} ||_1} \cdot \mathds{1}_{\vec{i} + \vec{o} \in I} \tag{5.27}
$$

over $k \in \{ 0, \ldots, d \}$ for every $\vec{i} \in I$.

*Induction start.*
Let $\vec{i} \in I$ be arbitrary and $k = 0$.
Equation (5.27) above obviously holds true:

$$
\sum_{\vec{o} \in M_0} (-1)^{|| \vec{o} ||_1} \cdot \mathds{1}_{\vec{i} + \vec{o} \in I} = (-1)^{||\vec{0}||_1} \cdot \mathds{1}_{\vec{i} + \vec{0} \in I} = \mathds{1}_{\vec{i} \in I} \overset{(5.25)}{=} \text{Coeff}(\vec{i}, 0)
$$

*Induction assumption.*
We assume that there exists an arbitrary but fixed $k_0$ such that Equation (5.27) above holds true for all $k \le k_0$.
Based on the downwards-closedness (dw-c) of $I$,

*Induction step.*

$$
\begin{aligned}
\text{Coeff}(\vec{i}, k_0 + 1) &= \text{Coeff}(\vec{i}, k_0) - \mathds{1}_{\vec{i} + \vec{e}_{k_0+1} \in I} \cdot \text{Coeff}(\vec{i} + \vec{e}_{k_0+1}, k_0) \quad \text{by (5.25)} \\
&= \sum_{\vec{o} \in M_{k_0}} (-1)^{|| \vec{o} ||_1} \cdot \mathds{1}_{\vec{i} + \vec{o} \in I} - \mathds{1}_{\vec{i} + \vec{e}_{k_0+1} \in I} \cdot \sum_{\vec{o} \in M_{k_0}} (-1)^{||\vec{o}||_1} \cdot \mathds{1}_{\vec{i} + \vec{e}_{k_0 + 1} + \vec{o} \in I} \quad \text{(IA)} \\
&= \sum_{\vec{o} \in M_{k_0}} (-1)^{|| \vec{o} ||_1} \cdot \mathds{1}_{\vec{i} + \vec{o} \in I} - \sum_{\vec{o} \in M_{k_0}} (-1)^{||\vec{o}||_1} \cdot \mathds{1}_{\vec{i} + \vec{e}_{k_0 + 1} + \vec{o} \in I} \quad \text{(dw-c)} \\
&=  \sum_{\vec{o} \in M_{k_0}} (-1)^{|| \vec{o} ||_1} \cdot \mathds{1}_{\vec{i} + \vec{o} \in I} + \sum_{\vec{o} \in M_{k_0}} (-1)^{||\vec{o}||_1 + 1} \cdot \mathds{1}_{\vec{i} + \vec{e}_{k_0 + 1} + \vec{o} \in I} \\
&= \sum_{\vec{o} \in M_{k_0+1}} (-1)^{|| \vec{o} ||_1} \cdot \mathds{1}_{\vec{i} + \vec{o} \in I}
\end{aligned}
$$

follows.

Equation (5.27) above proves the correctness of the algorithm, because $\text{Coeff}(\vec{i}, d)$ equals the expression from Equation (5.24) above. $\blacksquare$

## 5.7 Boundary Management {#sec:algorithmic_and_design_choices:boundary_management}

The boundary is automatically managed by the SG++ module.
This includes the decision-making of where and when to add nodes on the boundary based on the user's specification.
For a given tensor grid specified by the multi-index $\vec{i}$, the module adds nodes — and therefore grid points — on the boundary in dimension $k$ if and only if

$$
\vec{i}[k] \ge \text{boundaryLevelOffset},
$$

where $\text{boundaryLevelOffset} \in \mathbb{N}$ is the user-specified value as introduced in Section 4.5.5 (Sparse Grid Generation Instructions) in file [04-new-ct-module-for-sgpp.md](04-new-ct-module-for-sgpp.md).

Figure 5.4 below illustrates this concept based on a complete sparse grid construction.
The tensor grid tableau contains the adjusted tensor grids and the resulting sparse grid for a boundary level offset of $1$.

This concept was developed based on a similar one in the SGMK [SGMK GitHub, @sgmk_github; Piazzola & Tamellini, @sgmk_manual] which always offsets the boundary by one level.

<!-- label: fig:algorithmic_and_design_choices:boundary_management -->
**Figure 5.4.** Illustration of the tensor grid tableau for the complete sparse grid with a max. level of 3 and a boundary level offset of 1.
*Image: `figures/visualization boundary mangement.pdf`* (note: filename contains the original typo "mangement" instead of "management", preserved verbatim from the source repository)

## References cited in this chapter

- **[Piazzola & Tamellini 2024, @sparse_grids_matlab_kit]** C. Piazzola, L. Tamellini. "Algorithm 1040: The Sparse Grids Matlab Kit — a Matlab implementation of sparse grids for high-dimensional function approximation and uncertainty quantification." *ACM Transactions on Mathematical Software*, 50(1), 2024. DOI: 10.1145/3630023.
- **[Obersteiner, @sparseSpACE]** Dr. rer. nat. Michael Obersteiner. "Sparse Grid spatially adaptive combination environment." <https://github.com/obersteiner/sparseSpACE>. Accessed 2026-04-18.
- **[Trefethen 2009, @trefethen2009approximation]** Lloyd N. Trefethen. "Approximation theory and approximation practice." In *Book of abstracts*, p. 21, 2009.
- **[Zimmer, Piller & Uekermann 2024, @numerische_grundlagen_lecture]** Stefan Zimmer, Lukas Piller, Benjamin Uekermann. "Numerische Grundlagen." Lecture slides, University of Stuttgart, 2024.
- **[Schulte 2025, @gwr_lecture]** Miriam Schulte. "Grundlagen Wissenschaftliches Rechnen." Lecture slides, University of Stuttgart, 2025.
- **[Higham 2004, @higham2004numerical]** Nicholas J. Higham. "The numerical stability of barycentric Lagrange interpolation." *IMA Journal of Numerical Analysis*, 24(4):547–556, Oxford University Press, 2004.
- **[Waldvogel 2006, @waldvogel2006fast]** Jörg Waldvogel. "Fast construction of the Fejér and Clenshaw–Curtis quadrature rules." *BIT Numerical Mathematics*, 46(1):195–202, Springer, 2006.
- **[SGMK GitHub, @sgmk_github]** C. Piazzola, L. Tamellini. "Sparse Grids Matlab Kit GitHub." <https://github.com/lorenzo-tamellini/sparse-grids-matlab-kit>. Accessed 2026-04-18.
- **[Piazzola & Tamellini, @sgmk_manual]** C. Piazzola, L. Tamellini. "Sparse Grids Matlab Kit Manual." 2023. <https://drive.google.com/file/d/1e0yeJ7IGUq3G014JD0q3-GUQLCjyA44n/view>. Accessed 2026-04-18.
