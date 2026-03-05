// // Copyright (C) 2008-today The SG++ project
// // This file is part of the SG++ project. For conditions of distribution and
// // use, please see the copyright notice provided with SG++ or at
// // sgpp.sparsegrids.org
// #define BOOST_TEST_DYN_LINK

// #include <boost/test/tools/old/interface.hpp>
// #include <boost/test/unit_test.hpp>
// #include <boost/test/unit_test_log.hpp>
// #include <boost/test/unit_test_suite.hpp>

// #include <complex>
// #include <sgpp/base/tools/RandomNumberGenerator.hpp>
// #include <sgpp/combigrid/constants.hpp>
// #include <sgpp/combigrid/tools/combitech_coefficients.hpp>
// #include <sgpp/combigrid/tools/comparison/nearly_equal.hpp>
// #include <sgpp/combigrid/tools/math/fourier_transform.hpp>
// #include <sgpp/combigrid/tools/math/inverse_fourier_transform.hpp>
// #include
// <sgpp/combigrid/tools/sparse_grid_generation_instructions/full_sparse_grid_generation.hpp>
// #include <sgpp/combigrid/type_defs.hpp>
// #include <vector>

// using namespace sgpp::combigrid;

// static sgpp::base::RandomNumberGenerator& randGen =
//     sgpp::base::RandomNumberGenerator::getInstance();

// namespace {

// template <typename T>
// void checkVectorsClose(const std::vector<T>& a, const std::vector<T>& b, const T eps = 1e-06) {
//   BOOST_REQUIRE_EQUAL(a.size(), b.size());
//   for (size_t i = 0; i < a.size(); ++i) {
//     BOOST_CHECK_SMALL(std::abs(a[i] - b[i]), eps);
//   }
// }

// template <typename T>
// void checkVectorsClose(const std::vector<std::complex<T>>& a, const std::vector<std::complex<T>>&
// b,
//                        const T eps = 1e-06) {
//   BOOST_REQUIRE_EQUAL(a.size(), b.size());

//   for (size_t i = 0; i < a.size(); ++i) {
//     BOOST_CHECK_SMALL(std::abs(a[i] - b[i]), eps);
//   }
// }

// }  // namespace

// BOOST_AUTO_TEST_SUITE(complex_numbers)

// BOOST_AUTO_TEST_CASE(fft_delta_signal) {
//   using cplx = std::complex<double>;

//   std::vector<cplx> vec = {{1, 0}, {0, 0}, {0, 0}, {0, 0}};

//   tools::fft(vec);

//   for (const auto& v : vec) {
//     BOOST_CHECK_SMALL(std::abs(v - cplx(1, 0)), 1e-06);
//   }
// }

// BOOST_AUTO_TEST_CASE(fft_constant_signal) {
//   using cplx = std::complex<double>;

//   std::vector<cplx> vec = {{1, 0}, {1, 0}, {1, 0}, {1, 0}};

//   tools::fft(vec);

//   BOOST_CHECK_SMALL(std::abs(vec[0] - cplx(4, 0)), 1e-06);

//   for (size_t i = 1; i < vec.size(); ++i) {
//     BOOST_CHECK_SMALL(std::abs(vec[i]), 1e-06);
//   }
// }

// BOOST_AUTO_TEST_CASE(fft_ifft_identity) {
//   using cplx = std::complex<double>;

//   const std::vector<cplx> original = {{1.3, -2.1}, {0.7, 3.4}, {-4.2, 0.5}, {2.0, -1.0}};

//   auto vec = original;

//   tools::fft(vec);
//   tools::ifft(vec);

//   checkVectorsClose(vec, original);
// }

// BOOST_AUTO_TEST_CASE(fft_linearity) {
//   using cplx = std::complex<double>;

//   const std::vector<cplx> a = {{1, 0}, {2, 0}, {3, 0}, {4, 0}};
//   const std::vector<cplx> b = {{5, 0}, {6, 0}, {7, 0}, {8, 0}};

//   auto sum = a;

//   for (size_t i = 0; i < sum.size(); ++i) sum[i] += b[i];

//   auto fft_a = a;
//   auto fft_b = b;
//   auto fft_sum = sum;

//   tools::fft(fft_a);
//   tools::fft(fft_b);
//   tools::fft(fft_sum);

//   for (size_t i = 0; i < fft_sum.size(); ++i) {
//     BOOST_CHECK_SMALL(std::abs(fft_sum[i] - (fft_a[i] + fft_b[i])), 1e-6);
//   }
// }

// BOOST_AUTO_TEST_CASE(fft_real_signal_symmetry) {
//   using cplx = std::complex<double>;

//   std::vector<cplx> vec = {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}, {8, 0}};

//   tools::fft(vec);

//   size_t N = vec.size();

//   for (size_t k = 1; k < N / 2; k++) {
//     auto lhs = vec[k];
//     auto rhs = std::conj(vec[N - k]);

//     BOOST_CHECK_SMALL(std::abs(lhs - rhs), 1e-6);
//   }
// }

// BOOST_AUTO_TEST_CASE(fft_large_roundtrip) {
//   using cplx = std::complex<double>;

//   const size_t N = 64;

//   std::vector<cplx> vec(N);

//   for (size_t i = 0; i < N; i++) vec[i] = cplx(std::sin(i), std::cos(i));

//   auto original = vec;

//   tools::fft(vec);
//   tools::ifft(vec);

//   checkVectorsClose(vec, original, 1e-8);
// }

// BOOST_AUTO_TEST_CASE(fft_ifft_power_of_two) {
//   using cplx = std::complex<double>;

//   const size_t N = 16;  // Power of two
//   std::vector<cplx> original(N);

//   for (size_t i = 0; i < N; ++i) original[i] = cplx(std::sin(i), std::cos(i));

//   auto vec = original;

//   tools::fft(vec);
//   tools::ifft(vec);

//   checkVectorsClose(vec, original, 1e-9);
// }

// BOOST_AUTO_TEST_CASE(fft_ifft_non_power_of_two) {
//   using cplx = std::complex<double>;

//   const size_t N = 15;  // Not a power of two
//   std::vector<cplx> original(N);

//   for (size_t i = 0; i < N; ++i) original[i] = cplx(std::sin(0.1 * i), std::cos(0.3 * i));

//   auto vec = original;

//   tools::fft(vec);
//   tools::ifft(vec);

//   checkVectorsClose(vec, original, 1e-9);
// }

// BOOST_AUTO_TEST_CASE(fft_non_power_of_two_known_result) {
//   using cplx = std::complex<double>;

//   std::vector<cplx> vec = {{1, 0}, {2, 0}, {3, 0}};

//   tools::fft(vec);

//   std::vector<cplx> expected = {
//       {6, 0},                 // X[0] = 1+2+3
//       {-1.5, 0.86602540378},  // X[1] = 1 + 2*exp(-2πi/3) + 3*exp(-4πi/3)
//       {-1.5, -0.86602540378}  // X[2] = conjugate
//   };

//   checkVectorsClose(vec, expected, 1e-9);
// }

// BOOST_AUTO_TEST_SUITE_END()

// // BOOST_AUTO_TEST_SUITE(real_numbers)

// // BOOST_AUTO_TEST_CASE(fft_delta_signal) {
// //   const std::vector<double> vec = {1, 0, 0, 0};
// //   const auto fft_vec = tools::fftReal(vec);

// //   // FFT einer Delta-Funktion = alle 1
// //   for (const auto& v : fft_vec) {
// //     BOOST_CHECK_SMALL(std::abs(v - std::complex<double>(1, 0)), 1e-6);
// //   }
// // }

// // BOOST_AUTO_TEST_CASE(fft_constant_signal) {
// //   const std::vector<double> vec = {2, 2, 2, 2};
// //   const auto fft_vec = tools::fftReal(vec);

// //   BOOST_CHECK_SMALL(std::abs(fft_vec[0] - std::complex<double>(8, 0)), 1e-6);

// //   for (size_t i = 1; i < fft_vec.size(); ++i) {
// //     BOOST_CHECK_SMALL(std::abs(fft_vec[i]), 1e-6);
// //   }
// // }

// // BOOST_AUTO_TEST_CASE(fft_ifft_identity) {
// //   const std::vector<double> original = {1.5, -2.0, 0.7, 3.3};

// //   const auto fft_vec = tools::fftReal(original);
// //   const auto ifft_vec = tools::ifftReal(fft_vec);

// //   checkVectorsClose(ifft_vec, original, 1e-8);
// // }

// // BOOST_AUTO_TEST_CASE(fft_linearity) {
// //   const std::vector<double> a = {1, 2, 3, 4};
// //   const std::vector<double> b = {5, 6, 7, 8};

// //   std::vector<double> sum(a.size());
// //   for (size_t i = 0; i < a.size(); ++i) sum[i] = a[i] + b[i];

// //   const auto fft_a = tools::fftReal(a);
// //   const auto fft_b = tools::fftReal(b);
// //   const auto fft_sum = tools::fftReal(sum);

// //   for (size_t i = 0; i < fft_sum.size(); ++i) {
// //     BOOST_CHECK_SMALL(std::abs(fft_sum[i] - (fft_a[i] + fft_b[i])), 1e-6);
// //   }
// // }

// // BOOST_AUTO_TEST_CASE(fft_symmetry) {
// //   const std::vector<double> vec = {1, 2, 3, 4, 5, 6, 7, 8};
// //   const auto fft_vec = tools::fftReal(vec);

// //   size_t N = fft_vec.size();
// //   for (size_t k = 1; k < N / 2; ++k) {
// //     auto lhs = fft_vec[k];
// //     auto rhs = std::conj(fft_vec[N - k]);
// //     BOOST_CHECK_SMALL(std::abs(lhs - rhs), 1e-6);
// //   }
// // }

