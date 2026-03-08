#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sgpp/base/datatypes/DataVector.hpp>
#include <sgpp/base/tools/RandomNumberGenerator.hpp>
#include <type_traits>
#include <vector>

using namespace sgpp::base;

namespace {

static RandomNumberGenerator& randGen = RandomNumberGenerator::getInstance();

template <class Op>
inline double eval_kernel(const Op& op, const DataVector& point) noexcept {
  return op(point);
}

using RuntimeOp = std::function<double(const DataVector&)>;

class TestSourceFunction {
 public:
  template <class Op>
  explicit TestSourceFunction(Op op) : eval_fn_(nullptr) {
    // Funktor wird typ-sicher gespeichert
    static_assert(std::is_copy_constructible<Op>::value, "Functor must be copyable");

    new (&storage_) Op(std::move(op));
    eval_fn_ = &eval_functor<Op>;
    destroy_fn_ = &destroy_functor<Op>;
  }

  // Runtime path (Python / SWIG)
  explicit TestSourceFunction(RuntimeOp op) : eval_fn_(nullptr) {
    new (&storage_) RuntimeOp(std::move(op));
    eval_fn_ = &eval_runtime;
    destroy_fn_ = &destroy_runtime;
  }

  ~TestSourceFunction() { destroy_fn_(&storage_); }

  double eval(const DataVector& point) const { return eval_fn_(&storage_, point); }

 private:
  // --- Storage ---
  using Storage = typename std::aligned_storage<64>::type;

  Storage storage_;

  // --- Dispatch ---
  using EvalFn = double (*)(const void*, const DataVector&);

  using DestroyFn = void (*)(void*);

  EvalFn eval_fn_;
  DestroyFn destroy_fn_;

  // --- Functor path ---
  template <class Op>
  static double eval_functor(const void* s, const DataVector& point) {
    const Op& op = *reinterpret_cast<const Op*>(s);
    return eval_kernel(op, point);  // inline!
  }

  template <class Op>
  static void destroy_functor(void* s) {
    reinterpret_cast<Op*>(s)->~Op();
  }

  // --- Runtime path ---
  static double eval_runtime(const void* s, const DataVector& point) {
    const RuntimeOp& op = *reinterpret_cast<const RuntimeOp*>(s);
    return op(point);
  }

