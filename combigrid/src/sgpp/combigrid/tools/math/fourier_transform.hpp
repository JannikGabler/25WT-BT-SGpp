#pragma once

#include <complex>
#include <sgpp/combigrid/tools/math/fft_internals/fft_dispatch.hpp>
#include <vector>
#include "sgpp/combigrid/tools/math/fft_internals/real_fft.hpp"

namespace sgpp {
namespace combigrid {
namespace tools {

/*
Complex Fast Fourier Transform (inplace)
*/
template <typename T>
void fft(std::vector<std::complex<T>>& vec) {
  fourier_transform::complexFFTDispatch(vec, false);
}

/*
Complex Fast Fourier Transform
*/
template <typename T>
std::vector<std::complex<T>> fft(const std::vector<std::complex<T>>& vec) {
  std::vector<std::complex<T>> result(vec);

  fourier_transform::complexFFTDispatch(vec, false);

  return result;
}

// /*
// Real Fast Fourier Transform
// */
// template <typename T>
// std::vector<T> fftReal(const std::vector<T>& vec) {
//   return fourier_transform::realFFTForward(vec);
// }

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp