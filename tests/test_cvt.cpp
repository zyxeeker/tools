//
// Created by zyxeeker on 2024/8/5.
//

#include <vector>
#include <iostream>

#include <gtest/gtest.h>

#include <cvt.hpp>

#define FLOAT_MAX       3.40282e+038
#define DOUBLE_MAX      1.79769e+308

///// Base Type Test

template <typename T>
bool CvtTest(const T& val, T& result) {
  std::vector<uint8_t> buffer;
  tools::cvt::Binary::SerializedElemOnly(buffer, val);
  return tools::cvt::Binary::DeserializedElemOnly(result, buffer);
}

TEST(TEST_CVT_ELEM, BASIC_TYPE) {
  int8_t i8 = 'F', _i8 {};
  int16_t i16 = 0x1234, _i16 {};
  int32_t i32 = 456789, _i32 {};
  int64_t i64 = 1011121345789, _i64 {};
  float f4 = FLOAT_MAX, _f4 {};
  double f8 = DOUBLE_MAX, _f8 {};

#define XX(VAL)                            \
  ASSERT_NE(CvtTest(VAL, _##VAL), false);  \
  EXPECT_EQ(VAL, _##VAL);

  XX(i8)
  XX(i16)
  XX(i32)
  XX(i64)
  XX(f4)
  XX(f8)
#undef XX
}

TEST(TEST_CVT_ELEM, STD_VECTOR_TYPE) {
  std::vector<int8_t> i8_arr { 'A', 'B', 'C', 'D' }, _i8_arr {};
  std::vector<int16_t> i16_arr { 0x1234, 0x4254 }, _i16_arr {};
  std::vector<int32_t> i32_arr { 456789, 456789, 456789, 456789 }, _i32_arr {};
  std::vector<int64_t> i64_arr { 1011121345789 }, _i64_arr {};
  std::vector<float> f4_arr { FLOAT_MAX, FLOAT_MAX }, _f4_arr {};
  std::vector<double> f8_arr { FLOAT_MAX, DOUBLE_MAX }, _f8_arr {};

#define XX(VAL)                              \
  ASSERT_NE(CvtTest(VAL, _##VAL), false);  \
  EXPECT_EQ(VAL.size(), _##VAL.size());      \
  EXPECT_EQ(VAL, _##VAL);

  XX(i8_arr)
  XX(i16_arr)
  XX(i32_arr)
  XX(i64_arr)
  XX(f4_arr)
  XX(f8_arr)
#undef XX
}

TEST(TEST_CVT_ELEM, STD_STRING) {
  std::string string_empty {""}, _string_empty {};
  std::string string_a {"ABD"}, _string_a {};
  std::string string_b {"abcdef123456"}, _string_b {};
  std::string string_c {"abcdABCD123456-./,`@#$@"}, _string_c {};

#define XX(VAL)                              \
  ASSERT_NE(CvtTest(VAL, _##VAL), false);  \
  EXPECT_EQ(VAL.size(), _##VAL.size());      \
  EXPECT_EQ(VAL, _##VAL);

  XX(string_empty)
  XX(string_a)
  XX(string_b)
  XX(string_c)
#undef XX
}

TEST(TEST_CVT_ELEM, STD_STRING_VECTOR) {
  std::vector<std::string> str_vec_1 {"", "ABD"}, _str_vec_1 {};
  std::vector<std::string> str_vec_2 {"ABC", "abcdef123456", "abcdABCD123456-./,`@#$@"}, _str_vec_2 {};

#define XX(VAL)                              \
  ASSERT_NE(CvtTest(VAL, _##VAL), false);  \
  EXPECT_EQ(VAL.size(), _##VAL.size());      \
  EXPECT_EQ(VAL, _##VAL);

  XX(str_vec_1)
  XX(str_vec_2)
#undef XX
}

///// Struct/Class/Json Global Var

struct Complex {
  int8_t int_8;
  int16_t int_16;
  int32_t int_32;
  int64_t int_64;
  float f_4;
  double f_8;
  std::string str;
  std::vector<int> int_arr;
  std::vector<std::string> str_arr;

  VAR_PROPERTY_SCOPE(Complex,
                     VAR_PROPERTY(int_8),
                     VAR_PROPERTY(int_16),
                     VAR_PROPERTY(int_32),
                     VAR_PROPERTY(int_64),
                     VAR_PROPERTY(f_4),
                     VAR_PROPERTY(f_8),
                     VAR_PROPERTY(str),
                     VAR_PROPERTY(int_arr),
                     VAR_PROPERTY(str_arr)
                     )
};

Complex g_complex {
    .int_8 = 'F',
    .int_16 = 0x1234,
    .int_32 = 456789,
    .int_64 = 1011121345789,
    .f_4 = FLOAT_MAX,
    .f_8 = DOUBLE_MAX,
    .str = "ABCDabcd1234",
    .int_arr = { 1,2,3,4,5,6,7 },
    .str_arr = { "12345", "6789AB", "CD", "E" }
};

///// Struct/Class Test

template <typename T>
bool CvtComplexTest(const T& val, T& result) {
  std::vector<uint8_t> buffer;
  // 不为struct/class则返回false
  // TODO: !!!无法判断基本的容器类型
  if constexpr (!std::is_class_v<T>) {
    return false;
  }
  tools::cvt::Binary::Serialized(buffer, val);
  return tools::cvt::Binary::Deserialized(result, buffer);
}

TEST(TEST_CVT_COMPLEX, STRUCT) {
  Complex _complex {};

  ASSERT_NE(CvtComplexTest(g_complex, _complex), false);
  EXPECT_EQ(g_complex.int_8, _complex.int_8);
  EXPECT_EQ(g_complex.int_16, _complex.int_16);
  EXPECT_EQ(g_complex.int_32, _complex.int_32);
  EXPECT_EQ(g_complex.int_64, _complex.int_64);
  EXPECT_EQ(g_complex.f_4, _complex.f_4);
  EXPECT_EQ(g_complex.f_8, _complex.f_8);
  EXPECT_EQ(g_complex.str, _complex.str);
  EXPECT_EQ(g_complex.int_arr, _complex.int_arr);
  EXPECT_EQ(g_complex.str_arr, _complex.str_arr);
}

///// Struct/Class Json Test

template <typename T>
bool CvtJsonComplexTest(const T& val, T& result) {
  nlohmann::json json;
  tools::cvt::Json::Serialized(json, val);
  return tools::cvt::Json::Deserialized(result, json);
}

TEST(TEST_CVT_JSON, STRUCT) {
  Complex _complex {};

  ASSERT_NE(CvtJsonComplexTest(g_complex, _complex), false);
  EXPECT_EQ(g_complex.int_8, _complex.int_8);
  EXPECT_EQ(g_complex.int_16, _complex.int_16);
  EXPECT_EQ(g_complex.int_32, _complex.int_32);
  EXPECT_EQ(g_complex.int_64, _complex.int_64);
  EXPECT_EQ(g_complex.f_4, _complex.f_4);
  EXPECT_EQ(g_complex.f_8, _complex.f_8);
  EXPECT_EQ(g_complex.str, _complex.str);
  EXPECT_EQ(g_complex.int_arr, _complex.int_arr);
  EXPECT_EQ(g_complex.str_arr, _complex.str_arr);
}

int main() {
  testing::InitGoogleTest();
  return RUN_ALL_TESTS();
}
