---
source: bachelor thesis/thesis/chapters/application.tex (+ chapters/sections/application/{quadrature_of_an_absolute_sine_function,interpolation_of_the_classical_cosine_function,forward_uncertainty_quantification_of_an_electron_beam_through_one_dimensional_layers}.tex)
chapter: 6
title: Application
---

# 6 Application {#sec:application}

This chapter demonstrates the practical usage of the developed sparse grid combination technique (CT) module for SG++ by means of three representative application examples.
The goal is to illustrate how the module can be used in different numerical settings and how its configuration changes depending on the underlying task.

The first two examples focus on classical approximation problems, namely numerical quadrature and function interpolation.
They serve to validate the implemented workflow and to show how sparse grids can be applied to compute integrals and reconstruct functions in a reliable and efficient way.
The third example addresses a forward uncertainty quantification (UQ) problem for an electron beam propagating through one-dimensional layers.
This example demonstrates that the module is not limited to purely deterministic approximation tasks, but can also be used in a stochastic collocation setting to propagate input uncertainty through a physical model.

Together, these examples cover a range of typical sparse-grid applications and provide a compact overview of the implemented module.

## 6.1 Quadrature of an Absolute Value Sine Function {#sec:application:quadrature_absolute_sin}

As a first application, we consider the absolute value of a sine function in $d$ dimensions, defined as

<!-- label: eq:application:quadrature_absolute_sin:function -->
$$
f(\vec{x}) = \prod_{k=1}^d \left| \sin \left( 3 \pi \cdot \vec{x}[k] \right) \right|. \tag{6.1}
$$

This function serves as our model integrand.
Figure 6.1 below shows a plot of the one- and two-dimensional cases.

The objective is to approximate the integral

$$
\int_{[0, 1]^d} f(\vec{x}) \, d \vec{x}
$$

by means of numerical quadrature.

<!-- label: eq:application:quadrature_absolute_sin:visualization_function -->
**Figure 6.1.** Visualization of Equation (6.1) above for one and two dimensions.
*Image: `figures/visualization_absolute_sine_functions.pdf`*

(Note: in the original document this figure carries the label `eq:application:quadrature_absolute_sin:visualization_function`, which is a figure despite the `eq:` prefix — preserved verbatim from the source.)

The function exhibits moderately high-frequency behavior due to the factor $3\pi$, has a decently global behavior over the complete domain, and remains separable across dimensions.
Its exact integral can be computed analytically and is given by

<!-- label: eq:application:quadrature_absolute_sin:analytical_solution -->
$$
\int_{[0, 1]^d} f(\vec{x}) \, d \vec{x} = \left( \frac{2}{\pi} \right)^d. \tag{6.3}
$$

This combination of the global behavior (i.e., suitable for non-adaptive methods) and known closed-form solution makes it a suitable first non-trivial problem for the new SG++ module.

Listing 6.1 below shows the complete workflow for this example.
First, the source function Equation (6.1) above is implemented as a small standalone method.
In addition, a second function provides the exact value of the integral.
This closed-form reference solution is used later to assess the accuracy of the numerical result.

In the main routine, we start with wrapping the source function into the corresponding `SourceFunc` object so that it can be used by the quadrature operation.
Then the sparse grid generation instruction is configured.
In this example, equidistant nodes are selected, which means that the grid points are placed regularly within the unit interval.
Additionally, grid points on the boundary are disabled, because $f$ is zero on it.

Once the configuration is complete, the sparse grid is generated from the instruction object.
The quadrature operation is then applied to the `SourceFunc` and `SparseGrid` to compute the numerical approximation of the integral.
Finally, the program outputs both the numerical result and the analytical reference value Equation (6.3) above.
The absolute error of those can be seen in Table 6.2 below for different number of dimensions and maximum levels of the complete sparse grid.

<!-- label: lst:application:quadrature_absolute_sin:code -->
**Listing 6.1.** Code to approximate the integral of the source function Equation (6.1) above.

```cpp
double absSinFunction(DataVector& x) {
  double result = 1.0;

  for (size_t dim = 0; dim < x.size(); dim++)
    result *= std::sin(3 * M_PI * x[dim]);

  return std::abs(result);
}

double analyticalSolution(size_t nDim) { return std::pow(2 / M_PI, nDim); }

int main() {
  /* === 0. Parameters === */
  size_t nDim = ...   // Number of dimensions
  size_t maxLvl = ... // Maximum level of the complete sparse grid

  /* === 1. Initialize Source Function === */
  SourceFunc sourceFunc(absSinFunction);

  /* === 2. Configure Sparse Grid Generation Instruction === */
  CompleteSGGenInstr genInstr(maxLvl, nDim);

  // Use equidistant nodes -> Simpson quadrature rule
  genInstr.setNodeGenFunc(getEquidistantNodeGenFunc());
  // No grid points on the boundary are used
  genInstr.setBoundaryUsage(False);

  /* === 3. Generate Sparse Grid === */
  SparseGrid sg(genInstr);

  /* === 4. Apply Quadrature === */
  double result = quadrature(sourceFunc, sg);

  /* === 5. Output Results === */
  cout << "Result: " << result << endl;
  cout << "Analytical solution: " << analyticalSolution(nDim) << endl;
}
```

<!-- label: tab:application:quadrature_absolute_sin:results -->
**Table 6.2.** Relative error of the quadrature results produced by Listing 6.1 above for different number of dimensions $d$ and max. level $l_{max}$ (rounded to 2 decimal places).

| $d \backslash l_{max}$ | 0 | 2 | 4 | 6 | 8 | 10 | Exact solution |
|---|---|---|---|---|---|---|---|
| 1 | 5.71E-1 | 1.38E-1 | 5.48E-2 | 1.52E-2 | 3.88E-3 | 9.75E-4 | 0.636620 |
| 2 | 1.47E+0 | 1.30E-1 | 4.75E-2 | 9.09E-3 | 8.52E-3 | 3.92E-3 | 0.405285 |
| 3 | 2.88E+0 | 2.20E-1 | 1.94E-2 | 6.75E-2 | 2.42E-2 | 6.25E-3 | 0.258012 |

## 6.2 Interpolation of the Classical Cosine Function {#sec:application:interpolation_cos_function}

As a second application, we consider the global polynomial interpolation of the classical cosine function in $d$ dimensions, defined by

<!-- label: eq:application:interpolation_cos_function:function -->
$$
f(\vec{x}) = \prod_{k=1}^d \cos \left( \vec{x}[k] \right). \tag{6.4}
$$

This function is smooth, separable across all dimensions, and therefore well suited as a representative interpolation test case.
In contrast to the first examples, the goal here is not to approximate an integral, but to interpolate the function value at a prescribed point $\vec{x} \in [-2\pi, 2\pi]^d$.

Listing 6.2 below shows the complete implementation of this example.
First, the source function Equation (6.4) above is again implemented as a small standalone method and then wrapped into the corresponding `SourceFunc` object in the main routine.

Compared to the quadrature example, the sparse grid generation is configured differently.
Here, Chebyshev nodes of the second kind are selected for the grid points.
This choice is important because it keeps the interpolation numerically stable even for larger numbers of grid points (i.e., higher levels).
Using equidistant nodes in a global polynomial setting would quickly lead to severe stability issues, especially near the boundary of the domain, and would therefore deteriorate the quality of the interpolant.
In addition, the interpolation domain is changed to $[-2\pi, 2\pi]^d$, which matches the periodic structure of the cosine function.
After the sparse grid has been generated, the interpolation is evaluated at a prescribed point $\vec{x}$.
Finally, the program prints both the interpolated result and the exact value of the cosine product.

The numerical results for

$$
\vec{x} = \left( \begin{array}{ccc} \pi / 2 & \ldots & \pi / 2 \end{array} \right)^T,
$$

are listed in Table 6.3 below with an exact function value of

$$
f(\vec{x}) = 0.
$$

This point is particularly convenient for verification, since it is simple but still requires the interpolation to resolve the global structure of the cosine function.
Note that $\vec{x}$ cannot equal an exact grid point based on the Chebyshev node definition Equation (2.24) in file [02-sparse-grids.md](02-sparse-grids.md).

The table shows that the interpolant converges rapidly towards the exact value $0$ as the maximum level increases.
For $d=1$, machine precision is already reached at relatively low levels, while for higher dimensions the convergence is slower as we expect from Section 2.3.6 (Interpolation and Quadrature) in file [02-sparse-grids.md](02-sparse-grids.md).

<!-- label: lst:application:interpolation_cos_function:code -->
**Listing 6.2.** Code to interpolate the source function Equation (6.4) above in a point $\vec{x}$.

```cpp
double cosFunction(DataVector& x) {
  double result = 1.0;

  for (size_t dim = 0; dim < x.size(); dim++)
    result *= std::cos(x[dim]);

  return result;
}

int main() {
  /* === 0. Parameters === */
  size_t nDim = ...   // Number of dimensions
  size_t maxLvl = ... // Maximum level of the complete sparse grid
  DataVector point = ... // Interpolation point

  /* === 1. Initialize Source Function === */
  SourceFunc sourceFunc(absSinFunction);

  /* === 2. Configure Sparse Grid Generation Instruction === */
  CompleteSGGenInstr genInstr(maxLvl, nDim);

  // Use Chebyshev nodes of the second kind
  genInstr.setNodeGenFunc(getSecondTypeChebyshevNodeGenFunc());
  // Change the domain to [-2 * pi, 2 * pi]^d
  genInstr.setDomain({-2 * M_PI, 2 * M_PI});

  /* === 3. Generate Sparse Grid === */
  SparseGrid sg(genInstr);

  /* === 4. Interpolate sourceFunc in point === */
  double result = interpolate(sourceFunc, point, sg);

  /* === 5. Output Results === */
  cout << "Result: " << result << endl;
  cout << "Actual value: " << cosFunction(point) << endl;
}
```

<!-- TODO: conversion unclear -->
Note (not in the original prose, added for the machine reader): the listing above calls `SourceFunc sourceFunc(absSinFunction);` even though the function just defined is `cosFunction` — this inconsistency is present verbatim in the thesis source (`chapters/sections/application/interpolation_of_the_classical_cosine_function.tex`) and is preserved here rather than silently corrected.

<!-- label: tab:application:interpolation_cos_function:results -->
**Table 6.3.** Interpolation results of Listing 6.2 above in the point $\vec{x} = \left( \begin{array}{ccc} \pi / 2 & \ldots & \pi / 2 \end{array} \right)^T$ for different number of dimensions $d$ and max. level $l_{max}$ (rounded to 2 decimal places).

| $d \backslash l_{max}$ | 0 | 2 | 4 | 6 | 8 | 10 | $f(\vec{x})$ |
|---|---|---|---|---|---|---|---|
| 1 | 1.00E+00 | 2.06E-02 | 1.34E-16 | -3.92E-16 | -1.88E-16 | -1.10E-16 | 0 |
| 2 | 1.00E+00 | -8.71E-01 | -2.85E-02 | 5.37E-09 | 3.03E-16 | 2.15E-16 | 0 |
| 3 | 1.00E+00 | -1.67E+00 | 7.08E-01 | 3.00E-02 | 8.69E-06 | 1.01E-15 | 0 |

## 6.3 Forward Uncertainty Quantification of an Electron Beam Through One-Dimensional Layers {#sec:application:forward_uq}

We consider a forward uncertainty quantification (UQ) problem adapted from Talbot [Talbot 2016, @talbot2016advanced] for an electron beam passing through a one-dimensional layered medium.
In UQ, the input parameters of a model are not treated as fixed values, but as random variables with prescribed probability distributions.
The resulting output is therefore also random and can no longer be described by a single deterministic value, but rather by statistical quantities such as the expected value or the variance [Talbot 2016, @talbot2016advanced; Rehme, Franzelin & Pflüger 2021, @rehme2021b].

In the forward UQ setting, the uncertainty of the input is propagated through the model in order to determine the induced uncertainty in the output.
For a model $u$ and a random input $X$ with density $\varrho$, one quantity of interest is the expected value

<!-- label: eq:application:forward_uq:expectation -->
$$
\mathbb{E}(u(X)) = \int_{\Omega} u(\vec{x})\,\varrho(\vec{x})\, d\vec{x}. \tag{6.7}
$$

In our framework, this integral is approximated by stochastic collocation on a sparse grid.
More precisely, the original model is replaced by its sparse grid surrogate $\tilde{u}$, and the expected value is computed via numerical quadrature,

<!-- label: eq:application:forward_uq:sparse_grid_expectation -->
$$
\mathbb{E}(u(X)) \approx \mathbb{E}(\tilde{u}(X))
= \int_{\Omega} \tilde{u}(\vec{x})\,\varrho(\vec{x})\, d\vec{x}
\approx \sum_{\vec{i}} \tilde{u}(\vec{x}_{\vec{i}}) \,\varrho(\vec{x}_{\vec{i}}) \, w_{\vec{i}}, \tag{6.8}
$$

where $\vec{x}_{\vec{i}}$ denotes the grid points and $w_{\vec{i}}$ the corresponding quadrature weights.

For the attenuation model, we consider an electron beam that traverses a geometry consisting of $d$ one-dimensional layers.
Each layer contributes an attenuation factor, which is described by the input parameter $x_k$ for layer $k$.
The transmitted beam fraction is modeled by

<!-- label: eq:application:forward_uq:function -->
$$
u(\vec{x}) = \prod_{k=1}^{d} e^{-x_k / d}. \tag{6.9}
$$

An illustration for $d=5$ layers can be seen in Figure 6.4 below.

<!-- label: fig:application:forward_uq:visualization_attenuation_model -->
**Figure 6.4.** Illustration of the attenuation model for five layers (adapted from [Talbot 2016, @talbot2016advanced]).
*Image: `figures/visualization forward uq attenuation model.pdf`*

The input uncertainty is represented in our example by the beta-like density

<!-- label: eq:application:forward_uq:density -->
$$
\varrho(\vec{x}) = \prod_{k=1}^{d} 4(1-x_k)^3, \tag{6.10}
$$

which is defined on the unit hypercube and favors smaller attenuation values.
The quantity approximated by the quadrature rule is therefore the expected transmitted beam intensity under uncertain layer properties.

Since both the model and the density are separable, the analytical solution can be derived in closed form.
Using Equations (6.7), (6.9), and (6.10) above, we obtain

$$
\begin{aligned}
\mathbb{E}(u(X))
&= \int_{[0,1]^d} \prod_{k=1}^{d} \left( 4(1-x_k)^3 e^{-x_k/d} \right) d\vec{x} \\
&= \left( \int_0^1 4(1-x)^3 e^{-x/d} \, dx \right)^d \\
&= \left( 4d - 12d^2 + 24d^3 - 24d^4 + 24d^4 e^{-1/d} \right)^d.
\end{aligned}
$$

This exact value is used as a reference for the numerical approximation.

The implementation in Listing 6.3 below follows the same basic workflow as the previous quadrature example.
First, the source function is implemented as a standalone method.
In contrast to the purely deterministic examples, this method directly combines the physical model and the probability density, so that the quadrature routine integrates the complete UQ integrand in one step.
The sparse grid is generated with equidistant nodes, which yields a Simpson-type quadrature rule in the present setting.
After the grid has been constructed, the quadrature operator is applied to the source function and the sparse grid to compute the expected value.
Finally, the program prints both the numerical result and the analytical reference value.

Table 6.1 below shows the numerical results for different numbers of dimensions $d$ (i.e., layers) and maximum sparse grid levels $l_{max}$.
Even for low levels, the approximation is already close to the exact solution, and the values stabilize quickly as the level increases.
This behavior is consistent with the smooth and separable structure of the integrand, which makes the problem well suited for sparse grid quadrature.

<!-- label: tab:application:forward_uq:results -->
**Table 6.1.** Relative error of the quadrature results produced by Listing 6.3 below for different number of dimensions $d$ and max. level $l_{max}$ (rounded to 2 decimal places).

| $d \backslash l_{max}$ | 0 | 2 | 4 | 6 | 8 | 10 | Exact solution |
|---|---|---|---|---|---|---|---|
| 1 | 4.79E-2 | 2.07E-4 | 8.12E-7 | 3.17E-09 | 1.24E-11 | 4.89E-14 | 0.829107 |
| 2 | 4.12E-2 | 1.16E-4 | 2.52E-7 | 1.95E-10 | 2.33E-12 | 3.62E-14 | 0.824052 |
| 4 | 3.76E-2 | 8.93E-5 | 1.61E-7 | 4.94E-11 | 5.35E-13 | 1.43E-12 | 0.821426 |

<!-- label: lst:application:forward_uq:code -->
**Listing 6.3.** Code to approximate the integral of the source function Equation (6.9) above.

```cpp
double integrant(DataVector& x) {
  double result = 1.0;

  for (size_t dim = 0; dim < x.size(); dim++) {
    result *= 4 * std::pow(1 - x[dim], 3);  // Density rho
    result *= std::exp(-x[dim] / x.size()); // Model u
  }

  return result;
}

double analyticalSolution(size_t nDim) {
  double v = nDim
             - 3 * std::pow(nDim, 2)
             + 6 * std::pow(nDim, 3)
             - 6 * std::pow(nDim, 4)
             + 6 * std::pow(nDim, 4) * std::exp(-1.0 / nDim);

  return std::pow(4 * v, nDim);
}

int main() {
  /* === 0. Parameters === */
  size_t nDim = ...   // Number of dimensions
  size_t maxLvl = ... // Maximum level of the complete sparse grid

  /* === 1. Initialize Source Function === */
  SourceFunc sourceFunc(integrant);

  /* === 2. Configure Sparse Grid Generation Instruction === */
  CompleteSGGenInstr genInstr(maxLvl, nDim);

  // Use equidistant nodes -> Simpson quadrature rule
  genInstr.setNodeGenFunc(getEquidistantNodeGenFunc());

  /* === 3. Generate Sparse Grid === */
  SparseGrid sg(genInstr);

  /* === 4. Apply Quadrature === */
  double result = quadrature(sourceFunc, sg);

  /* === 5. Output Results === */
  cout << "Result: " << result << endl;
  cout << "Analytical solution: " << analyticalSolution(nDim) << endl;
}
```

## References cited in this chapter

- **[Talbot 2016, @talbot2016advanced]** Paul W. Talbot. "Advanced Stochastic Collocation Methods for Polynomial Chaos in RAVEN." PhD thesis, The University of New Mexico, 2016.
- **[Rehme, Franzelin & Pflüger 2021, @rehme2021b]** Michael F. Rehme, Fabian Franzelin, Dirk Pflüger. "B-splines on sparse grids for surrogates in uncertainty quantification." *Reliability Engineering & System Safety*, 209:107430, Elsevier, 2021.
