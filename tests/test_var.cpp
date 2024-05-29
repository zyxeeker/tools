//
// Created by zyxeeker on 2024/5/27.
//

#include <iostream>

#include <var.hpp>
#include <log.h>

using namespace tools;

// 继承VarBase进行实现
struct Obj : public var::Base {
  std::string text;
  int32_t num{};

  void FromJson(const nlohmann::json& json) override {
    // 目标值, 键值, 不存在时的默认值
    const auto cvt = [&](auto& dst, const char* key, auto def_value) {
      dst = json.contains(key) ? json[key].get<std::remove_reference_t<decltype(dst)> >() : def_value;
    };
    cvt(text, "text", "(null)");
    cvt(num, "num", 0);
  }

  nlohmann::json ToJson() override {
    nlohmann::json json;
    json["text"] = text;
    json["num"] = num;
    return json;
  }
};

int main() {
  Obj obj;
  obj.text = "TEST";
  obj.num = 12;

  Obj obj_1;
  obj_1.text = "TEST_1";
  obj_1.num = 2346;

  LOG_D() << "obj: " << obj.ToJsonString() << ", "
          << "obj_1: " << obj_1.ToJsonString() << std::endl;

  nlohmann::json value;
  value["text"] = "TEST!";
  value["num"] = 1222;

  // 复制构造
  var::ObserverObj<Obj> ob(obj);

  auto cb_1 = [](const Obj* old_val, const Obj* new_val) {
    LOG_D() << "onchange: old(" << old_val->num << ", " << old_val->text << "), "
            << "new(" << new_val->num << ", " << new_val->text << ")"<< std::endl;
  };
  auto cb_2 = [](const Obj* old_val, const Obj* new_val) {
    LOG_D() << "onchange: old(" << old_val->num << ", " << old_val->text << "), "
            << "new(" << new_val->num << ", " << new_val->text << ")"<< std::endl;
  };
  auto listener_1 = ob.RegisterListener(cb_1);
  auto listener_2 = ob.RegisterListener(cb_2);

  // 赋值
  ob = value;
  ob = obj_1;
}
