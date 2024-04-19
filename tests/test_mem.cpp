//
// Created by zyxeeker on 2024/4/16.
//

#include <chrono>
#include <iostream>

#include <log.h>
#include <mem.hpp>

void FixedArenaTest(size_t epoch, size_t count, bool enabled_step_time = false) {
  LOG("AREA test begin: epoch(%d), count(%d)\n", epoch, count);
  std::vector<int8_t*> v1, v2;
  v1.reserve(count);
  v2.reserve(count);

  auto begin1 = std::chrono::system_clock::now();
  for (auto j = 0; j < epoch; ++j) {
    for (int i = 0; i < count; ++i) {
      auto ptr = new int8_t[640 * 480 * 3];
      v1.push_back(ptr);
    }
    for (int i = 0; i < count; ++i) {
      delete [] v1[i];
    }
    v1.clear();
  }
  auto end1 = std::chrono::system_clock::now();

  tools::mem::FixedArena arena({ 640 * 360 * 3, 1280 * 720 * 3, 1920 * 1080 * 3, 3840 * 2160 * 3});

  auto begin2 = std::chrono::system_clock::now();
  auto _begin = std::chrono::system_clock::now();
  auto _end = std::chrono::system_clock::now();
  for (size_t j = 0; j < epoch; ++j) {
    if (enabled_step_time) {
      _begin = std::chrono::system_clock::now();
    }
    for (int i = 0; i < count; ++i) {
      int8_t* ptr = nullptr;
      arena.Allocate(640 * 480 * 3, ptr);
      v2.push_back(ptr);
    }
    if (enabled_step_time) {
      _end = std::chrono::system_clock::now();
      std::cout << "Arena=>Allocate Total:" << arena.GetTotalSize()
                << ", Free: " << arena.GetFreeSize()
                << ", In Used: " << arena.GetInUsedSize()
                << ", Spend time:"
                << std::chrono::duration_cast<std::chrono::milliseconds>(_end - _begin).count()
                << " ms" << std::endl;
    }
    if (enabled_step_time) {
      _begin = std::chrono::system_clock::now();
    }
    for (int i = 0; i < count; ++i) {
      auto ptr = v2[i];
      arena.Deallocate(ptr);
    }
    if (enabled_step_time) {
      _end = std::chrono::system_clock::now();
      std::cout << "Arena=>Recycle Total:" << arena.GetTotalSize()
                << ", Free: " << arena.GetFreeSize()
                << ", In Used: " << arena.GetInUsedSize()
                << ", Spend time:"
                << std::chrono::duration_cast<std::chrono::milliseconds>(_end - _begin).count()
                << " ms" << std::endl;
    }
    v2.clear();
  }
  auto end2 = std::chrono::system_clock::now();
  std::cout << "Arena NODES: " << arena.GetNodeCount() << std::endl;

  std::cout << "malloc SPEND TIME: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end1 - begin1).count() << " ms" << std::endl;

  std::cout << "Arena SPEND TIME: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end2 - begin2).count() << " ms" << std::endl;
}

int main() {
#if 0
  tools::mem::RingBuffer<int, 4> rb;
  LOG() << "RingBuffer Put: " << std::boolalpha << rb.Put(1) << std::endl;
  LOG() << "RingBuffer Put: " << std::boolalpha << rb.Put(2) << std::endl;
  LOG() << "RingBuffer Put: " << std::boolalpha << rb.Put(3) << std::endl;
  LOG() << "RingBuffer Put: " << std::boolalpha << rb.Put(4) << std::endl;
  LOG() << "RingBuffer Put: " << std::boolalpha << rb.Put(5) << std::endl;
  LOG() << "RingBuffer Put: " << std::boolalpha << rb.Put(6) << std::endl;
  LOG() << "RingBuffer Put: " << std::boolalpha << rb.Put(7) << std::endl;
  int num = 0;
  LOG() << "RingBuffer Get: " << std::boolalpha << rb.Get(num) << ", num: " << num << std::endl;
  LOG() << "RingBuffer Get: " << std::boolalpha << rb.Get(num) << ", num: " << num << std::endl;
  LOG() << "RingBuffer Get: " << std::boolalpha << rb.Get(num) << ", num: " << num << std::endl;
  LOG() << "RingBuffer Get: " << std::boolalpha << rb.Get(num) << ", num: " << num << std::endl;
  LOG() << "RingBuffer Get: " << std::boolalpha << rb.Get(num) << ", num: " << num << std::endl;
  LOG() << "RingBuffer Get: " << std::boolalpha << rb.Get(num) << ", num: " << num << std::endl;
  LOG() << "RingBuffer Get: " << std::boolalpha << rb.Get(num) << ", num: " << num << std::endl;
#endif
  FixedArenaTest(500, 200);
}