// // BOOST_AUTO_TEST_CASE(fft_large_roundtrip) {
// //   const size_t N = 64;
// //   std::vector<double> vec(N);
// //   for (size_t i = 0; i < N; ++i) vec[i] = std::sin(i) + 0.5 * std::cos(2 * i);

// //   const auto original = vec;
// //   const auto fft_vec = tools::fftReal(vec);
// //   const auto ifft_vec = tools::ifftReal(fft_vec);

// //   checkVectorsClose(ifft_vec, original, 1e-8);
// // }

// // BOOST_AUTO_TEST_CASE(fft_known_result) {
// //   std::vector<double> vec = {1, 2, 3};
// //   auto fft_vec = tools::fftReal(vec);

// //   std::vector<std::complex<double>> expected = {
// //       {6, 0}, {-1.5, 0.86602540378}, {-1.5, -0.86602540378}};

// //   checkVectorsClose(fft_vec, expected, 1e-9);
// // }

// // BOOST_AUTO_TEST_CASE(fft_ifft_identity_power_of_two) {
// //   std::vector<double> vec = {0.1, -0.2, 0.3, -0.4, 0.5, -0.6, 0.7, -0.8};
// //   const auto fft_vec = tools::fftReal(vec);
// //   const auto ifft_vec = tools::ifftReal(fft_vec);

// //   checkVectorsClose(ifft_vec, vec, 1e-8);
// // }

// // BOOST_AUTO_TEST_CASE(fft_random_power_of_two) {
// //   const size_t N = 16;
// //   std::vector<double> vec(N);
// //   for (size_t i = 0; i < N; ++i) vec[i] = std::sin(i) + std::cos(2 * i);

// //   const auto fft_vec = tools::fftReal(vec);
// //   const auto ifft_vec = tools::ifftReal(fft_vec);

// //   checkVectorsClose(ifft_vec, vec, 1e-8);
// // }

// // BOOST_AUTO_TEST_CASE(fft_ifft_identity_non_power_of_two) {
// //   std::vector<double> vec = {1.0, -1.0, 0.5, -0.5, 2.0};
// //   const auto fft_vec = tools::fftReal(vec);
// //   const auto ifft_vec = tools::ifftReal(fft_vec);

// //   checkVectorsClose(ifft_vec, vec, 1e-8);
// // }

// // BOOST_AUTO_TEST_CASE(fft_random_non_power_of_two) {
// //   const size_t N = 7;
// //   std::vector<double> vec(N);
// //   for (size_t i = 0; i < N; ++i) vec[i] = std::sin(i) + std::cos(0.5 * i);

// //   const auto fft_vec = tools::fftReal(vec);
// //   const auto ifft_vec = tools::ifftReal(fft_vec);

// //   checkVectorsClose(ifft_vec, vec, 1e-8);
// // }

// // BOOST_AUTO_TEST_CASE(fft_empty_vector) {
// //   std::vector<double> vec;
// //   const auto fft_vec = tools::fftReal(vec);
// //   BOOST_CHECK(fft_vec.empty());
// // }

// // BOOST_AUTO_TEST_CASE(fft_single_element) {
// //   std::vector<double> vec = {42.0};
// //   const auto fft_vec = tools::fftReal(vec);
// //   BOOST_CHECK_EQUAL(fft_vec.size(), 1);
// //   BOOST_CHECK_SMALL(std::abs(fft_vec[0] - std::complex<double>(42.0, 0.0)), 1e-12);

// //   const auto ifft_vec = tools::ifftReal(fft_vec);
// //   checkVectorsClose(ifft_vec, vec, 1e-12);
// // }

// // BOOST_AUTO_TEST_CASE(fft_two_elements) {
// //   std::vector<double> vec = {1.0, -1.0};
// //   const auto fft_vec = tools::fftReal(vec);

// //   std::vector<std::complex<double>> expected = {{0, 0}, {2, 0}};
// //   // Achtung: abhängig von Implementierung, ggf. Vorzeichen prüfen
// //   BOOST_CHECK_CLOSE(std::abs(fft_vec[0]), std::abs(expected[0]), 1e-12);
// //   BOOST_CHECK_CLOSE(std::abs(fft_vec[1]), std::abs(expected[1]), 1e-12);
// // }

// // BOOST_AUTO_TEST_CASE(fft_large_non_power_of_two) {
// //   const size_t N = 15;
// //   std::vector<double> vec(N);
// //   for (size_t i = 0; i < N; ++i) vec[i] = static_cast<double>(i % 3) - 1;

// //   const auto fft_vec = tools::fftReal(vec);
// //   const auto ifft_vec = tools::ifftReal(fft_vec);

// //   checkVectorsClose(ifft_vec, vec, 1e-8);
// // }

// // BOOST_AUTO_TEST_SUITE_END()