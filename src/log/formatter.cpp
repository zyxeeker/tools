/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-04-03 14:18:39
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-24 16:40:47
 * @Description: 
 */

#include "formatter.h"

#include <ctime>
#include <map>
#include <functional>
#include <utility>

#include "logger.h"

#define DEFAULT_DATETIME_PATTERN      "%Y-%m-%d %H:%M:%S"

namespace tools {
namespace log {
namespace formatter {

/**
 * 时间戳处理模块
 */
class TimeStamp : public Formatter::Item {
 public:
  explicit TimeStamp(const std::string& format)
    : format_(std::move(format)) {
    // TODO: verify format
  }
  ~TimeStamp() = default;
  std::string operator()(const struct Msg& msg) override {
    char buffer[256]{0};
    struct tm _tm{};
    localtime_s(&_tm, &msg.time);
    std::strftime(buffer, sizeof(buffer), format_.c_str(), &_tm);
    return buffer;
  }
 private:
  const std::string format_;
};

/**
 * 文件名处理模块
 */
class FileName : public Formatter::Item {
 public:
  ~FileName() = default;
  std::string operator()(const struct Msg& msg) override {
#ifdef _WIN32
    auto str = std::strrchr(msg.file, '\\');
#else
    auto str = std::strrchr(msg.file, '/');
#endif
    return str ? str + 1 : msg.file;
  }
};

/**
 * 函数名处理模块
 */
class FuncName : public Formatter::Item {
 public:
  ~FuncName() = default;
  std::string operator()(const struct Msg& msg) override {
    return msg.func;
  }
};

/**
 * 行号处理模块
 */
class Line : public Formatter::Item {
 public:
  ~Line() = default;
  std::string operator()(const struct Msg& msg) override {
    return std::to_string(msg.line);
  }
};

/**
 * 日志等级处理模块
 */
class Level : public Formatter::Item {
 public:
  ~Level() = default;
  std::string operator()(const struct Msg& msg) override {
    switch (msg.level) {
#define XX(LVL) \
    case level::LVL: \
      return #LVL;

      XX(DEBUG)
      XX(INFO)
      XX(WARN)
      XX(ERROR)
      XX(FATAL)
#undef XX
      default:
        return "UNKNOWN";
    }
  }
};

/**
 * 内容处理模块
 */
class Content : public Formatter::Item {
 public:
  ~Content() = default;
  std::string operator()(const struct Msg& msg) override {
    return msg.content;
  }
};

/**
 * 原有字符处理模块
 */
class RawStr : public Formatter::Item {
 public:
  explicit RawStr(const std::string& buf)
    : buf_(std::move(buf)) {};
  ~RawStr() = default;
  std::string operator()(const struct Msg& msg) override {
    return buf_;
  }
 private:
  const std::string buf_;
};

} // formatter

Formatter::Formatter() = default;

Formatter::~Formatter() = default;

bool Formatter::Parse(const std::string &pattern) {
  static std::map<std::string, std::function<Item::Ptr()> > k_formatters = {
#define XX(KEY, ITEM_CLASS) \
    { #KEY, [&](){ return Item::Ptr(new formatter::ITEM_CLASS()); } },

      XX(p, Level)            // p 日志等级
      XX(f, FileName)         // f 文件名
      XX(c, FuncName)         // c 函数名
      XX(l, Line)             // l 行号
      XX(m, Content)          // m 消息
#undef XX
  };
  std::string sub_str;
  for (int i = 0, j; i < pattern.size(); i++) {
    // 不为%
    if (pattern[i] != '%') {
      sub_str.append(1, pattern[i]);
      continue;
    }
    // 为%
    pattern_.emplace_back(Item::Ptr(new formatter::RawStr(sub_str)));
    sub_str.clear();

    j = ++i;
    if (j >= pattern.size()) break;
    // 判断key值
    // d 时间 : 时间解析需单独判断是否带有格式, 比如: %d{%Y-%m-%d %H:%M:%S}
    if (pattern[j] == 'd') {
      ++j;
      std::string date_str = DEFAULT_DATETIME_PATTERN;
      // 判断是否有自定义格式
      if (pattern[j] == '{') {
        // { }对应判断
        int front_bracket_index = j, front_bracket_count = 0, last_bracket_index = 0;
        for (++j; j < pattern.size(); j++) {
          if (pattern[j] == '{')
            ++front_bracket_count;
          if (pattern[j] == '}') {
            last_bracket_index = j;
            if (front_bracket_count > 0)
              --front_bracket_count;
            else
              break;
          }
        }
        // 若对应上则记为格式字符串
        if (last_bracket_index > front_bracket_index) {
          date_str = pattern.substr(++front_bracket_index,
                                    last_bracket_index - front_bracket_index);
          i = last_bracket_index;
        }
      }
//      std::cout << date_str << std::endl;
      pattern_.emplace_back(Item::Ptr(new formatter::TimeStamp(date_str)));
    } else {
      auto res = k_formatters.find(pattern.substr(j, 1));
      // 找到对应处理函数则推入数组, 未找到则记为原始字符串继续循环
      if (res != k_formatters.end()) {
        pattern_.push_back(res->second());
      } else {
        sub_str.append(1, pattern[j]);
      }
    }
  }
  return true;
}

} // log
} // tools