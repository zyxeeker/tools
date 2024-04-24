//
// Created by zyxeeker on 2024/4/24.
//

#include <iostream>
#include <pattern.hpp>

class A {
 public:
  void Test(int num) {
    std::cout << "TEST#" << num << std::endl;
  }
};

int main() {
  using HSMgr = tools::pattern::HungrySingleton<A>;
  using LSMgr = tools::pattern::LazySingleton<A>;
  HSMgr::GetInstance().Test(1);
  LSMgr::GetInstance().Test(2);
}