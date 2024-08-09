#ifndef CVT_HPP_
#define CVT_HPP_

#include <cstring>
#include <cstdint>

#include <tuple>
#include <vector>
#include <string>
#include <type_traits>

#include <nlohmann/json.hpp>

namespace tools {

// 用于生成属性元组的区域
#define VAR_PROPERTY_SCOPE(CLASS, ...) \
  using __Var = CLASS;                 \
  static constexpr auto __Property = std::make_tuple(__VA_ARGS__);

// 默认方式
#define VAR_PROPERTY(MEMBER) \
  tools::cvt::PropertyMeta(&__Var::MEMBER, #MEMBER)

// 自定义成员名称
#define VAR_PROPERTY_SCHEME(MEMBER, MEMBER_NAME) \
  tools::cvt::PropertyMeta(&__Var::MEMBER, MEMBER_NAME)

namespace cvt {
namespace _impl {

////////////////////  Implementation of Property  ////////////////////

/**
 * 属性(成员)元数据
 * @tparam Class 类/结构体
 * @tparam T 成员类型
 */
template <typename Class, typename T>
struct PropertyMeta {
  /**
   * 编译器构造函数
   * @param member 成员地址
   * @param name 成员名/字段
   */
  constexpr PropertyMeta(T Class::*member, const char* name)
      : Name(name),
        Member(member) {}
  // 类型
  using Type = T;
  // 成员名称
  const char* Name;
  // 成员地址
  T Class::*Member;
};

namespace buffer {
// buffer类型
using Type = std::vector<uint8_t>;
// 字符串长度类型
using StrSizeType = uint8_t;
// 数组长度类型
using ArrSizeType = uint8_t;

struct Helper {
  enum RANGE_STATUS : int8_t {
    OUT_OF_RANGE = -1,
    TO_BUFFER_END = 0,
    IN_RANGE,
  };
  // 范围判断
  static RANGE_STATUS IsInRange(size_t buff_size, off_t offset, int size_len) {
    if (offset > buff_size || offset + size_len > buff_size) return OUT_OF_RANGE;
    if (offset == buff_size) return TO_BUFFER_END;
    return IN_RANGE;
  }
};

#define RANGE_CHK(BUFF_SIZE, OFFSET, TYPE_SIZE) \
if (auto res = buffer::Helper::IsInRange(BUFF_SIZE, OFFSET, TYPE_SIZE) != buffer::Helper::IN_RANGE) { return res; }

} // buffer

////////////////////  Implementation of Binary Cvt  ////////////////////

/**
 * 序列化核心类
 * @tparam T 源类型
 */
template<typename T>
struct ToBuffer {
  /**
   * 重载()用于转换返回
   * @param dst 目标buffer
   * @param src 源类型
   */
  void operator()(buffer::Type& buffer, const T& src) {
    auto ptr = reinterpret_cast<const uint8_t *>(&src);
    for (int i = 0; i < sizeof(T); i++) {
      buffer.push_back(ptr[i]);
    }
  }
};

// String/Array structure
// +----------------+---------------------------------------------+
// | size (2 bytes) |    data (size * (element`s size) bytes)     |
// +----------------+---------------------------------------------+
//
// +------+-------------------+-------------------+
// | size |      Elem 1       |      Elem 2       | ....
// +------+------+------------+------+------------+
// |      | size |    data    | size |    data    | ....
// +------+------+------------+------+------------+

// string偏特化
template<>
struct ToBuffer<std::string> {
  void operator()(buffer::Type& buffer, const std::string& src) {
    ToBuffer<buffer::StrSizeType>()(buffer, static_cast<buffer::StrSizeType>(src.size()));
    for (auto i : src) {
      buffer.push_back(i);
    }
  }
};

// vector偏特化
template<typename T>
struct ToBuffer<std::vector<T> > {
  void operator()(buffer::Type& buffer, const std::vector<T>& src) {
    ToBuffer<buffer::ArrSizeType>()(buffer, static_cast<buffer::ArrSizeType>(src.size()));
    for (const auto& i : src) {
      ToBuffer<T>()(buffer, i);
    }
  }
};

/**
 * 反序列化的核心类
 * @tparam T 目标类型
 */
template<typename T>
struct FromBuffer {
  /**
   * 重载()用于转换返回
   * @param dst 目标地址
   * @param buff 待转换的buffer空间
   * @param offset buffer指针偏移量
   * @return 正常返回已处理字节数, 异常时返回 -1
   */
  int operator()(T& dst, const buffer::Type& buff, off_t offset) {
    RANGE_CHK(buff.size(), offset, sizeof(T))
    memcpy(&dst, buff.data() + offset, sizeof(T));
    return sizeof(T);
  }
};

// string偏特化
template<>
struct FromBuffer<std::string> {
  int operator()(std::string& dst, const buffer::Type& buff, off_t offset) {
    constexpr int SizeLen = sizeof(buffer::StrSizeType);
    RANGE_CHK(buff.size(), offset, SizeLen)
    auto _data = buff.data();
    auto _size = *reinterpret_cast<const buffer::StrSizeType *>(_data + offset);
    dst.assign(reinterpret_cast<const char *>(_data + offset + SizeLen), _size);
    return _size + SizeLen;
  }
};

// vector偏特化
template<typename T>
struct FromBuffer<std::vector<T> > {
  int operator()(std::vector<T>& dst, const buffer::Type& buff, off_t offset) {
    constexpr int SizeLen = sizeof(buffer::ArrSizeType);
    RANGE_CHK(buff.size(), offset, SizeLen)
    auto _data = buff.data();
    auto _size = *reinterpret_cast<const buffer::ArrSizeType *>(_data + offset);
    // 判断是否为class, 用于处理嵌套情况
    if constexpr (std::is_class_v<T>) {
      off_t _offset = offset + SizeLen;
      for (int i = 0; i < _size; i++) {
        T _value;
        // 递归调用, 当解析失败时返回
        auto res = FromBuffer<T>()(_value, buff, _offset);
        if (res < 0) return res;
        _offset += res;
        dst.push_back(std::move(_value));
      }
      return _offset - offset;
    } else {
      auto _array = reinterpret_cast<const T *>(_data + offset + SizeLen);
      for (int i = 0; i < _size; i++) {
        dst.push_back(_array[i]);
      }
      return _size * sizeof(T) + SizeLen;
    }
  }
};

////////////////////  Implementation of Json Cvt  ////////////////////

/**
 * 序列化至Json数据核心类
 * @tparam T 源类型
 */
template <typename T>
struct ToJson {
  void operator()(nlohmann::json& json, const T& value) {
    json = value;
  }
};

// vector偏特化
template <typename T>
struct ToJson<std::vector<T> > {
  void operator()(nlohmann::json& json, const std::vector<T>& value) {
    for (const auto& i : value) {
      nlohmann::json _json;
      ToJson<T>()(_json, i);
      json.push_back(_json);
    }
  }
};

/**
 * 反序列化Json数据核心类
 * @tparam T 目标类型
 */
template <typename T>
struct FromJson {
  bool operator()(T& val, const nlohmann::json& json) {
    val = json.get<T>();
    return true;
  }
};

// vector偏特化
template <typename T>
struct FromJson<std::vector<T> > {
  bool operator()(std::vector<T>& val, const nlohmann::json& json) {
    if (!json.is_array()) {
      return false;
    }
    for (const auto& i : json) {
      T _val;
      if (!FromJson<T>()(_val, i)) {
        return false;
      }
      val.push_back(_val);
    }
    return true;
  }
};

} // _impl

////////////////////  Property Api  ////////////////////
/**
 * 生成对应属性(成员)元数据
 * @tparam Class 类/结构体
 * @tparam T 成员类型
 * @param member 成员地址
 * @param name 成员名/字段
 * @return 返回元数据
 */
template <typename Class, typename T>
constexpr auto PropertyMeta(T Class::*member, const char* name) {
  return _impl::PropertyMeta<Class, T>(member, name);
}

#if _HAS_CXX17
// TODO: Support other version
/**
 * 遍历属性数据
 * @tparam Args 元素类型
 * @tparam Handler 处理函数类型
 * @param tuple 元组
 * @param handler 处理函数回调
 */
template<typename... Args, typename Handler>
void PropertyForeach(const std::tuple<Args...>& tuple, Handler&& handler) {
  std::apply([&](const Args&... args) {
    (handler(args), ...);
  }, tuple);
}
#endif

////////////////////  Binary Cvt Api  ////////////////////
struct Binary {
  /**
   * 序列化
   * @tparam T 值类型
   * @param buffer 目标空间
   * @param value 待转换值
   */
  template<typename T>
  static void Serialized(_impl::buffer::Type& buffer, const T& value) {
    // 遍历属性中的成员数据, 按照顺序进行转换
    PropertyForeach(T::__Property, [&](auto elem) {
      _impl::ToBuffer<typename decltype(elem)::Type>()(buffer, value.*(elem.Member));
    });
  }

  /**
   * 反序列化
   * @tparam T 目标类型
   * @param value 目标值地址
   * @param buffer 带转换内存空间
   * @return 转换成功与否
   */
  template<typename T>
  static bool Deserialized(T& value, const _impl::buffer::Type& buffer) {
    int offset = 0;
    try {
      // 遍历属性中的成员数据, 按照顺序进行转换
      PropertyForeach(T::__Property, [&](auto elem) {
        auto len = _impl::FromBuffer<typename decltype(elem)::Type>()(value.*(elem.Member), buffer, offset);
        // 解析出错时及时抛出异常结束遍历
        if (len < 0) throw std::logic_error("tools::cvt => bad parsed");
        offset += len;
      });
    }
    catch (...) {
      return false;
    }
    return true;
  }

  /**
   * 单元素序列化
   * @tparam T 值类型
   * @param buffer 目标空间
   * @param value 待转换值
   */
  template<typename T>
  static void SerializedElemOnly(_impl::buffer::Type& buffer, const T& value) {
    _impl::ToBuffer<T>()(buffer, value);
  }

  /**
   * 单元素反序列化
   * @tparam T 目标类型
   * @param value 目标值地址
   * @param buffer 带转换内存空间
   * @return 转换成功与否
   */
  template<typename T>
  static bool DeserializedElemOnly(T& value, const _impl::buffer::Type& buffer) {
    return _impl::FromBuffer<T>()(value, buffer, 0) >= 0;
  }
};

////////////////////  Json Cvt Api  ////////////////////
struct Json {
  template<typename T>
  static void Serialized(nlohmann::json& json, const T& value) {
    // 遍历属性中的成员数据, 按照顺序进行转换
    PropertyForeach(T::__Property, [&](auto elem) {
      nlohmann::json json_value;
      _impl::ToJson<typename decltype(elem)::Type>()(json_value, value.*(elem.Member));
      json[elem.Name] = json_value;
    });
  }
  template<typename T>
  static bool Deserialized(T& value, const nlohmann::json& json) {
    try {
      // 遍历属性中的成员数据, 按照顺序进行转换
      PropertyForeach(T::__Property, [&](auto elem) {
        using ValueType = typename decltype(elem)::Type;
        auto res = json.find(elem.Name);
        if (res == json.end()) {
          value.*(elem.Member) = ValueType {};
        } else {
          _impl::FromJson<ValueType>()(value.*(elem.Member), *res);
        }
      });
    }
    catch (...) {
      return false;
    }
    return true;
  }
  template <typename T>
  static std::string ToString(const T& value) {
    nlohmann::json json;
    Serialized(json, value);
    return nlohmann::to_string(json);
  }
};

} // cvt
} // tools

#endif // CVT_HPP_