  static void destroy_runtime(void* s) { reinterpret_cast<RuntimeOp*>(s)->~RuntimeOp(); }
};

double func1(const DataVector& point) { return 0; }

double func2(const DataVector& point) {
  double result = 0;

  for (const double v : point) {
    result += v * v;
  }

  return std::sqrt(result);
}

std::vector<DataVector> randomPoints(const size_t n, const size_t nDim) {
  std::vector<DataVector> points(n);

  for (size_t i = 0; i < n; i++) {
    DataVector vec(nDim);

    for (size_t dim = 0; dim < nDim; dim++) {
      vec[dim] = randGen.getUniformRN(0, 100);
    }
    points[i] = vec;
  }

  return points;
}

void benchmarkFunction(const std::string& label, const TestSourceFunction& f,
                       const std::vector<DataVector>& points, int warmupLoops = 3,
                       int measureRuns = 5, int evalsPerPoint = 10) {
  std::cout << "Benchmark: " << label << "\n";
  std::cout << " points: " << points.size() << ", warmupLoops: " << warmupLoops
            << ", measureRuns: " << measureRuns << ", evalsPerPoint: " << evalsPerPoint << "\n";

  // Warmup: ungetimte Evaluationen (hilft ggf. Caches/JIT/branch-predictor)
  volatile double warmupSink = 0.0;  // volatile damit nicht optimiert
  for (int w = 0; w < warmupLoops; ++w) {
    for (int r = 0; r < evalsPerPoint; ++r) {
      for (const auto& pt : points) {
        // evalsPerPoint zur Erhöhung der Belastung auch im Warmup
        warmupSink += f.eval(pt);
      }
    }
  }
  (void)warmupSink;  // nur um Verwendung zu zeigen

  // Messungen
  std::vector<double> runTimesSec;
  runTimesSec.reserve(measureRuns);
  volatile double resultSink =
      0.0;  // Akkumuliere Ergebnisse, damit der Compiler nicht alles weglässt

  for (int run = 0; run < measureRuns; ++run) {
    auto t0 = std::chrono::high_resolution_clock::now();
    // innerer Loop: evalsPerPoint mal pro Punkt ausführen
    for (int r = 0; r < evalsPerPoint; ++r) {
      for (const auto& pt : points) {
        resultSink += f.eval(pt);
      }
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> dur = t1 - t0;
    runTimesSec.push_back(dur.count());
    // Kleine Zwischeninfo (optional)
    std::cout << "  run " << (run + 1) << " time: " << std::fixed << std::setprecision(6)
              << dur.count() << " s\n";
  }

  // Ergebnisverarbeitung
  double totalTime = 0.0;
  for (double s : runTimesSec) totalTime += s;
  double avgTime = totalTime / static_cast<double>(runTimesSec.size());

  std::uint64_t totalEvaluations = static_cast<std::uint64_t>(points.size()) *
                                   static_cast<std::uint64_t>(evalsPerPoint) *
                                   static_cast<std::uint64_t>(measureRuns);

  // Durchschnitt pro einzelne evaluate()-Aufruf (aus allen Messungen gemittelt)
  double avgPerEvalSec =
      avgTime / (static_cast<double>(points.size()) * static_cast<double>(evalsPerPoint));
  double avgPerEvalNs = avgPerEvalSec * 1e9;

  // Durchsatz: Evaluierungen pro Sekunde
  double evalsPerSec =
      (static_cast<double>(points.size()) * static_cast<double>(evalsPerPoint)) / avgTime;

  std::cout << std::fixed << std::setprecision(3);
  std::cout << "--> Average run time: " << avgTime << " s\n";
  std::cout << "    Avg time per evaluate() : " << avgPerEvalNs << " ns\n";
  std::cout << "    Throughput (evals/sec)  : " << evalsPerSec << " evaluations/s\n";
  std::cout << "    Total evaluations measured (across all runs): "
            << (static_cast<std::uint64_t>(points.size()) *
                static_cast<std::uint64_t>(evalsPerPoint) * static_cast<std::uint64_t>(measureRuns))
            << "\n";

  // Verwende resultSink so, dass die Summen nicht entfernt werden (sie werden nicht weiter genutzt,
  // aber wir geben einen kleinen Hash zur Kontrolle aus).
  double checksum = std::fmod(resultSink, 1e9);
  std::cout << "    checksum (mod 1e9) to prevent optimizer removal: " << checksum << "\n\n";
}

}  // namespace

int main() {
  constexpr size_t nPoints = 100000;
  constexpr size_t nDim = 16;
  constexpr size_t warmupLoops = 3;
  constexpr size_t measureRuns = 5;
  constexpr size_t evalsPerPoint = 1000;  // wie oft jeder Punkt pro run evaluiert wird

  const TestSourceFunction staticFunc1(func1);
  const TestSourceFunction staticFunc2(func2);

  double randValue = randGen.getUniformRN(0, 100);
  const TestSourceFunction dynamicFunc1([randValue](const DataVector& point) { return randValue; });
  const TestSourceFunction dynamicFunc2([](const DataVector& point) {
    double result = 0;

    for (const double v : point) {
      result += v * v;
    }

    return std::sqrt(result);
  });

  const std::vector<DataVector> points = randomPoints(nPoints, nDim);

  benchmarkFunction("staticFunc1 (func1)", staticFunc1, points, warmupLoops, measureRuns,
                    evalsPerPoint);
  benchmarkFunction("staticFunc2 (func2)", staticFunc2, points, warmupLoops, measureRuns,
                    evalsPerPoint);
  benchmarkFunction("dynamicFunc1 (lambda returns 0)", dynamicFunc1, points, warmupLoops,
                    measureRuns, evalsPerPoint);
  benchmarkFunction("dynamicFunc2 (lambda sqrt of sumsquares)", dynamicFunc2, points, warmupLoops,
                    measureRuns, evalsPerPoint);

  return 0;
}