//
// Created by zyxeeker on 2024/3/29.
//

#include <sstream>
#include <iostream>
#include <async.h>

std::mutex g_mutex;

void th_test1(int a) {
  std::lock_guard<std::mutex> lk(g_mutex);
  std::this_thread::sleep_for(std::chrono::microseconds(200));
  std::ostringstream oss;
  oss << __FUNCTION__ << ", " << "a: " << a;
  std::cout << oss.str() << std::endl;
}

void th_test2() {
  std::lock_guard<std::mutex> lk(g_mutex);
  std::cout << __FUNCTION__ << std::endl;
}

void task_test1(int a) {
  std::lock_guard<std::mutex> lk(g_mutex);
  std::this_thread::sleep_for(std::chrono::microseconds(200));
  std::ostringstream oss;
  oss << __FUNCTION__ << ", " << "a: " << a;
  std::cout << oss.str() << std::endl;
}

int task_test2() {
  std::lock_guard<std::mutex> lk(g_mutex);
  std::cout << __FUNCTION__ << std::endl;
  return 10000;
}

class Test {
 public:
  void Th_1() {
    std::lock_guard<std::mutex> lk(g_mutex);
    std::this_thread::sleep_for(std::chrono::microseconds(200));
    std::cout << __FUNCTION__ << std::endl;

  }
  void Th_2(int a) {
    std::lock_guard<std::mutex> lk(g_mutex);
    std::ostringstream oss;
    oss << __FUNCTION__ << ", " << "a: " << a;
    std::cout << oss.str() << std::endl;
  }
  void Task_1() {
    std::lock_guard<std::mutex> lk(g_mutex);
    std::this_thread::sleep_for(std::chrono::microseconds(200));
    std::cout << __FUNCTION__ << std::endl;

  }
  int Task_2(int a) {
    std::lock_guard<std::mutex> lk(g_mutex);
    std::ostringstream oss;
    oss << __FUNCTION__ << ", " << "a: " << a;
    return a;
  }
};

int main() {
  Test test;
#if 1
  auto th_0 = tools::async::Thread("th::test0", &th_test1, 1);
  auto th_1 = tools::async::Thread("th::test1", &th_test2);
  th_0.Join();
  th_1.Join();
  auto th_2 = tools::async::Thread("th::test2", &Test::Th_1, &test);
  auto th_3 = tools::async::Thread("th::test3", &Test::Th_2, &test, 1);
  th_2.Join();
  th_3.Join();
#endif

#if 1
  auto func = [&](int a) {
    std::lock_guard<std::mutex> lk(g_mutex);
    std::cout << a << std::endl;
  };
  tools::async::ThreadPool<int> th_pool("test_", 3, func);
  th_pool.Add(1);
  th_pool.Add(2);
  th_pool.Add(3);
  th_pool.Add(4);
  th_pool.Add(5);
  th_pool.Add(6);
#endif

#if 1
  tools::async::TaskPool task_pool("task_", 4);
  auto task_0 = tools::async::MakeTask("task::test0", &task_test1, 3);
  auto task_1 = tools::async::MakeTask("task::test1", &task_test2);
  auto task_2 = tools::async::MakeTask("task::test2", &Test::Task_1, &test);
  auto task_3 = tools::async::MakeTask("task::test3", &Test::Task_2, &test, 10086);
  task_pool.Add(&task_0);
  task_pool.Add(&task_1);
  task_pool.Add(&task_2);
  task_pool.Add(&task_3);
  task_0.Wait();
  std::cout << "task::test1 result: " << task_1.Wait() << std::endl;
  task_2.Wait();
  std::cout << "task::test3 result: " << task_3.Wait() << std::endl;
#endif
}

