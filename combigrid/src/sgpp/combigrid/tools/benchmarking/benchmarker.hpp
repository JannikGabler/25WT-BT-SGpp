/**
 * @file benchmarker.hpp
 * @brief Lightweight benchmarking harness with checkpoint timings and
 * accuracy tracking, used by the @c examples / @c tests programs.
 */
#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <ios>
#include <iostream>
#include <limits>
#include <mutex>
#include <numeric>
#include <ostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {

/**
 * @brief Repeated-run benchmark harness with named checkpoints.
 *
 * Drives a callable through warmup and measured runs, records per-run
 * total time and per-checkpoint deltas, and
 * reports mean/stddev (plus absolute and relative error against an
 * analytical reference when available). Used by the example programs
 * to compare configurations of the combination technique.
 *
 * Each phase (warmup / measured) keeps running until both its minimum
 * number of runs and its minimum duration (in seconds) are reached.
 *
 * @tparam R     Return type of the benchmarked callable.
 * @tparam Args  Argument types forwarded to the callable.
 */
template <typename R, typename... Args>
class Benchmarker {
 public:
  /**
   * @brief Per-run context handed to the benchmarked callable.
   *
   * The callable invokes @ref mark_checkpoint to record the elapsed
   * time since the previous checkpoint (or the start of the run).
   * Checkpoint names must have been registered up front via
   * @ref add_checkpoint.
   */
  class BenchmarkerContext {
   public:
    /**
     * @param parent                Owning benchmarker (for checkpoint name lookup).
     * @param run_checkpoint_buffer Per-run buffer; entry @c k stores the
     *                              elapsed time at the @c k-th checkpoint.
     * @param record                Whether checkpoints actually contribute
     *                              to the recorded statistics (warmup runs
     *                              pass @c false).
     */
    BenchmarkerContext(Benchmarker* parent, std::vector<double>* run_checkpoint_buffer, bool record)
        : parent_(parent),
          runBuffer_(run_checkpoint_buffer),
          record_(record),
          lastTime_(std::chrono::steady_clock::now()) {}

    /**
     * @brief Records the elapsed time since the last checkpoint under
     * the given name.
     *
     * Thread-safe (guarded by an internal mutex). Times are stored in
     * seconds.
     *
     * @param name Identifier of the checkpoint (must have been added via
     *             @ref Benchmarker::add_checkpoint).
     * @throws std::runtime_error if @p name was never registered.
     */
    void markCheckpoint(const std::string& name) {
      using namespace std::chrono;
      auto now = steady_clock::now();
      std::lock_guard<std::mutex> lk(mutex_);

      auto it = parent_->checkpointIndex_.find(name);
      if (it == parent_->checkpointIndex_.end()) {
        throw std::runtime_error("Unknown checkpoint name: " + name);
      }
      size_t idx = it->second;

      // measure seconds (double)
      double duration_s = duration_cast<duration<double>>(now - lastTime_).count();

      if (record_ && runBuffer_) {
        // store duration for this checkpoint in the per-run buffer
        (*runBuffer_)[idx] = duration_s;
      }
      lastTime_ = now;
    }

   private:
    Benchmarker* parent_;
    std::vector<double>* runBuffer_;
    bool record_;
    std::chrono::steady_clock::time_point lastTime_;
    std::mutex mutex_;
  };

  /// @brief Constructs a benchmarker with default 10 runs, 2 warmups and no minimum durations.
  Benchmarker()
      : rounds_(10),
        warmupRounds_(2),
        minRunTime_(0.0),
        minWarmupTime_(0.0),
        showProgress_(false) {}

  /// @brief Sets the minimum number of measured runs (excluding warmups).
  void setRounds(size_t runs) { rounds_ = runs; }

  /// @brief Sets the minimum number of warmup runs (not included in statistics).
  void setWarmupRounds(size_t warmups) { warmupRounds_ = warmups; }

  /**
   * @brief Sets the minimum duration of the measured phase.
   * @param seconds Measured runs continue until at least this much time (in seconds) has
   *                elapsed since the first measured run started (and the minimum number of
   *                runs is reached).
   */
  void setMinRunTime(double seconds) { minRunTime_ = seconds; }

  /**
   * @brief Sets the minimum duration of the warmup phase.
   * @param seconds Warmup runs continue until at least this much time (in seconds) has
   *                elapsed since the first warmup run started (and the minimum number of
   *                warmup runs is reached).
   */
  void setMinWarmupTime(double seconds) { minWarmupTime_ = seconds; }

  /**
   * @brief Provides the analytical reference value used to compute the
   * absolute and relative error per run.
   * @param value Reference value; pass @c NaN to skip error
   * statistics.
   */
  void setAnalyticalResult(const double value) { analyticalResult = value; }

  /// @brief Enables or disables a textual progress indicator on stdout.
  void enableProgress(bool on) { showProgress_ = on; }

  /**
   * @brief Registers a checkpoint name in the order in which it will be hit.
   *
   * The duration recorded at this checkpoint is the time between the
   * previous checkpoint (or the run start) and this checkpoint. Calling
   * the function with an already-registered name is a no-op.
   *
   * @param name Checkpoint identifier.
   */
  void addCheckpoint(const std::string& name) {
    if (checkpointIndex_.count(name)) return;
    size_t idx = checkpointNames_.size();
    checkpointNames_.push_back(name);
    checkpointIndex_[name] = idx;
    perCheckpointSamples_.emplace_back();  // vector storing samples for this checkpoint
  }

  /**
   * @brief Executes the benchmark.
   *
   * Runs @p func through the warmup and measured phases, collecting checkpoint timings, total
   * run time, and (if a reference is configured) absolute / relative errors of the returned
   * value. Each phase continues until both its minimum number of runs and its minimum duration
   * are reached.
   *
   * @param func Callable to benchmark. Must accept a @ref BenchmarkerContext
   * reference followed by @p Args... and return a value comparable against
   * @c analyticalResult.
   * @param args Arguments forwarded to @p func on every run.
   */
  void run(std::function<R(BenchmarkerContext&, Args...)> func, Args... args) {
    runWarmups(func, args...);
    runMeasured(func, args...);
  }

  /**
   * @brief Pretty-prints aggregated results (mean / stddev / counts).
   * @param os Output stream (defaults to @c std::cout).
   */
  void printResults(std::ostream& os = std::cout) const {
    os << std::fixed << std::setprecision(6);
    os << "=== Benchmark Results ===\n";
    os << "Runs: " << totalDurations_.size() << " (min " << rounds_ << ", min " << minRunTime_
       << " s)  Warmups: " << performedWarmups_ << " (min " << warmupRounds_ << ", min "
       << minWarmupTime_ << " s)\n\n";

    // Total runtime statistics
    const auto [totalMin, totalMean, totalSD, totalN] = stats(totalDurations_);
    os << "Total per-run (s):\n";
    os << "  min = " << totalMin << " s"
       << "    mean = " << totalMean << " s"
       << "    stddev = " << totalSD << " s"
       << "    count = " << totalN << "\n\n";

    os << std::scientific;
    if (!results.empty()) os << "Result of the 1. run: " << results[0] << std::endl;

    os << std::fixed;
    // Per-checkpoint statistics
    os << "Checkpoints (time since previous checkpoint) [s]:\n";
    os << std::left << std::setw(30) << "Name" << std::right << std::setw(16) << "min"
       << std::setw(16) << "mean" << std::setw(18) << "stddev" << std::setw(10) << "count" << "\n";
    os << std::string(96, '-') << "\n";

    for (size_t i = 0; i < checkpointNames_.size(); ++i) {
      const auto& samples = perCheckpointSamples_[i];
      const auto [cpMin, cpMean, cpSD, cpN] = stats(samples);
      os << std::left << std::setw(30) << checkpointNames_[i] << std::right << std::setw(16)
         << cpMin << std::setw(16) << cpMean << std::setw(18) << cpSD << std::setw(10) << cpN
         << "\n";
    }
    os << std::endl;

    os << "Errors" << std::endl;

    if (!std::isnan(analyticalResult)) {
      os << std::scientific;
      const auto [AbsErrMin, AbsErrMean, AbsErrSD, absErrN] = stats(absErrors);
      const auto [RelErrMin, RelErrMean, RelErrSD, RelErrN] = stats(relErrors);

      os << "  Absolute error:    mean = " << AbsErrMean << "    stddev = " << AbsErrSD
         << std::endl;
      os << "  Relative error:    mean = " << RelErrMean << "    stddev = " << RelErrSD
         << std::endl;
    } else {
      os << "  No analytical solution set." << std::endl;
    }
  }

 private:
  size_t rounds_;
  size_t warmupRounds_;
  double minRunTime_;     // seconds
  double minWarmupTime_;  // seconds
  size_t performedWarmups_ = 0;
  bool showProgress_;
  double analyticalResult = NAN;

  std::vector<std::string> checkpointNames_;
  std::unordered_map<std::string, size_t> checkpointIndex_;

  // Per checkpoint: vector of samples (seconds) across runs
  std::vector<std::vector<double>> perCheckpointSamples_;
  std::vector<double> results;
  std::vector<double> absErrors;
  std::vector<double> relErrors;

  // Total duration per run (seconds)
  std::vector<double> totalDurations_;

  // Helper function: computes min, mean and standard deviation
  static std::tuple<double, double, double, size_t> stats(const std::vector<double>& samples) {
    // Filter out NaN values (usually none except empty cases)
    std::vector<double> s;
    s.reserve(samples.size());
    for (double v : samples) {
      if (!std::isnan(v)) s.push_back(v);
    }
    size_t n = s.size();
    if (n == 0) return {0.0, 0.0, 0.0, 0};
    const double min = *std::min_element(s.begin(), s.end());
    const double mean = std::accumulate(s.begin(), s.end(), 0.0) / static_cast<double>(n);
    double accum = 0.0;
    for (double v : s) accum += (v - mean) * (v - mean);
    const double variance = accum / static_cast<double>(n);  // population variance
    const double sd = std::sqrt(variance);
    return {min, mean, sd, n};
  }

  // Seconds elapsed since `start`
  static double secondsSince(std::chrono::steady_clock::time_point start) {
    using namespace std::chrono;
    return duration_cast<duration<double>>(steady_clock::now() - start).count();
  }

  // A phase is finished once both its minimum number of runs and its minimum duration are reached
  static bool phaseDone(size_t done, size_t min_runs, double elapsed_s, double min_time_s) {
    return done >= min_runs && elapsed_s >= min_time_s;
  }

  // Executes warmup runs (no recording) until the warmup minimums are reached
  void runWarmups(const std::function<R(BenchmarkerContext&, Args...)>& func, Args... args) {
    const auto phase_start = std::chrono::steady_clock::now();
    std::vector<double> dummy(checkpointNames_.size(), std::numeric_limits<double>::quiet_NaN());
    size_t w = 0;
    double elapsed_s = 0.0;

    while (!phaseDone(w, warmupRounds_, elapsed_s, minWarmupTime_)) {
      // Context initialized; user may call mark_checkpoint but no recording is done
      BenchmarkerContext ctx(this, &dummy, false);
      func(ctx, args...);
      ++w;
      elapsed_s = secondsSince(phase_start);

      if (showProgress_) {
        printProgressLabel("Warmup", w, warmupRounds_, elapsed_s, minWarmupTime_);
      }
    }
    performedWarmups_ = w;
    if (showProgress_ && w > 0) std::cout << std::endl;
  }

  // Executes measured runs until the run minimums are reached
  void runMeasured(const std::function<R(BenchmarkerContext&, Args...)>& func, Args... args) {
    totalDurations_.clear();
    for (auto& v : perCheckpointSamples_) v.clear();
    results.clear();
    absErrors.clear();
    relErrors.clear();

    const auto phase_start = std::chrono::steady_clock::now();
    size_t run_i = 0;
    double elapsed_s = 0.0;

    while (!phaseDone(run_i, rounds_, elapsed_s, minRunTime_)) {
      recordRun(func, args...);
      ++run_i;
      elapsed_s = secondsSince(phase_start);

      if (showProgress_) {
        printProgressLabel("Run   ", run_i, rounds_, elapsed_s, minRunTime_);
      }
    }
    if (showProgress_ && run_i > 0) std::cout << std::endl;
  }

  // Executes a single measured run and stores its timings, result and errors
  void recordRun(const std::function<R(BenchmarkerContext&, Args...)>& func, Args... args) {
    const auto run_start = std::chrono::steady_clock::now();

    // Per-run buffer: one slot per checkpoint; initialized with NaN (means "not reached")
    std::vector<double> run_buffer(checkpointNames_.size(),
                                   std::numeric_limits<double>::quiet_NaN());

    BenchmarkerContext ctx(this, &run_buffer, true);

    // Execute user function (exceptions propagate)
    const double result = func(ctx, args...);

    totalDurations_.push_back(secondsSince(run_start));

    // Transfer recorded checkpoint durations into global storage
    for (size_t idx = 0; idx < checkpointNames_.size(); ++idx) {
      // If NaN -> checkpoint was not hit in this run, ignore it
      if (!std::isnan(run_buffer[idx])) perCheckpointSamples_[idx].push_back(run_buffer[idx]);
    }

    const double absError = std::abs(analyticalResult - result);
    const double relError = std::abs(absError / analyticalResult);
    results.push_back(result);
    absErrors.push_back(absError);
    relErrors.push_back(relError);
  }

  // Prints progress labeled with `label` ("Warmup" or "Run") to console. The percentage is the
  // smaller one of the run-count and the elapsed-time progress (both minimums must be reached).
  void printProgressLabel(const char* label, size_t done, size_t min_runs, double elapsed_s,
                          double min_time_s) const {
    const double count_frac =
        min_runs > 0 ? static_cast<double>(done) / static_cast<double>(min_runs) : 1.0;
    const double time_frac = min_time_s > 0.0 ? elapsed_s / min_time_s : 1.0;
    const double pct = std::min(1.0, std::min(count_frac, time_frac)) * 100.0;
    // label padded to 6 chars to keep alignment (e.g. "Warmup" or "Run   ")
    std::cout << "\r" << label << " " << done << "/" << min_runs << ", " << std::fixed
              << std::setprecision(2) << elapsed_s << "/" << min_time_s << " s ("
              << std::setprecision(1) << pct << "%)   " << std::flush;
  }

  // Allow Context access to private members
  friend class BenchmarkerContext;
};

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp