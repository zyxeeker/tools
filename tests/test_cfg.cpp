//
// Created by zyxeeker on 2024/4/23.
//

#include <vector>
#include <sstream>
#include <iostream>

#include <log.h>
#include <cfg.hpp>

struct Test {
  int a;
  float b;
  std::string c;
  DEFINE_PROPERTIES(
      Test,
      PROPERTY(a),
      PROPERTY_SCHEME(b, "B"),
      PROPERTY_SCHEME(c, "str")
  )
};

struct TestVec {
  int a;
  std::vector<int> array;
  DEFINE_PROPERTIES(
      TestVec,
      PROPERTY(a),
      PROPERTY_SCHEME(array, "arr")
  )
};

void TestBasicFunc() {
  auto out = [&](const char* title, const Test& val){
    LOG_D() << title << "=>elements:"
            << "Test.a: " << val.a
            << ", Test.b: " << val.b
            << ", Test.c: \"" << val.c << "\"\n";
  };
  auto str_1 = R"({"a":1, "B": 10089, "str": "Test Only!"})";
  auto test_1 = tools::cfg::FromString<Test>(str_1);
  out("test_1", test_1);
  auto str_2 = R"({"a":1})";
  auto test_2 = tools::cfg::FromString<Test>(str_2);
  out("test_2", test_2);

  auto json_str_1 = tools::cfg::ToString(test_1);
  LOG_D() << "test_1=>str: " << json_str_1 << "\n";
  auto json_str_2 = tools::cfg::ToString(test_2);
  LOG_D() << "test_2=>str: " << json_str_2 << "\n";
  auto test_3 = tools::cfg::FromString<Test>(json_str_2);
  out("test_3", test_3);
}

void TestVecFunc() {
  auto out = [&](const char* title, const TestVec& val){
    std::ostringstream oss;
    for (auto& i : val.array) {
      oss << i << ", ";
    }
    LOG_D() << title << "=>elements:"
            << "TestVec.a: " << val.a
            << ", TestVec.array: [" << oss.str() << "]\n";
  };
  auto str_1 = R"({"a": 2, "arr": [1, 2, 3, 4, 88, 99, 999]})";
  auto test_vec_1 = tools::cfg::FromString<TestVec>(str_1);
  out("test_vec_1", test_vec_1);
  auto str_2 = R"({"a":1})";
  auto test_vec_2 = tools::cfg::FromString<TestVec>(str_2);
  out("test_vec_2", test_vec_2);

  auto json_str_1 = tools::cfg::ToString(test_vec_1);
  LOG_D() << "test_vec_1=>str: " << json_str_1 << "\n";
  auto json_str_2 = tools::cfg::ToString(test_vec_2);
  LOG_D() << "test_vec_2=>str: " << json_str_2 << "\n";
  auto test_vec_3 = tools::cfg::FromString<TestVec>(json_str_2);
  out("test_vec_3", test_vec_3);
}

int main() {
  TestBasicFunc();
  TestVecFunc();
}
