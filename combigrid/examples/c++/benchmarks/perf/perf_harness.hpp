// Copyright (C) 2008-today The SG++ project
// This file is part of the SG++ project. For conditions of distribution and
// use, please see the copyright notice provided with SG++ or at
// sgpp.sparsegrids.org

/**
 * @file perf_harness.hpp
 * @brief Dependency-free benchmark harness of the combigrid performance suite.
 *
 * PROTECTED FILE (see combigrid/perf/README.md): changing it changes how every benchmark of the
 * optimization phase is measured. Changes only on explicit instruction of the project owner.
 *
 * Two kinds of cases are supported:
 * - @b Pipeline cases (end-to-end and phase level): the case function is executed once per sample
 *   and times its phases via @ref PipelineRun::phase. The total wall time of the case function is
 *   recorded as metric @c total.
 * - @b Micro cases: the case function prepares its data and passes the body to be measured to
 *   @ref MicroRun::measure, which calibrates a batch size (body calls per sample) such that a
 *   sample takes at least @c --min-sample-time seconds, and records the time per body call.
 *
 * Every executable built on this harness accepts:
 * @code
 *   --list                 list the case names and exit
 *   --filter REGEX         run only the cases whose name matches REGEX (ECMAScript, search)
 *   --samples N            measured samples per case
 *   --warmups N            unmeasured warm-up executions per case
 *   --min-sample-time S    micro cases: minimum duration of one sample in seconds
 *   --threads N            OpenMP thread count (default: OpenMP default / OMP_NUM_THREADS)
 *   --json                 one JSON object per case on stdout (instead of a table)
 *   --build-info           print the compile-time configuration as JSON and exit
 * @endcode
 * Without arguments the case list is printed (so that running the examples via SCons is cheap).
 */
#pragma once

#include <omp.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace sgpp {
namespace combigrid {
namespace perf {

/// @brief Forces the compiler to materialize @p value (prevents dead-code elimination).
template <typename T>
inline void doNotOptimize(const T& value) {
  asm volatile("" : : "r,m"(value) : "memory");
}

/// @brief Forces the compiler to materialize @p value and to assume it was modified.
template <typename T>
inline void doNotOptimize(T& value) {
  asm volatile("" : "+r,m"(value) : : "memory");
}

/// @brief Compiler barrier: all pending writes to memory are assumed to be observed.
inline void clobberMemory() { asm volatile("" : : : "memory"); }

using Clock = std::chrono::steady_clock;

inline double secondsSince(const Clock::time_point start) {
  return std::chrono::duration<double>(Clock::now() - start).count();
}

/// @brief Formats a double such that it can be read back exactly.
inline std::string fmtDouble(const double value) {
  if (std::isnan(value)) return "null";
  if (std::isinf(value)) return value > 0 ? "1e308" : "-1e308";
  char buffer[32];
  std::snprintf(buffer, sizeof(buffer), "%.17g", value);
  return buffer;
}

/// @brief Escapes a string for JSON output.
inline std::string jsonString(const std::string_view text) {
  std::string out = "\"";
  for (const char c : text) {
    if (c == '"' || c == '\\') {
      out += '\\';
      out += c;
    } else if (static_cast<unsigned char>(c) < 0x20) {
      char buffer[8];
      std::snprintf(buffer, sizeof(buffer), "\\u%04x", static_cast<unsigned>(c));
      out += buffer;
    } else {
      out += c;
    }
  }
  return out + "\"";
}

inline double median(std::vector<double> values) {
  if (values.empty()) return std::nan("");
  std::sort(values.begin(), values.end());
  const size_t n = values.size();
  return (n % 2 == 1) ? values[n / 2] : 0.5 * (values[n / 2 - 1] + values[n / 2]);
}

/// @brief Compile-time configuration of the executable, as JSON.
inline std::string buildInfoJson() {
  std::string json = "{";
  json += "\"compiler\":" + jsonString(__VERSION__);
#ifdef NDEBUG
  json += ",\"ndebug\":true";
#else
  json += ",\"ndebug\":false";
#endif
#ifdef __OPTIMIZE__
  json += ",\"optimize\":true";
#else
  json += ",\"optimize\":false";
#endif
#ifdef __AVX2__
  json += ",\"avx2\":true";
#else
  json += ",\"avx2\":false";
#endif
#ifdef __FMA__
  json += ",\"fma\":true";
#else
  json += ",\"fma\":false";
#endif
#if defined(__znver3__) || defined(__znver4__) || defined(__znver5__)
  json += ",\"march_native_zen\":true";
#else
  json += ",\"march_native_zen\":false";
#endif
#ifdef USE_EIGEN
  json += ",\"use_eigen\":true";
#else
  json += ",\"use_eigen\":false";
#endif
#ifdef USE_ARMADILLO
  json += ",\"use_armadillo\":true";
#else
  json += ",\"use_armadillo\":false";
#endif
#ifdef __SANITIZE_ADDRESS__
  json += ",\"asan\":true";
#else
  json += ",\"asan\":false";
#endif
  json += ",\"openmp\":" + std::to_string(_OPENMP);
  json += ",\"omp_max_threads\":" + std::to_string(omp_get_max_threads());
  json += "}";
  return json;
}

/// @brief Per-sample context of a pipeline case.
class PipelineRun {
 public:
  /// @brief Executes @p func and records its wall time under the phase @p name.
  template <typename F>
  void phase(const std::string& name, F&& func) {
    const Clock::time_point start = Clock::now();
    func();
    clobberMemory();
    phases_.emplace_back(name, secondsSince(start));
  }

  /// @brief Records the numerical result of the case and its exact value (NaN if unknown).
  void setResult(const double value, const double exact = std::nan("")) {
    result_ = value;
    exact_ = exact;
  }

  /// @brief Records an informative counter (e.g. number of grid points); last value wins.
  void setCounter(const std::string& name, const double value) {
    for (auto& [key, existing] : counters_) {
      if (key == name) {
        existing = value;
        return;
      }
    }
    counters_.emplace_back(name, value);
  }

 private:
  friend class Suite;
  std::vector<std::pair<std::string, double>> phases_;
  std::vector<std::pair<std::string, double>> counters_;
  double result_ = std::nan("");
  double exact_ = std::nan("");
};

/// @brief Context of a micro case.
class MicroRun {
 public:
  /// @brief Number of items (e.g. grid points) processed by one body call; used for ns/item.
  void setItems(const double items) { items_ = items; }

  /// @brief Records an informative counter; last value wins.
  void setCounter(const std::string& name, const double value) {
    for (auto& [key, existing] : counters_) {
      if (key == name) {
        existing = value;
        return;
      }
    }
    counters_.emplace_back(name, value);
  }

  /**
   * @brief Measures @p body: one warm-up call, batch size calibration, then the samples.
   * Must be called exactly once per case function. @p body should feed its result into
   * @ref doNotOptimize.
   */
  template <typename F>
  void measure(F&& body) {
    body();  // warm-up
    clobberMemory();

    size_t batch = 1;
    while (true) {
      const Clock::time_point start = Clock::now();
      for (size_t i = 0; i < batch; i++) {
        body();
        clobberMemory();
      }
      const double elapsed = secondsSince(start);
      if (elapsed >= minSampleTime_ || batch >= (size_t{1} << 40)) {
        break;
      }
      // Aim at 1.2 * minSampleTime, at most a factor 10 per step
      const double factor = (elapsed <= 0.0) ? 10.0 : 1.2 * minSampleTime_ / elapsed;
      batch = std::max(batch + 1, static_cast<size_t>(static_cast<double>(batch) *
                                                      std::min(10.0, factor)));
    }

    for (size_t s = 0; s < warmups_; s++) {
      for (size_t i = 0; i < batch; i++) {
        body();
        clobberMemory();
      }
    }

    for (size_t s = 0; s < samples_; s++) {
      const Clock::time_point start = Clock::now();
      for (size_t i = 0; i < batch; i++) {
        body();
        clobberMemory();
      }
      perCall_.push_back(secondsSince(start) / static_cast<double>(batch));
    }
    batch_ = batch;
  }

 private:
  friend class Suite;
  size_t samples_ = 5;
  size_t warmups_ = 1;
  double minSampleTime_ = 0.05;
  double items_ = 1.0;
  size_t batch_ = 0;
  std::vector<double> perCall_;
  std::vector<std::pair<std::string, double>> counters_;
};

/// @brief A named collection of cases with a command-line driver.
class Suite {
 public:
  Suite(std::string name, const size_t defaultSamples, const size_t defaultWarmups)
      : name_(std::move(name)), samples_(defaultSamples), warmups_(defaultWarmups) {}

  void addPipeline(std::string name, std::function<void(PipelineRun&)> func) {
    cases_.push_back({std::move(name), std::move(func), {}});
  }

  void addMicro(std::string name, std::function<void(MicroRun&)> func) {
    cases_.push_back({std::move(name), {}, std::move(func)});
  }

  int main(const int argc, char** argv) {
    bool list = (argc <= 1);
    bool json = false;
    std::optional<std::regex> filter;

    for (int i = 1; i < argc; i++) {
      const std::string arg = argv[i];
      auto next = [&]() -> std::string {
        if (i + 1 >= argc) {
          std::fprintf(stderr, "Missing value for %s\n", arg.c_str());
          std::exit(2);
        }
        return argv[++i];
      };

      if (arg == "--list") {
        list = true;
      } else if (arg == "--json") {
        json = true;
      } else if (arg == "--build-info") {
        std::printf("%s\n", buildInfoJson().c_str());
        return 0;
      } else if (arg == "--filter") {
        filter.emplace(next());
      } else if (arg == "--samples") {
        samples_ = std::stoull(next());
      } else if (arg == "--warmups") {
        warmups_ = std::stoull(next());
      } else if (arg == "--min-sample-time") {
        minSampleTime_ = std::stod(next());
      } else if (arg == "--threads") {
        omp_set_num_threads(std::stoi(next()));
      } else {
        std::fprintf(stderr, "Unknown argument %s (see perf_harness.hpp)\n", arg.c_str());
        return 2;
      }
    }

    size_t nRun = 0;
    for (const Case& c : cases_) {
      if (filter && !std::regex_search(c.name, *filter)) continue;

      if (list) {
        std::printf("%s\n", c.name.c_str());
        continue;
      }

      nRun++;
      if (c.pipeline) {
        runPipeline(c, json);
      } else {
        runMicro(c, json);
      }
      std::fflush(stdout);
    }

    if (!list && nRun == 0) {
      std::fprintf(stderr, "No case matches the filter.\n");
      return 1;
    }
    return 0;
  }

 private:
  struct Case {
    std::string name;
    std::function<void(PipelineRun&)> pipeline;
    std::function<void(MicroRun&)> micro;
  };

  std::string name_;
  size_t samples_;
  size_t warmups_;
  double minSampleTime_ = 0.05;
  std::vector<Case> cases_;

  std::string header(const std::string& caseName, const char* kind) const {
    return "{\"suite\":" + jsonString(name_) + ",\"case\":" + jsonString(caseName) +
           ",\"kind\":\"" + kind + "\",\"threads\":" + std::to_string(omp_get_max_threads()) +
           ",\"samples\":" + std::to_string(samples_) + ",\"warmups\":" + std::to_string(warmups_);
  }

  static std::string countersJson(const std::vector<std::pair<std::string, double>>& counters) {
    std::string json = "{";
    for (size_t i = 0; i < counters.size(); i++) {
      json += (i ? "," : "") + jsonString(counters[i].first) + ":" + fmtDouble(counters[i].second);
    }
    return json + "}";
  }

  static std::string arrayJson(const std::vector<double>& values) {
    std::string json = "[";
    for (size_t i = 0; i < values.size(); i++) {
      json += (i ? "," : "") + fmtDouble(values[i]);
    }
    return json + "]";
  }

  void runPipeline(const Case& c, const bool json) const {
    for (size_t w = 0; w < warmups_; w++) {
      PipelineRun run;
      c.pipeline(run);
    }

    std::vector<std::string> metricNames{"total"};
    std::vector<std::vector<double>> metricSamples(1);
    PipelineRun last;

    for (size_t s = 0; s < samples_; s++) {
      PipelineRun run;
      const Clock::time_point start = Clock::now();
      c.pipeline(run);
      metricSamples[0].push_back(secondsSince(start));

      for (const auto& [phaseName, seconds] : run.phases_) {
        const auto it = std::find(metricNames.begin(), metricNames.end(), phaseName);
        const size_t idx = static_cast<size_t>(it - metricNames.begin());
        if (it == metricNames.end()) {
          metricNames.push_back(phaseName);
          metricSamples.emplace_back();
        }
        metricSamples[idx].push_back(seconds);
      }
      last = std::move(run);
    }

    if (json) {
      std::string line = header(c.name, "pipeline") + ",\"unit\":\"s\",\"metrics\":{";
      for (size_t m = 0; m < metricNames.size(); m++) {
        line += (m ? "," : "") + jsonString(metricNames[m]) + ":" + arrayJson(metricSamples[m]);
      }
      line += "},\"result\":" + fmtDouble(last.result_) + ",\"exact\":" + fmtDouble(last.exact_) +
              ",\"counters\":" + countersJson(last.counters_) + "}";
      std::printf("%s\n", line.c_str());
    } else {
      std::printf("%-46s", c.name.c_str());
      for (size_t m = 0; m < metricNames.size(); m++) {
        std::printf("  %s=%.4gs", metricNames[m].c_str(), median(metricSamples[m]));
      }
      const double relErr = std::abs((last.result_ - last.exact_) / last.exact_);
      std::printf("  relErr=%.2e\n", relErr);
    }
  }

  void runMicro(const Case& c, const bool json) const {
    MicroRun run;
    run.samples_ = samples_;
    run.warmups_ = warmups_;
    run.minSampleTime_ = minSampleTime_;
    c.micro(run);

    if (run.perCall_.empty()) {
      std::fprintf(stderr, "Micro case %s did not call measure()\n", c.name.c_str());
      std::exit(3);
    }

    if (json) {
      std::string line = header(c.name, "micro") + ",\"unit\":\"s\",\"metrics\":{\"call\":" +
                         arrayJson(run.perCall_) + "},\"items\":" + fmtDouble(run.items_) +
                         ",\"batch\":" + std::to_string(run.batch_) +
                         ",\"counters\":" + countersJson(run.counters_) + "}";
      std::printf("%s\n", line.c_str());
    } else {
      const double med = median(run.perCall_);
      std::printf("%-46s  call=%.4gs  per_item=%.4gns  batch=%zu\n", c.name.c_str(), med,
                  1e9 * med / run.items_, run.batch_);
    }
  }
};

}  // namespace perf
}  // namespace combigrid
}  // namespace sgpp
