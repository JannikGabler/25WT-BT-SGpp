#pragma once

#include <chrono>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <mutex>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

namespace sgpp {
namespace combigrid {

namespace tools {

template <typename R, typename... Args>
class Benchmarker {
 public:
  // Context passed to the benchmarked function.
  // The function calls context.mark_checkpoint("name") to record checkpoints.
  class BenchmarkerContext {
   public:
    BenchmarkerContext(Benchmarker* parent, std::vector<double>* run_checkpoint_buffer, bool record)
        : parent_(parent),
          run_buffer_(run_checkpoint_buffer),
          record_(record),
          last_time_(std::chrono::steady_clock::now()) {}

    // Marks a checkpoint with the given name.
    // Thread-safe (guarded by mutex), measures time since last checkpoint in seconds.
    void mark_checkpoint(const std::string& name) {
      using namespace std::chrono;
      auto now = steady_clock::now();
      std::lock_guard<std::mutex> lk(mutex_);

      auto it = parent_->checkpoint_index_.find(name);
      if (it == parent_->checkpoint_index_.end()) {
        throw std::runtime_error("Unknown checkpoint name: " + name);
      }
      size_t idx = it->second;

      // measure seconds (double)
      double duration_s = duration_cast<duration<double>>(now - last_time_).count();

      if (record_ && run_buffer_) {
        // store duration for this checkpoint in the per-run buffer
        (*run_buffer_)[idx] = duration_s;
      }
      last_time_ = now;
    }

   private:
    Benchmarker* parent_;
    std::vector<double>* run_buffer_;
    bool record_;
    std::chrono::steady_clock::time_point last_time_;
    std::mutex mutex_;
  };

  Benchmarker() : runs_(10), warmups_(2), show_progress_(false) {}

  // Set number of measured runs (excluding warmups)
  void set_runs(size_t runs) { runs_ = runs; }

  // Set number of warmup runs (not included in statistics)
  void set_warmups(size_t warmups) { warmups_ = warmups; }

  // Enable/disable console progress output
  void enable_progress(bool on) { show_progress_ = on; }

  // Adds a checkpoint (name) in the desired order.
  // Order is important: duration of a checkpoint is measured
  // since the previous checkpoint (or start for the first).
  void add_checkpoint(const std::string& name) {
    if (checkpoint_index_.count(name)) return;
    size_t idx = checkpoint_names_.size();
    checkpoint_names_.push_back(name);
    checkpoint_index_[name] = idx;
    per_checkpoint_samples_.emplace_back();  // vector storing samples for this checkpoint
  }

  // Executes the benchmark.
  // func must have signature: R(Context&, Args...)
  void run(std::function<R(BenchmarkerContext&, Args...)> func, Args... args) {
    using namespace std::chrono;

    // --- Warmup runs ---
    for (size_t w = 0; w < warmups_; ++w) {
      std::vector<double> dummy(checkpoint_names_.size(), std::numeric_limits<double>::quiet_NaN());
      BenchmarkerContext ctx(this, &dummy, false);
      // Context initialized; user may call mark_checkpoint but no recording is done
      func(ctx, args...);

      // show warmup progress if enabled
      if (show_progress_) {
        print_progress_label("Warmup", w + 1, warmups_);
      }
    }
    if (show_progress_ && warmups_ > 0) {
      std::cout << std::endl;
    }

    // --- Actual runs ---
    total_durations_.clear();

    // Ensure checkpoint sample containers are empty
    for (auto& v : per_checkpoint_samples_) v.clear();

    for (size_t run_i = 0; run_i < runs_; ++run_i) {
      auto run_start = steady_clock::now();

      // Per-run buffer: one slot per checkpoint; initialized with NaN (means "not reached")
      std::vector<double> run_buffer(checkpoint_names_.size(),
                                     std::numeric_limits<double>::quiet_NaN());

      BenchmarkerContext ctx(this, &run_buffer, true);

      // Execute user function (exceptions propagate)
      func(ctx, args...);

      auto run_end = steady_clock::now();
      double total_s = duration_cast<duration<double>>(run_end - run_start).count();
      total_durations_.push_back(total_s);

      // Transfer recorded checkpoint durations into global storage
      for (size_t idx = 0; idx < checkpoint_names_.size(); ++idx) {
        double val = run_buffer[idx];
        if (!std::isnan(val)) {
          per_checkpoint_samples_[idx].push_back(val);
        }
        // If NaN -> checkpoint was not hit in this run, ignore it
      }

      // Progress output
      if (show_progress_) {
        print_progress_label("Run   ", run_i + 1, runs_);
      }
    }

    if (show_progress_ && runs_ > 0) {
      std::cout << std::endl;
    }
  }

