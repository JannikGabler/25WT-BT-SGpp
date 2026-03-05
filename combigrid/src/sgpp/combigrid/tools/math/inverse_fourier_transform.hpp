#pragma once

#include <complex>
#include <sgpp/combigrid/tools/math/fft_internals/fft_dispatch.hpp>
#include <vector>
#include "sgpp/combigrid/tools/math/fft_internals/real_fft.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

/*
Complex Inverse Fast Fourier Transform (inplace)
*/
template <typename T>
void ifft(std::vector<std::complex<T>>& vec) {
  fourier_transform::complexFFTDispatch(vec, true);
}

/*
Complex Inverse Fast Fourier Transform
*/
template <typename T>
std::vector<std::complex<T>> ifft(const std::vector<std::complex<T>>& vec) {
  std::vector<std::complex<T>> result(vec);

  fourier_transform::complexFFTDispatch(vec, true);

  return result;
}

/*
Real Inverse Fast Fourier Transform
*/
template <typename T>
std::vector<T> ifftReal(const std::vector<std::complex<T>>& vec) {
  return fourier_transform::realFFTInverse(vec);
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
