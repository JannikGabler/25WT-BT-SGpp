/**
 * @file tensor_grid.hpp
 * @brief Anisotropic tensor-product grid used as a building block of the
 * combination technique.
 */
#ifndef COMBIGRID_TENSOR_GRID_HPP
#define COMBIGRID_TENSOR_GRID_HPP

#include <cassert>
#include <iterator>
#include <sgpp/base/datatypes/DataMatrix.hpp>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/combigrid/grids/tensor_grid_iterator.hpp>
#include <sgpp/combigrid/type_defs.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {

/**
 * @brief Anisotropic Cartesian tensor-product grid in fixed dimension.
 *
 * The grid is defined by a per-dimension count of nodes
 * @f$(n_1, \dots, n_d)@f$ and a flat list of 1D node positions per
 * dimension. The total number of grid points is @f$\prod_k n_k@f$ and the
 * grid points themselves are the Cartesian product of the per-dimension
 * node sets. Tensor grids of this form are the elementary components of a
 * combination-technique sparse grid.
 *
 * The dimensionality of a @c TensorGrid is fixed at construction time:
 * copy assignment is intentionally unavailable.
 */
class TensorGrid {
 public:
  /**********
  Constructor
  **********/
  /// @brief Constructs an empty 0-dimensional tensor grid.
  TensorGrid() noexcept;

  /**
   * @brief Constructs a tensor grid with the requested per-dimension grid-point
   * counts. The node positions are left default-initialized and must be
   * filled in afterwards by accessing the underlying storage.
   * @param nGPPerDim Per-dimension number of grid points.
   */
  TensorGrid(const GPMI& nGPPerDim) noexcept;

  /**
   * @brief Constructs a tensor grid by copying the per-dimension grid-point
   * counts and node positions.
   *
   * @param nGPPerDim   Per-dimension number of grid points.
   * @param nodesPerDim Concatenation of per-dimension node lists; must
   * contain @f$\sum_k n_k@f$ entries.
   */
  TensorGrid(const GPMI& nGPPerDim, const base::DataVector& nodesPerDim) noexcept;

  /**
   * @brief Constructs a tensor grid by moving the per-dimension grid-point
   * counts and node positions.
   *
   * @param nGPPerDim   Per-dimension number of grid points (moved-from).
   * @param nodesPerDim Concatenation of per-dimension node lists (moved-from).
   */
  TensorGrid(GPMI&& nGPPerDim, base::DataVector&& nodesPerDim) noexcept;

  /*****
  Getter
  *****/
  /// @brief Returns the dimensionality of the grid.
  size_t nDim() const;

  /// @brief Returns the total number of grid points (the product of per-dimension counts).
  size_t nGP() const;

  /// @brief Returns the per-dimension grid-point counts.
  const GPMI& getGPCntPerDim() const;

  /**
   * @brief Returns the flat list of node positions, concatenated across
   * dimensions.
   *
   * Per-dimension node positions are stored back-to-back. The first
   * @c nGPPerDim[0] entries belong to dimension @c 0, the next
   * @c nGPPerDim[1] entries to dimension @c 1, and so on.
   */
  const base::DataVector& getNodesPerDim() const;

  /**
   * @brief Writes the grid point at linear index @p idx into @p out.
   *
   * Linear indexing follows the row-major (first-dim-fastest) convention
   * used throughout the combigrid module.
   *
   * @param idx Linear grid-point index in @c [0, nGP()).
   * @param out Output vector; must have @c size() >= nDim().
   */
  void getGridPoint(size_t idx, base::DataVector& out) const;

  /**
   * @brief Out-of-place variant of @ref getGridPoint(size_t, base::DataVector&) const.
   * @param idx Linear grid-point index.
   * @return Newly allocated vector with the grid-point coordinates.
   * @note Allocates memory; avoid in hot loops.
   */
  base::DataVector getGridPoint(size_t idx) const;

  /**
   * @brief Writes the grid point identified by the multi-index @p mi into @p out.
   * @param mi  Per-dimension grid-point indices.
   * @param out Output vector; must have @c size() >= nDim().
   */
  void getGridPoint(const GPMI& mi, base::DataVector& out) const;

  /**
   * @brief Out-of-place variant of @ref getGridPoint(const GPMI&, base::DataVector&) const.
   * @param mi Per-dimension grid-point indices.
   * @return Newly allocated vector with the grid-point coordinates.
   * @note Allocates memory; avoid in hot loops.
   */
  base::DataVector getGridPoint(const GPMI& mi) const;

  /**
   * @brief Writes both the grid point at linear index @p idx and its
   * multi-index representation.
   *
   * @param idx   Linear grid-point index.
   * @param outGP Output vector for coordinates (size @c >=nDim()).
   * @param outMI Output multi-index for per-dimension indices (size @c >=nDim()).
   */
  void getGridPointAndMI(size_t idx, base::DataVector& outGP, GPMI& outMI) const;

  /**
   * @brief Out-of-place variant of
   * @ref getGridPointAndMI(size_t, base::DataVector&, GPMI&) const.
   * @param idx Linear grid-point index.
   * @return Pair of (coordinates, multi-index).
   * @note Allocates memory; avoid in hot loops.
   */
  std::pair<base::DataVector, GPMI> getGridPointAndMI(size_t idx) const;

  /**
   * @brief Writes all grid points into @p out, one per row.
   *
   * Row @c i of @p out contains the coordinates of the @c i -th grid point
   * (linear indexing).
   *
   * @param out Output matrix; must have @c >=nGP() rows and @c >=nDim() columns.
   */
  void getGridPoints(base::DataMatrix& out) const;

  /**
   * @brief Out-of-place variant of @ref getGridPoints(base::DataMatrix&) const.
   * @return Newly allocated matrix with all grid points.
   * @note Allocates memory; avoid in hot loops.
   */
  base::DataMatrix getGridPoints() const;

#ifndef SWIG
  /*******
  Iterator
  *******/
  /// @brief Read-only input iterator over the grid points, see @ref begin().
  using const_iterator = TensorGridConstIterator;
  /// @brief Same as @ref const_iterator: iterating a tensor grid never modifies it.
  using iterator = const_iterator;
  /// @brief Input iterator of the range returned by @ref mutableGridPoints().
  using mutable_iterator = TensorGridMutableIterator;

  /**
   * @brief Returns an iterator to the first grid point.
   *
   * Makes the tensor grid a range of its grid points. Each one is yielded
   * together with its multi-index as a @c TensorGridPoint, in the order of the
   * linear grid-point index:
   * @code
   * // const GPMI& mi, const base::DataVector& gp
   * for (const auto& [mi, gp] : tensorGrid) { ... }
   * // point.mi, point.gp
   * for (const TensorGridPoint& point : tensorGrid) { ... }
   * @endcode
   * The grid points are assembled on the fly in storage owned by the
   * iterator. That storage is allocated here, once per loop; advancing does
   * not allocate and only updates the entries that changed. Bind the element
   * by reference: binding it by value copies both containers in every
   * iteration.
   *
   * The element is read-only. Use @ref mutableGridPoints() if the loop body
   * needs to modify the multi-index or the coordinates.
   *
   * The end is marked by a sentinel (see @ref end()), so the @c std::ranges
   * algorithms apply, e.g. <tt>std::ranges::for_each(tensorGrid, f)</tt>.
   */
  const_iterator begin() const { return const_iterator(nGPPerDim, nodesPerDim, nGP_); }
  /// @brief Returns the sentinel that marks the end of the grid points.
  std::default_sentinel_t end() const noexcept { return std::default_sentinel; }
  /// @copydoc begin()
  const_iterator cbegin() const { return begin(); }
  /// @copydoc end()
  std::default_sentinel_t cend() const noexcept { return end(); }

  /**
   * @brief Returns a range of the grid points whose elements may be modified.
   *
   * Like iterating the tensor grid directly, but every element is a working
   * copy whose entries the loop body may overwrite, e.g. to transform the
   * coordinates in place:
   * @code
   * // GPMI& mi, base::DataVector& gp
   * for (auto& [mi, gp] : tensorGrid.mutableGridPoints()) { ... }
   * @endcode
   * The tensor grid itself is not modified, which is why this is available
   * for const tensor grids. Since the iterator has to assume that every entry
   * was altered, advancing rewrites all of them instead of only the changed
   * ones; prefer iterating the tensor grid directly for read-only access.
   *
   * The loop body must not change the number of entries of @c mi or @c gp
   * (see @c TensorGridMutableIterator).
   */
  TensorGridMutableRange mutableGridPoints() const noexcept {
    return TensorGridMutableRange(nGPPerDim, nodesPerDim, nGP_);
  }
#endif

  /*****
  Setter
  *****/
  /*
  May invoke an extension of the internal vector if there is no left over capacity.
  */
  // void setGridPoint(const size_t idx, const base::DataVector& gp);

  // void setGridPoint(const GPMI& mi, const base::DataVector& gp);

  //   bool containsABoundary();

  /********
  Operators
  ********/
  // base::DataVector operator[](size_t idx) const;

  // double operator()(size_t idx, size_t dim) const;
  // double& operator()(size_t idx, size_t dim);

  /**
   * @brief Tests for grid equality.
   *
   * Two tensor grids are equal if they have the same per-dimension counts
   * and (approximately) the same node positions.
   * @param other Tensor grid to compare against.
   * @return @c true if the grids are equal.
   */
  bool operator==(const TensorGrid& other) const;

  /*****************
  Utility operations
  *****************/

  /**
   * @brief Locates the per-dimension neighbors that bracket the query point.
   *
   * For each dimension the function finds the largest grid node @f$\le@f$
   * the query value and the smallest node @f$\ge@f$ it. The resulting
   * pairs can be used directly by piecewise-linear interpolation.
   *
   * Special cases:
   * - The query coordinate equals a grid node: both pair entries are set
   *   to that node.
   * - The query coordinate falls below all grid nodes: the lower neighbor
   *   is set to @c 0.0 and the upper neighbor is the smallest grid node.
   * - The query coordinate exceeds all grid nodes: the lower neighbor is
   *   the largest grid node and the upper neighbor is set to @c 1.0
   *   (sentinel signaling boundary extrapolation).
   *
   * @param point Query point; must have @c >=nDim() entries.
   * @param out   Output vector of @c (lower, upper) pairs (size @c >=nDim()).
   */
  void getNeighborsForLinInterpolation(const base::DataVector& point,
                                       std::vector<std::pair<double, double>>& out) const;

  /**
   * @brief Out-of-place variant of
   * @ref getNeighborsForLinInterpolation(const base::DataVector&, std::vector<std::pair<double,double>>&) const.
   * @param point Query point.
   * @return Newly allocated vector of @c (lower, upper) pairs.
   */
  std::vector<std::pair<double, double>> getNeighborsForLinInterpolation(
      const base::DataVector& point);

 private:
  size_t nGP_;     ///< Total number of grid points (product of @c nGPPerDim).
                   ///< Not const so that the implicitly-declared copy
                   ///< assignment operator is not deleted.
  GPMI nGPPerDim;  ///< Per-dimension grid-point counts. Same comment as @c nGP_.
  base::DataVector nodesPerDim;  ///< Flattened concatenation of per-dimension node lists.
};

}  // namespace combigrid
}  // namespace sgpp

#endif
