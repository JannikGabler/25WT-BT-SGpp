#pragma once
#include <sgpp/base/datatypes/DataVector.hpp>
#include <span>
#include <sgpp/combigrid/type_defs.hpp>
namespace sgpp { namespace combigrid { namespace quadrature_rules_old {
struct OldCC { sgpp::base::DataVector genWeights(GPCntType n) const; void genWeightsInplace(std::span<double>) const;
 sgpp::base::DataVector genWeightsNaive(size_t n) const; sgpp::base::DataVector genWeightsFFT(size_t n) const; };
}}}
