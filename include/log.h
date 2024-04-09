/*
 * @Author: zyxeeker zyxeeker@gmail.com
 * @Date: 2024-03-29 14:36:59
 * @LastEditors: zyxeeker zyxeeker@gmail.com
 * @LastEditTime: 2024-04-09 17:41:59
 * @Description: 
 */

#include <vector>
#include <memory>
#include <sstream>

#ifndef LOG_H
#define LOG_H

#define T_LOG_IMPL(NAME, LVL, ...) \
  tools::log::Log(__FILE__, __FUNCTION__, __LINE__, \
    tools::log::level::LVL, NAME) << tools::log::Log::CStringToStdString(__VA_ARGS__)
// Def Log
#define LOG(...)    T_LOG_IMPL("", INFO, __VA_ARGS__)
#define LOG_D(...)  T_LOG_IMPL("", DEBUG, __VA_ARGS__)
#define LOG_W(...)  T_LOG_IMPL("", WARN, __VA_ARGS__)
#define LOG_E(...)  T_LOG_IMPL("", ERROR, __VA_ARGS__)
#define LOG_F(...)  T_LOG_IMPL("", FATAL, __VA_ARGS__)

// Custom Log
#define CLOG(NAME, ...)    T_LOG_IMPL(NAME, INFO, __VA_ARGS__)
#define CLOG_D(NAME, ...)  T_LOG_IMPL(NAME, DEBUG, __VA_ARGS__)
#define CLOG_W(NAME, ...)  T_LOG_IMPL(NAME, WARN, __VA_ARGS__)
#define CLOG_E(NAME, ...)  T_LOG_IMPL(NAME, ERROR, __VA_ARGS__)
#define CLOG_F(NAME, ...)  T_LOG_IMPL(NAME, FATAL, __VA_ARGS__)


namespace tools {
namespace log {

namespace level {
/**
 * 等级枚举
 */
enum LEVEL : uint8_t {
  DEBUG = 0,
  INFO,
  WARN,
  ERROR,
  FATAL,
  NUM_LEVEL
};

} // level

/**
 * 日志器配置
 * 格式参数如下:
 * p -- 日志等级
 * f -- 文件名
 * c -- 函数名
 * l -- 行号
 * m -- 消息
 * eg. "%d [%p](%f:%l@%c) %m"
 */
struct Config {
  std::string name;             // 名称/Key(Unique)
  std::string pattern;          // 输出格式
  bool toConsole;               // 是否输出至控制台
  bool toFile;                  // 是否输出至文件
  std::string fileName;         // 文件路径
};

/**
 * 注册日志器
 * @param cfg 日志器配置
 * @return 注册是否成功
 */
bool RegisterLogger(const Config& cfg);

/**
 * 以默认参数注册日志器
 * @param name 名称/Key(Unique)
 * @return 注册是否成功
 */
bool RegisterLogger(const std::string& name);

/**
 * 反注册日志器
 * @param name 日志器名
 */
void UnregisterLogger(const std::string& name);

/**
 * 返回所有已注册日志器配置
 * @return 日志器配置数组
 */
std::vector<std::weak_ptr<Config> > GetAllLoggers();

/**
 * 设置最低输出等级
 * @param level 等级枚举
 */
void SetLevel(level::LEVEL level);

/**
 * 获取最低输出等级
 * @return 等级枚举
 */
level::LEVEL GetLevel();

class Log {
 public:
  /**
   * 构建流式输出
   * @param file 文件名
   * @param function 函数名
   * @param line 行号
   * @param level 等级
   * @param name 日志器名
   */
  Log(const char* file,
      const char* function,
      uint64_t line,
      level::LEVEL level,
      const char* name);
  /**
   * 在对象析构时处理缓存的字符串
   */
  ~Log();
  /**
   * 用于转换C风格输入为C++标准字符串
   * @param format 输出格式
   * @param ... 输出的参数
   * @return 标准字符串
   */
  static std::string CStringToStdString(const char* format = nullptr, ...);
  /**
   * 流输出方式 重写输入运算符
   * @tparam T 模板参数来适应不同类型的参数传递
   * @param value 需要打印的参数
   * @return 返回自身引用用于多次 << 调用, 比如 << ... << ...
   */
  template <typename T>
  Log& operator<<(const T& value) {
    oss_ << value;
    return *this;
  }
  /**
   * 用于适配 std::endl, std::ends, std::flush
   * @param func 函数指针
   * @return 返回自身引用
   */
  Log& operator<<(std::ostream& (*func)(std::ostream&)) {
    oss_ << func;
    return *this;
  }

 private:
  /**
   * 用于存储缓冲字符串
   */
  std::ostringstream oss_;
  class Impl;
  std::unique_ptr<Impl> impl_;
};

} // log
} // tools

#endif //LOG_H
