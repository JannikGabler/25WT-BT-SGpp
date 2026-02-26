#ifndef COMBIGRID_MISC_CONCURRENT_TASK_QUEUE_HPP
#define COMBIGRID_MISC_CONCURRENT_TASK_QUEUE_HPP

#include <condition_variable>
#include <cstddef>
#include <memory>
#include <mutex>
#include <stdexcept>

// Thread-safe, optional-bounded queue für Pipelinestufe
template <typename T>
class ConcurrentTaskQueue {
  using storage_t = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

 public:
  ConcurrentTaskQueue(const size_t initial_capacity = 1024)
      : capacity_(initial_capacity > 0 ? initial_capacity : 1),
        buffer_(new storage_t[capacity_]),
        head_(0),
        tail_(0),
        count_(0),
        closed_(false) {}

  ~ConcurrentTaskQueue() {
    // Destroy remaining elements
    std::unique_lock<std::mutex> lk(mtx_);
    for (std::size_t i = 0; i < count_; ++i) {
      std::size_t idx = (head_ + i) % capacity_;
      elem_ptr(idx)->~T();
    }
  }

  // Copy constructor
  ConcurrentTaskQueue(const ConcurrentTaskQueue& other) {
    std::unique_lock<std::mutex> lk(other.mtx_);

    capacity_ = other.capacity_;
    buffer_.reset(new storage_t[capacity_]);
    head_ = 0;
    count_ = other.count_;
    tail_ = count_;
    closed_ = other.closed_;

    for (std::size_t i = 0; i < count_; ++i) {
      std::size_t idx = (other.head_ + i) % other.capacity_;
      T* src = reinterpret_cast<T*>(&other.buffer_[idx]);
      new (reinterpret_cast<T*>(&buffer_[i])) T(*src);  // copy
    }
  }

  // Copy assignment operator
  ConcurrentTaskQueue& operator=(const ConcurrentTaskQueue& other) {
    if (this == &other) return *this;

    // Lock both mutexes without deadlock
    std::unique_lock<std::mutex> lk1(mtx_, std::defer_lock);
    std::unique_lock<std::mutex> lk2(other.mtx_, std::defer_lock);
    std::lock(lk1, lk2);

    // destroy current contents
    for (std::size_t i = 0; i < count_; ++i) {
      std::size_t idx = (head_ + i) % capacity_;
      elem_ptr(idx)->~T();
    }

    capacity_ = other.capacity_;
    buffer_.reset(new storage_t[capacity_]);
    head_ = 0;
    count_ = other.count_;
    tail_ = count_;
    closed_ = other.closed_;

    for (std::size_t i = 0; i < count_; ++i) {
      std::size_t idx = (other.head_ + i) % other.capacity_;
      T* src = reinterpret_cast<T*>(&other.buffer_[idx]);
      new (reinterpret_cast<T*>(&buffer_[i])) T(*src);
    }

    return *this;
  }

  // Move constructor
  ConcurrentTaskQueue(ConcurrentTaskQueue&& other) noexcept {
    std::unique_lock<std::mutex> lk(other.mtx_);

    capacity_ = other.capacity_;
    buffer_ = std::move(other.buffer_);
    head_ = other.head_;
    tail_ = other.tail_;
    count_ = other.count_;
    closed_ = other.closed_;

    // leave other in valid empty state
    other.capacity_ = 0;
    other.head_ = other.tail_ = other.count_ = 0;
    other.closed_ = false;
  }

  // Move assignment operator
  ConcurrentTaskQueue& operator=(ConcurrentTaskQueue&& other) noexcept {
    if (this == &other) return *this;

    std::unique_lock<std::mutex> lk1(mtx_, std::defer_lock);
    std::unique_lock<std::mutex> lk2(other.mtx_, std::defer_lock);
    std::lock(lk1, lk2);

    // destroy current contents
    for (std::size_t i = 0; i < count_; ++i) {
      std::size_t idx = (head_ + i) % capacity_;
      elem_ptr(idx)->~T();
    }

    capacity_ = other.capacity_;
    buffer_ = std::move(other.buffer_);
    head_ = other.head_;
    tail_ = other.tail_;
    count_ = other.count_;
    closed_ = other.closed_;

    // reset other
    other.capacity_ = 0;
    other.head_ = other.tail_ = other.count_ = 0;
    other.closed_ = false;

    return *this;
  }

  // Push by copy
  void push(const T& item) {
    std::unique_lock<std::mutex> lk(mtx_);
    if (closed_) throw std::runtime_error("push on closed queue");
    if (count_ == capacity_) grow_locked();
    // placement-new copy
    new (elem_ptr(tail_)) T(item);
    tail_ = (tail_ + 1) % capacity_;
    ++count_;
    not_empty_.notify_one();
  }

  // Push by move
  void push(T&& item) {
    std::unique_lock<std::mutex> lk(mtx_);
    if (closed_) throw std::runtime_error("push on closed queue");
    if (count_ == capacity_) grow_locked();
    new (elem_ptr(tail_)) T(std::move(item));
    tail_ = (tail_ + 1) % capacity_;
    ++count_;
    not_empty_.notify_one();
  }

  /* pop(...) blocks until element available or queue closed.
   Returns true if an element was returned (moved into out), false if queue closed and no element
   available. */
  bool pop(T& out) {
    std::unique_lock<std::mutex> lk(mtx_);
    not_empty_.wait(lk, [this] { return count_ > 0 || closed_; });
    if (count_ == 0 && closed_) return false;  // closed and empty

    // move the element out
    T* src = elem_ptr(head_);
    out = std::move(*src);  // move-assign into out
    src->~T();              // destroy original
    head_ = (head_ + 1) % capacity_;
    --count_;
    not_full_.notify_one();
    return true;
  }

  // close the queue: wake all waiters. Existing elements can still be popped.
  void close() {
    std::unique_lock<std::mutex> lk(mtx_);
    closed_ = true;
    not_empty_.notify_all();
    not_full_.notify_all();
  }

  bool closed() const {
    std::unique_lock<std::mutex> lk(mtx_);
    return closed_;
  }

  // current number of elements (approximate, mutex-protected)
  std::size_t size() const {
    std::unique_lock<std::mutex> lk(mtx_);
    return count_;
  }

  bool empty() const { return size() == 0; }

 private:
  // pointer helper
  T* elem_ptr(std::size_t idx) { return reinterpret_cast<T*>(&buffer_[idx]); }

  // grow capacity (must hold lock)
  void grow_locked() {
    std::size_t newcap = capacity_ * 2;
    if (newcap == 0) newcap = 1;
    std::unique_ptr<storage_t[]> newbuf(new storage_t[newcap]);

    // move existing elements into new buffer contiguously
    for (std::size_t i = 0; i < count_; ++i) {
      std::size_t old_idx = (head_ + i) % capacity_;
      T* src = elem_ptr(old_idx);
      // placement-new move into new buffer slot i
      new (reinterpret_cast<T*>(&newbuf[i])) T(std::move(*src));
      src->~T();  // destroy old
    }

    buffer_.swap(newbuf);
    capacity_ = newcap;
    head_ = 0;
    tail_ = count_ % capacity_;
  }

 private:
  std::size_t capacity_;
  std::unique_ptr<storage_t[]> buffer_;
  std::size_t head_;
  std::size_t tail_;
  std::size_t count_;

  mutable std::mutex mtx_;
  std::condition_variable not_empty_;
  std::condition_variable not_full_;
  bool closed_;
};

#endif