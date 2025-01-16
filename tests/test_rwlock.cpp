/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2025-01-10 10:13:07
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2025-01-16 16:37:17
 * @Description: 
 */

// 不适用于MSVC
#ifdef _MSC_VER
  #warning "AtomicShareMutex cannot support in MSVC"
#else

#include <cstdlib>

#include <thread>
#include <shared_mutex>

#include <rwlock.hpp>

using namespace tools::async;

/*
 * 测试线程数
 */
int g_thread_count        = 4;
/*
 * 循环次数
 */
int g_loop_count          = 10000;
/*
 * 测试迭代次数
 */
int g_epoch               = 10;

class STDRWLock {
 public:
  STDRWLock() = default;
  ~STDRWLock() = default;
  void RLock() {
    mutex_.lock_shared();
  }
  void RUnlock() {
    mutex_.unlock_shared();
  }
  void WLock() {
    mutex_.lock();
  }
  void WUnlock() {
    mutex_.unlock();
  }
 private:
  std::shared_mutex mutex_;
};

class AtomicRWLock {
 public:
  AtomicRWLock() = default;
  ~AtomicRWLock() = default;
  void RLock() {
    mutex_.LockShared();
  }
  void RUnlock() {
    mutex_.UnlockShared();
  }
  void WLock() {
    mutex_.Lock();
  }
  void WUnlock() {
    mutex_.Unlock();
  }
 private:
  AtomicSharedMutex mutex_;
};

template <class Lock>
class RWLockBenchMark {
 public:
  RWLockBenchMark() = default;
  ~RWLockBenchMark() = default;
  int GetValue() {
    int value = 0;
    lock_.RLock();
    value = num_;
    lock_.RUnlock();
    return value;
  }
  void ModifyValue(int& value) {
    lock_.WLock();
    num_ = value;
    lock_.WUnlock();
  }
 private:
  int num_;
  Lock lock_;
};


template <class Lock>
void test_impl(clock_t& reader_total, clock_t& writer_total) {
  RWLockBenchMark<Lock> ops;
  clock_t writer_spend_time{}, reader_spend_time[g_thread_count]{}, reader_time{};
  std::thread* reader_ths[g_thread_count]{};
  std::thread writer_th;
  int value;
  
  auto reader = [&](int index) {
    clock_t start = clock();
    for (int i = 0; i < g_loop_count; i++) {
      value = ops.GetValue();
    }
    clock_t end = clock();
    reader_spend_time[index] = end - start;
  };

  auto writer = [&]{
    clock_t start = clock();
    for (int i = 0; i < g_loop_count; i++) {
      ops.ModifyValue(i);
    }
    clock_t end = clock();
    writer_spend_time = end - start;
  };

  for (int i = 0; i < g_thread_count; i++) {
    reader_ths[i] = new std::thread(reader, i);
  }

  writer_th = std::thread(writer);

  for (int i = 0; i < g_thread_count; i++) {
    reader_ths[i]->join();
  }
  writer_th.join();

  for (int i = 0; i < g_thread_count; i++) {
    reader_time += reader_spend_time[i];
  }
  reader_time /= g_thread_count;

  reader_total = reader_time;
  writer_total = writer_spend_time;
}

void std_test() {
  clock_t writer_spend, reader_spend;
  clock_t writer_spend_sum{}, reader_spend_sum{};

  for (int i = 0; i < g_epoch; i++) {
    test_impl<STDRWLock>(reader_spend, writer_spend);
    writer_spend_sum += writer_spend;
    reader_spend_sum += reader_spend;
  }

  printf("(%d, %ld, %ld)\n", g_thread_count, reader_spend_sum / g_epoch, writer_spend_sum / g_epoch);
}

void atomic_test() {
  clock_t writer_spend, reader_spend;
  clock_t writer_spend_sum{}, reader_spend_sum{};

  for (int i = 0; i < g_epoch; i++) {
    test_impl<AtomicRWLock>(reader_spend, writer_spend);
    writer_spend_sum += writer_spend;
    reader_spend_sum += reader_spend;
  }

  printf("(%d, %ld, %ld)\n", g_thread_count, reader_spend_sum / g_epoch, writer_spend_sum / g_epoch);
}

// a/s threads_count epoch_count loop_count
int main(int argv, char* args[]) {
  g_thread_count  = atoi(args[2]);
  g_epoch         = atoi(args[3]);
  g_loop_count    = atoi(args[4]);
  switch(args[1][1]) {
    case 'a':
      atomic_test();
      break;
    case 's':
      std_test();
      break;
  }
  return 0;
}

#endif