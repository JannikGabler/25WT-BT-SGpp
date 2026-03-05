#pragma once

#include <complex>
#include <sgpp/combigrid/tools/math/fft_internals/fft_dispatch.hpp>
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

template <typename T>
std::vector<std::complex<T>> realFFTForward(const std::vector<T>& x) {
  const size_t n = x.size();

  if (n == 0) return {};
  if (n % 2 != 0) {
    // For odd lengths, default to complex path: construct complex input and run generic FFT
    std::vector<std::complex<T>> cx(n);
    for (size_t i = 0; i < n; ++i) cx[i] = std::complex<T>(x[i], 0);
    complexFFTDispatch<T>(cx, false);
    return cx;
  }
  const size_t n2 = n / 2;

  // pack
  std::vector<std::complex<T>> a(n2);
  for (size_t k = 0; k < n2; ++k) {
    a[k] = std::complex<T>(x[2 * k], x[2 * k + 1]);
  }

  // A = FFT(a)
  complexFFTDispatch<T>(a, false);

  // Reconstruct full X[0..N-1] (complex spectrum)
  std::vector<std::complex<T>> X(n);
  const T PI = static_cast<T>(3.14159265358979323846264338327950288L);

  for (size_t k = 0; k < n2; ++k) {
    size_t k_conj = (n2 - k) % n2;
    std::complex<T> Ak = a[k];
    std::complex<T> Akc = std::conj(a[k_conj]);

    // compute W_k = exp(-2pi i k / N)
    T angle = -2 * PI * static_cast<T>(k) / static_cast<T>(n);
    std::complex<T> Wk = std::polar(static_cast<T>(1), angle);

    // S = 0.5*(Ak + Akc)
    // D = 0.5*(Ak - Akc)
    std::complex<T> S = std::complex<T>(0.5) * (Ak + Akc);
    std::complex<T> D = std::complex<T>(0.5) * (Ak - Akc);

    // X[k] = S - i * Wk * D
    // X[k + n2] = S + i * Wk * D
    std::complex<T> iD = std::complex<T>(-D.imag(), D.real());  // i * D
    // Careful: i * D = complex( -Im(D), Re(D) )
    std::complex<T> term = Wk * D;
    std::complex<T> i_term = std::complex<T>(-term.imag(), term.real());

    X[k] = S + i_term * static_cast<T>(-1.0) + std::complex<T>(0, 0);  // S - i*Wk*D
    X[k + n2] = S + i_term;                                            // S + i*Wk*D
    // The previous two lines implement the same formula in a stable manner.
  }

  return X;
}

/*
Inverse real FFT: given complex spectrum X[0..N-1] with Hermitian symmetry produce real x[0..N-1]
*/
template <typename T>
std::vector<T> realFFTInverse(const std::vector<std::complex<T>>& X) {
  const size_t N = X.size();
  if (N == 0) return {};
  if (N % 2 != 0) {
    // fallback: perform inverse complex FFT and take real parts
    auto cx = X;
    complexFFTDispatch<T>(cx, true);
    std::vector<T> x(N);
    for (size_t i = 0; i < N; ++i) x[i] = cx[i].real();
    return x;
  }
  const size_t n2 = N / 2;

  // Reconstruct A[k] for k=0..n2-1 (complex spectrum of half-length packed sequence)
  std::vector<std::complex<T>> A(n2);
  const T PI = static_cast<T>(3.14159265358979323846264338327950288L);

  for (size_t k = 0; k < n2; ++k) {
    // Xk = X[k], Xk2 = X[k + n2]
    const std::complex<T> Xk = X[k];
    const std::complex<T> Xk2 = X[k + n2];

    // Wk = exp(2pi i k / N)  (note inverse of forward W)
    const T angle = 2 * PI * static_cast<T>(k) / static_cast<T>(N);
    const std::complex<T> Wk = std::polar(static_cast<T>(1), angle);

    // Solve for S and D:
    // S = Xk + Xk2
    // (Xk2 - Xk) = i * Wk * (S_diff)   => S_diff = -i * Wk^{-1} * (Xk2 - Xk)
    const std::complex<T> S = Xk + Xk2;
    const std::complex<T> diff = Xk2 - Xk;
    const std::complex<T> Winv = std::polar(static_cast<T>(1), angle);  // Wk^{-1}
    // -i * Winv * diff = ( -i ) * Winv * diff
    const std::complex<T> minus_i(0, -1);
    const std::complex<T> Sdiff = minus_i * (Winv * diff);

    // A = 0.5*(S + Sdiff)
    A[k] = std::complex<T>(0.5) * (S + Sdiff);
  }

  // inverse FFT on A to get a[k] = x[2k] + i*x[2k+1]
  complexFFTDispatch<T>(A, true);

  // unpack
  std::vector<T> x(N);
  for (size_t k = 0; k < n2; ++k) {
    x[2 * k] = A[k].real();
    x[2 * k + 1] = A[k].imag();
  }
  return x;
}

}  // namespace fourier_transform

}  // namespace tools
}  // namespace combigrid
}  // namespace sgpp