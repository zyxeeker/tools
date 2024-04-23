/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-23 10:42:45
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-23 15:03:14
 * @Description: 
 */

#ifndef CFG_HPP_
#define CFG_HPP_

#include <tuple>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

#define DEFINE_PROPERTIES(STRUCT, ...)                                \
  using _ValueType = STRUCT;                                          \
  static constexpr auto _Properties = std::make_tuple(__VA_ARGS__);   \

#define PROPERTY(NAME)                                   \
  tools::cfg::VarProperty(&_ValueType::NAME, #NAME)      \

#define PROPERTY_SCHEME(NAME, DST_NAME)                  \
  tools::cfg::VarProperty(&_ValueType::NAME, DST_NAME)   \

namespace tools {
namespace cfg {

template <typename Class, typename T>
struct VarPropertyImpl {
  constexpr VarPropertyImpl(T Class::*member, const char* name)
      : Name(name),
        Member(member) {}
  using Type = T;
  const char* Name;
  T Class::*Member;
};

template <typename Class, typename T>
constexpr auto VarProperty(T Class::*member, const char* name) {
  return VarPropertyImpl<Class, T>(member, name);
}

template <typename Tuple, typename Handler, std::size_t... Indexs>
constexpr void TupleForEachImpl(const Tuple& tuple, Handler&& handler,
                                std::index_sequence<Indexs...>) {
  (handler(std::get<Indexs>(tuple)), ...);
}

template <typename Tuple, typename Handler>
constexpr void TupleForEach(const Tuple& tuple, Handler&& handler) {
  TupleForEachImpl(tuple, std::forward<Handler>(handler),
                   std::make_index_sequence<std::tuple_size_v<Tuple> >());
}

///// To Json
template <typename T>
struct ToJsonImpl {
  nlohmann::json operator()(const T& value) {
    if constexpr (std::is_class<T>::value) {
      nlohmann::json json;
      TupleForEach(value._Properties, [&](const auto e) {
        using ValueType = typename decltype(e)::Type;
        json[e.Name] = ToJsonImpl<ValueType>()(value.*(e.Member));
      });
      return json;
    } else {
      return value;
    }
  }
};

template <>
struct ToJsonImpl<std::string> {
  nlohmann::json operator()(const std::string& value) {
    return value;
  }
};

template <typename T>
struct ToJsonImpl<std::vector<T> > {
  nlohmann::json operator()(const std::vector<T>& value) {
    nlohmann::json json;
    for (auto& i : value) {
      json.emplace_back(ToJsonImpl<T>()(i));
    }
    return json;
  }
};


template <typename T>
static nlohmann::json ToJson(const T& src) {
  return ToJsonImpl<T>()(src);
}

///// From Json
template <typename T>
struct FromJsonImpl {
  T operator()(const nlohmann::json& json) {
    T value;
    if constexpr (std::is_class<T>::value) {
      TupleForEach(value._Properties, [&](const auto e) {
        using ValueType = typename decltype(e)::Type;
        auto res = json.find(e.Name);
        if (res == json.end()) {
          value.*(e.Member) = ValueType {};
        } else {
          value.*(e.Member) = FromJsonImpl<ValueType>()(*res);
        }
      });
    } else {
      value = json.get<T>();
    }
    return value;
  }
};

template <>
struct FromJsonImpl<std::string> {
  std::string operator()(const nlohmann::json& json) {
    return json.get<std::string>();
  }
};

template <typename T>
struct FromJsonImpl<std::vector<T> > {
  std::vector<T> operator()(const nlohmann::json& json) {
    std::vector<T> value;
    for (auto& i : json) {
      value.emplace_back(FromJsonImpl<T>()(i));
    }
    return value;
  }
};

template <typename T>
static T FromJson(const nlohmann::json& src) {
  return FromJsonImpl<T>()(src);
}

//// String Transfer
template <typename T>
static std::string ToString(const T& src) {
  return nlohmann::to_string(ToJsonImpl<T>()(src));
}

template <typename T>
static T FromString(const std::string& src) {
  return FromJsonImpl<T>()(nlohmann::json::parse(src));
}

} // cfg
} // tools

#endif //CFG_HPP_
