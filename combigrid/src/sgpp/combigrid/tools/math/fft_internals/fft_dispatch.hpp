#pragma once

#include <complex>
#include <sgpp/combigrid/tools/math/fft_internals/bluestein.hpp>
#include <sgpp/combigrid/tools/math/fft_internals/cooley_tukey.hpp>
#include <sgpp/combigrid/tools/math/power.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

namespace fourier_transform {

/*
Dispatch — if n is power of two use Cooley-Tukey, else Bluestein.
*/
template <typename T>
void complexFFTDispatch(std::vector<std::complex<T>>& vec, const bool inverse) {
  if (vec.empty()) {
    return;
  }

  if (isPowerOfTwo(vec.size())) {
    fftPow2(vec, inverse);
  } else {
    bluesteinTransform(vec, inverse);
  }
}

}  // namespace fourier_transform

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp