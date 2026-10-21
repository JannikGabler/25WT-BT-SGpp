---
source: bachelor thesis/thesis/chapters/sparse_grids.tex (+ chapters/sections/sparse_grids/{basics,boundary_treatment,combination_technique,hierarchical_representation}.tex)
chapter: 2
title: Sparse Grids
---

# 2 Sparse Grids {#sec:sparse_grids}

Regular approaches based on full grid suffer from the curse of dimensionality, a phenomenon typical for high-dimensional problems.
Extending a typical one-dimensional node distribution of $n$ nodes to $d$ dimensions leads to $n^d$ total grid points.
This exponential growth of grid points and therefore computational cost only allows for a few dimensions, even on modern high-end hardware [Pflüger 2010, @pflueger_diss; Garcke 2012, @sparse_grids_in_a_nutshell].

Sparse grids are able to mitigate the curse of dimensionality to some extent.
They require considerably less grid points, while keeping the asymptotic error for many operations close to the full grid variants.
For example, the asymptotic error for interpolation and quadrature is preserved up to a logarithmic factor [Pflüger 2010, @pflueger_diss; Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations; Garcke 2012, @sparse_grids_in_a_nutshell; Pflüger & Schulte 2025, @akwr_lecture].

While they have been originally developed for partial differential equations (PDEs), they are now used for many operations just as interpolation, quadrature, differentiation, or regression [Pflüger 2010, @pflueger_diss; Garcke 2012, @sparse_grids_in_a_nutshell; Balduzzi, Bonizzoni & Tamellini 2023, @uq_in_beams_using_sg; Neumann 2019, @regression_for_predictions_using_sg; Honarmandi & Arróyave 2020, @uq_for_material_science_using_sg; Brumm et al. 2021, @economic_models_using_sg].
Applications can be found in many domains ranging from uncertainty quantification and economics to modeling and simulation [Balduzzi, Bonizzoni & Tamellini 2023, @uq_in_beams_using_sg; Neumann 2019, @regression_for_predictions_using_sg; Honarmandi & Arróyave 2020, @uq_for_material_science_using_sg; Brumm et al. 2021, @economic_models_using_sg].
Currently, they are used for uncertainty quantification (UQ) in structural engineering [Balduzzi, Bonizzoni & Tamellini 2023, @uq_in_beams_using_sg; Honarmandi & Arróyave 2020, @uq_for_material_science_using_sg] and they solve high-dimensional dynamic economic models by interpolating policy/value functions [Brumm et al. 2021, @economic_models_using_sg].
They also enable fast surrogate modeling of complex simulations (for instance, predicting run-times of molecular dynamics or climate/weather models) [Neumann 2019, @regression_for_predictions_using_sg].

Sparse grids in their regular, hierarchical representation are based on a hierarchical decomposition of the underlying spaces.
Employed are they, however, often using the so-called combination technique (CT) that also forms the basis of this work.
We still briefly cover the hierarchical representation in Section 2.1.7 (Tensor Grids) and Section 2.4 (Hierarchical Representation) below, as it is the underlying concept behind the combination technique [Pflüger 2010, @pflueger_diss; Garcke 2012, @sparse_grids_in_a_nutshell].

Section 2.1 (Basics) starts by introducing the basics of sparse grids from the combination technique perspective.
This is followed by the treatment of boundaries in Section 2.2 (Boundary Treatment) and the combination technique itself in Section 2.3 (Combination Technique), covering important aspects like the inclusion-exclusion principle.
Finally, we conclude this chapter with a small note about the hierarchical representation in Section 2.4 (Hierarchical Representation).

The presented contents of this chapter are primarily drawn from [Pflüger 2010, @pflueger_diss; Pflüger & Schulte 2025, @akwr_lecture; Garcke 2012, @sparse_grids_in_a_nutshell; Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations].
However, the presentation, explanations, and figures are original.

## 2.1 Basics {#sec:sparse_grids:basics}

First, let us introduce some basic definitions and concepts used later on.
Let $\Omega \subseteq \mathbb{R}^d$ be a bounded domain.
A *source function* is a function

$$
f: \Omega \rightarrow \mathbb{R}
$$

that represents the underlying continuous quantity of interest.
Depending on the problem setting, $f$ may denote the integrand in a numerical integration problem or the target function in an interpolation problem.
In the context of sparse grids, the common goal is to approximate $f$ by its representation on the underlying discretized space.

### 2.1.1 Global Polynomial Interpolation {#sec:sparse_grids:basics:global_polynomial_interpolation}

Let $\mathcal{X} = \{ \vec{x}_{\vec{i}_1}, \ldots, \vec{x}_{\vec{i}_m} \} \subseteq \Omega \subseteq \mathbb{R}^d$ be a set of interpolation nodes.
The *global polynomial interpolant* of a source function $f$ with respect to $\mathcal{X}$ is the unique polynomial

$$
P\left(f \, | \, \vec{x}_{\vec{i}_1}, \ldots, \vec{x}_{\vec{i}_m} \right)
$$

from a suitable finite-dimensional polynomial space with $m$ degrees of freedom that satisfies

$$
P\left(f \, | \, \vec{x}_{\vec{i}_1}, \ldots, \vec{x}_{\vec{i}_m} \right)(\vec{x}_{\vec{i}_j}) = f(\vec{x}_{\vec{i}_j}), \qquad \text{for } j = 1, \ldots, m.
$$

As we will frequently consider interpolation nodes that possess a Cartesian structure

<!-- label: eq:sparse_grids:basics:global_polynomial_interpolation:cartesian_interpolation_nodes -->
$$
\mathcal{X} = \{ x^{(1)}_1, \ldots, x^{(1)}_{n_1} \} \times \cdots \times \{ x^{(d)}_1, \ldots, x^{(d)}_{n_d} \}, \tag{2.4}
$$

it is natural to choose a corresponding tensor product polynomial space.
More precisely, we employ

$$
\Pi_{(n_1, \ldots, n_d)} := \Pi_{n_1-1} \oplus \cdots \oplus \Pi_{n_d-1},
$$

where $\Pi_n$ denotes the space of univariate polynomials of degree at most $n$.

In the *Lagrange form*, the interpolant can be written as

$$
P\left(f \, | \, \vec{x}_{\vec{i}_1}, \ldots, \vec{x}_{\vec{i}_m} \right)(\vec{x}) = \sum^m_{j=1} f(\vec{x}_{\vec{i}_j}) \cdot \ell_{\vec{i}_j}(\vec{x}),
$$

where $\ell_{\vec{i}}$ denotes the multivariate Lagrange basis polynomial associated with the node $\vec{x}_{\vec{i}}$.

These basis polynomials satisfy

$$
\ell(\vec{x}_{i_j})(\vec{x}_{\vec{i}_k}) = \delta_{jk}, \qquad \text{for } j,k = 1, \ldots, m,
$$

with $\delta_{jk}$ being the Kronecker delta.

For Equation (2.4) above, the multivariate Lagrange basis can be expressed as a tensor product of one-dimensional Lagrange polynomials.
One has

$$
\ell_{\vec{i}}(\vec{x}) = \prod_{k=1}^d \ell_{\vec{i}[k]}^{(k)} (\vec{x}[k]),
$$

where the one-dimensional Lagrange polynomials are defined by

$$
\ell_{i}^{(k)}(\xi) = \prod_{\substack{j=1\\j\neq i}}^{n_k} \frac{\xi - x_j^{(k)}}{x_i^{(k)} - x_j^{(k)}}
$$

with the interpolation nodes $x_1^{(k)}, \ldots, x_{n_k}^{(k)}$ in dimension $k$.

### 2.1.2 Linear Interpolation {#sec:sparse_grids:basics:linear_interpolation}

In one dimension, *linear interpolation* approximates a source function $f$ on an interval $[a, b]$ by the unique linear function that matches $f$ at the endpoints:

$$
L(x) = f(a) \cdot \frac{b - x}{b - a} + f(b) \cdot \frac{x - a}{b - a}
$$

In higher dimensions, the corresponding generalization is usually called *multilinear interpolation*.
On a rectangular cell, it is obtained by tensor products of one-dimensional linear basis functions, i.e., the interpolant is linear in each coordinate separately.

### 2.1.3 Multi-indices {#sec:sparse_grids:basics:multi-indices}

First, let us introduce the notion of *multi-indices*, which we will use later on.
A multi-index $\vec{i} \in \mathbb{N}^d$ is a $d$-dimensional index represented as a vector.
For example, the multi-index $(1, 5)^T$ indexes the element at the first position in the first dimension and the fifth position in the second dimension.

Arithmetic operators such as $+, -, \cdot, /$ and relational operators such as $=, \neq, \le, <, >$ and $\ge$ are defined component-wise.
For example, the operator $\le$ is defined as

<!-- label: eq:sparse_grids:basics:le_rel_between_mi -->
$$
\vec{i} \le \vec{j} \quad :\Leftrightarrow \quad \forall k \in \{ 1, \ldots, d \}: \vec{i}_d \le \vec{j}_d. \tag{2.11}
$$

We consider a multi-index $\vec{i}$ to be a *predecessor* of another multi-index $\vec{j}$ if and only if $\vec{i} \le \vec{j}$ holds true.

Let $I \subseteq \mathbb{N}^d$ be an arbitrary set of multi-indices.
$I$ is called *downwards-closed* if and only if

<!-- label: eq:spare_grids:basics:downwards-closedness_property -->
$$
\forall \vec{j} \in \mathbb{N}^d: \; \left( \exists \vec{i} \in I: \vec{j} \le \vec{i} \right) \; \Rightarrow \; \left( \vec{j} \in I \right) \tag{2.12}
$$

is fulfilled.
Intuitively, a set of multi-indices is downwards-closed if it contains the predecessors of all multi-indices contained.

A multi-index $\vec{p}$ is a Pareto maximum of a multi-index set $I$ if and only if

<!-- label: eq:sparse_grids:basics:pareto_maximum_property -->
$$
\not\exists \vec{i} \in I: \; \vec{p} \le \vec{i} \land \vec{p} \neq \vec{i} \tag{2.13}
$$

is true.

### 2.1.4 Multisets {#sec:sparse_grids:basics:multisets}

A *multiset* is a generalization of a set that allows elements to occur multiple times.
In particular, negative multiplicities are also be permitted.
Formally, a multiset can be defined as a function

$$
M: \mathcal{E} \rightarrow \mathbb{Z},
$$

where $\mathcal{E}$ is the underlying set of potential elements [Stanley 2011, @stanley2011enumerative].
We denote multisets as a set of pairs $e : n$, where $e$ is represents the element and $n$ the multiplicity of $e$.
For example,

$$
\{ a : 2, b : 1, c : 3 \}
$$

contains the element $a$ twice, $b$ once, and $c$ three times.
Stating elements with a multiplicity of zero and not directly stating them is considered to be equivalent.

In order to work with multisets, we first need to define some important operators:

- **Multiplication** of a multiset $M$ with a scalar $\lambda \in \mathbb{R}$:
  $$
  \text{domain}(\lambda \cdot M) := \text{domain}(M), \qquad (\lambda \cdot M)(e) := \lambda \cdot M(e), \quad \forall e
  $$
  Example: $2 \cdot \{ a{:}2, b{:}1, c{:}3 \} = \{ a{:}4, b{:}2, c{:}6 \}$

- **Union "$\cup$"** of two multisets $M_1$ and $M_2$:
  $$
  \text{domain}(M_1 \cup M_2) := \text{domain}(M_1) \cup \text{domain}(M_2), \qquad (M_1 \cup M_2)(e) := \max\{ M_1(e), M_2(e) \}, \quad \forall e
  $$
  Example: $\{a{:} 2, b{:} 1\} \cup \{a{:}1 , b{:} 2, c{:}1\} = \{ a{:}2, b{:}2, c{:}1 \}$

- **Intersection "$\cap$"** of two multisets $M_1$ and $M_2$:
  $$
  \text{domain}(M_1 \cap M_2) := \text{domain}(M_1) \cup \text{domain}(M_2), \qquad (M_1 \cap M_2)(e) := \min\{ M_1(e), M_2(e) \}, \quad \forall e
  $$
  Example: $\{a{:} 2, b{:} 1\} \cap \{a{:}1 , b{:} 2, c{:}1\} = \{ a{:}1, b{:}1 \}$

- **Addition "$+$"** of two multisets $M_1$ and $M_2$:
  $$
  \text{domain}(M_1 + M_2) := \text{domain}(M_1) \cup \text{domain}(M_2), \qquad (M_1 + M_2)(e) := M_1(e) + M_2(e), \quad \forall e
  $$
  Example: $\{a{:} 2, b{:} 1\} + \{a{:} 1, b{:} 2, c{:}1\} = \{ a{:}3, b{:}3, c{:}1 \}$

- **Subtraction "$-$"** of two multisets $M_1$ and $M_2$:
  $$
  \text{domain}(M_1 - M_2) := \text{domain}(M_1) \cup \text{domain}(M_2), \qquad (M_1 - M_2)(e) := M_1(e) - M_2(e), \quad \forall e
  $$
  Example: $\{ a{:}1, b{:}2, c{:}1 \} - \{ a{:}1, b{:}1 \} = \{ b{:}1, c{:}1 \}$

- **Cartesian product "$\times$"** of two multisets $M_1$ and $M_2$:
  $$
  \text{domain}(M_1 \times M_2) := \text{domain}(M_1) \times \text{domain}(M_2), \qquad (M_1 \times M_2)(e_1, e_2) := M_1(e_1) \cdot M_2(e_2), \quad \forall e_1,e_2
  $$
  Example: $\{ a{:}1, b{:}2, c{:}1 \} \times \{ a{:}1, b{:}3 \} = \{ (a,a){:}1, (b,a){:}2, (a,b){:}3, (b,b){:}6, (c,a){:}1, (c,b){:}3 \}$ (illustrative; the source lists a shortened example: "$\{ a:1, b:2, c:1 \} - \{ a:1, b:3 \} = \{ a:1, b:6 \}$", which appears to be a typo in the original for the Cartesian product example).

### 2.1.5 Nodes {#sec:sparse_grids:basics:nodes}

Nodes (or also called knots) are one-dimensional positions like $0$, $0.5$, and $1$.
They are typically specified by through a distribution, which are called node distributions.
Popular node distributions are equidistant and Chebyshev nodes of the first and second kind.
Nodes are very commonly used in mathematics and all sorts of numerical methods.
Typical examples are grids, interpolation, and quadrature.

In the following, we assume that $[a, b]$ is the interval over which all nodes are defined and $n \in \mathbb{N}^+$ represents the number of nodes.

*Equidistant nodes* are defined as

<!-- label: eq:sparse_grids:basics:nodes:equidistant -->
$$
x_i = a + \frac{i}{n-1} (b - a)  , \quad i \in \{ 0, \ldots, n-1 \}, \tag{2.22}
$$

with $n \in \mathbb{N}$ being the number of nodes.
They are very easy to compute and cover the interval evenly, but they cause numerical instabilities in many numerical methods.
This makes them in many cases unusable.

*Chebyshev nodes* come in two different types.
The first kind are the roots of the Chebyshev polynomials and defined as

<!-- label: eq:sparse_grids:basics:nodes:chebyshev_first_kind -->
$$
x_i = \frac{a+b}{2} + \cos\left( \frac{(i + 0.5) \cdot \pi}{n} \right) \cdot \frac{b-a}{2}, \quad i \in \{ 0, \ldots, n-1 \}. \tag{2.23}
$$

They represent the projection of $n$ equispaced points, excluding the boundaries, on a half circle onto the interval $[a, b]$.
Chebyshev nodes of the second kind are the extrema of the Chebyshev polynomials and defined as

<!-- label: eq:sparse_grids:basics:nodes:chebyshev_second_kind -->
$$
x_i = \frac{a+b}{2} + \cos\left( \frac{i \cdot \pi}{n-1} \right) \cdot \frac{b-a}{2}, \quad i \in \{ 0, \ldots, n-1 \}. \tag{2.24}
$$

They represent the projection of $n$ equispaced points, including the boundaries, on a half circle onto the interval $[a, b]$.
The node density of both types increases towards the ends, which generally makes them less susceptible to numerical instabilities.
However, they are slightly more expensive to compute than equidistant points.

A visualization of all three distributions can be seen in Figure 2.1 below.
For $n = 1$, the midpoint $(a + b) / 2$ is used for all three distributions.

<!-- label: fig:sparse_grids:basics:nodes:overview -->
**Figure 2.1.** Visualization of equidistant and first and second kind Chebyshev nodes.
*Image: `figures/nodes/equidistant_chebyshev_nodes.pdf`*

### 2.1.6 Grid Points {#sec:sparse_grids:basics:grid_points}

The term *node* denotes a more general concept than a simple grid position.
In general, nodes may also be defined in higher dimensions and can serve different purposes depending on the numerical method under consideration.
In this thesis, however, we restrict ourselves to one-dimensional nodes, since this is consistent with the implementation and sufficient for the constructions used later on.

A node only becomes a *grid point* when it is interpreted as part of a grid structure.
In other words, the distinction between nodes and grid points is mainly semantic: Nodes are the abstract building blocks, whereas grid points are nodes viewed as elements of a grid.
Thus, every grid point is based on nodes, but not every node has to be a grid point.

This distinction is important in our setting.
Nodes are not only used to represent grid points, but may also serve other purposes, for example as interpolation or quadrature points.
Grid points, on the other hand, always refer to the concrete points of a grid.

In our module, nodes are used as the basic ingredients for constructing grid points.
Since we restrict ourselves to one-dimensional nodes, a grid point in $d$ dimensions is obtained by selecting one node in each dimension and combining them into a vector.

### 2.1.7 Tensor Grids {#sec:sparse_grids:basics:tensor_grids}

Tensor grids are full grids formed by the Cartesian product $\times$ of one-dimensional node distributions $\text{Nodes}_{k}$ over all dimensions $k \in \{ 1, \ldots, d \}$:

<!-- label: eq:sparse_grids:basics:tensor_grids:definition -->
$$
\text{Grid\_points} = \text{Nodes}_{1} \times \text{Nodes}_{2} \times \ldots \times \text{Nodes}_{d} \tag{2.25}
$$

Different dimensions are allowed to have different node distributions.
Intuitively, this can be imagined as an expansion of a one-dimensional node distribution to $d$ dimensions.
An illustration of this concept can be seen in Figure 2.2 below.

<!-- label: fig:sparse_grids:basics:tensor_grids:illustration_tensor_grid_creation -->
**Figure 2.2.** Illustration of the creation of a 2D tensor.
*Image: `figures/grids/tensor grids/visualization tensor grid generation.pdf`*

Tensor grids form the foundation for sparse grids, in particular for the combination technique, as the subsequent section will cover.
It should be noted here that tensor grids are conceptually no sparse grids yet, they are "only" the basis on which sparse grids are build.

Another important concept are *levels*.
A level $l \in \mathbb{N}^+$ is a description of a certain number of grid points a tensor grid has in a specific dimension.
Note that the number of grid points a level describes is not necessarily equal to the level's number.
For example, one level description could be:

$$
\text{Level 1: 1 grid point}, \quad \text{Level 2: 3 grid points}, \quad \text{Level 3: 7 grid points}, \ldots
$$

The conversion from a level number to the number of grid points can be chosen arbitrarily.
Common conversions are

<!-- label: eq:sparse_grids:basics:tensor_grids:doubling_level_conversion -->
$$
\#grid\_points(l) = 2^{l} - 1 \tag{2.26}
$$

and

$$
\#grid\_points(l) = a \cdot l
$$

with $a \in \mathbb{N}^+$.
An illustration of the level concept using the doubling conversion Equation (2.26) can be seen in Figure 2.3 below.

We can now use this level concept to describe tensor grids through multi-indices.
A multi-index $\vec{i} \in \mathbb{N}^d$ describes the $d$-dimensional tensor grid, where the level of each dimension $k$ equals the $k$-th component $\vec{i}[k]$.
The tensor grid used throughout the illustrations would be, for example, $(2, 3)$, because it has level $2$ in the first dimension and level $3$ in the second.
Of course, this description depends on the level conversion and the type of nodes chosen.

From this point onward, we consistently employ the doubling conversion Equation (2.26) throughout the remainder of this chapter.

<!-- label: fig:sparse_grids:basics:tensor_grids:examples -->
**Figure 2.3.** Examples for tensor grids.
*Image: `figures/grids/tensor grids/example tensor grids.pdf`*

One question remains: What does the "tensor" in "tensor grid" stand for?
In the regular mathematical description of sparse grids, nodes are associated with basis functions (usually one-dimensional nodal hat functions).
These basis functions are then extended to $d$-dimensions over the tensor grids in a manner similar to the Cartesian product $\times$.
However, to obtain actual $d$-dimensional basis functions from the one-dimensional ones, a simple cross product is insufficient.
The tensor product $\oplus$ is required.
For more details, see Section 2.4 (Hierarchical Representation) below.

### 2.1.8 Sparse Grids {#sec:sparse_grids:basics:sparse_grids}

To visualize and aid with the understanding of the sparse grid construction, we begin by introducing tensor grid tableaus.
A tensor grid tableau is a $d$-dimensional table where each entry holds the $d$-dimensional tensor grid represented by its index in the table.
Figure 2.4 below depicts the two-dimensional variant.

<!-- label: fig:sparse_grids:basics:sparse_grids:2d_tableau -->
**Figure 2.4.** Two-dimensional tensor grid tableau.
*Image: `figures/tensor grid tableaus/2d tensor grid tableau up to level 3.pdf`*

A *sparse grid* is now constructed based on a selection of multi-indices $I \subseteq \mathbb{N}^d$.
The multi-indices specify the tensor grids used and therefore contained in the resulting sparse grid.
A selection $I$ is valid if and only if it is downwards-closed (as defined in Section 2.1, Basics, above).
Intuitively, the downwards-closed property requires all predecessors of multi-indices contained in $I$ to be also contained in $I$ itself.
This is illustrated by two examples in Figure 2.5 below.

<!-- label: fig:sparse_grids:basics:sparse_grids:examples_valid_and_invalid_mi_choices -->
**Figure 2.5.** Two examples illustrating valid and invalid multi-indices choices.
*Image: `figures/tensor grid tableaus/examples valid and invalid mi choices.pdf`*

The sparse grid of such a downwards-closed set of multi-indices $I$ is defined as the union of all tensor grids in $I$:

<!-- label: eq:sparse_grids:basics:sparse_grids:gps_union -->
$$
\text{Sparse\_grid\_points}(I) = \bigcup_{\vec{i} \in I} \text{Tensor\_grid\_points} (\vec{i}) \tag{2.28}
$$

Here, the union operator $\cup$ refers to the regular union operator between sets.
Illustrations of the creation can be seen in the following Figure 2.6.

<!-- label: fig:sparse_grids:basics:sparse_grids:visualization_sparse_grid_creation -->
**Figure 2.6.** Visualization of the sparse grid creation.
*Image: `figures/grids/sparse grid creation/visualization 2d sparse grid creations.pdf`*

Some special types of sparse grid exist, which we will cover the following.
The most well-known type is what we call complete sparse grids.
Note that "complete" does not refer to the fullness property of grids.
In fact, a complete sparse grid is generally not a full grid.
A complete sparse grid contains all tensor grids with multi-indices $\vec{i} \in \mathbb{N}^d$ that fulfill

<!-- label: eq:sparse_grids:basics:sparse_grids:complete_sparse_grid_mi_criterion -->
$$
|| \vec{i} ||_1 \le l_{max} + d - 1, \tag{2.29}
$$

where $d \in \mathbb{N}$ is the number of dimensions and $l_{max} \in \mathbb{N}^+$ is a parameter that limits the maximum level that a dimension in a tensor grid can have.
The multi-index sets also have a highly characteristic form in the tableau.
They form a hypertriangle that is limited by the diagonal with the Manhattan distance of $l_{max}$.
Complete sparse grids are also particularly advantageous for the combination technique, as we discuss in Section 5.6 (Sparse Grid Generation) in file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md).

A second special case of sparse grids are generated through multi-index sets with only one Pareto maximum.
Intuitively, this is the case if there is exactly one "master" multi-index and all others are predecessors of it.
Such sparse grids are equal to the tensor grid of their Pareto maxima, because all other contained tensor grids are just subsets.
Examples of both types can be seen in Figure 2.6 above.

To further illustrate how sparse grids may be formed, six more examples of various different sparse grids are depicted in Figure 2.7 below.

<!-- label: fig:sparse_grids:basics:sparse_grids:spare_grid_examples -->
**Figure 2.7.** Examples of various sparse grids.
*Image: `figures/grids/sparse grids/sparse_grid_examples.pdf`*

## 2.2 Boundary Treatment {#sec:sparse_grids:boundary_treatment}

In the past section, we have mostly neglected the boundary.
However, boundary handling is also an essential aspect of sparse grids.

In many cases, values on the boundary are important.
Grid points on the boundary are introduced to allow for their coverage.
The most common approach for the CT adds nodes on both ends of the interval to each node distribution.
Some node distributions might already define nodes on the interval ends.
Level conversions might also need adjustments to accommodate for the newly introduced nodes.
For example, the doubling level conversion Equation (2.26) above would be changed to

$$
\#grid\_points(l) = 2^l + 1.
$$

A comparison between a sparse grid with and without grid points on the boundary can be seen in Figure 2.7 above.

This introduction of grid points on the boundary has a significant downside to it.
In two dimensions there are already two times the number of grid points on the boundary as on the main axes.
This effect gets worse the larger the number of dimensions is: The ratio of grid points on the boundary to those in the interior increases with the number of dimensions.
As Table 2.1 below shows, the increase is rapid.
While the effect itself is not directly problematic, it does mean that a lot of computational resources are used for the boundary.
Therefore, in scenarios where no high accuracy towards the boundary is required, it might be beneficial to reduce the amount of grid points on the boundary.
This is commonly achieved through a level reduction of the grid points on the boundary compared to the main axes.
An example can be seen in Figure 2.8 below.
There are different implementations of such a reduction.
The one designed by this work for the new SG++ module is described in Section 5.7 (Boundary Management) in file [05-algorithmic-and-design-choices.md](05-algorithmic-and-design-choices.md).

<!-- label: tab:sparse_grids:boundary_treatment:ratio_of_grid_points_on_the_boundary -->
**Table 2.1.** Percentages of grid points on the boundary for complete sparse grids, depending on the number of dimensions $d$ and the max. level $l_{max}$.

| $l_{max} \backslash d$ | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---|---|---|---|---|---|---|---|
| 2 | 40.0% | 76.2% | 91.4% | 97.0% | 99.0% | 99.6% | 99.9% |
| 4 | 11.8% | 56.6% | 81.3% | 92.5% | 97.1% | 98.9% | 99.6% |
| 6 | 3.1% | 44.4% | 72.4% | 87.5% | 94.6% | 97.8% | 99.1% |

<!-- label: fig:sparse_grids:boundary_treatment:visualization_boundary_level_offset -->
**Figure 2.8.** Visualization of the boundary level offset concept.
*Image: `figures/grids/sparse grids/visualization boundary level offset.pdf`*

## 2.3 Combination Technique {#sec:sparse_grids:combination_technique}

Now that understand what sparse grids are, how are sparse grids used to compute solutions to certain problems like interpolation or quadrature?
Assume that we have the following complete sparse grid.

<!-- label: fig:sparse_grids:combination_technique:2d_complete_sg_3m -->
**Figure 2.9.** Complete sparse grid with a max. level of $3$.
*Image: `figures/grids/sparse grids/2d_complete_sg_3m.pdf`*

How do we compute an interpolant or the integral of a function $f$ over this grid?
Conventional approaches, like the tensor grid extension of the one-dimensional quadrature, cannot be utilized because the sparse grid is not a full grid.
Here comes the combination technique (CT) into play.

The CT is a method to compute a solution over the sparse grid based on tensor grid solutions.
Its main idea is to compute a solution to a problem for each tensor grid contained in the sparse grid.
This can be achieved way easier for many problems using known approaches like multidimensional polynomial interpolation or quadrature.
After obtaining those solutions, we then combine them suitably to get a solution for the sparse grid.

But how do we combine them suitably?
A naive approach would be to simply add all solutions up like we unified all tensor grids into the sparse grid.
However, this does not yield a valid solution, as we can already see in trivial examples.
Let's look at a constant function

$$
f: \mathbb{R}^2 \rightarrow \mathbb{R}, \, (x_1, x_2) \mapsto c
$$

for some $c \in \mathbb{R}$.
Our goal is to compute the integral

$$
\int_{[0, 1]^2} f(x_1, x_2) \, dx_1 dx_2.
$$

The correct solution is $c$, which is also the result for each contained tensor grid.
Since our sparse grid is composed of 6 tensor grids (as depicted in Figure 2.6 above), the overall solution would be $6 \cdot c$.
This is obviously wrong.

What is the problem?
The problem is that grid points are contained in multiple tensor grids, as shown in Figure 2.6 above.
For our chosen sparse grid, for example all contained tensor grids contain the center point.
Therefore, grid points are considered multiple times for the overall solution.

This issue can be fixed by weighting all grid points the same.
Grid points contained multiple times need to be removed like the union operator $\cup$ in Equation (2.28) above does.
This brings us to the inclusion-exclusion principle.
For the purposes of the following explanation, we will assume that the nodes are nested.
The CT can also be applied to non-nested nodes, but this is outside the scope of this work.

### 2.3.1 Inclusion-Exclusion Principle {#sec:sparse_grids:combination_technique:inclusion-exclusion_principle}

The inclusion-exclusion principle is a basic concept from set theory [Stanley 2011, @stanley2011enumerative].
While the principle is less known in its core form, theorems derivable from it are very commonly known.
For example the theorem

$$
\mathbb{P}(A \cup B) = \mathbb{P}(A) + \mathbb{P}(B) - \mathbb{P}(A \cap B)
$$

from stochastics for two stochastic events $A, B \subseteq \Omega$ of a sample space $\Omega$.
Another example is the theorem

$$
\left| \bigcup_{i=1}^n A_i \right| = \underbrace{(| A_1 | + \ldots + |A_n|)}_{\text{All combin. of size 1}} - \underbrace{(|A_1 \cap A_2| + \ldots + |A_{n-1} \cap A_n|)}_{\text{All combin. of size 2}} + \underbrace{(|A_1 \cap A_2 \cap A_3| + \ldots)}_{\text{All combin. of size 3}} - \ldots
$$

for arbitrary sets $A_i$ [Zimmer, Piller & Uekermann 2024, @numerische_grundlagen_lecture].

The core concept can be formulated using multisets.
As a reminder, multisets and their operators are defined in Section 2.1.4 (Multisets) above.
Let $\binom{M}{k}$ be the set of all $k$-sized combinations of elements in a set $M$.

The core principle states how ordinary sets $M_i$ can be unified using the multisets addition, subtraction and intersection.
The $M_i$ are interpreted in the following theorem as multisets, but they are required to equal ordinary sets (i.e., be multisets with multiplicities of $0$ or $1$).
Their union can be written as:

<!-- label: eq:sparse_grids:combination_technique:inclusion-exclusion_principle:theorem -->
$$
\begin{split}
\bigcup_{i=1}^n M_i &= \underbrace{\sum_{k=1}^n}_{\text{Comb. size}} \left( (-1)^{k+1} \cdot \underbrace{\sum_{\vec{i} \in \binom{\{ 1, \ldots, n \}}{k}}}_{\text{Comb. of size } k} \left( \cap_{j \in \vec{i}} M_j \right) \right) \\
&= \underbrace{(M_1 + \ldots + M_n)}_{\text{All combin. of size 1}} - \underbrace{(M_1 \cap M_2 + \ldots + M_{n-1} \cap M_n)}_{\text{All combin. of size 2}} + \underbrace{(M_1 \cap M_2 \cap M_3 + \ldots)}_{\text{All combin. of size 3}} - \ldots
\end{split}
\tag{2.35}
$$

### 2.3.2 Application of the Inclusion-Exclusion Principle {#sec:sparse_grids:combination_technique:application_inclusion-exclusion_principle}

We can now use the inclusion-exclusion principle Equation (2.35) above to solve our tensor grid combination issue from before.
If we set the $M_i$ to the grid points of each tensor grid, then the union $\cup_{i=1}^n M_i$ will exactly represent what we desire.

The right hand side of Equation (2.35) seems way more complicated, but most of it is just sums.
The only term that seems more complicated are the intersections $\cap_{j \in \vec{i}} M_j$.
For example, take a look at Figure 2.10 below.
It illustrates the intersection of two tensor grids.
The intersection is also a tensor grid.
This is not a coincidence but a general rule that always applies.
The intersection between tensor grids, represented as multi-indices $\vec{i}_1, \ldots, \vec{i}_k \in \mathbb{N}^d$, is always the tensor grid corresponding to the component-wise minimum

$$
\left( \begin{array}{c} \min\{ \vec{i}_1[0], \ldots, \vec{i}_k[0] \} \\ \vdots \\ \min\{ \vec{i}_1[d-1], \ldots, \vec{i}_k[d-1]  \} \end{array} \right).
$$

We can now use the inclusion-exclusion principle to combine the individual tensor grid solutions suitably:

1. Solve the problem on each tensor grid contained in the sparse grid.
2. Replace all tensor grid intersections in Equation (2.35) above with the solution over the corresponding tensor grids.
3. Sum up the individual solutions for an overall solution.

<!-- label: fig:sparse_grids:combination_technique:application_inclusion-exclusion_principle:visualization_intersection_of_tensor_grids -->
**Figure 2.10.** Visualization intersection of two tensor grids.
*Image: `figures/tensor grid tableaus/visualization intersection between tensor grids.pdf`*

### 2.3.3 Combination Technique Coefficients and Improvements {#sec:sparse_grids:combination_technique:ct_coefficients_and_improvements}

The formula can be used even more efficiently when only considering the tensor grids that are Pareto maxima as $M_i$.
This yields the same result, because the union over all tensor grids representing Pareto maxima is identical to the union over all tensor grids.

Intersections in Equation (2.35) above will often result in the same tensor grids.
We can therefore factorize the corresponding solution and aggregate their coefficients.
The resulting coefficients that weigh the individual tensor grid solutions are known as the combination technique coefficients.

For complete sparse grids, there is even a closed form for the CT coefficients.
They can be computed via

