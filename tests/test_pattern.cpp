//
// Created by zyxeeker on 2024/4/24.
//

#include <log.h>
#include <pattern.hpp>

class A {
 public:
  void Test(int num) {
    LOG_D() << "TEST#" << num << std::endl;
  }
};

class Elem : public tools::pattern::Observer<int> {
 public:
  Elem() = default;
  ~Elem() = default;
  inline void set_num(int num) {
    LOG_D() << "set num: " << num << std::endl;
    num_ = num;
    DispatchData(num_);
  }
  inline int num() const { return num_; }
 private:
  int num_{0};
};

class ElemListener : public tools::pattern::Observer<int>::Listener {
 public:
  explicit ElemListener(const char* name)
      : name_(name) {}
  ~ElemListener() = default;
  void Handle(const int& val) override {
    LOG_D() << name_ << " handle update: " << val << std::endl;
  }
 private:
  const char* name_;
};

int main() {
  using HSMgr = tools::pattern::HungrySingleton<A>;
  using LSMgr = tools::pattern::LazySingleton<A>;
  HSMgr::GetInstance().Test(1);
  LSMgr::GetInstance().Test(2);

  auto elem = std::make_unique<Elem>();
  auto elem_listener_1 = std::make_shared<ElemListener>("elem listener 1");
  auto elem_listener_2 = std::make_shared<ElemListener>("elem listener 2");
  elem->RegisterListener(elem_listener_1);
  elem->RegisterListener(elem_listener_2);

  elem->set_num(1);
  elem->set_num(2);
  elem->set_num(3);
}