  // Prints results to console (mean + standard deviation), units are seconds
  void print_results(std::ostream& os = std::cout) const {
    os << std::fixed << std::setprecision(6);
    os << "=== Benchmark Results ===\n";
    os << "Runs: " << runs_ << "  Warmups: " << warmups_ << "\n\n";

    // Total runtime statistics
    const std::tuple<double, double, size_t> sTotal = stats(total_durations_);
    os << "Total per-run (s):\n";
    os << "  mean = " << std::get<0>(sTotal) << " s"
       << "    stddev = " << std::get<1>(sTotal) << " s"
       << "    count = " << std::get<2>(sTotal) << "\n\n";

    // Per-checkpoint statistics
    os << "Checkpoints (time since previous checkpoint) [s]:\n";
    os << std::left << std::setw(30) << "Name" << std::right << std::setw(16) << "mean"
       << std::setw(18) << "stddev" << std::setw(10) << "count" << "\n";
    os << std::string(80, '-') << "\n";

    for (size_t i = 0; i < checkpoint_names_.size(); ++i) {
      const auto& samples = per_checkpoint_samples_[i];
      const std::tuple<double, double, size_t> s = stats(samples);
      os << std::left << std::setw(30) << checkpoint_names_[i] << std::right << std::setw(16)
         << std::get<0>(s) << std::setw(18) << std::get<1>(s) << std::setw(10) << std::get<2>(s)
         << "\n";
    }
    os << std::endl;
  }

 private:
  size_t runs_;
  size_t warmups_;
  bool show_progress_;

  std::vector<std::string> checkpoint_names_;
  std::unordered_map<std::string, size_t> checkpoint_index_;

  // Per checkpoint: vector of samples (seconds) across runs
  std::vector<std::vector<double>> per_checkpoint_samples_;

  // Total duration per run (seconds)
  std::vector<double> total_durations_;

  // Helper function: computes mean and standard deviation
  static std::tuple<double, double, size_t> stats(const std::vector<double>& samples) {
    // Filter out NaN values (usually none except empty cases)
    std::vector<double> s;
    s.reserve(samples.size());
    for (double v : samples) {
      if (!std::isnan(v)) s.push_back(v);
    }
    size_t n = s.size();
    if (n == 0) return {0.0, 0.0, 0};
    double mean = std::accumulate(s.begin(), s.end(), 0.0) / static_cast<double>(n);
    double accum = 0.0;
    for (double v : s) accum += (v - mean) * (v - mean);
    double variance = accum / static_cast<double>(n);  // population variance
    double sd = std::sqrt(variance);
    return {mean, sd, n};
  }

  // Prints progress labeled with `label` ("Warmup" or "Run") to console
  void print_progress_label(const char* label, size_t done, size_t total) const {
    double pct = (static_cast<double>(done) / static_cast<double>(total)) * 100.0;
    // label padded to 6 chars to keep alignment (e.g. "Warmup" or "Run   ")
    std::cout << "\r" << label << " " << done << "/" << total << " (" << std::fixed
              << std::setprecision(1) << pct << "%)   " << std::flush;
  }

  // Allow Context access to private members
  friend class BenchmarkerContext;
};

}  // namespace tools

}  // namespace combigrid
}  // namespace sgpp

// // benchmark.hpp
// #pragma once

// #include <chrono>
// #include <cmath>
// #include <functional>
// #include <iomanip>
// #include <iostream>
// #include <limits>
// #include <mutex>
// #include <numeric>
// #include <string>
// #include <unordered_map>
// #include <vector>

// namespace sgpp {
// namespace combigrid {

// namespace tools {

// template <typename R, typename... Args>
// class Benchmarker {
//  public:
//   // Context passed to the benchmarked function.
//   // The function calls context.mark_checkpoint("name") to record checkpoints.
//   class BenchmarkerContext {
//    public:
//     BenchmarkerContext(Benchmarker* parent, std::vector<double>* run_checkpoint_buffer, bool
//     record)
//         : parent_(parent),
//           run_buffer_(run_checkpoint_buffer),
//           record_(record),
//           last_time_(std::chrono::steady_clock::now()) {}

//     // Marks a checkpoint with the given name.
//     // Thread-safe (guarded by mutex), measures time since last checkpoint.
//     void mark_checkpoint(const std::string& name) {
//       using namespace std::chrono;
//       auto now = steady_clock::now();
//       std::lock_guard<std::mutex> lk(mutex_);

//       auto it = parent_->checkpoint_index_.find(name);
//       if (it == parent_->checkpoint_index_.end()) {
//         throw std::runtime_error("Unknown checkpoint name: " + name);
//       }
//       size_t idx = it->second;

//       double duration_ms = duration_cast<duration<double, std::milli>>(now - last_time_).count();

//       if (record_ && run_buffer_) {
//         // Store duration for this checkpoint in the per-run buffer
//         (*run_buffer_)[idx] = duration_ms;
//       }
//       last_time_ = now;
//     }

//    private:
//     Benchmarker* parent_;
//     std::vector<double>* run_buffer_;
//     bool record_;
//     std::chrono::steady_clock::time_point last_time_;
//     std::mutex mutex_;
//   };

//   Benchmarker() : runs_(10), warmups_(2), show_progress_(false) {}

//   // Set number of measured runs (excluding warmups)
//   void set_runs(size_t runs) { runs_ = runs; }

//   // Set number of warmup runs (not included in statistics)
//   void set_warmups(size_t warmups) { warmups_ = warmups; }

//   // Enable/disable console progress output
//   void enable_progress(bool on) { show_progress_ = on; }

//   // Adds a checkpoint (name) in the desired order.
//   // Order is important: duration of a checkpoint is measured
//   // since the previous checkpoint (or start for the first).
//   void add_checkpoint(const std::string& name) {
//     if (checkpoint_index_.count(name)) return;
//     size_t idx = checkpoint_names_.size();
//     checkpoint_names_.push_back(name);
//     checkpoint_index_[name] = idx;
//     per_checkpoint_samples_.emplace_back();  // vector storing samples for this checkpoint
//   }

//   // Executes the benchmark.
//   // func must have signature: R(Context&, Args...)
//   void run(std::function<R(BenchmarkerContext&, Args...)> func, Args... args) {
//     using namespace std::chrono;

//     // --- Warmup runs ---
//     for (size_t w = 0; w < warmups_; ++w) {
//       std::vector<double> dummy(checkpoint_names_.size(),
//       std::numeric_limits<double>::quiet_NaN()); BenchmarkerContext ctx(this, &dummy, false);
//       // Context initialized; user may call mark_checkpoint but no recording is done
//       func(ctx, args...);
//     }

//     // --- Actual runs ---
//     total_durations_.clear();

//     // Ensure checkpoint sample containers are empty
//     for (auto& v : per_checkpoint_samples_) v.clear();

//     for (size_t run_i = 0; run_i < runs_; ++run_i) {
//       auto run_start = steady_clock::now();

//       // Per-run buffer: one slot per checkpoint; initialized with NaN (means "not reached")
//       std::vector<double> run_buffer(checkpoint_names_.size(),
//                                      std::numeric_limits<double>::quiet_NaN());

//       BenchmarkerContext ctx(this, &run_buffer, true);

//       // Execute user function (exceptions propagate)
//       func(ctx, args...);

//       auto run_end = steady_clock::now();
//       double total_ms = duration_cast<duration<double>>(run_end - run_start).count();
//       total_durations_.push_back(total_ms);

//       // Transfer recorded checkpoint durations into global storage
//       for (size_t idx = 0; idx < checkpoint_names_.size(); ++idx) {
//         double val = run_buffer[idx];
//         if (!std::isnan(val)) {
//           per_checkpoint_samples_[idx].push_back(val);
//         }
//         // If NaN -> checkpoint was not hit in this run, ignore it
//       }

//       // Progress output
//       if (show_progress_) {
//         print_progress(run_i + 1, runs_);
//       }
//     }

//     if (show_progress_) {
//       std::cout << std::endl;
//     }
//   }

//   // Prints results to console (mean + standard deviation)
//   void print_results(std::ostream& os = std::cout) const {
//     os << std::fixed << std::setprecision(3);
//     os << "=== Benchmark Results ===\n";
//     os << "Runs: " << runs_ << "  Warmups: " << warmups_ << "\n\n";

//     // Total runtime statistics
//     const std::tuple<double, double, size_t> sTotal = stats(total_durations_);
//     os << "Total per-run (ms):\n";
//     os << "  mean = " << std::get<0>(sTotal) << " ms"
//        << "    stddev = " << std::get<1>(sTotal) << " ms"
//        << "    count = " << std::get<2>(sTotal) << "\n\n";

//     // Per-checkpoint statistics
//     os << "Checkpoints (time since previous checkpoint) [ms]:\n";
//     os << std::left << std::setw(30) << "Name" << std::right << std::setw(12) << "mean"
//        << std::setw(16) << "stddev" << std::setw(10) << "count" << "\n";
//     os << std::string(70, '-') << "\n";

//     for (size_t i = 0; i < checkpoint_names_.size(); ++i) {
//       const auto& samples = per_checkpoint_samples_[i];
//       const std::tuple<double, double, size_t> s = stats(samples);
//       os << std::left << std::setw(30) << checkpoint_names_[i] << std::right << std::setw(10)
//          << std::get<0>(s) << std::setw(14) << std::get<1>(s) << std::setw(10) << std::get<2>(s)
//          << "\n";
//     }
//     os << std::endl;
//   }

//  private:
//   size_t runs_;
//   size_t warmups_;
//   bool show_progress_;

//   std::vector<std::string> checkpoint_names_;
//   std::unordered_map<std::string, size_t> checkpoint_index_;

//   // Per checkpoint: vector of samples (ms) across runs
//   std::vector<std::vector<double>> per_checkpoint_samples_;

//   // Total duration per run (ms)
//   std::vector<double> total_durations_;

//   // Helper function: computes mean and standard deviation
//   static std::tuple<double, double, size_t> stats(const std::vector<double>& samples) {
//     // Filter out NaN values (usually none except empty cases)
//     std::vector<double> s;
//     s.reserve(samples.size());
//     for (double v : samples) {
//       if (!std::isnan(v)) s.push_back(v);
//     }
//     size_t n = s.size();
//     if (n == 0) return {0.0, 0.0, 0};
//     double mean = std::accumulate(s.begin(), s.end(), 0.0) / static_cast<double>(n);
//     double accum = 0.0;
//     for (double v : s) accum += (v - mean) * (v - mean);
//     double variance = accum / static_cast<double>(n);  // population variance
//     double sd = std::sqrt(variance);
//     return {mean, sd, n};
//   }

//   // Prints progress to console
//   void print_progress(size_t done, size_t total) const {
//     double pct = (static_cast<double>(done) / static_cast<double>(total)) * 100.0;
//     std::cout << "\rProgress: " << done << "/" << total << " (" << std::fixed
//               << std::setprecision(1) << pct << "%)   " << std::flush;
//   }

//   // Allow Context access to private members
//   friend class BenchmarkerContext;
// };

// }  // namespace tools

// }  // namespace combigrid
// }  // namespace sgpp