<!-- label: eq:sparse_grids:combination_technique:ct_coefficients_and_improvements:ct_coefficients_compelete_sg -->
$$
\text{CT-Coeffs}(\vec{i}) = \left\{\begin{array}{cc} (-1)^{q} \cdot \binom{d - 1}{q}, & || \vec{i} ||_1 \ge l_{max} \\ 0, & || \vec{i} ||_1 < l_{max} \end{array} \right., \tag{2.37}
$$

where $\vec{i}$ is the multi-index of a contained tensor grid, $l_{max}$ is the maximum level of the complete sparse grid, $d$ is the number of dimensions, and $q := l_{max} + d - 1 - || \vec{i} ||_1$.
This creates a very characteristic pattern, as illustrated in Figure 2.11 below.

<!-- label: fig:sparse_grids:combination_technique:ct_coefficients_and_improvements:ct_coeffs_complete_sg -->
**Figure 2.11.** CT coefficients for the complete sparse grid with a max. level of $3$.
*Image: `figures/tensor grid tableaus/ct coeffs complete sg 3m.pdf`*

### 2.3.4 Dimension Adaptivity {#sec:sparse_grids:combination_technique:dimension_adaptivity}

Dimension-adaptive refinement extends the combination technique by allowing the set of multi-indices to be selected dynamically with respect to the source function $f$.
Instead of fixing the sparse grid structure in advance, the multi-index set is initialized with a small admissible subset and then iteratively expanded according to profit indicators until a prescribed termination criterion is satisfied.
These indicators commonly balance the expected benefit of adding a new multi-index against its computational cost, thereby guiding the refinement towards the most promising directions.
As a result, dimension-adaptive sparse grids can allocate more grid points to dimensions that have a stronger influence on the solution or in which the source function exhibits larger variations, while saving grid points in less relevant dimensions [Piazzola & Tamellini, @sgmk_manual; Piazzola & Tamellini 2024, @sparse_grids_matlab_kit].

Figure 2.12 below illustrates a typical step of the dimension-adaptive refinement process.
The red crosses denote the current set $I$ of multi-indices used to construct the sparse grid.
Blue markers indicate indices whose neighboring candidates have already been explored, whereas the white squares represent indices whose neighborhoods have not yet been examined.
The green marker highlights the next index selected for refinement.
Thus, the figure visualizes how the algorithm incrementally expands the index set by focusing on the most promising regions of the index space.

<!-- label: fig:sparse_grids:combination_technique:dimension_adaptivity:visualization -->
**Figure 2.12.** Illustration of the dimension-adaptive refinement process generated by the Sparse Grids Matlab Kit [Piazzola & Tamellini 2024, @sparse_grids_matlab_kit].
*Image: `figures/dimension-adaptive refinement.pdf`*

### 2.3.5 Strengths and weaknesses of the Combination Technique {#sec:sparse_grids:combination_technique:strengths_weaknesses}

The CT is attractive because it allows sparse grid approximations to be built from full grid solvers that are already available in many settings.
This is a major practical advantage: instead of designing a dedicated sparse grid solver from scratch, one can reuse established algorithms for interpolation, quadrature, or PDE discretizations on full grids and combine the resulting tensor grid solutions afterwards.
In this sense, the CT acts as a thin abstraction layer that transfers existing full grid methods to sparse grid settings.

A second strength is its inherent parallelism.
Since the individual tensor grid problems are independent of each other, they can be solved concurrently without synchronization during the solve phase.
Only the final combination step requires aggregation of the partial solutions.
This makes the CT well suited for parallel architectures and distributed environments, especially when the tensor grid solves dominate the overall computational cost.

For complete sparse grids, the CT coefficients can even be written in closed form.
This simplifies both the theoretical analysis and the implementation, since the combination coefficients do not need to be computed by a general inclusion-exclusion procedure.
Instead, they can be generated directly from the grid levels and the dimension of the problem.
In practice, this yields a compact and elegant formulation of the approximation.

However, the CT also has several limitations.
Its most important drawback is that it does not support local adaptivity in a direct way.
Refinement is not performed on arbitrary local subregions of the domain, but rather on selected dimensions.
As a consequence, the method is less flexible than locally adaptive sparse grid approaches when the solution exhibits sharp local features or strongly inhomogeneous behavior.
Dimension-adaptive variants exist, but they require additional bookkeeping and a more elaborate refinement strategy.
These adaptations such as dimension-adaptive refinement or opticom (optimal combination technique) introduce extra computational cost and limit parallelism compared to the basic CT.
Their usage must be therefore considered carefully.

A further limitation is that the quality of the resulting approximation depends strongly on the structure of the underlying problem.
As we will discuss in Section 2.4.1 (Exactness of the Combination Technique) below, the CT is not guaranteed to provide equally good results for all problem classes.
In particular, for more complex or less regular problems, the sparse grid approximation may no longer provide the expected qualities, even if the CT itself is still employable.
Thus, the CT should not be viewed as a universally optimal method, but rather as an efficient and highly parallelizable construction whose success depends on the problem structure and on the suitability of the chosen sparse grid method.

### 2.3.6 Interpolation and Quadrature {#sec:sparse_grids:combination_technique:interpolation_and_integration}

To conclude the CT, we will briefly cover the interpolation and integration as two problems that are commonly solved.
They are the foundation for many more complex problems like UQ.
Listing 2.1 below illustrates how the interpolation of a source function $f$ works on a conceptual level using the CT.
Integration works completely analog by using quadrature instead of interpolation.

Table 2.2 below summarizes the asymptotic behavior of interpolation and quadrature on sparse grids and compares it with the corresponding full grid formulations [Pflüger & Schulte 2025, @akwr_lecture; Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations; Garcke 2012, @sparse_grids_in_a_nutshell].

A complete sparse grid consists in the CT of tensor grids with a total number of grid points in $\mathcal{O}(d \cdot n \cdot (\log n)^{d-1})$ with $d$ being the number of dimensions and $n$ the number of grid points per dimension.
The runtime for global polynomial interpolation (evaluation) is in the same asymptotic class because interpolation on a full grid can be achieved in constant time per grid point.
This represents a significant reduction in the runtime compared to full grids, since the dominant term has been reduced from $n^d$ to $(\log n)^{d-1}$.
While the asymptotic error decay worsens slightly by a logarithmic factor $(\log n)^{d-1}$, this is generally a worthy trade-off.

Examining the quadrature reveals that the runtime remains unchanged.
Likewise, the asymptotic error decay worsens by a similar logarithmic factor $\frac{1}{d}^p \cdot (\log n)^{d-1}$.

<!-- label: tab:sparse_grids:combination_technique:interpolation_and_integration:runtime_cost_and_error -->
**Table 2.2.** Asymptotic runtime and error decay of interpolation and quadrature based on number of grid points per dimension $n$, number of dimensions $d$, and order of the quadrature rule $p$ (assuming complete sparse grid and sufficiently smooth function).

| | Total grid points / Runtime | Error |
|---|---|---|
| Global interpolation (CT) | $\mathcal{O}(d \cdot n \cdot (\log n)^{d-1})$ | $\mathcal{O}\left( \frac{1}{n}^2 \cdot (\log n)^{d-1}\right)$ |
| Quadrature (CT) | $\mathcal{O}(d \cdot n \cdot (\log n)^{d-1})$ | $\mathcal{O}\left( \frac{1}{d}^{p} \cdot \frac{1}{n}^{p} \cdot (\log n)^{d-1} \right)$ |
| Global interpolation (full grid) | $\mathcal{O}(n^d)$ | $\mathcal{O}\left( \frac{1}{n}^2 \right)$ |
| Quadrature (full grid) | $\mathcal{O}(n^d)$ | $\mathcal{O}\left( \frac{1}{n}^{p} \right)$ |

<!-- label: lst:sparse_grids:combination_technique:interpolation_and_integration:interpolation -->
**Listing 2.1.** Interpolation of a source function over a sparse grid (on a conceptual level).

```python
f = ... # Source function
multi_indices = ... # Generate multi-indices of contained tensor grids in sparse grid
ct_coefficients = ... # Generate corresponding CT coefficients

# May be the interpolant or an evaluation of it in some point x
result = 0

for (mi, coeff) in (multi_indices, ct_coefficients):
  tensor_grid = generate_tensor_grid(mi)

  value = interpolate_over_full_grid(f, tensor_grid)

  result += coeff * value
```

## 2.4 Hierarchical Representation {#sec:sparse_grids:hierarchical_representation}

The common introduction to sparse grids in the literature is given through the hierarchical representation.
The presentation used in this work is an abstraction of this more general and mathematically detailed description, adapted specifically to the combination technique.
In order to keep the scope of this thesis manageable and to provide a more accessible introduction to sparse grids in the context of the CT, several aspects that are less relevant for the combination technique are omitted or only treated abstractly.
The concepts are presented to the extent necessary for the purposes of this work, so that the exposition remains conceptually self-contained, albeit not exhaustive.
For a more detailed treatment, we refer to [Pflüger 2010, @pflueger_diss; Garcke 2012, @sparse_grids_in_a_nutshell].

### 2.4.1 Exactness of the Combination Technique {#sec:sparse_grids:combination_technique:exactness}

Having introduced the combination technique and its underlying principles, the question remains: How accurate are the results obtained by the combination technique?
This strongly depends on the kind of problem.
The following provides a brief overview over this topic for the sake of completeness.
For a detailed discussion, we refer to Hegland, Garcke & Challis [Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations].

In general, we are given a function $f$ and seek to determine $P(f)$, where $P: H \rightarrow V$ denotes a projection from a (typically infinite-dimensional) space $H$ onto a constructed solution space $V$.
Intuitively, $H$ contains all admissible functions, while $V$ contains the possible representations on the sparse grid (sparse grid space).
The projection $P$ thus maps a function $f \in H$ to a representation in the sparse grid space $V$.
The quality of this projection, of course, depends on the specific mapping and the underlying discretization.

As discussed in Section 2.3 (Combination Technique) above, the combination technique does not compute $P(f)$ directly.
Instead, it constructs a solution over the whole sparse grid by a linear combination of the individual tensor-grid representations,

<!-- label: eq:sparse_grids:combination_technique:correctness:partial_projections_linear_combination -->
$$
P_{\text{CT}}(f) := \sum_{\vec{i} \in I} c_{\vec{i}} \cdot P_{\vec{i}}(f), \tag{2.38}
$$

where the operators $P_{\vec{i}}: H \rightarrow V_{\vec{i}}$ denote the partial projections onto the solution spaces $V_{\vec{i}}$ of the individual tensor grids.

This linear combination $P_{\text{CT}}(f)$ equals the sparse grid solution $P(f)$ of the regular hierarchical representation, if the partial projections commute, that is,

<!-- label: eq:sparse_grids:combination_technique:correctness:commutativity_of_partial_projections -->
$$
\forall \vec{i}, \vec{j}:\, P_{\vec{i}} P_{\vec{j}} = P_{\vec{j}} P_{\vec{i}}. \tag{2.39}
$$

In this case, results known for the sparse grid projection, such as the asymptotic decay of the error, also carry over to the combination technique.
Interpolation and quadrature satisfy this condition, which is why Table 2.2 above applies.

However, not every problem class fulfills Equation (2.39) above.
For instance, PDEs or regression problems may violate this property [Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations; Pflüger 2010, @pflueger_diss].
This does not automatically imply divergence from the analytical solution or inaccurate results.
In fact, there are cases in which the commutativity condition is not satisfied, yet the combination technique still converges to the correct analytical solution [Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations].

## References cited in this chapter

- **[Pflüger 2010, @pflueger_diss]** Dirk Pflüger. *Spatially Adaptive Sparse Grids for High-Dimensional Problems*. Verlag Dr. Hut, 2010. ISBN 978-3-86853-555-6.
- **[Garcke 2012, @sparse_grids_in_a_nutshell]** Jochen Garcke. "Sparse grids in a nutshell." In *Sparse grids and applications*, pp. 57–80. Springer, 2012.
- **[Hegland, Garcke & Challis 2007, @combination_technique_and_generalisations]** Markus Hegland, Jochen Garcke, Vivien Challis. "The combination technique and some generalisations." *Linear Algebra and its Applications*, 420(2-3):249–275, Elsevier, 2007.
- **[Pflüger & Schulte 2025, @akwr_lecture]** Dirk Pflüger, Miriam Schulte. "Ausgewählte Kapitel des Wissenschaftlichen Rechnens." Lecture slides, University of Stuttgart, 2025.
- **[Balduzzi, Bonizzoni & Tamellini 2023, @uq_in_beams_using_sg]** Giuseppe Balduzzi, Francesca Bonizzoni, Lorenzo Tamellini. "Uncertainty quantification in timber-like beams using sparse grids: Theory and examples with off-the-shelf software utilization." *Construction and Building Materials*, 406:132730, 2023.
- **[Neumann 2019, @regression_for_predictions_using_sg]** Philipp Neumann. "Sparse grid regression for performance prediction using high-dimensional run time data." In *European Conference on Parallel Processing*, pp. 601–612, Springer, 2019.
- **[Honarmandi & Arróyave 2020, @uq_for_material_science_using_sg]** Pejman Honarmandi, Raymundo Arróyave. "Uncertainty quantification and propagation in computational materials science and simulation-assisted materials design." *Integrating Materials and Manufacturing Innovation*, 9(1):103–143, Springer, 2020.
- **[Brumm et al. 2021, @economic_models_using_sg]** Johannes Brumm, Christopher Krause, Andreas Schaab, Simon Scheidegger. "Sparse grids for dynamic economic models." *Available at SSRN 3979412*, 2021.
- **[Stanley 2011, @stanley2011enumerative]** Richard P. Stanley. "Enumerative combinatorics volume 1 second edition." *Cambridge studies in advanced mathematics*, 2011.
- **[Zimmer, Piller & Uekermann 2024, @numerische_grundlagen_lecture]** Stefan Zimmer, Lukas Piller, Benjamin Uekermann. "Numerische Grundlagen." Lecture slides, University of Stuttgart, 2024.
- **[Piazzola & Tamellini, @sgmk_manual]** C. Piazzola, L. Tamellini. "Sparse Grids Matlab Kit Manual." 2023. <https://drive.google.com/file/d/1e0yeJ7IGUq3G014JD0q3-GUQLCjyA44n/view>. Accessed 2026-04-18.
- **[Piazzola & Tamellini 2024, @sparse_grids_matlab_kit]** C. Piazzola, L. Tamellini. "Algorithm 1040: The Sparse Grids Matlab Kit — a Matlab implementation of sparse grids for high-dimensional function approximation and uncertainty quantification." *ACM Transactions on Mathematical Software*, 50(1), 2024. DOI: 10.1145/3630023.
