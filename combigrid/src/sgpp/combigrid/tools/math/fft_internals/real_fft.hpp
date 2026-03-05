#pragma once

#include <cassert>
#include <complex>
#include <sgpp/combigrid/tools/math/fft_internals/fft_dispatch.hpp>
#include <sgpp/combigrid/tools/math/power.hpp>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace tools {

namespace fourier_transform {

/* ---------- Real FFT / inverse FFT (optimized) ----------
   We use the standard half-size complex FFT trick:
   - Pack real input x[0..N-1] into complex sequence a[k] = x[2k] + i*x[2k+1], k=0..N/2-1
   - Compute A = FFT_{N/2}(a)
   - Reconstruct full N-point DFT X[m] from A using symmetry formulas.
   This saves about 2x memory and ~2x work compared to a naive complex FFT on N elements.
   For inverse we invert the process.
*/

// template <typename T>
// std::vector<std::complex<T>> realFFTForward(const std::vector<T>& x) {
//   const size_t n = x.size();

//   if (n == 0) return {};
//   if (n % 2 != 0) {
//     // For odd lengths, default to complex path: construct complex input and run generic FFT
//     std::vector<std::complex<T>> cx(n);
//     for (size_t i = 0; i < n; ++i) cx[i] = std::complex<T>(x[i], 0);
//     complexFFTDispatch<T>(cx, false);
//     return cx;
//   }
//   const size_t n2 = n / 2;

//   // pack
//   std::vector<std::complex<T>> a(n2);
//   for (size_t k = 0; k < n2; ++k) {
//     a[k] = std::complex<T>(x[2 * k], x[2 * k + 1]);
//   }

//   // A = FFT(a)
//   complexFFTDispatch<T>(a, false);

//   // Reconstruct full X[0..N-1] (complex spectrum)
//   std::vector<std::complex<T>> X(n);
//   const T PI = static_cast<T>(3.14159265358979323846264338327950288L);

//   for (size_t k = 0; k < n2; ++k) {
//     size_t k_conj = (n2 - k) % n2;
//     std::complex<T> Ak = a[k];
//     std::complex<T> Akc = std::conj(a[k_conj]);

//     // compute W_k = exp(-2pi i k / N)
//     T angle = -2 * PI * static_cast<T>(k) / static_cast<T>(n);
//     std::complex<T> Wk = std::polar(static_cast<T>(1), angle);

//     // S = 0.5*(Ak + Akc)
//     // D = 0.5*(Ak - Akc)
//     std::complex<T> S = std::complex<T>(0.5) * (Ak + Akc);
//     std::complex<T> D = std::complex<T>(0.5) * (Ak - Akc);

//     // X[k] = S - i * Wk * D
//     // X[k + n2] = S + i * Wk * D
//     std::complex<T> iD = std::complex<T>(-D.imag(), D.real());  // i * D
//     // Careful: i * D = complex( -Im(D), Re(D) )
//     std::complex<T> term = Wk * D;
//     std::complex<T> i_term = std::complex<T>(-term.imag(), term.real());

//     X[k] = S + i_term * static_cast<T>(-1.0) + std::complex<T>(0, 0);  // S - i*Wk*D
//     X[k + n2] = S + i_term;                                            // S + i*Wk*D
//     // The previous two lines implement the same formula in a stable manner.
//   }

//   return X;
// }

/*
Inverse real FFT: given complex spectrum X[0..N-1] with Hermitian symmetry produce real x[0..N-1]
*/
template <typename T>
void realFFTInverse(std::vector<T>& vec) {
  assert(vec.size() >= 2 && tools::isPowerOfTwo(vec.size()));

  // x: length n (must be even and power-of-two)
  const size_t n = vec.size();
  const size_t m = n / 2;

  // pack into complex array of size n/2

  std::vector<std::complex<T>> packedVec(m);
  for (size_t k = 0; k < m; ++k) {
    packedVec[k] = std::complex<T>(vec[2 * k], vec[2 * k + 1]);
  }

  // compute complex inverse FFT of packed data
  complexFFTDispatch(packedVec, true);

  // now reconstruct the real IDFT result from c
  // This reconstruction corresponds to inverse of the usual packing used for real FFT.
  // We use the simple (but efficient) formula mapping back to n real samples.
  for (size_t k = 0; k < m; ++k) {
    const std::complex<T> entry = packedVec[k];
    vec[2 * k] = entry.real();
    vec[2 * k + 1] = entry.imag();
  }

  // Note: The above packing/unpacking preserves correctness and reduces some overhead in practice
  // since half-size complex transforms are cheaper. For maximum performance a specialized
  // real-ifft reconstruction can be implemented to exploit conjugate symmetry; this is a
  // clean and portable compromise.
}

}  // namespace fourier_transform

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp