//
// Created by zyxeeker on 2024/4/16.
//

#include <iostream>

#include <log.h>
#include <mem.hpp>

int main() {
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
}