#pragma once

#include <complex>
#include <sgpp/combigrid/tools/math/fft_internals/fft_dispatch.hpp>
#include <sgpp/combigrid/tools/math/fft_internals/real_fft.hpp>
#include <vector>

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
Real Inverse Fast Fourier Transform (inplace)
*/
template <typename T>
void ifftReal(std::vector<T>& vec) {
  fourier_transform::realFFTInverse(vec);
}

/*
Real Inverse Fast Fourier Transform
*/
template <typename T>
std::vector<T> ifftReal(const std::vector<T>& vec) {
  std::vector<T> result(vec);

  fourier_transform::realFFTInverse(vec);

  return result;
}

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp
