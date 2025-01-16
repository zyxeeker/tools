/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-12-23 15:20:05
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2025-01-16 16:36:56
 * @Description: 
 */

#ifndef RWLOCK_H_
#define RWLOCK_H_

#include <atomic>
#include <ctime>

namespace tools {
namespace async {

// 不适用于MSVC
#ifdef _MSC_VER
  #warning "AtomicShareMutex cannot support in MSVC"
#else
/**
 * 自旋最大尝试值
 */
#define ATOMIC_SHARED_MUTEX_TRY_COUNT_MAX 5

/**
 * 用于自旋优化
 */
inline static void SpinLoop() {
#if defined(__x86_64__)
  __asm__ volatile("pause":::"memory");
#elif defined(__arm__) || defined(__aarch64__)
  __asm__ volatile("isb":::"memory");
#else
  __asm__ volatile("nop":::"memory");
#endif  
}

class AtomicSharedMutex {
  /**
   * 写者占用标志
   */
  static constexpr uint32_t writer_enterd_ = 1U << (sizeof(uint32_t) * __CHAR_BIT__ - 1);
  /**
   * 用于到达一定次数后自旋优化的实现
   * @param try_count 尝试次数
   */
  inline static void SpinLoopSleep(int32_t& try_count) {
    if (try_count >= ATOMIC_SHARED_MUTEX_TRY_COUNT_MAX) {
      static struct timespec ts { 0, 1000 };
      nanosleep(&ts, nullptr);
      try_count = 0;
    } else {
      SpinLoop();
    }
  }

 public:
  AtomicSharedMutex() = default;
  AtomicSharedMutex(const AtomicSharedMutex&) = delete;
  ~AtomicSharedMutex() = default;

  /**
   * 读者加锁
   */
  void LockShared() {
    uint32_t expected, desired;
    int32_t try_count{};

    for (;;) {
      expected = state_.load(std::memory_order_acquire) & ~writer_enterd_;
      desired = expected + 1;

      if (
#if defined(__x86_64__)
        state_.compare_exchange_strong(expected, desired, std::memory_order_release)
#else
        state_.compare_exchange_weak(expected, desired, std::memory_order_release)
#endif
        ) {
        break;
      }
      SpinLoopSleep(try_count);
    }
  }
  /**
   * 尝试进行读者加锁, 如果出现写者占用会阻塞执行
   * @return 返回是否成功加锁
   */
  bool TryLockShared() {
    uint32_t expected, desired;

    expected = state_.load(std::memory_order_acquire) & ~writer_enterd_;
    desired = expected + 1;

#if defined(__x86_64__) || defined(_M_X64)
    return state_.compare_exchange_strong(expected, desired, std::memory_order_release);
#else
    return state_.compare_exchange_weak(expected, desired, std::memory_order_release);
#endif
  }
  /**
   * 读者解锁
   */
  void UnlockShared() {
    state_.fetch_sub(1);
  }
  /**
   * 写者加锁
   */
  void Lock() {
    uint32_t expected, desired;
    int32_t try_count{};
    
    for(;;) {
      expected = state_.load(std::memory_order_acquire) & writer_enterd_;
      desired = expected | writer_enterd_;

      if (
#if defined(__x86_64__)
        state_.compare_exchange_strong(expected, desired, std::memory_order_release)
#else
        state_.compare_exchange_weak(expected, desired, std::memory_order_release)
#endif
        ) {
        try_count = 0;

        for (;;) {
          expected = state_.load(std::memory_order_acquire) & writer_enterd_;
          if (
#if defined(__x86_64__)
            state_.compare_exchange_strong(expected, expected, std::memory_order_release)
#else
            state_.compare_exchange_weak(expected, expected, std::memory_order_release)
#endif
            ) {
            return;
          }
          SpinLoopSleep(try_count);
        }
      }
      SpinLoopSleep(try_count);
    }
  }
  /**
   * 尝试进行写者加锁, 如果出现其他写者占用以及读者占用时将会阻塞执行
   * @return 返回是否成功加锁
   */
  bool TryLock() {
    uint32_t expected, desired;
  
    expected = state_.load(std::memory_order_acquire) & writer_enterd_;
    desired = expected | writer_enterd_;

    if (
#if defined(__x86_64__)
      state_.compare_exchange_strong(expected, desired, std::memory_order_release)
#else
      state_.compare_exchange_weak(expected, desired, std::memory_order_release)
#endif
      ) {
      expected = state_.load(std::memory_order_acquire) & writer_enterd_;

      if (
#if defined(__x86_64__)
        state_.compare_exchange_strong(expected, expected, std::memory_order_release)
#else
        state_.compare_exchange_weak(expected, expected, std::memory_order_release)
#endif
        ) {
          return true;
      }
    }
    return false;
  }
  /**
   * 写者解锁
   */
  void Unlock() {
    state_.fetch_and(~writer_enterd_);
  }

  AtomicSharedMutex& operator=(const AtomicSharedMutex&) = delete;
 private: 
  /**
   * 保存写者状态与读者数量
   * |          unsinged int            |
   * | occupied flag |   reader count   |
   * |       1       |  000..(31)..000  |
   */
  std::atomic_uint32_t state_{0};
};

/**
 * 用于读者加锁
 */
class SharedLock {
 public:
  SharedLock(AtomicSharedMutex& lock)
    : lock_(std::addressof(lock)) {
      lock_->LockShared();
    }
  ~SharedLock() {
    lock_->UnlockShared();
  }
 private:
  AtomicSharedMutex* lock_;
};

/**
 * 用于写者加锁
 */
class UniqueLock {
 public:
  UniqueLock(AtomicSharedMutex& lock)
    : lock_(std::addressof(lock)) {
      lock_->Lock();
    }
  ~UniqueLock() {
    lock_->Unlock();
  }
 private:
  AtomicSharedMutex* lock_;
};

} // async
} // tools

#endif

#endif /* RWLOCK_H_ */
